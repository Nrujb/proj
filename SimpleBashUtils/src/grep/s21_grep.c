#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e, o, f, s, h, n, l, c, v, i;
  char buffer[4096];
} flags;

int parser(int argc, char **argv, flags *flag);
int realization_comp(char **argv, regex_t *regex, char *pattern, char *line_f,
                     FILE *file_open, flags *flag);
void print_if_no_reti(int count_n, int i, char **argv, char *line, flags *flag);
void print_if_reti(int count_n, int i, char **argv, char *line, flags *flag);
void print_file(int argc, char **argv, flags *flag);
int flag_s_debug(FILE *fp, int *contin, flags *flag);

int main(int argc, char **argv) {
  flags flag = {0};
  flag.e = 1;
  parser(argc, argv, &flag);
  print_file(argc, argv, &flag);
  return 0;
}

int parser(int argc, char **argv, flags *flag) {
  int c = 0;
  while ((c = getopt(argc, argv, "e:ivclnhsfo")) !=
         -1) {  // getopt перебирает массив символов,пока не встретит нужный
                // символ.-1 значит конец
    switch (c) {
      case 'e':  //включает режим использования строки в качестве регулярного
                 //выражения
        flag->e = 2;
        strcat(flag->buffer, optarg);  // optarg при помощи гетопта
                                       // записывавется строка которую мы ищем
        strcat(flag->buffer, "|");
        break;
      case 'i':  //Игнорирует различия регистра
        flag->i = 1;
        flag->e = 0;
        break;
      case 'v':  //Выводит все, кроме искомого
        flag->v = 1;
        break;
      case 'c':  //Выводит только количество совпадающих строк
        flag->c = 1;
        break;
      case 'l':  //Выводит только совпадающие файлы
        flag->l = 1;
        break;
      case 'n':  //нумерует строку
        flag->n = 1;
        break;
      case 'h':  //Выводит совпадающие строки для регулярных выражений
        flag->h = 1;
        break;
      case 's':  //Подавляет сообщения об ошибках о несуществующих или
                 //нечитаемых файлах
        flag->s = 1;
        break;
      case 'o':  //Печатает только совпадающие (непустые) части совпавшей строки
        flag->o = 1;
        break;
      case 'f':  //Получает регулярные выражения из файла
        flag->f = 1;
        break;
      default:
        break;
    }
  }
  if (flag->e == 2) {
    flag->buffer[strlen(flag->buffer) - 1] =
        '\0';  //ищеим конец строки и ставим туда /0,чтобы не выйти за ее
               //пределы
  }
  return 0;
}
void print_file(int argc, char **argv, flags *flag) {
  regex_t regex;  //эта структура для использования регулярных выражений
  int reti = 0, count_pattern = 0, out = 0,
      count_h = argc - optind,  //все для использования рег.выр. последнее-для
                                //подсчета файлов
      i = optind + 1;           //индекс первого файла
  char line[4096] = "";
  char line_f[4096] = "";
  char *pattern = "";
  char c_end = 0;
  FILE *fp_1 = NULL;
  if (flag->f == 1) {
    fp_1 = fopen(argv[optind], "r");  //открываем файл для чтения
    flag_s_debug(fp_1, &out, flag);  //не выводит ошибку если она есть (есть s)
    if (out == 1) return;
    while (fgets(line_f, 4096, fp_1))
      count_pattern++;  //открываем файл и считаем сколько там символов
    fseek(fp_1, 0, SEEK_SET);  //курсор на начало файла
  }
  if (flag->f == 0) count_pattern = 1;
  if (flag->e == 2) i = optind;
  if (flag->e == 2) count_h = argc - optind + 1;
  if (count_h <= 2) flag->h = 1;
  for (; i < argc; i++) {  // перебираем каждый файл
    int count_c = 0, count_n = 0, contin = 0;
    FILE *fp = fopen(argv[i], "r");
    flag_s_debug(fp, &contin, flag);
    if (contin == 1) continue;  // если continue=1, то есть ошибка
    while (fgets(line, 4096, fp)) {
      count_n++;  //переменная,в которую мы записываем количество символов
      if (line[strlen(line) - 1] != '\n')
        strcat(line, "\n");  //условие установки переноса
      for (int j = 0; j < count_pattern;
           j++) {  // перебираем каждый символ регулярного выражения
        realization_comp(argv, &regex, pattern, line_f, fp_1,
                         flag);  //делаем 3 этапа
        reti = regexec(&regex, line, 0, NULL, 0);
        regfree(&regex);
        if (flag->v != 1 && !reti && flag->l != 1) {
          count_c++;
          c_end = line[strlen(line) - 1];
          print_if_no_reti(count_n, i, argv, line, flag);
          break;
        }  // no_reti  есть /n
        if (flag->v == 1 && reti && flag->l != 1) {
          count_c++;
          c_end = line[strlen(line) - 1];
          print_if_reti(count_n, i, argv, line, flag);
          break;
        }
      }  //закончили перебирать символы регулряного выражения
      if (flag->l == 1 && !reti) {  //если есть флаг, то выводит имя файла
        printf("%s\n", argv[i]);
        break;
      }
      if (flag->f == 1) fseek(fp_1, 0, SEEK_SET);
    }  //закончили перебирать файл
    if (flag->c == 1 && flag->h != 1)
      printf("%s:%d\n", argv[i], count_c);  // количество строк с именами файлов
    if (flag->c == 1 && flag->h == 1)
      printf("%d\n", count_c);  // выводим количество строк
    if (count_c != 0 && c_end != '\n' && i < argc - 1)
      printf("\n");  // если закончится файл, то делаем перенос
    fclose(fp);
  }  //закончили перебирать файлы
  if (flag->f == 1) fclose(fp_1);
}  // конец печать

