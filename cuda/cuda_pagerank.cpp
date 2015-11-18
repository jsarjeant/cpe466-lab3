#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cuda_pagerank.h"
#include <omp.h>

int askbdfmain() {
   int i, j, tmp, count, numVerts = MATRIX_SIZE, 
    outSum, outDegrees[MATRIX_SIZE];
   float *ranks;
   // Setup Graph[toIndex][fromIndex]
   char graph[MATRIX_SIZE][MATRIX_SIZE] = {{0, 0, 0, 0, 0},  // A: in edges: none
                                           {1, 0, 0, 0, 0},  // B: in-edges: A
                                           {1, 1, 0, 0, 0},  // C: in-edges: A B
                                           {0, 0, 1, 0, 0},  // D: in-edges: C
                                           {1, 1, 0, 0, 0}}; // E: in-edges: A B
   int **g = (int**)malloc(sizeof(int *) * numVerts);

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
      g[i] = (int*)malloc(sizeof(int) * count);
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
extern float *runPageRankE(int **graph, int *outDegrees, int numVerts) {
   int i, j, ittrCount = 0;
   float *oldRanks = (float*)malloc(sizeof(float) * numVerts),
    *newRanks = (float*)malloc(sizeof(float) * numVerts),
    diff = 0, ep = 0.000001, sum = 0;
   
   memset(oldRanks, 0, sizeof(float)*numVerts);
   memset(newRanks, 0, sizeof(float)*numVerts);


   // Basecase (n)
   for (i = 0; i < numVerts; i++) {
      newRanks[i] = (float) 1 / numVerts;
      diff += newRanks[i];
   }
   
   // Ittrate until difference is less than ep
   while (diff >= ep) {
      ittrCount++;
      memcpy(oldRanks, newRanks, sizeof(float) * numVerts);
      diff = sum = 0;
       
      {  
      // For each node in graph, calculate pagerank (n*n)
      for (i = 0; i < numVerts; i++) {
         sum = 0;
         //#pragma simd reduction(+:sum)
         for (j = 1; j <= graph[i][0]; j++) {
            sum += ((float) oldRanks[graph[i][j]]) / ((float) outDegrees[graph[i][j]]);
         }
         
         newRanks[i] = (1 - D_VAL) / numVerts + D_VAL * sum;
         diff += fabsf(newRanks[i] - oldRanks[i]); 
      }
      }

    //  printf("\n%d. diff: %f\n", ittrCount, diff);
   }
   free(oldRanks);

   return newRanks;
}

