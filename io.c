
/***********************************
 * char - 文字の入出力に関する関数 *
 ***********************************/

#include "io.h"

#define MAXLINE 1024            /* バッファの大きさ */
#define STACK_SIZE 100          /* スタックの大きさ */

static char stack[STACK_SIZE];
static int stack_pos;
static FILE *fp;

/* 文章読込 */
void init_ch(FILE *ptr) {
  fp = ptr;
}

static void
stack_push(char ch) {
  if (stack_pos >= STACK_SIZE) {
    printf("stack overflow\n");
    return;
  }
  stack[stack_pos++] = ch;
}

static char
stack_pop(void) {
  char ch;
  if (stack_pos <= 0) {
    printf("stack underflow\n");
    return 0;
  }
  ch = stack[--stack_pos];
  return ch;
}

/* 一字読み込む */
char getch() {
  if (fp == NULL) {
    printf("init_ch で初期化が行われていません.\n");
    return -1;
  }
  
  if (stack_pos != 0) {
    return stack_pop();
  }
  else return fgetc(fp); 
}

/* 一字 stack に入れる */
void pushch(char ch) {
  stack_push(ch);
}
