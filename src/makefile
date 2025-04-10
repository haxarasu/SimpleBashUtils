CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

TARGETS = s21_cat s21_grep

all: $(TARGETS)

s21_cat: cat/s21_cat.o
	$(CC) $(CFLAGS) -o cat/s21_cat cat/s21_cat.o
	rm -f cat/*.o

s21_grep: grep/s21_grep.o
	$(CC) $(CFLAGS) -o grep/s21_grep grep/s21_grep.o
	rm -f grep/*.o

cat/s21_cat.o: cat/s21_cat.c
	$(CC) $(CFLAGS) -c cat/s21_cat.c -o cat/s21_cat.o

grep/s21_grep.o: grep/s21_grep.c
	$(CC) $(CFLAGS) -c grep/s21_grep.c -o grep/s21_grep.o

s21_cat_test: 
	./../test/s21_cat_test.sh

s21_grep_test: 
	./../test/s21_grep_test.sh

clean:
	rm -f cat/s21_cat grep/s21_grep