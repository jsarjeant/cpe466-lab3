#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pagerank.h"

int calcNumNodes(char * fileName) {
    FILE *fp = fopen(fileName, "r");
    char line[20];
    char *split;
    int line_vals[4];
    int from, i, j = 0;
    int numNodes, numEdges;
    int commentCount = 0;
    char comment;
    char * node, * edge;
    int base_length = 20, nodes_length = 20;
    int *nodes = calloc(base_length, sizeof(int));
    void *tmp;    

    while (fgets(line, 20, fp) != NULL) {      
        i = 0;
        if (line[0] == '#') {          
            if (commentCount++ == 2) {
                sscanf(line, "%c %s %d %s %d",&comment, &node, &numNodes, &edge, &numEdges);
                return numNodes;
            }
            continue;
        }
        else {
            split = strtok(line, ",");      
            while (split != NULL) {                
                line_vals[i++] = atoi(split);
                split = strtok(NULL, ",");
            }
            from = line_vals[0];           
            while (from > nodes_length) {
                printf("Trying to insert %d in %d. Increasing by %d\n", from, nodes_length, base_length);
                tmp = realloc(nodes, (nodes_length + base_length) * sizeof(int));
                if (tmp != NULL) {
                    nodes = tmp;
                }
                else {
                    free(nodes);
                    perror("Realloc Failed");
                }
                for (j = nodes_length; j < nodes_length + base_length; j++) {
                    nodes[j] = 0;
                }
                nodes_length += base_length;
            }            
            nodes[from] = nodes[from] + 1;
            /**printf("From: %d (%d)\tTo: %d\t--%d\n", from, nodes[from], line_vals[2], &nodes);**/
        }
    }
    j = nodes_length;
    printf("Nodes Length: %d\n", nodes_length);
    for (i = 0; i < nodes_length; i++) {
        printf("%d - %d\n", i, nodes[i]); 
    }
    while(nodes[--j] == 0); 
    printf("Nodes: %d\n", j);

    return j;
}

int main (int argc, char *argv[]) {
    int text = 0;
    if (argv[2] != NULL) {
        text = 1;
    }
    int numNodes = calcNumNodes(argv[1]);
    printf("numNodes: %d\n", numNodes);
    return 1;
    int i = 0;
    int nf, nt;
    char **adjMatrix = calloc(numNodes, sizeof(char *));
    char line[20];
    char *split;
    int line_vals[4];
    float * ranks;    
    for (i = 0; i < numNodes; i++) {
        adjMatrix[i] = calloc(numNodes, sizeof(char));   
    }
    printf("Matrix initialized\n");
    FILE *fp = fopen(argv[1], "r");
    while (fgets(line, 20, fp) != NULL) {      
        i = 0;
        if (line[0] == '#') {
            continue;
        }
        split = strtok(line, ",");
        while (split != NULL) {
            line_vals[i++] = atoi(split);
            split = strtok(NULL, ",");
        }
        adjMatrix[line_vals[nt] - 1][line_vals[nf] - 1] = 1;
    }
    printf("Matrix populated\n");
    /**for (i = 0; i < numNodes; i++) {
        for (j = 0; j < numNodes; j++) {
            printf("%d,", adjMatrix[i][j]);
        }
        printf("\n");
    }**/
    printf("Starting pageRank Calculation");
    ranks = runPageRankE(adjMatrix, numNodes);
    printf("Ranks: \n");
    for (i = 0; i < numNodes; i++) {
        printf("%d\t%0.13f\n", i + 1, ranks[i]);
        free(adjMatrix[i]);
    }
    free(adjMatrix);
    free(ranks);
    printf("\n");
    return 1;
}

