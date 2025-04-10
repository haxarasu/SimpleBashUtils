#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int number_nonblank;
  int show_ends;
  int number;
  int squeeze_blank;
  int show_tabs;
  int show_nonprint;
} Options;

void print_line_number(int *line_number);
void print_show_tabs(void);
void print_show_ends(void);
void process_file(const char *filename, const Options *options);
void process_line(char *buffer, const Options *options, int *line_number,
                  int *prev_blank);
void process_character(char c, const Options *options);
void usage(const char *prog_name);
void print_nonprint(char c);

void print_line_number(int *line_number) { printf("%6d\t", (*line_number)++); }

void print_show_tabs(void) { printf("^I"); }

void print_show_ends(void) { printf("$"); }

Options parse_options(int argc, char *argv[]) {
  Options options = {0};

  struct option long_options[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                  {"number", no_argument, NULL, 'n'},
                                  {"squeeze-blank", no_argument, NULL, 's'},
                                  {0, 0, 0, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "benstvET", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'b':
        options.number_nonblank = 1;
        options.number = 0;
        break;
      case 'e':
        options.show_ends = 1;
        options.show_nonprint = 1;
        break;
      case 'E':
        options.show_ends = 1;
        break;
      case 'n':
        if (!options.number_nonblank) {
          options.number = 1;
        }
        break;
      case 's':
        options.squeeze_blank = 1;
        break;
      case 't':
        options.show_tabs = 1;
        options.show_nonprint = 1;
        break;
      case 'T':
        options.show_tabs = 1;
        break;
      case 'v':
        options.show_nonprint = 1;
        break;
      default:
        usage(argv[0]);
    }
  }

  return options;
}

void process_file(const char *filename, const Options *options) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("s21_cat");
  } else {
    char buffer[4096];
    int line_number = 1;
    int prev_blank = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
      process_line(buffer, options, &line_number, &prev_blank);
    }

    fclose(file);
  }
}

void process_line(char *buffer, const Options *options, int *line_number,
                  int *prev_blank) {
  if (options->squeeze_blank && buffer[0] == '\n' && *prev_blank) {
    return;
  }

  *prev_blank = (buffer[0] == '\n');

  if ((options->number_nonblank && buffer[0] != '\n') || options->number) {
    print_line_number(line_number);
  }

  for (char *c = buffer; *c; c++) {
    if (*c == '\n' && options->show_ends) {
      print_show_ends();
    }
    process_character(*c, options);
  }
}

void process_character(char c, const Options *options) {
  if (c == '\t' && options->show_tabs) {
    print_show_tabs();
  } else if (options->show_nonprint && iscntrl(c) && c != '\n' && c != '\t') {
    print_nonprint(c);
  } else {
    putchar(c);
  }
}

void print_nonprint(char c) {
  if (c >= 32 && c <= 126) {
    putchar(c);
  } else if (c < 32) {
    printf("^%c", c + '@');
  } else {
    printf("M-%c", c - 128);
  }
}

void usage(const char *prog_name) {
  fprintf(stderr, "Usage: %s [-benstv] [file...]\n", prog_name);
}

int main(int argc, char *argv[]) {
  Options options = parse_options(argc, argv);

  if (optind >= argc) {
    usage(argv[0]);
    return EXIT_FAILURE;
  } else {
    for (int i = optind; i < argc; i++) {
      process_file(argv[i], &options);
    }
  }

  return 0;
}
