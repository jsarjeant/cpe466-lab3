#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "cuda_pagerank.h"
#include "uthash.h"
#include "cuda_runtime.h"
#define BLOCKSIZE 512
#define MICROSEC_CONV 1000000.0

__global__ void testKernel(int* list, int* listidx, int* outDegrees, int numNodes, float* cudaOldRanks, float* cudaNewRanks);

__global__ void testKernel(int* list, int* listidx, int* outDegrees, int numNodes, float* cudaOldRanks, float* cudaNewRanks) {
   int threadNum = blockIdx.x * blockDim.x + threadIdx.x;
   int i = 0, sum = 0, length = list[listidx[threadNum]];
   int* curIdx = &list[listidx[threadNum]] + 1;

   for(; i < length; i++) {
      sum += (cudaOldRanks[*(curIdx+i)]/(float)outDegrees[*(curIdx+i)]); 
   }
}

static double sys_time() {
   struct timeval time;
   gettimeofday(&time, NULL);
   return time.tv_sec + (time.tv_usec / MICROSEC_CONV);

}

int fromIndex, toIndex;
char arg;
char *format = NULL;

int MAX_VAL = 4847571; 
int numNodes = 0;

struct node {
    char *key;
    int index;
    UT_hash_handle hh;
};

void processArg(char argument) {
    arg = argument;
   
    if (arg == 't' || arg == 'n') {
        fromIndex = 0;
        toIndex = 2;
        format = "%[^','],%*[^','],%[^','],%*[^',']"; 
    }
    else {
        fromIndex = 0;
        toIndex = 1;
        format = "%s %s";
    }
}

int find (char * element, struct node **nodesMap, char ** array, int currentIndex, int arrayLength) {
    struct node *n;
    HASH_FIND_STR(*nodesMap, element, n);
    char * key;

    if (!n) {
        n = (struct node*) malloc(sizeof(struct node));
        //printf("Finding %s\n", element);
        int keyLen = strlen(element);
        key = (char*)malloc(keyLen + 1);
        strcpy(key, element);
        n->key = key;
        n->index = currentIndex;
        HASH_ADD_KEYPTR(hh, *nodesMap, n->key, keyLen, n);
        
        array[currentIndex] = (char*)malloc(keyLen + 1);
        strcpy(array[currentIndex], element);
        
        numNodes++;
        /*struct node *tmp;
        HASH_ITER(hh, *nodesMap, n, tmp) {
            printf("Key: %s\n", n->key);
        }
        getchar();*/
        return -1;
    }
    return n->index;
}

char ** increaseNodesSpace(char ** nodes, int newSize) {
    void * tmp = realloc(nodes, newSize * sizeof(char *));
    if (tmp != NULL) {
        nodes = (char**)tmp;
        return nodes;
    }
    else {
        printf("Error in realloc\n");
        free(nodes);
        perror("Realloc Failed");
        exit(EXIT_FAILURE);
    }
}


char ** calcNumNodes(char * fileName, struct node **nodesMap, char ** nodes, int baseLength) {
    FILE *fp = fopen(fileName, "r");
    char line[21];
    int nodesLength = baseLength;
    
    int commentCount = 0, currentIndex = 0;
    char from[10], to[10];

    while (fgets(line, 21, fp) != NULL) {
        if (line[0] == '#') {
            commentCount++;
            while (fgets(line, 21, fp) != NULL && commentCount < 4) {
                if (line[0] == '#') {
                    commentCount++;
                }
            } 
        }
        else {
            sscanf(line, format, &from, &to);
            if (find(from, nodesMap, nodes, currentIndex, nodesLength) == -1) { 
                if (++currentIndex >= nodesLength) {
                    nodesLength += baseLength;
                    nodes = increaseNodesSpace(nodes, nodesLength);  
                } 
            }
            if (find(to, nodesMap, nodes, currentIndex, nodesLength) == -1) {
                if (++currentIndex >= nodesLength) {
                    nodesLength += baseLength;
                    nodes = increaseNodesSpace(nodes, nodesLength);
                }
            }
        }
    }
    
    return nodes;
}

void printNodesMap(char ** nodesMap) {
    int i = 0;
    printf("Printing nodes map: %d\n", numNodes);
    printf("[");
    for (i = 0; i < numNodes; i++) {
        printf("%s,", nodesMap[i]);
    }
    printf("]\n");
}

