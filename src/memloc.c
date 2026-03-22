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
    move = move + (sizeof(WORD) + (*block & ~0x7) + sizeof(WORD));
  }
  WORD *header = (WORD *)mem_brk;
  *header = data_size + 1;
  WORD *footer = (WORD *)(mem_brk + sizeof(WORD) + (*header & ~0x7));
  *footer = data_size + 1;
  mem_brk = mem_brk + (sizeof(WORD) + (*header & ~0x7) + sizeof(WORD));
  return header + 1;
}

void freeloc(void *p) {
  WORD *block = p - sizeof(WORD);
  WORD *footer = p + (*block & ~0x7);
  WORD *next = (WORD *)(p + (*block & ~0x7) + sizeof(WORD));
  WORD *next_footer = (WORD *)((char *)next + sizeof(WORD) + (*next & ~0x7));
  WORD *prev = (WORD *)(p - sizeof(WORD) * 2);
  WORD *prev_header = (WORD *)((char *)prev - sizeof(WORD) - (*prev & ~0x7));

  *block -= 1;
  *footer -= 1;
  if (((*next & 0x1) == 0 && (char *)next <= mem_max_addr) &&
      ((*prev & 0x1) == 0 && (char *)prev >= mem_start)) {

    *prev_header =
        *block + (*next & ~0x7) + (*prev_header & ~0x7) + sizeof(WORD) * 4;
    *next_footer = *prev_header;
  } else if ((*next & 0x1) == 0 && (char *)next <= mem_max_addr) {
    *block = *block + (*next & ~0x7) + sizeof(WORD) * 2;
    *next_footer = *block;
  } else if ((*prev & 0x1) == 0 && (char *)prev >= mem_start) {
    *prev_header = (*prev_header & ~0x7) + (*block & ~0x7) + sizeof(WORD) * 2;
    *footer = *prev_header;
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

  char *very_last = memloc(8);
  very_last[0] = '5';

  freeloc(test);
  freeloc(last);
  freeloc(next);
  freeloc(very_last);

  return 0;
};
