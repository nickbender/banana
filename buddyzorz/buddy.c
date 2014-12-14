

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include "buddy.h"

static void *pool = NULL;
static Boolean init = FALSE;
static union block_header *buddy_headers[30];

void * malloc(size_t size) {
  return buddy_malloc(size);
}

void * realloc(void *ptr, size_t size) {
  return buddy_realloc(ptr, size);
}

void * calloc(size_t nitems, size_t item_size) {
  return buddy_calloc(nitems, item_size);
}

void free(void *ptr) {
  buddy_free(ptr);
}

void buddy_init(size_t size){
  pool = sbrk(BUDDY_POOL_SIZE);
  if (pool < 0 || errno == ENOMEM) {
    perror("Could not allocate memory pool!");
    exit(1);
  }

  int i;
  for (i = 0; i < 29; i++){
    buddy_headers[i] = NULL;
  }
  buddy_headers[29] = pool;
  buddy_headers[29]->data.kval = 29;
  buddy_headers[29]->data.tag = TRUE;
  init = TRUE;
}

void* buddy_calloc(size_t items, size_t size){
  if (!init){
    buddy_init(0);
  }
  void * mem_block = buddy_malloc(items * size);

  if (mem_block) {
    memset(mem_block, 0, items * size);
    return mem_block;
  } else {
    return NULL;
  }

  return NULL;
}

void *buddy_malloc(size_t size) {
  if (!init) {
    buddy_init(0);
  }

  int required_block_size = size + sizeof(union block_header);

  int i;
  for (i = 5; i <= 29; i++) {
    // Potential fit.
    if (required_block_size < (1<<i)) {
      // Is there a block available?
      if (buddy_headers[i] != NULL && buddy_headers[i]->data.tag == TRUE) {
        // Iterate over the buddy_headers starting at i, splitting as we need.
        int j;
        for (j = i; j > 4; j--) {
          // Begin splitting until we find the correct size of block.
          if (required_block_size*2 < (1 << buddy_headers[j]->data.kval)) {
            buddy_headers[j-1] = buddy_headers[j];

            if ((buddy_headers[j] = buddy_headers[j]->data.next)) {
              buddy_headers[j]->data.prev = NULL;
            } else {
              buddy_headers[j] = NULL;
            }

            buddy_headers[j-1]->data.kval = j-1;
            buddy_headers[j-1]->data.tag = TRUE;
            buddy_headers[j-1]->data.prev = NULL;
            buddy_headers[j-1]->data.next = (union block_header*)((char*)buddy_headers[j-1] + (1 << (j-1)));

            buddy_headers[j-1]->data.next->data.kval = j-1;
            buddy_headers[j-1]->data.next->data.tag = TRUE;
            buddy_headers[j-1]->data.next->data.prev = buddy_headers[j-1];
            buddy_headers[j-1]->data.next->data.next = NULL;
          } else {
            // We found the block, remove and return.
            void* address = (union block_header*)((char*)buddy_headers[j] + sizeof(union block_header));

            buddy_headers[j]->data.tag = FALSE;
            buddy_headers[j] = buddy_headers[j]->data.prev;
            if (buddy_headers[j]) {
              buddy_headers[j]->data.next = NULL;
            }

            return address;
          }
        }
      } else {
        // No available block here, move on to the next.
        continue;
      }
      //Larger memory than we can allocate.
      errno = ENOMEM;
      return NULL;
    }
  }

  return NULL;
}

void* buddy_realloc(void *ptr, size_t size){
  if (!init){
    buddy_init(0);
  }

  if (!ptr) {
    return buddy_malloc(size);
  }

  union block_header* current_block = (union block_header*)(ptr - sizeof(union block_header));

  if (current_block && size > 0) {
    int kval = current_block->data.kval;


    void * new_block = buddy_malloc(size);

    if(new_block) {
      memcpy(new_block, ptr, (1 << kval) - sizeof(union block_header));
      buddy_free(ptr);
      return new_block;
    }
  }
  return NULL;
}

void buddy_free(void *ptr) {
  if (!init || !ptr){
    return;
  }

  union block_header* block = (ptr - sizeof(union block_header));
  block->data.next = NULL;
  block->data.prev = NULL;
  merge_r(block);

}

void merge_r(union block_header* block){

  union block_header* buddy = (union block_header*)(((size_t) (((void *) block)) - (size_t)pool)^(1<<block->data.kval));
  block->data.tag = 1;
  union block_header* curr = buddy_headers[block->data.kval];

  while(curr){
    if((void *)(((void *)curr) - pool) == (void *) buddy){
      union block_header* joined;

      if(curr->data.next){
        curr->data.next->data.prev = curr->data.prev;
      }

      if(curr->data.prev){
        curr->data.prev->data.next = curr->data.next;
      }
      else {
        buddy_headers[curr->data.kval] = curr->data.next;
      }

      curr->data.prev = NULL;
      curr->data.next = NULL;

      if(block < curr) {
        joined = block;
      } else {
        joined = curr;
      }

      joined->data.kval++;

      return merge_r(joined);
    }
    curr = curr->data.next;
  }

  if(buddy_headers[block->data.kval]){
    buddy_headers[block->data.kval]->data.prev = block;
    block->data.next = buddy_headers[block->data.kval];
  }
  buddy_headers[block->data.kval] = block;

}

void printBuddyLists() {
  if (!init) {
    printf("No pool initialized.\n");
  }

  int i;
  for (i = 0; i <= 29; i++) {
    int j = 0;
    union block_header* curr = buddy_headers[i];
    if (curr == NULL) {
      printf("%d", j);
      printf("[KVAL: %d, ADDR: %d, TAG: %d]\n",
      i,
      -1,
      0);
      continue;
    } else {
      while(curr) {
        printf("%d", j);
        printf("[KVAL: %d, ADDR: %p, TAG: %d, NEXT: %p, PREV: %p]\n",
        curr->data.kval,
        curr,
        curr->data.tag,
        curr->data.next,
        curr->data.prev);

        curr = curr->data.next;
        j++;
      }
    }
  }

  printf("DONE WITH LIST\n");
}

void printBuddyNode(int i) {
  if (!buddy_headers[i]) {
    printf("NULL NODE AT %d\n", i);
    return;
  }
  printf("[KVAL: %d, ADDR: %p, TAG: %d, NEXT: %p, PREV: %p]\n",
  buddy_headers[i]->data.kval,
  buddy_headers[i],
  buddy_headers[i]->data.tag,
  buddy_headers[i]->data.next,
  buddy_headers[i]->data.prev);
}
