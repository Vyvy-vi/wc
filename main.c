#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"

struct Options {
  bool c;
  bool l;
  bool m;
  bool w;
  int filename_index;
};

struct Options parse_options(int argc, char* argv[]) {
  struct Options options = {0, 0, 0, 0, 0};

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      options.filename_index = i;
      break;
    }
    char *ptr = argv[i];
    while (*(++ptr) != '\0') {
      switch (*ptr) {
        case 'c': {
          options.m = 0;
          options.c = 1;
          break;
        }
        case 'l': {
          options.l = 1;
          break;
        }
        case 'm': {
          options.c = 0;
          options.m = 1;
          break;
        }
        case 'w': {
          options.w = 1;
          break;
        }
        default: {
          printf("wc: illegal option -- %c\n", *ptr);
          printf("usage: wc [-clmw] [file ...]\n");
          exit(-1);
        }
      }
    }
  }

  if (options.filename_index == 1) {
    options.c = 1;
    options.w = 1;
    options.l = 1;
  }

  return options;
}

void processStdin(struct Options options) {
  printf("stdin\n");
}

void processFiles(int argc, char* argv[], struct Options options) {
  printf("%d %d\n", options.filename_index, argc);
  if (argc == 1 || options.filename_index == 0) return processStdin(options);
  for (int i = options.filename_index; i < argc; i++) {    
    printf("%s\n", argv[i]);
  }
}

int main(int argc, char* argv[]) {
  struct Options options = parse_options(argc, argv);
  processFiles(argc, argv, options);

  return 0;
}
