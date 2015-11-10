CC = icc
CFLAGS = -g -O3 -debug inline-debug-info 
OBJS = pagerank.o
OUTPUT = rank 

serial:
	$(CC) $(CFLAGS) pagerank.c -c
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTPUT)

openmp:
	$(CC) $(CFLAGS) -openmp pagerank.c -c
	$(CC) $(CFLAGS) -openmp $(OBJS) -o $(OUTPUT)

%.o: %.c %.h
	#$(CC) $(CFLAGS) -c $<

.PHONY: clean

clean:
	rm *.o $(OUTPUT) *.out 
