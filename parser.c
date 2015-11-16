#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <time.h>
#include "new_pagerank.h"
#include "uthash.h"

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
        key = malloc(keyLen + 1);
        strcpy(key, element);
        n->key = key;
        n->index = currentIndex;
        HASH_ADD_KEYPTR(hh, *nodesMap, n->key, keyLen, n);
        
        array[currentIndex] = malloc(keyLen + 1);
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
        nodes = tmp;
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
    void * tmp;

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
        printf("%s: [",key);
        if (adjList[i] != NULL) {
            //printf("%d is not null\n", i);
            //printf("adjList[%d][%d] = %d\n", i, ii, adjList[i][ii]);
            while (adjList[i][ii] != -1) {
                //printf("%d: ", ii); 
                printf("adjList[%d][%d] = %d", i, ii, adjList[i][ii]);
                printf(" - %s, \n", nodesMap[adjList[i][ii++]]);
            }
            printf("%d", adjList[i][ii]);
        }
        printf("]\t%d\n", outDegrees[i]);
    }
}


void addEdge(int ** adjList, char *to, char *from, struct node ** nodesMap, char ** nodeKeys, int * adjListCounts, int * outDegrees) {
    int index = find(to, nodesMap, nodeKeys, numNodes, 0);
    //printf("Index: %d\n", index);
    int innerIndex = 0;
    if (adjList[index] == NULL) {
        //printf("Null at index %d\n", index);
        adjList[index] = calloc(20, sizeof(int));
        adjList[index][0] = -1; 
        //printf("Calloc Complete\n");
       
        //printf("adjListCounts[%d] = %d\n", index, adjListCounts[index]);
        adjListCounts[index] += 20;

        //printf("adjListCounts[%d] = %d\n", index, adjListCounts[index]);
    }    
    
    while(innerIndex < adjListCounts[index]) {
        if (adjList[index][innerIndex] == -1) {
            //printf("Index %d is %d with length %d\n", innerIndex, adjList[index][innerIndex], adjListCounts[index]);
            int fromIndex = find(from, nodesMap, nodeKeys, numNodes, 0);
            adjList[index][innerIndex] = fromIndex;
            outDegrees[fromIndex]++;
            
            //printf("Index %d is %d with length %d\n", innerIndex, adjList[index][innerIndex], adjListCounts[index]);
            if (innerIndex + 1 != adjListCounts[index]) {
                adjList[index][innerIndex + 1] = -1;
                //printf("Index %d is %d with length %d\n", innerIndex + 1 , adjList[index][innerIndex + 1], adjListCounts[index]);
                //printAdjList(adjList, nodesMap);
                return;
            }
        }
        ++innerIndex;
    }
    adjListCounts[index] += 20;
    void * tmp = realloc(adjList[index], adjListCounts[index] * sizeof(int));
    if (tmp != NULL) {
        adjList[index] = tmp;
        adjList[index][innerIndex] = -1;
        return;
    }
    else {
        printf("Error in realloc\n");
        free(adjList[index]);
        perror("Realloc Failed");
        exit(EXIT_FAILURE);
    }

//    printf("%s -> %s\n", to, adjList[index][innerIndex]);
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
//            printf("%s -> %s\n", to, from);
            addEdge(adjList, to, from, nodesMap, nodeKeys, adjListCounts, outDegrees);
//            getchar();
        }
    }
}


int main (int argc, char *argv[]) {
    if (argv[1] != NULL && argv[2] != NULL) {
        processArg(argv[2][1]);
    }
    else {
        perror("Error: Usage requires a file name and flag inidicating the type of file.\nFlag\tFile Format\n---------------------\n-n\t1,0,2,0\n-t\t\"One\",0,\"Two\",0\n-s\t1\t2\n");
        exit(EXIT_FAILURE);
    }
    time_t startTime, nextTime;
    time(&startTime);
    char ** nodeKeys = malloc(20 * sizeof(char*));
    struct node **nodesMap = calloc(1, sizeof(struct node *));
    nodeKeys = calcNumNodes(argv[1], nodesMap, nodeKeys, 20);
    time(&nextTime);
    printf("Creating node map: %f\n", difftime(nextTime, startTime));
    time(&startTime);
    
    //while (nodesMap[++numNodes] != NULL); 
    //printNodesMap(nodeKeys);
    printf("Number of Nodes: %d\n", numNodes); 
    //getchar();
    int ** adjList = malloc(numNodes * sizeof(int *));
    int * adjListCounts = calloc(numNodes, sizeof(int));
    int * outDegrees = calloc(numNodes, sizeof(int));
    createAdjList(argv[1], adjList, nodesMap, nodeKeys, adjListCounts, outDegrees);
    //printAdjList(adjList, nodesMap, outDegrees);
    //getchar();
    int i = 0;
    for (i = 0; i < numNodes; i++) {
        if (adjList[i] == NULL) {
            adjList[i] = malloc(sizeof(int));
            adjList[i][0] = -1;
        }
    }
    time(&nextTime);
    printf("Creating Adjacency List: %f\n", difftime(nextTime, startTime));
    time(&startTime);
    printf("Starting pagerank calculations\n");
    float * pageRanks = runPageRankE(adjList, outDegrees, numNodes);
    time(&nextTime);
    printf("Running Calculations: %f\n", difftime(nextTime, startTime)); 
    time(&startTime);
    
    runMergeSort(pageRanks, nodeKeys, numNodes);
    time(&nextTime);
    printf("Sorting results: %f\n", difftime(nextTime, startTime));
    //getchar();
    for (i = 0; i < 10; i++) {
        printf("%s\t%1.12f\n", nodeKeys[i], pageRanks[i]);
    }
    time(&nextTime);
    _mm_free(pageRanks);
    /**char *arr[4]
    int curIndex = 0;
    if (find("a", arr, curIndex, 4) == -1) { curIndex++; }
    printf("Array Init\n");
    printf("a - %d\n", find("a", arr, curIndex, 4));
    printf("c - %d\n", find("c", arr, curIndex, 4));
    printf("c - %d\n", find("c", arr, curIndex, 4));**/
}
