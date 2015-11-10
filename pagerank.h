#ifndef PAGERANK_H
#define PAGERANK_H

#define MATRIX_SIZE 5
#define D_VAL .25

float *runPageRankE(char **, int);
float calcNodeRank(char **, float *, int *, int, int);

#endif
