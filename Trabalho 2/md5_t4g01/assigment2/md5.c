////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tomás Oliveira e Silva, December 2016
//
// ACA 2016/2017
//
// Reference implementation of the MD5 message digest for a single block of 512 bits (16 32-bit words) on
// * a CPU
// * optionaly, on an OpenCL device
// * optionaly, on a CUDA device, or
//
// The memory layout of the OpenCL and CUDA implementations are, on purpose, not optimal (*** OPTIMIZE THEM ***)
//

#ifndef _USE_OpenCL_
# define _USE_OpenCL_ 1
#endif
#ifndef _USE_CUDA_
# define _USE_CUDA_ 0
#endif

#if _USE_CUDA_ != 0
# undef _USE_CUDA_
# define _USE_CUDA_ 0 // not yet done!
#endif

#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    fprintf(stderr,"\"%s\" failed!",command);
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
// OpenCL stuff
//

#if _USE_OpenCL_ != 0

#include <CL/cl.h>

//
// Macro that can be used to call an OpenCL function and to test its return value
// It can, and should, be used to test the return value of calls such as
//   e = clGetPlatformIDs(1,&platform_id[0],&num_platforms);
// In this case, f_name is clGetPlatformIDs and args is (1,&platform_id[0],&num_platforms)
//

#define cl_call(f_name,args)                                                                     \
  do                                                                                             \
  {                                                                                              \
    cl_int e = f_name args;                                                                      \
    if(e != CL_SUCCESS)                                                                          \
    { /* the call failed, terminate the program */                                               \
      fprintf(stderr,"" # f_name "() returned %s (line %d)\n",cl_error_string((int)e),__LINE__); \
      exit(1);                                                                                   \
    }                                                                                            \
  }                                                                                              \
  while(0)


//
// Another macro that can be used to call an OpenCL function and to test its return value
// It can, and should, be used the test the error code value of calls such as
//   context = clCreateContext(NULL,1,&device_id[0],NULL,NULL,&e);
// In this case, f_name is context = clCreateContext and args is (NULL,1,&device_id[0],NULL,NULL,&e)
//

#define cl_call_alt(f_name,args)                                                                 \
  do                                                                                             \
  {                                                                                              \
    cl_int e;                                                                                    \
    f_name args;                                                                                 \
    if(e != CL_SUCCESS)                                                                          \
    { /* the call failed, terminate the program */                                               \
      fprintf(stderr,"" # f_name "() returned %s (line %d)\n",cl_error_string((int)e),__LINE__); \
      exit(1);                                                                                   \
    }                                                                                            \
  }                                                                                              \
  while(0)


//
// "User-friendly" description of OpenCL error codes
//

static char *cl_error_string(int e)
{
  static const char *error_description[100] =
  { // warning: C99 array initialization feature
    [ 0] = "CL_SUCCESS",
    [ 1] = "CL_DEVICE_NOT_FOUND",
    [ 2] = "CL_DEVICE_NOT_AVAILABLE",
    [ 3] = "CL_COMPILER_NOT_AVAILABLE",
    [ 4] = "CL_MEM_OBJECT_ALLOCATION_FAILURE",
    [ 5] = "CL_OUT_OF_RESOURCES",
    [ 6] = "CL_OUT_OF_HOST_MEMORY",
    [ 7] = "CL_PROFILING_INFO_NOT_AVAILABLE",
    [ 8] = "CL_MEM_COPY_OVERLAP",
    [ 9] = "CL_IMAGE_FORMAT_MISMATCH",
    [10] = "CL_IMAGE_FORMAT_NOT_SUPPORTED",
    [11] = "CL_BUILD_PROGRAM_FAILURE",
    [12] = "CL_MAP_FAILURE",
    [13] = "CL_MISALIGNED_SUB_BUFFER_OFFSET",
    [14] = "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST",
    [15] = "CL_COMPILE_PROGRAM_FAILURE",
    [16] = "CL_LINKER_NOT_AVAILABLE",
    [17] = "CL_LINK_PROGRAM_FAILURE",
    [18] = "CL_DEVICE_PARTITION_FAILED",
    [19] = "CL_KERNEL_ARG_INFO_NOT_AVAILABLE",
    [30] = "CL_INVALID_VALUE",
    [31] = "CL_INVALID_DEVICE_TYPE",
    [32] = "CL_INVALID_PLATFORM",
    [33] = "CL_INVALID_DEVICE",
    [34] = "CL_INVALID_CONTEXT",
    [35] = "CL_INVALID_QUEUE_PROPERTIES",
    [36] = "CL_INVALID_COMMAND_QUEUE",
    [37] = "CL_INVALID_HOST_PTR",
    [38] = "CL_INVALID_MEM_OBJECT",
    [39] = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
    [40] = "CL_INVALID_IMAGE_SIZE",
    [41] = "CL_INVALID_SAMPLER",
    [42] = "CL_INVALID_BINARY",
    [43] = "CL_INVALID_BUILD_OPTIONS",
    [44] = "CL_INVALID_PROGRAM",
    [45] = "CL_INVALID_PROGRAM_EXECUTABLE",
    [46] = "CL_INVALID_KERNEL_NAME",
    [47] = "CL_INVALID_KERNEL_DEFINITION",
    [48] = "CL_INVALID_KERNEL",
    [49] = "CL_INVALID_ARG_INDEX",
    [50] = "CL_INVALID_ARG_VALUE",
    [51] = "CL_INVALID_ARG_SIZE",
    [52] = "CL_INVALID_KERNEL_ARGS",
    [53] = "CL_INVALID_WORK_DIMENSION",
    [54] = "CL_INVALID_WORK_GROUP_SIZE",
    [55] = "CL_INVALID_WORK_ITEM_SIZE",
    [56] = "CL_INVALID_GLOBAL_OFFSET",
    [57] = "CL_INVALID_EVENT_WAIT_LIST",
    [58] = "CL_INVALID_EVENT",
    [59] = "CL_INVALID_OPERATION",
    [60] = "CL_INVALID_GL_OBJECT",
    [61] = "CL_INVALID_BUFFER_SIZE",
    [62] = "CL_INVALID_MIP_LEVEL",
    [63] = "CL_INVALID_GLOBAL_WORK_SIZE",
    [64] = "CL_INVALID_PROPERTY",
    [65] = "CL_INVALID_IMAGE_DESCRIPTOR",
    [66] = "CL_INVALID_COMPILER_OPTIONS",
    [67] = "CL_INVALID_LINKER_OPTIONS",
    [68] = "CL_INVALID_DEVICE_PARTITION_COUNT",
    [69] = "CL_INVALID_PIPE_SIZE",
    [70] = "CL_INVALID_DEVICE_QUEUE"
  };
  static char error_string[256];

  sprintf(error_string,"%d[%s]",e,(-e >= 0 && -e < 70 && error_description[-e] != NULL) ? error_description[-e] : "UNKNOWN");
  return &error_string[0];
}

static void md5_open_cl_speed(int n_blocks,int n_passes)
{
  double dt,dt_sum,dt_min,dt_max;
  int i,n,pass;
  FILE *fp;
  char open_cl_source_code[65536];
  size_t open_cl_source_code_size;
  cl_uint num_platforms;
  cl_platform_id platform_id[1];
  cl_uint num_devices;
  cl_device_id device_id[1];
  cl_context context;
  cl_command_queue command_queue;
  size_t data_size;
  size_t hash_size;
  unsigned int *host_data;
  unsigned int *host_hash;
  cl_mem device_data;
  cl_mem device_hash;
  size_t total_work_size[2];
  size_t local_work_size[2];
  char *program_lines[1];
  size_t program_line_lengths[1];
  cl_program program;
  cl_kernel kernel;
  char build_log[1024];

  n_blocks &= 0xFFFF0000; // make sure that the number of blocks is a multiple of 65536 (to avoid complications in the kernel launch grid)
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
    fprintf(stderr,"Not enough memory in the OpenCL device\n");
    exit(1);
  }
  if(n_passes <= 0)
    n_passes = 10;
  if(n_passes > 100)
    n_passes = 100;
  //
  // read the OpenCL kernel source code (we use a pipe here to make sure md5.h is properly included)
  //
  fp = popen("cc -x c -E md5.cl","r");
  if(fp == NULL)
  {
    perror("popen()");
    exit(1);
  }
  open_cl_source_code_size = fread((void *)&open_cl_source_code[0],sizeof(char),sizeof(open_cl_source_code),fp);
  if(open_cl_source_code_size < (size_t)1 || open_cl_source_code_size >= sizeof(open_cl_source_code))
  {
    fprintf(stderr,"fread(): the OpenCL kernel code is either too small or too large\n");
    exit(1);
  }
  pclose(fp);
  //
  // get the first OpenCL platform ID
  //
  cl_call( clGetPlatformIDs , (1,&platform_id[0],&num_platforms) );
  if(num_platforms < 1)
  {
    fprintf(stderr,"No OpenCL platform\n");
    exit(1);
  }
  if(num_platforms > 1)
    fprintf(stderr,"Warning: more than one OpenCL platform found (using the first one)\n");
  //
  // get the first OpenCL device
  //
  cl_call( clGetDeviceIDs , (platform_id[0],CL_DEVICE_TYPE_DEFAULT,1,&device_id[0],&num_devices) );
  if(num_devices < 1)
  {
    fprintf(stderr,"No OpenCL device\n");
    exit(1);
  }
  if(num_devices > 1)
    fprintf(stderr,"Warning: more than one OpenCL device found (using the first one)\n");
  //
  // create an OpenCL context
  //
  cl_call_alt( context = clCreateContext , (NULL,1,&device_id[0],NULL,NULL,&e) );
  //
  // create an OpenCL command queue
  //
#if 0
  cl_call_alt( command_queue = clCreateCommandQueueWithProperties, (context,device_id[0],NULL,&e) );
#else
  cl_call_alt( command_queue = clCreateCommandQueue, (context,device_id[0],0,&e) );
#endif
  //
  // create host and device memory areas
  //
  host_data = (unsigned int *)malloc(data_size);
  host_hash = (unsigned int *)malloc(hash_size);
  if(host_data == NULL || host_hash == NULL)
  {
    fprintf(stderr,"Out of host memory!\n");
    exit(1);
  }
  cl_call_alt( device_data = clCreateBuffer, (context,CL_MEM_READ_WRITE,data_size,NULL,&e) );
  cl_call_alt( device_hash = clCreateBuffer, (context,CL_MEM_READ_WRITE,hash_size,NULL,&e) );
  //
  // transfer the OpenCL code to the OpenCL context
  //
  program_lines[0] = &open_cl_source_code[0];
  program_line_lengths[0] = open_cl_source_code_size;
  cl_call_alt( program = clCreateProgramWithSource, (context,1,(const char **)&program_lines[0],&program_line_lengths[0],&e) );
  //
  // compile the OpenCL code and get the md5_kernel() kernel handle
  //
  cl_call( clBuildProgram , (program,1,&device_id[0],NULL,NULL,NULL) );
  cl_call_alt( kernel = clCreateKernel , (program,"md5_kernel",&e) );
  cl_call( clGetProgramBuildInfo , (program,device_id[0],CL_PROGRAM_BUILD_LOG,sizeof(build_log),(void *)&build_log[0],NULL) );
  if(strlen(build_log) > (size_t)2)
    printf("build log: [%s]\n",build_log);
  //
  // do the hard work
  //
  printf("md5_open_cl_speed (%d block%s, %d+2 passes, %d+%d bytes per thread):\n",
         n_blocks,(n_blocks == 1) ? "" : "s",
         n_passes,
         (int)(data_size / n_blocks),
         (int)(hash_size / n_blocks));
  dt_sum = 0.0;
  dt_min = +1.0e+100;
  dt_max = -1.0e+100;
  for(pass = -2;pass < n_passes;pass++)
  {
    //
    // initialize the host_data and send it to the device (this data layout is not the best for the device)
    //
    for(n = 0;n < 16u * n_blocks;n++)
      host_data[n] = ((unsigned int)rand() & 0xFFFF) | (((unsigned int)rand() & 0xFFFF) << 16);
    cl_call( clEnqueueWriteBuffer , (command_queue,device_data,CL_TRUE,0,data_size,(void *)host_data,0,NULL,NULL) );
    //
    // run the kernel (set its arguments and launch grid first)
    //
    // to allow a large launch grid, we will use a 65536x(n_blocks/65536) grid
    //
    cl_call( clSetKernelArg , (kernel,0,sizeof(cl_mem),(void *)&device_data) );
    cl_call( clSetKernelArg , (kernel,1,sizeof(cl_mem),(void *)&device_hash) );
    cl_call( clSetKernelArg , (kernel,2,sizeof(unsigned int),(void *)&n_blocks) );
    total_work_size[0] = (size_t)65536;              // a total of 65536 in the first dimension
    total_work_size[1] = (size_t)(n_blocks / 65536); // n_blocks/65536 in the second dimension
    local_work_size[0] = (size_t)256;                // locally, all threads are in the first dimension (try other powers of 2 here)
    local_work_size[1] = (size_t)1;
    (void)get_delta_time();
    cl_call( clEnqueueNDRangeKernel , (command_queue,kernel,2,NULL,&total_work_size[0],&local_work_size[0],0,NULL,NULL) );
    cl_call( clFlush , (command_queue) );
    cl_call( clFinish , (command_queue) );
    dt = get_delta_time();
    //
    // read and check the device hash data
    //
    cl_call( clEnqueueReadBuffer , (command_queue,device_hash,CL_TRUE,0,hash_size,(void *)host_hash,0,NULL,NULL) );
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

#endif


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
#if _USE_OpenCL_ != 0
  if(argc == 2 && strcmp(argv[1],"-OpenCL") == 0)
  {
    md5_open_cl_speed(1 << 22,10);
    return 0;
  }
#endif
  //
  // usage message
  //
  fprintf(stderr,"usage: %s -cpu     # run cpu tests\n",argv[0]);
#if _USE_OpenCL_ != 0
  fprintf(stderr,"       %s -OpenCL  # run OpenCL tests\n",argv[0]);
#endif
#if _USE_CUDA_ != 0
  fprintf(stderr,"       %s -CUDA    # run CUDA tests\n",argv[0]);
#endif
  return 1;
}
