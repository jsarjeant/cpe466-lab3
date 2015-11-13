#include "uthash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
   char *name;
   int index;
   UT_hash_handle hh;
};

int main() {
   FILE *fp = fopen("karate.csv", "r");
   char *name, **l, *line[] = {"joe", "bob", "betty", "bob", NULL};   
   struct node *n, *tmp, *nodes = NULL;
   int count = 0, nameLen = 0;

   for (l = line; *l != NULL; l++) {
      nameLen = strlen(*l);
      name = (char *)calloc(nameLen + 1, sizeof(char));
      strcpy(name, *l);
      HASH_FIND_STR(nodes, name, n);
      if (!n) {
         n = (struct node*)malloc(sizeof(struct node));
         n->name = *l;
         n->index = count++;
         HASH_ADD_KEYPTR(hh, nodes, n->name, nameLen, n);
      } 
   }

   HASH_ITER(hh, nodes, n, tmp) {
      printf("Name: %s\n", n->name);
   }
   HASH_FIND_STR(nodes, "bob", n);
   printf("%d\n", n->index);
   printf("%d\n", count);
   return 0;
}

