#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
struct flags {
  int b, e, E, n, s, t, T, v;
} flags;
int print_previous(int c1, int count, int *c1_continue);
int parser(int argc, char **argv);
int print_file(int argc, char **argv);

int main(int argc, char **argv) {
  parser(argc, argv);
  print_file(argc, argv);
  return 0;
}

int parser(int argc, char **argv) {
  int t;
  const struct option opt[] = {{"number-nonblank", 0, NULL, 'b'},
                               {"number", 0, NULL, 'n'},
                               {"squeeze-blank", 0, NULL, 's'},
                               {0, 0, NULL, 0}};
  while ((t = getopt_long(argc, argv, "beEnstTv", opt, NULL)) != -1) {
    switch (t) {
      case ('b'):  //нумерует непустые строки
        flags.b = 1;
        break;
      case ('e'):  //конец $ и шифрует символы
        flags.e = 1;
        flags.v = 1;
        break;
      case ('n'):  //нумерует вообще все строки
        flags.n = 1;
        break;
      case ('s'):  //преобразует несколько пустых строк в одну
        flags.s = 1;
        break;
      case ('t'):  //табуляция как "^I" и шифрует
        flags.t = 1;
        flags.v = 1;
        break;
      case ('E'):  //как -е, но без -v
        flags.e = 1;
        break;
      case ('T'):  //просто табуляция
        flags.t = 1;
        break;
      case ('v'):  //шифр нечитаемых символов
        flags.v = 1;
    }
  }
  return 0;
}

int print_file(int argc, char **argv) {
  for (int i = optind; i < argc;
       i++) {  // это индекс следующего обрабатываемого элемента argv
    FILE *fp = fopen(argv[i], "r");
    if (fp == NULL) return 1;
    int c;
    int c1 = getc(fp);
    fseek(fp, 0, SEEK_SET);  // смещение отсчитывается от начала файла
    int count = 1, s_check = 0;
    if (c1 == '\n')
      s_check = 1;  //проверка на пустой файл и если да->заканчиваем
    while ((c = getc(fp)) != EOF) {
      if (flags.b == 1 &&
          ((count == 1 && c1 != '\n') ||
           (c != '\n' &&
            c1 ==
                '\n'))) {  // для корректного отображения нумерации при флаге б
        printf("%6d\t", count);
        count++;
      }
      if (flags.b == 0 && flags.n == 1 &&
          (c1 == '\n' ||
           count == 1)) {  // то же самое,но без условий(нумеруем все строки)
        printf("%6d\t", count);
        count++;
      }
      if (flags.e == 1 && flags.v == 1 && c == '\n') printf("$");
      if (flags.t == 1 && flags.v == 1 && c == '\t') printf("^I");
      if (flags.v == 1) {
        if (c < 32 && c != 9 && c != 10) {  // шифр нечитаемых символов
          printf("^");
          c = c + 64;
        } else if (c > 127 && c < 160) {
          printf("M-^");
          c = c - 64;
        } else if (c == 127) {
          printf("^");
          c = c - 64;
        }
      }
      if (flags.s == 1 && c == '\n') {  // сжимает непустые строки
        s_check++;
        if (s_check > 2) continue;
      } else
        s_check = 0;
      printf("%c", c);
      c1 = c;
    }
    fclose(fp);
  }
  return 0;
}