void printAdjList(int ** adjList, char ** nodesMap, int * outDegrees) {
    int i = 0, ii = 0;
    printf("Printing Adjacency List\n");
    for (i = 0; i < numNodes; i++) {
        ii = 0;
        char * key = nodesMap[i];
        printf("%s",key);
        if (adjList[i] != NULL) {
            //printf("%d is not null\n", i);
            //printf("adjList[%d][%d] = %d\n", i, ii, adjList[i][ii]);
            printf("(%d): [", adjList[i][0]); 
            for (ii = 1; ii <= adjList[i][0]; ii++) {
               //printf("%d: ", ii); 
               //printf("adjList[%d][%d] = %d", i, ii, adjList[i][ii]);
               printf("%s, ", nodesMap[adjList[i][ii]]);
            }
        }
        else {
            printf("*******ERROR*********\n");
            getchar();
        }
        printf("]\t%d\n", outDegrees[i]);
    }
}


void addEdge(int ** adjList, char *to, char *from, struct node ** nodesMap, char ** nodeKeys, int * adjListCounts, int * outDegrees) {
    int index = find(to, nodesMap, nodeKeys, numNodes, 0);
    //printf("Index of %s: %d\n", to, index);    
    if (adjList[index] == NULL) {
    //    printf("Is Null\n");
        adjList[index] = (int*)calloc(20, sizeof(int));
        adjListCounts[index] = 1;
    }

    int innerIndex = adjListCounts[index];
    //printf("Inner Index: %d\n", innerIndex);
    int fromIndex = find(from, nodesMap, nodeKeys, numNodes, 0);
    adjList[index][innerIndex++] = fromIndex;
    //printf("adjList[%d][%d] = %d(%d)\n", index, innerIndex - 1, adjList[index][innerIndex - 1], fromIndex); 
    outDegrees[fromIndex]++;
             
    adjListCounts[index] = innerIndex;
    if (innerIndex % 20) {
        //printf("%d % 20 != 0\n", innerIndex);
        //getchar();
        return;
    }

    void * tmp = realloc(adjList[index], (adjListCounts[index] + 20) * sizeof(int));
    if (tmp != NULL) {
        adjList[index] = (int*)tmp;
        return;
    }
    else {
        printf("Error in realloc\n");
        free(adjList[index]);
        perror("Realloc Failed");
        exit(EXIT_FAILURE);
    }
    getchar();
}

void createAdjList(char * fileName, int ** adjList, struct node ** nodesMap, char ** nodeKeys, int * adjListCounts, int * outDegrees) {
    FILE *fp = fopen(fileName, "r");
    char line[21];
    int commentCount = 0;
    char to[10], from[10];

    while (fgets(line, 21, fp) != NULL) {
        if (line[0] == '#') {
            commentCount++;
            while (fgets(line, 21, fp) != NULL && commentCount < 4) {
                if (line[0] == '#') {
                    commentCount++;
                }
            } 
        }
        else {
            sscanf(line, format, &from, &to);
 //           printf("%s -> %s\n", to, from);
            addEdge(adjList, to, from, nodesMap, nodeKeys, adjListCounts, outDegrees);
//            getchar();
        }
    }
}
 
int completeAdjList(int ** adjList, int * adjListCounts) {
    int i = 0;
    int total = 0;
    for (; i < numNodes; i++) {
        if (adjList[i] == NULL) {
            adjList[i] = (int*)calloc(1, sizeof(int));
            adjListCounts[i] = 1;
        }
        adjList[i][0] = adjListCounts[i] - 1;
        total += adjListCounts[i];
    }
    return total;
}

void populateList(int *list, int *listIndexes, int **adjList) {
   int i = 0, currentIndex = 0, length;
   for (; i < numNodes; i++) {
      listIndexes[i] = currentIndex;
      length = adjList[i][0] + 1;
      memcpy(list + currentIndex, adjList[i], length * sizeof(int));
      currentIndex += length;
   }
}

void printList(int *list, int * listIndexes, int total, char ** nodeKeys) {
  int i = 0;
  printf("[");
  for (; i < total; i++) {
   printf("%s, ", nodeKeys[list[i]]);
  }
  printf("]\n");

  printf("[");
  for (i = 0; i < numNodes; i++) {
    printf("%d, ", listIndexes[i]);
  }
  printf("]\n");
}

