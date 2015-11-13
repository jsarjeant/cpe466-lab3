#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include "pagerank.h"
#include "uthash.h"

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

void addEdge(char *** adjList, char *to, char *from, char ** nodesMap, int * adjListCounts) {
    int index = find(to, nodesMap, numNodes, 0);
    //printf("Index: %d\n", index);
    int innerIndex = 0;
    if (adjList[index] == NULL) {
        //printf("Null at index %d\n", index);
        adjList[index] = malloc(20 * sizeof(char*));
        //printf("Malloc Complete\n");
        //printf("adjListCounts[%d] = %d\n", index, adjListCounts[index]);
        adjListCounts[index] += 20;
        //printf("adjListCounts[%d] = %d\n", index, adjListCounts[index]);
    }    
    
    while(innerIndex < adjListCounts[index]) {
       // printf("AdjList[%d] = %s\n",index, adjList[index]);
        if (adjList[index][innerIndex] == NULL) {
         //   printf("adjList[%d][%d] is NULL\n", index, innerIndex);
            adjList[index][innerIndex] = malloc(sizeof(MAX_VAL));
            strcpy(adjList[index][innerIndex], from);
            printf("%s -> %s\n", to, adjList[index][innerIndex]);
            return;
        }
        ++innerIndex;
    }
    adjListCounts[index] += 20;
    void * tmp = realloc(adjList[index], adjListCounts[index] * sizeof(char *));
    adjList[index][innerIndex] = malloc(sizeof(MAX_VAL));
    strcpy(adjList[index][innerIndex], from);
    printf("%s -> %s\n", to, adjList[index][innerIndex]);
}

void createAdjList(char * fileName, char *** adjList, char ** nodesMap, int * adjListCounts) {
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
            printf("%s -> %s\n", to, from);
            addEdge(adjList, to, from, nodesMap, adjListCounts);
            getchar();
        }
    }
}

void printAdjList(char *** adjList, char ** nodesMap, int *adjListCounts) {
    int i = 0, ii = 0;

    for (i = 0; i < numNodes; i++) {
        char * key = nodesMap[i];
        printf("%s: [",key);
        while (adjList[i][ii] != NULL && ii < adjListCounts[i]) {
            printf("%s, ", adjList[i][ii++]);
        }
        printf("]\n");
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

    printf("Number of Nodes: %d\n", numNodes); 

    char *** adjList = malloc(numNodes * sizeof(char **));
    int * adjListCounts = calloc(numNodes, sizeof(int));
    createAdjList(argv[1], adjList, nodesMap, adjListCounts);
    printAdjList(adjList, nodesMap, adjListCounts);
    /**char *arr[4]
    int curIndex = 0;
    if (find("a", arr, curIndex, 4) == -1) { curIndex++; }
    printf("Array Init\n");
    printf("a - %d\n", find("a", arr, curIndex, 4));
    printf("c - %d\n", find("c", arr, curIndex, 4));
    printf("c - %d\n", find("c", arr, curIndex, 4));**/
}
