#include <stdlib.h>
#include <string.h>

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

extern void start(BeryDynArr args);

int main(int argc, char** argv) {
  BeryDynArr args;
  args.size = argc;
  args.capacity = argc;
  args.elements = (BeryString*)malloc(sizeof(BeryString) * args.capacity);

  for (unsigned long i = 0; i < args.size; i++) {
    args.elements[i].size = strlen(argv[i]);
    args.elements[i].capacity = args.elements[i].size;
    args.elements[i].data = (char*)malloc(args.elements[i].capacity + 1);
    memcpy(args.elements[i].data, argv[i], args.elements[i].size + 1);
  }

  start(args);

  return 0;
}