int main (int argc, char *argv[]) {
    if (argv[1] != NULL && argv[2] != NULL) {
        processArg(argv[2][1]);
    }
    else {
        perror("Error: Usage requires a file name and flag inidicating the type of file.\nFlag\tFile Format\n---------------------\n-n\t1,0,2,0\n-t\t\"One\",0,\"Two\",0\n-s\t1\t2\n");
        exit(EXIT_FAILURE);
    }
    printf("Reading...\n");
    double start, end;
    int i, j;
    start = sys_time();
    char ** nodeKeys = (char**)malloc(20 * sizeof(char*));
    struct node **nodesMap = (struct node **)calloc(1, sizeof(struct node *));
    nodeKeys = calcNumNodes(argv[1], nodesMap, nodeKeys, 20);
    end = sys_time();
    //printf("Creating node map: %f\n", end-start);
    
    //start = sys_time(); 
    //while (nodesMap[++numNodes] != NULL); 
    //printNodesMap(nodeKeys);
    //printf("Number of Nodes: %d\n", numNodes); 
    //getchar();
    int ** adjList = (int**)malloc(numNodes * sizeof(int *));
    int * adjListCounts = (int*)calloc(numNodes, sizeof(int));
    int * outDegrees = (int*)calloc(numNodes, sizeof(int));
    createAdjList(argv[1], adjList, nodesMap, nodeKeys, adjListCounts, outDegrees);
    //getchar();
    int total = completeAdjList(adjList, adjListCounts);
    int *list = (int *)malloc(total * sizeof(int));
    int *listIndexes = (int *)malloc(numNodes*sizeof(int));
    
    populateList(list, listIndexes, adjList);
    //printAdjList(adjList, nodeKeys, outDegrees);
    //printList(list, listIndexes, total, nodeKeys);
    //getchar();
    end = sys_time();
    printf("Read Time: %f\n", end-start);
     
    int* cudaOutDegrees;
    int* cudaList;
    int* cudaListIndexes;
    cudaError_t error;
    float* newRanks, *oldRanks, *cudaOldRanks, *cudaNewRanks;
    float diff = 0, sum = 0;

    oldRanks = (float*)malloc(numNodes*sizeof(float));
    newRanks = (float*)malloc(numNodes*sizeof(float));
   
    for (i = 0; i < numNodes; i++) {
       newRanks[i] = (float) 1 / numNodes;
       diff += newRanks[i];
    } 

    int numBlocks = (int) ceil((double) numNodes/(BLOCKSIZE));
    printf("blocks is %d\n", numBlocks);

    dim3 dimBlock(BLOCKSIZE, 1);
    dim3 dimGrid(numBlocks, 1);

    error = cudaMalloc((void**)&cudaOutDegrees, sizeof(int)*numNodes);
    if(error == cudaSuccess)
      printf("cudaOutDegrees malloc successful!\n");
    error = cudaMemcpy(cudaOutDegrees, outDegrees, numNodes*sizeof(int), cudaMemcpyHostToDevice); 

    error = cudaMalloc((void**)&cudaList, sizeof(int)*total);
    if(error == cudaSuccess)
      printf("cudaList malloc successful!\n");
    error = cudaMemcpy(cudaList, list, total*sizeof(int), cudaMemcpyHostToDevice); 

    error = cudaMalloc((void**)&cudaListIndexes, sizeof(int)*numNodes);
    if(error == cudaSuccess)
      printf("cudaListIndexes malloc successful!\n");
    error = cudaMemcpy(cudaListIndexes, listIndexes, numNodes*sizeof(int), cudaMemcpyHostToDevice); 

    error = cudaMalloc((void**)&cudaOldRanks, sizeof(float)*numNodes);
    if(error == cudaSuccess)
      printf("cudaListIndexes malloc successful!\n");

    error = cudaMalloc((void**)&cudaNewRanks, sizeof(float)*numNodes);
    if(error == cudaSuccess)
      printf("cudaranks malloc successful!\n");

    printf("Starting pagerank calculations...\n");
    start = sys_time(); 
    while(diff >= EPSILON) {
      diff = 0;
      memcpy(oldRanks, newRanks, sizeof(float) * numNodes);
      cudaMemcpy(cudaOldRanks, oldRanks, numNodes*sizeof(int), cudaMemcpyHostToDevice); 
      testKernel<<<dimGrid, dimBlock>>>(cudaList, cudaListIndexes, cudaOutDegrees, numNodes, cudaOldRanks, cudaNewRanks);
      cudaMemcpy(newRanks, cudaNewRanks, numNodes*sizeof(float), cudaMemcpyDeviceToHost);
         
      for (i = 0; i < numNodes; i++) {
         sum = 0; 
         for (j = 1; j <= adjList[i][0]; j++) {
            sum += ((float) oldRanks[adjList[i][j]]) / ((float) outDegrees[adjList[i][j]]);
         }  
         
         newRanks[i] = (1 - D_VAL) / numNodes + D_VAL * sum;
         diff += fabsf(newRanks[i] - oldRanks[i]); 
      }  

    }

    //float * pageRanks = runPageRankE(adjList, outDegrees, numNodes);
    float * pageRanks = newRanks;
    end = sys_time();
    printf("Running Calculations: %f\n", end-start); 
    start = sys_time(); 
    runMergeSort(pageRanks, nodeKeys, numNodes);
    //end = sys_time();
    //printf("Sorting rankings: %f\n", end-start);
    //getchar();
    for (i = 0; i < 10; i++) {
        printf("%s\t%1.12f\n", nodeKeys[i], pageRanks[i]);
    }

    free(pageRanks);
}
