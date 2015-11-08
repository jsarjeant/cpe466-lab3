#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pagerank.h"

int main() {
   int i, numVerts = MATRIX_SIZE;
   float *ranks;
   // Setup Graph
   char graph[MATRIX_SIZE][MATRIX_SIZE] = {{0, 1, 1, 0, 1},
                                           {0, 0, 1, 0, 1},
                                           {0, 0, 0, 1, 0},
                                           {0, 0, 0, 0, 0},
                                           {0, 0, 0, 0, 0}};
   char **g = malloc(sizeof(char *) * numVerts);
   for (i = 0; i < numVerts; i++) {
      g[i] = malloc(sizeof(char) * numVerts);
      memcpy(g[i], graph[i], sizeof(char) * numVerts);
   }

   // Calculate ranks
   ranks = runPageRankE(g, numVerts);

   // print out results
   printf("Ranks: ");
   for (i = 0; i < numVerts; i++) {
      printf("(%c %f) ", 'a' + i, ranks[i]);
   }
   printf("\n");

   // free pointers
   for (i = 0; i < numVerts; i++) {
      free(g[i]);
   }
   free(g);
   free(ranks);
   
   return 0;
}

// Calculates Pagerank for a given graph
float *runPageRankE(char **graph, int numVerts) {
   int i, ittrCount = 0;

   float *oldRanks = calloc(sizeof(float), numVerts), *newRanks = calloc(sizeof(float), numVerts), 
    diff = 0, ep = 0.000001;

   // Basecase
   for (i = 0; i < numVerts; i++) {
      newRanks[i] = (float) 1 / numVerts;
      diff += newRanks[i];
   }
   
   // Ittrate until difference is less than ep
   while (diff >= ep) {
      ittrCount++;
      memcpy(oldRanks, newRanks, sizeof(float) * numVerts);
      diff = 0;
      
      // For each node in graph, calculate pagerank
      for (i = 0; i < numVerts; i++) {
         newRanks[i] = calcNodeRank(graph, oldRanks, numVerts, i);
         diff += fabsf(oldRanks[i] - newRanks[i]);
      }

      printf("%d. diff: %f\n", ittrCount, diff);
   }
   
   free(oldRanks);

   return newRanks;
}

float calcNodeRank(char **graph, float *oldRanks, int numVerts, int n) {
   int i, j, outCount;
   float sum = 0;

   for (i = 0; i < numVerts; i++) {
      if (graph[i][n] != 0) {
         outCount = 0;
         // sum up column
         for (j = 0; j < numVerts; j++) {
            outCount += graph[i][j];
         }
         sum += ((float) oldRanks[i]) / ((float) outCount);
      }
   }

   return (1 - D_VAL) / numVerts + D_VAL * sum;
}
