CC = icc
CFLAGS = -g -O3 -debug inline-debug-info -shared-intel -opt-report=3 
OBJS = new_pagerank.o parser.o mergesort.o
OUTPUT = rank 

openmp:
	$(CC) $(CFLAGS) -openmp new_pagerank.c parser.c mergesort.c -c
	$(CC) $(CFLAGS) -openmp $(OBJS) -o $(OUTPUT)

serial:
	$(CC) $(CFLAGS) new_pagerank.c parser.c mergesort.c -c
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTPUT)


%.o: %.c %.h
	#$(CC) $(CFLAGS) -c $<

.PHONY: clean

clean:
	rm *.o $(OUTPUT) *.out 
