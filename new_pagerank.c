#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pagerank.h"
#include <omp.h>

int main() {
   int i, j, tmp, count, numVerts = MATRIX_SIZE, 
    outSum, outDegrees[MATRIX_SIZE];
   float *ranks;
   // Setup Graph[toIndex][fromIndex]
   char graph[MATRIX_SIZE][MATRIX_SIZE] = {{0, 0, 0, 0, 0},  // A: in edges: none
                                           {1, 0, 0, 0, 0},  // B: in-edges: A
                                           {1, 1, 0, 0, 0},  // C: in-edges: A B
                                           {0, 0, 1, 0, 0},  // D: in-edges: C
                                           {1, 1, 0, 0, 0}}; // E: in-edges: A B
   int **g = malloc(sizeof(int *) * numVerts);

   // builds g such that it is a list[numVerts] of int arrays.
   // the int arrays have indexes of in nodes, ending with terminator -1 
   // example based on above [graph]: 
   //   g[0] = [-1]
   //   g[1] = [0, -1]
   //   g[2] = [0, 1, -1]
   //   g[3] = [2, -1]
   //   g[4] = [0, 1, -1]
   for (i = 0; i < numVerts; i++) {
      count = 1;
      for (j = 0; j < numVerts; j++) {
         count += graph[i][j];
      }
      g[i] = malloc(sizeof(int) * count);
      tmp = 0;
      for (j = 0; j < numVerts; j++) {
         if (graph[i][j])
            g[i][tmp++] = j;
      }
      g[i][tmp] = -1;
   }

   // calculate out degrees (n*n)
   // example based on above [graph]:
   //   [3, 2, 1, 0, 0]
   for (i = 0; i < numVerts; i++) {
      outSum = 0;
      for (j = 0; j < numVerts; j++) {
         outSum += graph[j][i];
      }
      outDegrees[i] = outSum;
   }

   // Calculate ranks given graph and coresponding outDegrees array.
   ranks = runPageRankE(g, outDegrees, numVerts);

   // print out results
   printf("Ranks: ");
   for (i = 0; i < numVerts; i++) {
      printf("(%c %f) ", 'a' + i, ranks[i]);
   }
   printf("\n");

   // TODO: Sort output (may need to concurrently sort two arrays)
   // array1 = ranks
   // array2 = node connected to given rank
   // example:
   //   [0.03, 0.0385, 0.054862, 0.76633, 0.054862]
   //   ["a", "b", "c", "d", "e"]
   //   sorts into
   //   [0.76633, 0.054862, 0.054862, 0.0385, 0.03]
   //   ["d", "c", "e", "b", "a"]

   // free pointers
   for (i = 0; i < numVerts; i++) {
      free(g[i]);
   }
   free(g);
   free(ranks);

   return 0;
}

/** 
 * Calculates Pagerank for a given graph and releated outDegrees
 */
float *runPageRankE(int **graph, int *outDegrees, int numVerts) {
   int i, j, ittrCount = 0;
   float *oldRanks = calloc(sizeof(float), numVerts), 
    *newRanks = calloc(sizeof(float), numVerts), 
    diff = 0, ep = 0.000001;

   // Basecase (n)
   for (i = 0; i < numVerts; i++) {
      newRanks[i] = (float) 1 / numVerts;
      diff += newRanks[i];
   }
   
   // Ittrate until difference is less than ep
   while (diff >= ep) {
      ittrCount++;
      memcpy(oldRanks, newRanks, sizeof(float) * numVerts);
      diff = 0;
      
      // For each node in graph, calculate pagerank (n*n)
      for (i = 0; i < numVerts; i++) {
         newRanks[i] = calcNodeRank(graph, oldRanks, outDegrees, numVerts, i);
         diff += fabsf(oldRanks[i] - newRanks[i]);
      }

      printf("\n%d. diff: %f\n", ittrCount, diff);
   }
   
   free(oldRanks);

   return newRanks;
}

/**
 * Calculates Page rank for given node N.
 */
float calcNodeRank(int **graph, float *oldRanks, int *outDegrees, int numVerts, int n) {
   int i, j, outCount;
   float sum = 0;
   
#ifdef _OPENMP
   #pragma omp parallel 
#endif
   {
   #ifdef _OPENMP
      #pragma omp single
      printf("%d threads running in parallel\n", omp_get_num_threads());
      #pragma omp for reduction(+:sum)
   #endif
   // while there are still in nodes, add to sum
   for (i = 0; graph[n][i] != -1; i++) {
         sum += ((float) oldRanks[graph[n][i]]) / ((float) outDegrees[graph[n][i]]);
   }
   }
   return (1 - D_VAL) / numVerts + D_VAL * sum;
}
