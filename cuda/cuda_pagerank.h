#ifndef PAGERANK_H
#define PAGERANK_H

#define MATRIX_SIZE 5
#define D_VAL .85

float *runPageRankE(int **, int *, int);
float calcNodeRank(int **, float *, int *, int, int);
void runMergeSort(float* ranks, char** names, int size);
void runKernel(int* outDegrees, int** adjList);


#endif
