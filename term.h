#ifndef __TERM_H
#define __TERM_H

#include "base.h"

/* タグの種類: 開始タグ, 終了タグ, エレメント（文字列）, コメント */
typedef enum {
  TOKEN_START = 1,
  TOKEN_END,
  TOKEN_CONTENT,
  TOKEN_ELEMENT,
  TOKEN_COMMENT,
  TOKEN_INFO,
  TOKEN_EOF
} type_t;

/* ノードの属性プロパティ */
typedef struct prop {
  struct prop *next;
  struct prop *prev;
  char *key;
  char *value;
} prop_t;

/* ノードの名前, 属性 */
typedef struct {
  char *name;
  prop_t *property;
} TAttr;

/* コメント */
typedef char* TComment;

typedef struct {
  type_t type;
  void *value;                  /* TAttr or TComment */
} token_t;

/* 全体の情報 */
typedef struct {
  char *encoding;
  char *doctype;
  double version;
} TInfo_t;

/* header info */
TInfo_t *get_info();

/* term を得る */
extern token_t *get_token();

/* 次の term のタイプを予測 */
extern bool expect_term(type_t type);

/* 次の term のタイプを返却 */
extern type_t next_term_type();

#endif // __TERM_H

