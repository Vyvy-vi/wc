#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "wctype.h"
#include "wchar.h"
#include "locale.h"

struct Options {
  bool c;
  bool l;
  bool m;
  bool w;
  int filename_index;
};

struct Data {
  long long bytes;
  long long lines;
  long long words;
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

void print_values(struct Data data, struct Options options, char* filename) {
  if (options.c && !options.w && !options.l)
    printf("%8lld %s\n", data.bytes, filename);
  else if (!options.c && options.w && !options.l)
    printf("%8lld %s\n", data.words, filename);
  else if (!options.c && !options.w && options.l)
    printf("%8lld %s\n", data.lines, filename);
  else if (options.c && options.w && options.l)    
    printf("%8lld%8lld%8lld %s\n", data.lines, data.words, data.bytes, filename);
  else if (!options.c)
    printf("%8lld%8lld %s\n", data.lines, data.words, filename);
  else if (!options.l)
    printf("%8lld%8lld %s\n", data.words, data.bytes, filename);
  else if (!options.w)
    printf("%8lld%8lld %s\n", data.lines, data.bytes, filename);

}

struct Data  extractData(int fd, struct Options options) {
  struct Data d = {0, 0, 0};
  bool word_buffered = 0;
  char buf;
  while (0 != read(fd, &buf, 1)) {
    if (buf == '\n') {
      d.lines++;
      word_buffered = 0;
    }
    else if (isspace(buf)) {
      if (word_buffered) {
        d.words++;
        word_buffered = 0;
      }
    } else {
      word_buffered = 1;
    }

    d.bytes++;
  }

  if (word_buffered)
    d.words++;


  return d;
}

void processStdin(struct Options options) {
  printf("aaaaaaa stdin\n");
}

void processFiles(int argc, char* argv[], struct Options options) {
  if (argc == 1 || options.filename_index == 0) return processStdin(options);
  
  struct Data total = {0, 0, 0};

  int cnt = 0;
  for (int i = options.filename_index; i < argc; i++) {    
    int fd = open(argv[i], O_RDONLY);
    if (fd == -1) {
      printf("wc: %s: open: No such file or directory\n", argv[i]);
      continue;
    }
    struct Data d = extractData(fd, options);
    cnt++;
    total.bytes += d.bytes;
    total.lines += d.lines;
    total.words += d.words;

    print_values(d, options, argv[i]);
  }
  if (cnt > 1) print_values(total, options, "total");
}

int main(int argc, char* argv[]) {
  setlocale(LC_ALL, "");
  struct Options options = parse_options(argc, argv);
  processFiles(argc, argv, options);

  return 0;
}
