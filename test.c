#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[]) {
    FILE *fp = fopen(argv[1], "r");
    char line[20];
    char *split;
    int base_length = 20;
    int nodes_length = 20;
    int *nodes = calloc(base_length, sizeof(int));
    void *tmp;
    int line_vals[4];
    int i = 0;

    int from;
    size_t length = 0;

    while (fgets(line, 20, fp) != NULL) {
        /**printf("%d------------", j++);
	    printf("%s\n", line);**/
        i = 0;
        split = strtok(line, ",");
        while (split != NULL) {
            line_vals[i++] = atoi(split);
            split = strtok(NULL, ",");
        }
        from = line_vals[0];
        while (from > nodes_length) {
            length = (nodes_length + base_length) * sizeof(int);
            printf("Looking for %d in %d\n***Extending array***\nFrom: %lu\tTo: %lu\n", from, nodes_length, nodes_length * sizeof(int), length);
            tmp = realloc(nodes, (nodes_length + base_length) * sizeof(int));
            printf("Done\n");
            if (tmp != NULL) {
                printf("Realloc completed\n");
                nodes = tmp;
            }
            else {
                free(nodes);
            }
            nodes_length += base_length;
        }
        nodes[from] = nodes[from] + 1;

        printf("From: %d (%d)\tTo: %d\t--%d\n", from, nodes[from], line_vals[2], &nodes);
    }
}
