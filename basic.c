
#include "basic.h"

/* エラーメッセージを出力して終了 */
void err_exit(char *fmt, ...) {
  va_list ap;
  char line[MAXLINE];
  va_start(ap, fmt);
  vsnprintf(line, MAXLINE, fmt, ap);
  printf("---- error ----\n");
  printf("%s", line);
  exit(1);
}

void *alloc(size_t size) {
  void *ptr;
  ptr = calloc(1, size);
  if (ptr == NULL) err_exit("memory allocate error");
  return ptr;
}
