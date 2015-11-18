## Makefile for Project 1 of CPE 419 Part C
##
## Authors: Christopher Hunt, Yu Huang
## Date: 10/02/15

CC = nvcc
CFLAGS = -O2
NVFLAGS= -g -arch compute_20 -ccbin g++ -m64 -gencode arch=compute_20,code=sm_20 -gencode arch=compute_30,code=sm_30 -gencode arch=compute_35,code=sm_35 -gencode arch=compute_37,code=sm_37 -gencode arch=compute_50,code=sm_50 -gencode arch=compute_52,code=sm_52 -gencode arch=compute_52,code=compute_52
OBJS = mergesort.o cuda_pagerank.o parser.o
OUTPUT = cuda_rank

single: 
	##$(CC) $(CFLAGS) $(NVFLAGS) $(OBJS) -o $(OUTPUT)
	$(CC) $(CFLAGS) mergesort.cpp cuda_pagerank.cpp parser.cu -c
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTPUT)
double: $(OBJS)
	$(CC) $(CFLAGS) $(NVFLAGS) -D STORE_AS_DOUBLE $(OBJS) -o $(OUTPUT)

debug: $(OBJS)
	$(CC) -G $(NVFLAGS) $(OBJS) -o $(OUTPUT)

%.o: %.c %.h %.cu 
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(OUTPUT) result.out
