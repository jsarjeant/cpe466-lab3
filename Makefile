NVCC = nvcc
NCFLAGS = -O2
NVFLAGS= -g -ccbin g++ -m64 -gencode arch=compute_20,code=sm_20 -gencode arch=compute_30,code=sm_30 -gencode arch=compute_35,code=sm_35 -gencode arch=compute_37,code=sm_37 -gencode arch=compute_50,code=sm_50 -gencode arch=compute_52,code=sm_52 -gencode arch=compute_52,code=compute_52
NOBJS = serial_mergesort.o parser.o
NOUTPUT = cuda_rank

	
CC = icc
CFLAGS = -g -O3 -debug inline-debug-info -shared-intel -opt-report=3
OBJS = new_pagerank.o parser.o mergesort.o
OUTPUT = rank 

cuda_rank: 
	$(NVCC) $(NCFLAGS) serial_mergesort.cpp parser.cu -c
	$(NVCC) $(NCFLAGS) $(NOBJS) -o $(NOUTPUT)

openmp:
	$(CC) $(CFLAGS) -openmp new_pagerank.c parser.c mergesort.c -c
	$(CC) $(CFLAGS) -openmp $(OBJS) -o $(OUTPUT)

mic:
	$(CC) $(CFLAGS) -mmic -openmp new_pagerank.c parser.c mergesort.c -c
	$(CC) $(CFLAGS) -mmic -openmp $(OBJS) -o $(OUTPUT)

%.o: %.c %.h
	#$(CC) $(CFLAGS) -c $<

.PHONY: clean

clean:
	rm *.o $(OUTPUT) $(NOUTPUT) *.out *.optrpt 
