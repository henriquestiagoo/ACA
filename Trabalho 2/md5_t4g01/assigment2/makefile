#
# TOS, December 2016
#
# ACA 2016/2017
#
# the CUDA stuff (optional) is assumed to be installed in /usr/local/cuda
# the CUDA device is assumed to have architecture sm_20 (for an old GTX 480)
#
# to be done (by the students):
#   md5_opt.c and md5_opt.cl are optimized versions (memory layout) of md5.c and md5.cl
#   md5_mod.c, md5_mod.h, and md5_mod.cl are modified versions (difficult to parallelize) of md5.c, md5.h, and md5.cl
#

clean:
	rm -f open_cl_hello cuda_hello cuda_hello.cubin md5 md5.cubin md5_opt md5_mod


open_cl_hello:		open_cl_hello.c
	cc -Wall -O2 -std=c99 open_cl_hello.c -o open_cl_hello -L/usr/local/cuda/lib64 -lOpenCL


cuda_hello:		cuda_hello.c cuda_hello.cubin
	cc -Wall -O2 -std=c99 -I/usr/local/cuda/include cuda_hello.c -o cuda_hello -L/usr/local/cuda/lib64 -lcuda

cuda_hello.cubin:	cuda_hello.cu
	nvcc -arch=sm_20 --compiler-options -O2,-Wall -I/usr/local/cuda/include --cubin cuda_hello.cu -o cuda_hello.cubin


md5:		md5.c md5.h
	cc -Wall -O2 -std=c99 -I/usr/local/cuda/include md5.c -o md5 -L/usr/local/cuda/lib64 -lOpenCL

md5_cu:		md5.cu md5.h
	nvcc -arch=sm_20 -O2 -I/usr/local/cuda/include  md5.cu -o md5

md5_mod_cu:         md5_mod.cu md5_mod.h
	nvcc -arch=sm_20 -O2 -I/usr/local/cuda/include  md5_mod.cu -o md5_mod

md5_opt:		md5_opt.c md5.h
	cc -Wall -O2 -std=c99 -I/usr/local/cuda/include md5_opt.c -o md5_opt -L/usr/local/cuda/lib64 -lOpenCL

md5_mod:		md5_mod.c md5_mod.h
	cc -Wall -O2 -std=c99 -I/usr/local/cuda/include md5_mod.c -o md5_mod -L/usr/local/cuda/lib64 -lOpenCL
