#ifndef __IO_H
#define __IO_H

#include <stdio.h>

/* 文章読込 */
extern void input();

/* 一字読み込む */
extern char getch();

/* 一字 stack に入れる */
extern void pushch(char ch);

/* 初期化 */
extern void init_ch(FILE *ptr);

#endif // __IO_H
