#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

uint16_t PAGE_SIZE = 4096;

typedef struct Block {
  int free;
  size_t payload_size;
  struct Block *next;
} Block;

Block *memory_list = NULL;
long long *heap_start = NULL;
long long *heap_end = NULL;

void *memloc(size_t incr) {
  Block *prev = NULL;
  Block *curr = memory_list;

  while (curr) {
    if (incr <= curr->payload_size && curr->free) {
      curr->free = 0;
      void *addr = curr + 1;
      return addr;
    }
    prev = curr;
    curr = curr->next;
  }

  if (!curr) {
    Block *block;
    if (!heap_start) {
      heap_start = sbrk(0);
      void *ret = sbrk(PAGE_SIZE);
      heap_end = ret + PAGE_SIZE;
      block = (Block *)heap_start;
      block->free = 0;
      block->payload_size = incr;
      memory_list = block;
      return (void *)(block + 1);
    }

    if (prev + 1 >= (Block *)heap_end) {
      void *ret = sbrk(PAGE_SIZE);
      heap_end = ret + PAGE_SIZE;
    }

    block = prev + 1;
    block = (Block *)((void *)block + prev->payload_size);
    block->free = 0;
    block->payload_size = incr;
    prev->next = block;
    return (void *)(block + 1);
  }

  return NULL;
};

void freeloc(void *ptr) {
  if (!ptr) {
    return;
  }
  Block *block_data = (Block *)ptr - 1;
  block_data->free = 1;
  return;
}

void visualize_memory_list(void) {
  Block *block = memory_list;
  while (block != NULL) {
    printf("Block addr: %p\n", block);
    printf("Block free: %d\n", block->free);
    printf("Block payload_size: %zu\n", block->payload_size);
    printf("Block next block addr: %p\n", block->next);
    block = block->next;
  }
}

struct test {
  int bar;
  char foo[25];
  long xyz;
};

int main(void) {
  int *p = memloc(sizeof(int));
  printf("First allocation\n");
  visualize_memory_list();
  printf("\n");

  int *b = memloc(sizeof(int));
  printf("Second allocation\n");
  visualize_memory_list();
  printf("\n");

  freeloc(p);
  printf("First free\n");
  visualize_memory_list();
  printf("\n");

  freeloc(b);
  printf("Second free\n");
  visualize_memory_list();
  printf("\n");

  char *asd = memloc(sizeof(char));
  printf("Third allocation\n");
  visualize_memory_list();
  printf("\n");

  printf("Size of this struct is: %zu\n", sizeof(struct test));
  struct test *testing_struct = memloc(sizeof(*testing_struct));
  printf("Fourth allocation\n");
  visualize_memory_list();
  printf("\n");

  struct test *another = memloc(sizeof(*another));
  printf("Fifth allocation\n");
  visualize_memory_list();
  printf("\n");

  return 0;
};
