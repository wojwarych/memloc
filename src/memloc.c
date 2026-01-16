#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PAGE_SIZE 4096

typedef struct Block {
  uint64_t header;
  size_t data_size;
  uint64_t footer;
} Block;

Block *heap_start = NULL;
Block *top = NULL;

Block *get_from_os(size_t data_size) {
  Block *data = sbrk(0);

  if (sbrk(data_size + sizeof(data->header) + sizeof(data->footer)) ==
      (void *)-1) {
    return NULL;
  }
  return data;
}

void *memloc(size_t incr) {
  // exceed size above possible nearest divisible by 8
  // and do bitwise AND on NOT 7 to find nearest size
  // of possible block round up to num divisble by 8
  size_t data_size = (incr + sizeof(intptr_t) - 1) & ~(sizeof(intptr_t) - 1);

  if (heap_start != NULL) {
    void *heap_jump = heap_start;
    while (heap_jump < sbrk(0)) {
      Block *data = heap_jump;
      if (((data->header & ~(~7)) == 0) && data_size <= data->data_size) {
        return (void *)(&data->data_size);
      }
      heap_jump = heap_jump + data->header - 1;
    }
  }

  Block *data = get_from_os(data_size);

  if (data == NULL) {
    return NULL;
  }

  data->data_size = data_size;
  data->header = (sizeof(data->header) + data_size + sizeof(data->footer) + 1);
  data->footer = (sizeof(data->header) + data_size + sizeof(data->footer) + 1);

  if (heap_start == NULL) {
    heap_start = data;
  }

  return (void *)(&data->data_size);
}

void freeloc(void *ptr) {
  if (!ptr) {
    return;
  }
  Block *data = ptr - sizeof(data->header);
  --data->header;
  --data->footer;
}

int main(void) {
  int *foo = memloc(sizeof(int));
  char *bar = memloc(sizeof(char));
  long long *asd = memloc(sizeof(long long));
  char *test = memloc(sizeof(char));
  printf("FOO %p\n", foo);
  printf("BAR %p\n", bar);
  printf("long %p\n", asd);
  printf("test %p\n", test);
  *test = 'x';

  *foo = 5;
  *bar = 'a';
  *asd = 0xFFFFFFFF + 1;
  return 0;
};
