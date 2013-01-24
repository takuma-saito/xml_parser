#ifndef __XML_PARSER_H
#define __XML_PARSER_H

#include "term.h"

/* xmlの解析木を構成する */
typedef struct node {
  struct node *next;        /* next sibling link */
  struct node *prev;        /* prev sibling link */
  struct node *children;    /* first chldren link */
  struct node *parent;      /* parent node */
  term_t *term;           /* ノードの名前 */
} node_t;

/* xml全体の情報 */
typedef struct xml {
  node_t *root;
  double version;
  char *filename;
  char *encoding;
  char *doctype;
  void (*show)(struct xml *root);
} xml_t;

void show_format_dot(xml_t *xml);

#endif // __XML_PARSER_H
