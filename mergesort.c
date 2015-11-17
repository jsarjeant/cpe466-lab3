#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define SWAP(a,b) {float temp = a; a = b; b = temp;}
#define SWAPNAME(a,b) {char* temp2 = a; a = b; b = temp2;}

void setUp(float** a, int size);
void tearDown(double start, double end, float* a, int size);
void merge(float* a, char** names, int size, float* temp, char** tempNames);
void mergesort_serial(float* a, char** names, int size, float* temp, char** tempNames);
void mergesort_parallel_omp
(float* a, char** names, int size, float* temp, char** tempNames, int threads);

/*int main() {
	float* a;
	float* temp = calloc(SIZE, sizeof(float));
	double startTime, endTime;
	int num_threads;

	#pragma omp parallel
	{
		#pragma omp master
		{
			num_threads = omp_get_num_threads();
         printf("Num threads: %d\n", num_threads);
		}
	}

	setUp(&a, SIZE);

	startTime = omp_get_wtime();
	//mergesort_parallel_omp(a, SIZE, temp, num_threads);
	endTime = omp_get_wtime();

	tearDown(startTime, endTime, a, SIZE);
}*/

float* runMergeSort(float* ranks, char** names, int size) {
	float* temp = calloc(size, sizeof(float));
   char** tempNames = calloc(size, sizeof(char*));
	double startTime, endTime;
	int num_threads = omp_get_num_threads();

	startTime = omp_get_wtime();
	mergesort_parallel_omp(ranks, names, size, temp, tempNames, num_threads);
	endTime = omp_get_wtime();

	tearDown(startTime, endTime, ranks, size);
}

void setUp(float** a, int size){
	int i;
   *a = calloc(size, sizeof(float));
   float* b = *a;

	srand(time(NULL));
	for (i = 0; i<size; ++i) {
		b[i] = rand() % size;
	}
}

void tearDown(double start, double end, float* a, int size) {
	int sorted = 1;
	int i;

	printf("Time to execute: %f\n", end-start);
	
	for (i = 0; i < size-1; ++i) {
		sorted &= (a[i] >= a[i+1]);
	}

	printf("Array sorted: %d\n", sorted);
   printf("Size is %d\n", size);
}

void merge(float* a, char** names, int size, float* temp, char** tempNames) {
	int i1 = 0;
	int i2 = size / 2;
	int it = 0;

	while(i1 < size/2 && i2 < size) {
		if (a[i1] > a[i2]) {
			temp[it] = a[i1];
         tempNames[it] = names[i1];
			i1 += 1;
		}
		else {
			temp[it] = a[i2];
         tempNames[it] = names[i2];
			i2 += 1;
		}
		it += 1;
	}

	while (i1 < size/2) {
	    temp[it] = a[i1];
       tempNames[it] = names[i1];
	    i1++;
	    it++;
	}
	while (i2 < size) {
	    temp[it] = a[i2];
       tempNames[it] = names[i2];
	    i2++;
	    it++;
	}

   memcpy(a, temp, size*sizeof(int));
	memcpy(names, tempNames, size*sizeof(char*));

}

void mergesort_serial(float* a, char** names, int size, float* temp, char** tempNames) {
	int i;

	if (size == 2) { 
		if (a[0] > a[1])
			return;
		else {
            SWAP(a[0], a[1]);
            SWAPNAME(names[0], names[1]); 
			
            return;
		}
	}

   if(size > 2) {
	   mergesort_serial(a, names, size/2, temp, tempNames);
	   mergesort_serial(a + size/2, names + size/2, size - size/2, temp, tempNames);
	   merge(a, names, size, temp, tempNames);
   }
}


void mergesort_parallel_omp
(float* a, char** names, int size, float* temp, char** tempNames, int threads) {
    if ( threads == 1) {
        mergesort_serial(a, names, size, temp, tempNames);
    }
    else if (threads > 1) {
        #pragma omp parallel sections
        {
            #pragma omp section
            mergesort_parallel_omp(a, names, size/2, temp, tempNames,  threads/2);

            #pragma omp section
            mergesort_parallel_omp(a + size/2, names+size/2, size-size/2,
                temp + size/2, tempNames + size/2, threads-threads/2);
        }

        merge(a, names, size, temp, tempNames);
        } // threads > 1
}
