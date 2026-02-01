#include <stdio.h>

typedef struct {
   unsigned long size;
   unsigned long capacity;
   char* data;
} BeryString;

typedef struct {
  unsigned long size;
  unsigned long capacity;
  BeryString* elements;
} BeryDynArr;

void start(BeryDynArr args) {
  printf("Arguments passed to Beryllium program:\n");
  for (unsigned long i = 0; i < args.size; i++) {
    printf("Arg %lu: %s\n", i, args.elements[i].data);
  }
}