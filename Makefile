CC = icc
CFLAGS = -g -O3 -debug inline-debug-info -fopenmp 
OBJS = pagerank.o
OUTPUT = rank 

single:
	$(CC) $(CFLAGS) pagerank.c -c
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTPUT)

#%.o: %.cpp %.c %.h
	#$(CC) $(CFLAGS) -c $<

.PHONY: clean

clean:
	rm *.o $(OUTPUT) *.out 
