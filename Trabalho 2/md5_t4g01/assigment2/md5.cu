////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tomás Oliveira e Silva, December 2016
//
// ACA 2016/2017
//
// Reference implementation of the MD5 message digest for a single block of 512 bits (16 32-bit words) on
// * a CPU
// * optionaly, on a CUDA device, or
//
// The memory layout of the CUDA implementation is, on purpose, not optimal (*** OPTIMIZE IT ***)
//

//#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/common.h"
#include <cuda_runtime.h>
#include "md5.h"
#define max_device_memory  ((size_t)1.3e9) // no more than 1.3GB


//
// Measure real elapsed time
//

static double get_delta_time(void)
{
  static struct timespec t0,t1;

  t0 = t1;
  if(clock_gettime(CLOCK_MONOTONIC,&t1) != 0)
  {
    perror("clock_gettime");
    exit(1);
  }
  return (double)(t1.tv_sec - t0.tv_sec) + 1.0e-9 * (double)(t1.tv_nsec - t0.tv_nsec);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CPU reference implementation
//

static void md5_cpu(const unsigned int data[16],unsigned int hash[4])
{
# define T(i)  K  // in our unmodified MD5 message digest the T[i] constants can all be stored in the same place
  unsigned int K; // place where the T[i] constants will be stored

  MD5(data,1,hash,1);
# undef T
}

static void md5_cpu_verification(void)
{
  static char text[56] = "ACA 2016/2017 (MD5 test)\n"; // text to hash
  unsigned int data[16],hash[4];
  char command[128];
  int i;

  //
  // build 512-bit block
  //
  for(i = 0;i < 16;i++)
    data[i] = 0;
  for(i = 0;i < 55 && text[i] != '\0';i++)
    data[i / 4] |= ((unsigned int)text[i] & 0xFF) << (8 * (i % 4));
  data[i / 4] |= 0x80 << (8 * (i % 4)); // append 0x80
  data[14] = 8 * i; // number of bits
  //
  // compute and display the MD5 message digest using our code
  //
  md5_cpu(data,hash);
  printf("MD5 test (the following two lines should be equal)\n");
  for(i = 0;i < 16;i++)
    printf("%02x",0xFF & (hash[i / 4] >> (8 * (i % 4)))); // least significant byte first!
  printf("  -\n");
  //
  // compute and display the MD5 message digest using the md5sum program
  //
  sprintf(command,"echo -n '%s' | md5sum",text);
  if(system(command) != 0)
    fprintf(stderr,"\"%s\" failed!\n",command);
  printf("\n");
}

static void md5_cpu_speed(int n_blocks,int n_passes)
{
  double dt1,dt2,dt,dt_sum,dt_min,dt_max;
  unsigned int data[16],hash[4];
  int i,n,pass;

  if(n_blocks <= 0)
    n_blocks = 1 << 20;
  if(n_blocks > (1 << 24))
    n_blocks = 1 << 24;
  if(n_passes <= 0)
    n_passes = 10;
  if(n_passes > 100)
    n_passes = 100;
  printf("md5_cpu_speed (%d block%s, %d+2 passes):\n",n_blocks,(n_blocks == 1) ? "" : "s",n_passes);
  dt_sum = 0.0;
  dt_min = +1.0e+100;
  dt_max = -1.0e+100;
  for(pass = -2;pass < n_passes;pass++)
  {
    (void)get_delta_time();
    //
    // first measurement, generate random data
    //
    srand(pass + 3);
    for(n = 0;n < n_blocks;n++)
      for(i = 0;i < 16;i++)
        data[i] = ((unsigned int)rand() & 0xFFFF) | (((unsigned int)rand() & 0xFFFF) << 16);
    dt1 = get_delta_time();
    //
    // second measurement, generate random data and compute the MD5 message digests
    //
    srand(pass + 3);
    for(n = 0;n < n_blocks;n++)
    {
      for(i = 0;i < 16;i++)
        data[i] = ((unsigned int)rand() & 0xFFFF) | (((unsigned int)rand() & 0xFFFF) << 16);
      md5_cpu(data,hash);
    }
    dt2 = get_delta_time();
    //
    // report
    //
    dt = dt2 - dt1;
    printf("  pass %2d: %.6f %.6f %.6f\n",pass,dt1,dt2,dt);
    dt_sum += dt;
    if(dt < dt_min)
      dt_min = dt;
    if(dt > dt_max)
      dt_max = dt;
  }
  dt_sum -= dt_min + dt_max; // ignore the smallest and largest values (discard one outlier on each side)
  printf("  %.0f MD5 message digests per second (one thread, no SIMD instructions)\n",(double)n_passes * (double)n_blocks / dt_sum);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CUDA stuff
//


// grid 2D block 1D
__global__ void computeMD5OnGPU(unsigned int dev_data[], unsigned int dev_hash[])
{
    unsigned int ix = threadIdx.x + blockIdx.x * blockDim.x;
    unsigned int iy = threadIdx.y + blockIdx.y * blockDim.y;
    unsigned int idx = iy * gridDim.x * blockDim.x + ix;
# define T(i)  K  // in our unmodified MD5 message digest the T[i] constants can all be stored in the same place
  unsigned int K; // place where the T[i] constants will be stored

    if (idx < (gridDim.x * gridDim.y * blockDim.x * blockDim.y))
       MD5(((unsigned int *) dev_data+16*idx),1,((unsigned int *)dev_hash+4*idx),1);
# undef T
}

static void md5_cuda_speed(int n_blocks,int n_passes)
{
    printf("Starting md5 GPU...\n");

  // set up device
  int dev = 0;
  cudaDeviceProp deviceProp;

  CHECK(cudaGetDeviceProperties(&deviceProp, dev));
  printf("Using Device %d: %s\n", dev, deviceProp.name);
  CHECK(cudaSetDevice(dev));

  // define data size
  size_t data_size;
  size_t hash_size;
  
  n_blocks &= 0xFFFF0000; // make sure that the number of blocks is a multiple of 65536
                          // (to avoid complications in the kernel launch grid)
  if(n_blocks <= 0)
    n_blocks = 1 << 24;
  if(n_blocks > (1 << 26))
    n_blocks = 1 << 26;
  for(;;)
  { // this could be done in a more elegant way ...
    data_size = (size_t)n_blocks * (size_t)16 * sizeof(unsigned int);
    hash_size = (size_t)n_blocks * (size_t) 4 * sizeof(unsigned int);
    if(data_size + hash_size <= max_device_memory)
      break;
    n_blocks -= 65536;
  }
  if(n_blocks == 0)
  {
    fprintf(stderr,"Not enough memory in the cuda device\n");
    exit(1);
  }
  if(n_passes <= 0)
    n_passes = 10;
  if(n_passes > 100)
    n_passes = 100;

  // create host and device memory areas
  //
  unsigned int *host_data;
  unsigned int *host_hash;
  unsigned int *dev_data;
  unsigned int *dev_hash;
  
  host_data = (unsigned int *)malloc(data_size);
  host_hash = (unsigned int *)malloc(hash_size);
  if(host_data == NULL || host_hash == NULL)
  {
    fprintf(stderr,"Out of host memory!\n");
    exit(1);
  }
  CHECK(cudaMalloc((void **)&dev_data, data_size));
  CHECK(cudaMalloc((void **)&dev_hash, hash_size));
  
  // do the hard work
  double dt,dt_sum,dt_min,dt_max;
  int i,n,pass;

  printf("md5_cuda_speed (%d block%s, %d+2 passes, %d+%d bytes per thread):\n",
         n_blocks,(n_blocks == 1) ? "" : "s",
         n_passes,
         (int)(data_size / n_blocks),
         (int)(hash_size / n_blocks));
  dt_sum = 0.0;
  dt_min = +1.0e+100;
  dt_max = -1.0e+100;

  // define configuration
  // to allow a large launch grid, we will use a 32768x(n_blocks/32768) grid
  int dimx = 512;
  int dimy = 128;
  dim3 block(64,1);
  dim3 grid(dimx, dimy);
  

  printf("execution configuration: grid(%d, %d) - block(%d, %d)\n", grid.x, grid.y, block.x, block.y);
  for(pass = -2;pass < n_passes;pass++)
  {
    // initialize the host_data and send it to the device (this data layout is not the best for the device)
   srand(pass + 3);
    for(n = 0;n < 16u * n_blocks;n++)
      host_data[n] = ((unsigned int)rand() & 0xFFFF) | (((unsigned int)rand() & 0xFFFF) << 16);
    CHECK(cudaMemcpy(dev_data, host_data, data_size, cudaMemcpyHostToDevice));
    CHECK(cudaMemcpy(dev_hash, host_hash, hash_size, cudaMemcpyHostToDevice));

    // invoke kernel at host side(set up launch configuration first)
    (void)get_delta_time();
    computeMD5OnGPU<<<grid, block>>>(dev_data, dev_hash);
    CHECK(cudaGetLastError());
    CHECK(cudaDeviceSynchronize());
    dt = get_delta_time();

    // read and check the device hash data
    CHECK(cudaMemcpy(host_hash, dev_hash, hash_size, cudaMemcpyDeviceToHost));
    for(i = 0;i < n_blocks;i++)
    {
      unsigned int l_hash[4];

      md5_cpu(&host_data[16 * i],l_hash);
      if(l_hash[0] != host_hash[4 * i + 0] || l_hash[1] != host_hash[4 * i + 1] || l_hash[2] != host_hash[4 * i + 2] || l_hash[3] != host_hash[4 * i + 3])
      {
        printf("MD5 hash mismatch:\n");
        printf("  %08X %08X\n",l_hash[0],host_hash[4 * i + 0]);
        printf("  %08X %08X\n",l_hash[1],host_hash[4 * i + 1]);
        printf("  %08X %08X\n",l_hash[2],host_hash[4 * i + 2]);
        printf("  %08X %08X\n",l_hash[3],host_hash[4 * i + 3]);
        exit(1);
      }
    }
    //
    // report
    //
    printf("  pass %2d: %.6f\n",pass,dt);
    dt_sum += dt;
    if(dt < dt_min)
      dt_min = dt;
    if(dt > dt_max)
      dt_max = dt;
  }
  dt_sum -= dt_min + dt_max; // ignore the smallest and largest values (discard one outlier on each side)
  printf("  %.0f MD5 message digests per second\n",(double)n_passes * (double)n_blocks / dt_sum);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Main program
//

int main(int argc,char **argv)
{
  //
  // run tests
  //
  if(argc == 2 && strcmp(argv[1],"-cpu") == 0)
  {
    md5_cpu_verification();
    md5_cpu_speed(1 << 20,10);
    return 0;
  }
  if(argc == 2 && strcmp(argv[1],"-CUDA") == 0)
  {
    md5_cuda_speed(1 << 22,10);
    return 0;
  }
  //
  // usage message
  //
  fprintf(stderr,"usage: %s -cpu     # run cpu tests\n",argv[0]);
  fprintf(stderr,"       %s -CUDA    # run CUDA tests\n",argv[0]);
  return 1;
}
