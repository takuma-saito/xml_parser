#ifndef __BASIC_H
#define __BASIC_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define MAXNAME 1024
#define MAXLINE 4096

/* エラーを表示させる */
extern void err_exit(char *fmt, ...);

/* メモリを 0 クリアで割り当てる */
extern void *alloc(size_t size);

/* 文字列を取得する */
extern char *str(char *string);

#endif // __BASIC_H
