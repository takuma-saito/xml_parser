#ifndef __TERM_H
#define __TERM_H

#include "basic.h"

/* タグの種類: 開始タグ, 終了タグ, エレメント（文字列）, コメント */
typedef enum {
  TERM_START = 1,
  TERM_END,
  TERM_CONTENT,
  TERM_ELEMENT,
  TERM_COMMENT,
  TERM_EOF
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
  type_t type;
  prop_t *property;
} term_t;

/* term を得る */
extern term_t *get_term(void);

/* 次の term のタイプを予測 */
extern bool expect_term(type_t type);

/* 次の term のタイプを返却 */
extern type_t next_term_type();

#endif // __TERM_H

