#ifndef PAGERANK_H
#define PAGERANK_H

#define MATRIX_SIZE 5
#define D_VAL .85

float *runPageRankE(int **, int *, int);
float calcNodeRank(int **, float *, int *, int, int);

#endif
