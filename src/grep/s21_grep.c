#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_PATTERNS 100

// Функция для проверки совпадения строки с шаблоном
char *my_strdup(const char *s) {
  size_t len = strlen(s) + 1;
  char *copy = malloc(len);
  if (copy) {
    memcpy(copy, s, len);
  }
  return copy;
}

int match(const char *line, const char *pattern, int ignore_case) {
  if (ignore_case) {
    char *lower_line = my_strdup(line);
    char *lower_pattern = my_strdup(pattern);

    if (!lower_line || !lower_pattern) {
      fprintf(stderr, "Memory allocation error.\n");
      free(lower_line);
      free(lower_pattern);
      return 0;
    }

    for (size_t i = 0; lower_line[i]; i++)
      lower_line[i] = tolower(lower_line[i]);
    for (size_t i = 0; lower_pattern[i]; i++)
      lower_pattern[i] = tolower(lower_pattern[i]);

    int result = strstr(lower_line, lower_pattern) != NULL;

    free(lower_line);
    free(lower_pattern);
    return result;
  }
  return strstr(line, pattern) != NULL;
}

// Функция для чтения шаблонов из файла
int read_patterns_from_file(const char *filename,
                            char patterns[][MAX_LINE_LENGTH],
                            int max_patterns) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening pattern file");
    return -1;
  }

  int count = 0;
  while (fgets(patterns[count], MAX_LINE_LENGTH, file) &&
         count < max_patterns) {
    patterns[count][strcspn(patterns[count], "\n")] =
        0;  // Удаление символа новой строки
    count++;
  }

  fclose(file);
  return count;
}

// Основная программа
int main(int argc, char *argv[]) {
  bool ignore_case = false, invert_match = false, count_only = false;
  bool show_filenames = false, show_line_numbers = false, no_filename = false;
  bool suppress_errors = false, only_matching = false;

  char patterns[MAX_PATTERNS][MAX_LINE_LENGTH];
  int pattern_count = 0;

  char *files[MAX_PATTERNS];
  int file_count = 0;

  bool pattern_provided =
      false;  // Флаг, чтобы проверить, был ли передан шаблон

  // Обработка аргументов командной строки
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
      if (pattern_count >= MAX_PATTERNS) {
        fprintf(stderr, "Error: Too many patterns specified.\n");
        return EXIT_FAILURE;
      }
      strncpy(patterns[pattern_count++], argv[++i], MAX_LINE_LENGTH);
      pattern_provided = true;
      no_filename = true;
    } else if (strcmp(argv[i], "-i") == 0) {
      ignore_case = true;
      no_filename = true;
    } else if (strcmp(argv[i], "-v") == 0) {
      invert_match = true;
      no_filename = true;
    } else if (strcmp(argv[i], "-c") == 0) {
      count_only = true;
    } else if (strcmp(argv[i], "-l") == 0) {
      show_filenames = true;
    } else if (strcmp(argv[i], "-n") == 0) {
      show_line_numbers = true;
      no_filename = true;
    } else if (strcmp(argv[i], "-h") == 0) {
      no_filename = true;
    } else if (strcmp(argv[i], "-s") == 0) {
      suppress_errors = true;
    } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
      int count = read_patterns_from_file(argv[++i], patterns + pattern_count,
                                          MAX_PATTERNS - pattern_count);
      if (count < 0) {
        if (!suppress_errors) perror("Error reading patterns from file");
        return EXIT_FAILURE;
      }
      pattern_count += count;
      pattern_provided = true;
      no_filename = true;
    } else if (strcmp(argv[i], "-o") == 0) {
      only_matching = true;
      no_filename = true;
    } else {
      if (strchr(argv[i], '-') == argv[i]) {
        fprintf(stderr, "Unknown option: %s\n", argv[i]);
        return EXIT_FAILURE;
      }
      if (!pattern_provided) {
        strncpy(patterns[pattern_count++], argv[i], MAX_LINE_LENGTH);
        pattern_provided = true;
        no_filename = true;
      } else {
        files[file_count++] = argv[i];
      }
    }
  }
  if (pattern_count == 0) {
    fprintf(stderr, "Error: No pattern specified.\n");
    return EXIT_FAILURE;
  }

  if (file_count == 0) {
    files[file_count++] = NULL;
  }

  int total_match_count = 0;

  // Обработка файлов
  for (int f = 0; f < file_count; f++) {
    FILE *fp = files[f] ? fopen(files[f], "r") : stdin;
    if (!fp) {
      if (!suppress_errors) perror(files[f]);
      continue;
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int match_count = 0;

    while (fgets(line, sizeof(line), fp)) {
      line_number++;
      bool found_match = false;

      for (int i = 0; i < pattern_count; i++) {
        if (match(line, patterns[i], ignore_case)) {
          found_match = true;
          break;
        }
      }

      if (invert_match) found_match = !found_match;

      if (found_match) {
        match_count++;

        if (count_only) continue;
        if (show_filenames) {
          printf("%s\n", files[f]);
          break;
        }

        if (!no_filename && files[f]) printf("%s:", files[f]);
        if (show_line_numbers) printf("%d:", line_number);

        if (only_matching) {
          for (int i = 0; i < pattern_count; i++) {
            char *match_ptr = strstr(line, patterns[i]);
            while (match_ptr) {
              printf("%.*s\n", (int)strlen(patterns[i]), match_ptr);
              match_ptr = strstr(match_ptr + 1, patterns[i]);
            }
          }
        } else {
          printf("%s", line);
        }
      }
    }

    if (count_only) {
      printf("%d\n", match_count);
    }

    total_match_count += match_count;

    if (fp != stdin) fclose(fp);
  }

  return total_match_count > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}