int realization_comp(char **argv, regex_t *regex, char *pattern,
                     char *line_f,  // получаем регулярное выражение из файла
                     FILE *file_open, flags *flag) {
  if (flag->f == 1) {
    fgets(line_f, 4096, file_open);
    if (line_f[0] != '\n') line_f[strcspn(line_f, "\n")] = 0;
    pattern = (char *)malloc(strlen(line_f) + 1);
    if (!pattern) {  // выводим,если не хватает памяти
      perror("Error allocating memory");
      exit(EXIT_FAILURE);
    }
    strcpy(pattern, line_f);
    if (flag->i == 1 &&
        flag->e == 0) {  // игнорирует различие регистра и если это текст,
      regcomp(regex, pattern,  //то выполняет следующие 2 шага
              REG_ICASE);
      free(pattern);
    } else if (flag->e == 1 &&
               flag->i == 0) {  // если чувствителен к регистру и это не
                                // текст,то делает следующее
      regcomp(regex, pattern, REG_EXTENDED);
      free(pattern);
    } else if (line_f[0] == '\n')  // случай,если пустой файл
      regcomp(regex, ".", 0);
  } else {
    if (flag->i == 1 &&
        flag->e == 0) {  // то же самое,что и выше, только в командной строке
      regcomp(regex, argv[optind], REG_ICASE);
    } else if (flag->e == 1 && flag->i == 0) {
      regcomp(regex, argv[optind], REG_EXTENDED);
    }
    if (flag->e == 2 && flag->i == 0) {
      regcomp(regex, flag->buffer, REG_EXTENDED);
    }
  }
  return 0;
}

void print_if_no_reti(int count_n, int i, char **argv, char *line,
                      flags *flag) {
  if (flag->c != 1 && flag->n != 1 &&
      flag->h != 1) {  //без номера строки и с именем файла
    if (flag->o == 1)  //без таба
      printf("%s:%s\n", argv[i], argv[optind]);
    else  // с табом
      printf("%s:%s", argv[i], line);
  }
  if (flag->c != 1 && flag->n == 1 && flag->h != 1) {  //с номером строки
    if (flag->o == 1)
      printf("%s:%d:%s", argv[i], count_n, argv[optind]);
    else
      printf("%s:%d:%s", argv[i], count_n, line);
  }
  if (flag->c != 1 && flag->n != 1 && flag->h == 1) {  // без имени файла
    if (flag->o == 1)
      printf("%s", argv[optind]);
    else
      printf("%s", line);
  }
  if (flag->c != 1 && flag->n == 1 &&
      flag->h == 1) {  //с номером  и с именем файла
    if (flag->o == 1)
      printf("%d:%s", count_n, argv[optind]);
    else
      printf("%d:%s", count_n, line);
  }
}
void print_if_reti(int count_n, int i, char **argv, char *line, flags *flag) {
  if (flag->c != 1 && flag->n != 1 && flag->h != 1) {
    if (flag->o == 1)
      printf("%s:%s", argv[i], argv[optind]);
    else
      printf("%s:%s", argv[i], line);
  }
  if (flag->c != 1 && flag->n == 1 && flag->h != 1) {
    if (flag->o == 1)
      printf("%s:%d:%s", argv[i], count_n, argv[optind]);
    else
      printf("%s:%d:%s", argv[i], count_n, line);
  }
  if (flag->c != 1 && flag->n != 1 && flag->h == 1) {
    if (flag->o == 1)
      printf("%s", argv[optind]);
    else
      printf("%s", line);
  }
  if (flag->c != 1 && flag->n == 1 && flag->h == 1) {
    if (flag->o == 1)
      printf("%d:%s", count_n, argv[optind]);
    else
      printf("%d:%s", count_n, line);
  }
}
int flag_s_debug(FILE *fp, int *contin, flags *flag) {
  if (!fp) {
    if (flag->s == 1)
      *contin = 1;
    else {
      perror("Error opening file");
      *contin = 1;
    }
  }
  return 1;
}
