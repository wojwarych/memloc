#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PAGE_SIZE 4096

typedef uint32_t WORD;

static char *mem_start;
static char *mem_brk;
static char *mem_max_addr;

void init_mem() {
  mem_start = (char *)sbrk(PAGE_SIZE);
  mem_brk = (char *)mem_start;
  mem_max_addr = (char *)(mem_start + PAGE_SIZE);
}

void *memloc(size_t incr) {
  if (incr == 0 || (mem_brk + incr) > mem_max_addr) {
    printf("Insufficient memory!\n");
    return NULL;
  }
  size_t data_size = (incr + sizeof(intptr_t) - 1) & ~(sizeof(intptr_t) - 1);
  char *move = mem_start;
  while (move < mem_brk) {
    WORD *block = (WORD *)move;
    if ((*block & 0x1) == 0 && data_size <= (*block & ~0x7)) {
      *block = data_size + 1;
      return block + 1;
    }
    move = move + (sizeof(*block) + (*block & ~0x7) + sizeof(*block));
  }
  WORD *payload = (WORD *)mem_brk;
  *payload = data_size + 1;
  mem_brk = mem_brk + (sizeof(*payload) + (*payload & ~0x7) + sizeof(*payload));
  return payload + 1;
}

void freeloc(void *p) {
  WORD *block = p - sizeof(WORD);
  WORD *next = (WORD *)((char *)block + sizeof(WORD) * 2 + (*block & ~0x7));
  WORD *prev = (WORD *)((char *)block - sizeof(WORD) * 2 - (*block & ~0x7));

  *block -= 1;
  if ((*next & 0x1) == 0 && (char *)next <= mem_max_addr) {
    *block = *block + (*next & ~0x7);
  }

  if ((*prev & 0x1) == 0 && (char *)prev >= mem_start) {
    *prev = *prev + (*block & 0x7);
  }
}

int main() {
  init_mem();

  char *test = memloc(3);
  test[0] = 'a';
  test[1] = 'b';
  test[2] = 'c';

  char *next = memloc(4);
  next[0] = 'W';
  next[1] = 'o';
  next[2] = 'j';
  next[3] = 't';

  char *last = memloc(6);
  last[0] = 'x';
  last[1] = 'x';
  last[2] = 'x';
  last[3] = 'x';
  last[4] = 'x';
  last[5] = 'x';

  printf("%p\n", test);
  printf("%s\n", test);
  printf("%p\n", next);
  printf("%s\n", next);
  printf("%p\n", last);
  printf("%s\n", last);

  freeloc(next);
  freeloc(test);
  freeloc(last);

  return 0;
};
