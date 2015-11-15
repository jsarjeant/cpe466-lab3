#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include "new_pagerank.h"

int fromIndex, toIndex;
char arg;
char *format = NULL;
int MAX_VAL = 4847571; 
int numNodes;

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

int find (char * element, char ** array, int currentIndex, int arrayLength) {
    int i = 0;
    while (i < currentIndex) {
        if (strcmp(element, array[i]) == 0) {
            return i;
        }
        ++i;
    }

    array[currentIndex] = malloc(20);
    strcpy(array[currentIndex], element);
    return -1;
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


char** calcNumNodes(char * fileName, char ** nodes, int baseLength) {
    FILE *fp = fopen(fileName, "r");
    char line[21];
    int nodesLength = baseLength;
    
    int numNodes = 0, commentCount = 0, currentIndex = 0;
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
            if (find(from, nodes, currentIndex, nodesLength) == -1) { 
                if (++currentIndex >= nodesLength) {
                    nodesLength += baseLength;
                    nodes = increaseNodesSpace(nodes, nodesLength);  
                } 
            }
            if (find(to, nodes, currentIndex, nodesLength) == -1) {
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

    for (i = 0; i < numNodes; i++) {
        ii = 0;
        char * key = nodesMap[i];
        printf("%s: [",key);
        if (adjList[i] != NULL) {
            //printf("%d is not null\n", i);
            //printf("adjList[%d][%d] = %d\n", i, ii, adjList[i][ii]);
            while (adjList[i][ii] != -1) {
                //printf("%d: ", ii);
                printf("%s, ", nodesMap[adjList[i][ii++]]);
            }
            printf("%d", adjList[i][ii]);
        }
        printf("]\t%d\n", outDegrees[i]);
    }
}


void addEdge(int ** adjList, char *to, char *from, char ** nodesMap, int * adjListCounts, int * outDegrees) {
    int index = find(to, nodesMap, numNodes, 0);
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
            int fromIndex = find(from, nodesMap, numNodes, 0);
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
//    printf("%s -> %s\n", to, adjList[index][innerIndex]);
}

void createAdjList(char * fileName, int ** adjList, char ** nodesMap, int * adjListCounts, int * outDegrees) {
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
            addEdge(adjList, to, from, nodesMap, adjListCounts, outDegrees);
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

    char ** nodesMap = malloc(20 * sizeof(char*));
    nodesMap = calcNumNodes(argv[1], nodesMap, 20);
    numNodes = -1;
    while (nodesMap[++numNodes] != NULL); 
    ///printNodesMap(nodesMap);
    printf("Number of Nodes: %d\n", numNodes); 

    int ** adjList = malloc(numNodes * sizeof(int *));
    int * adjListCounts = calloc(numNodes, sizeof(int));
    int * outDegrees = calloc(numNodes, sizeof(int));
    createAdjList(argv[1], adjList, nodesMap, adjListCounts, outDegrees);
    //printAdjList(adjList, nodesMap, outDegrees);
    
    int i = 0;
    for (i = 0; i < numNodes; i++) {
        if (adjList[i] == NULL) {
            adjList[i] = malloc(sizeof(int));
            adjList[i][0] = -1;
        }
    }
    printf("Starting pagerank calculations\n");
    float * pageRanks = runPageRankE(adjList, outDegrees, numNodes);
    for (i = 0; i < numNodes; i++) {
        printf("%s\t%1.12f\n", nodesMap[i], pageRanks[i]);
    }
    free(pageRanks);
    /**char *arr[4]
    int curIndex = 0;
    if (find("a", arr, curIndex, 4) == -1) { curIndex++; }
    printf("Array Init\n");
    printf("a - %d\n", find("a", arr, curIndex, 4));
    printf("c - %d\n", find("c", arr, curIndex, 4));
    printf("c - %d\n", find("c", arr, curIndex, 4));**/
}
