
#ifndef __BUDDY_H
#define __BUDDY_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0

#define BUDDY_POOL_SIZE (1 << 29)
#define MINIMUM_BLOCK_SIZE (1 << 5)

typedef int Boolean;

union block_header {
  struct block_info {
    int kval;
    int tag;
    union block_header *next;
    union block_header *prev;
  }data;
  double align;
};

void buddy_init(size_t);
void* buddy_calloc(size_t, size_t);
void* buddy_malloc(size_t);
void* buddy_realloc(void *ptr, size_t size);
void buddy_free(void *);
void merge_r(union block_header*);
void printBuddyLists();
void printBuddyNode(int);

#endif /* __BUDDY_H */
