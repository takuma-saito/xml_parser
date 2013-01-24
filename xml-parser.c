
/**********************************************
 * xml-parser - xmlを解析して構文木を出力する *
 * 目的 - xmlの構造を簡単俯瞰するためのツール *
 **********************************************/

#include "xml-parser.h"
#include "term.h"
#include "io.h"

#define GREEN "\033[32m"
#define BLUE  "\033[36m"
#define C_END "\033[0m"

void usage(void) {
  printf("xml-parser: [-d] file\n" \
         "  -d: output dot program format\n" \
         );
  exit(1);
}

/* ノードを next 方向に進める */
node_t *new_next_node(node_t *node, term_t *term) {
  node_t *new;
  new = alloc(sizeof(node_t));
  new->prev = node;
  new->parent = node->parent;
  new->term = term;
  node->next = new;
  return new;
}

/* ノードを prev 方向に進める */
node_t *new_prev_node(node_t *node, term_t *term) {
  node_t *new;
  new = alloc(sizeof(node_t));
  new->next = node;
  new->parent = node->parent;
  new->term = term;
  node->prev = new;
  return new;
}

/* ノードを children 方向に進める */
node_t *new_children_node(node_t *node) {
  node_t *new;
  new = alloc(sizeof(node_t));
  new->parent = node;
  node->children = new;
  return new;
}

/* 解析処理 */
/* node->term: 空 */
void xml_parse(node_t *node, term_t *curr_term) {
  term_t *next_term;
  /* printf("node->term->name: %s\n", node->term->name); */
  /* printf("node->term->type: %d\n", node->term->type); */
  /* puts(""); */
  /* fflush(stdout); */
  switch(curr_term->type) {
  case TERM_START:
    next_term = get_term();
    node->term = curr_term;
    node->children = new_children_node(node);
    node = node->children;
    xml_parse(node, next_term);
    break;
  case TERM_END:
    next_term = get_term(); 
    switch (next_term->type) {
    case TERM_END:
      node = node->parent;
      break;
    case TERM_START:      
      node = new_next_node(node, next_term);
      break;
    case TERM_EOF:
      return;
    default:
      break;
    }
    xml_parse(node, next_term);
    break;
  case TERM_CONTENT:
    node->term = curr_term;
    node = node->parent;
    xml_parse(node, get_term());
    break;
  case TERM_ELEMENT:
    if (node->term == NULL) {
      node->term = curr_term;
    }
    else {
      node = new_next_node(node, curr_term);
    }
    xml_parse(node, get_term());
    break;
  default:
    break;
  }
}

/* ノードのデバック出力 */
void xml_node_debug(node_t *node, int depth) {
  node_t *ptr;
  prop_t *prop;
  for (ptr = node; ptr != NULL; ptr = ptr->next) {
    switch(ptr->term->type) {
    case TERM_ELEMENT:
    case TERM_CONTENT:
      printf("[content] %d -> '%s'\n", depth, ptr->term->name);
      break;
    case TERM_START:
      printf("[node:%d] %d -> %s\n", ptr->term->type, depth, ptr->term->name);
      for (prop = ptr->term->property; prop != NULL; prop = prop->next) {
        printf("[prop]      - [%s] %s\n", prop->key, prop->value);
      } 
      xml_node_debug(ptr->children, depth + 1);
      break;
    default:
      printf("[debug]: term->name -> %s\n", ptr->term->name);
      printf("[debug]: term->type -> %d\n", ptr->term->type);
      /* assert(false); */
      break;
    }
  }
}

/* 線の出力 */
void show_line(int depth, bool *line_num) {
  int i;
  for (i = 0; i < depth; i++) {
    if (line_num[i] == true) {
      printf("  | ");
    }
    else {
      printf("    ");
    }
  }
}

#define PRINT(...)                 \
  printf("[%d]:", depth);          \
  show_line(depth, line_num);      \
  printf(__VA_ARGS__)

/* xml解析木を深さ優先探索で表示 */
void xml_node_show(node_t *node, int depth, bool *line_num) {
  node_t *ptr;
  prop_t *prop;
  for (ptr = node; ptr != NULL; ptr = ptr->next) {
    switch(ptr->term->type) {
    case TERM_ELEMENT:
    case TERM_CONTENT:          /* 終端 */
      PRINT("  |\n");
      PRINT("   -- %s%s%s\n", GREEN, ptr->term->name, C_END);
      break;
    case TERM_START:              /* 節 */
      PRINT("  |\n");
      PRINT("   -- %s\n", ptr->term->name);
      for (prop = ptr->term->property; prop != NULL; prop = prop->next) {
        PRINT("      - [%s] %s\n", prop->key, prop->value);
      } 
      /* 隣の要素がなければ, ラインを引かない */
      line_num[depth] = (ptr->next != NULL) ? true : false;
      xml_node_show(ptr->children, depth + 1, line_num);
      break;
    case TERM_EOF:
    case TERM_COMMENT:
      break;
    default:
      /* ありえないことが起こった */
      printf("%d\n", ptr->term->type);
      err_exit("unkown error\n");
      break;
    }
  }
}

/* versionとエンコーディングを読み込む */
void xml_version(xml_t *xml) {
  char line[MAXLINE];
  char encoding[MAXLINE];
  int ret, pos = -1;
  double version;
  char ch = getch();
  
  /* 空白を飛ばす */
  while (isspace(ch)) {
    ch = getch();
  }

  if (ch == '<') {
    if ((ch = getch()) != '?') {
      pushch(ch);
      pushch('<');
      return;
    }
    else pushch(ch); ch = '<';
    while (1) {
      if (ch == '>') {
        line[++pos] = ch;
        break;
      }
      if (MAXLINE - 1 <= pos) {
        err_exit("illegal version format");
      }
      line[++pos] = ch;
      ch = getch();
    }
    line[++pos] = '\0';
    ret = sscanf(line, "<?xml version=\"%lf\" encoding=\"%s\" ?>", &version, encoding);
    if (ret < 0) {
        err_exit("illegal version format");
    }
    encoding[strlen(encoding) - 1] = '\0';
  }
  else {
    err_exit("'<'が抜けています.\n");
  }
  xml->version = version;
  xml->encoding = malloc(sizeof(char) * (strlen(encoding) + 1));
  strcpy(xml->encoding, encoding);
}

/* doctype宣言の読込 */
void xml_doctype(xml_t *xml) {
  char ch = getch();
  char line[MAXLINE] = "";
  int pos = -1;
  
  /* 空白を飛ばす */
  while (isspace(ch)) {
    ch = getch();
  }

  /* Doctype宣言かどうかを確認 */
  if (ch == '<') {
    if ((ch = getch()) != '!') {
      pushch(ch);
      pushch('<');
      return;
    }
    else {
    }
    while (1) {
      if (ch == ' ') {
        line[++pos] = ch;
        break;
      }
      if (MAXLINE - 1 < pos) {
        err_exit("doctype length error.");
      }
      line[++pos] = ch;
      ch = getch();
    }
  }
  line[++pos] = '\0';
  if (strcmp(line, "!DOCTYPE")) {
    bzero(line, sizeof(line));
    pos = -1;
    while (1) {
      if (ch == '>') break;
      if (MAXLINE - 1 < pos) {
        err_exit("doctype length error.");
      }
      line[++pos] = ch;
      ch = getch();
    }
  }
  line[++pos] = '\0';

  xml->doctype = malloc(sizeof(char) * (strlen(line) + 1));
  strcpy(xml->doctype, line);
}

/* 深さの最大値を求める */
static int gb_max_depth = 0;
void get_max_depth(node_t *node, int depth) {
  node_t *ptr;
  for (ptr = node; ptr != NULL; ptr = ptr->next) {
    if (ptr->children != NULL) {
      get_max_depth(ptr->children, depth + 1);
      if (depth > gb_max_depth) gb_max_depth = depth;
    }
  }
}

static bool *alloc_line(node_t *root) {
  bool *line_num;
  int i;
  get_max_depth(root, 1);
  line_num = malloc(sizeof(int) * gb_max_depth);
  for (i = 0; i < gb_max_depth; i++) {
    line_num[i] = false;
  }
  return line_num;
}

/* xml木構造を表示 */
void show_format_line(xml_t *xml) {
  bool *line_num;
  line_num = alloc_line(xml->root);
  printf("Show xml tree ... %s\n", xml->filename);
  if (xml->encoding != NULL)
    printf("Version ... %2.1lf  Encoding ... %s\n", xml->version, xml->encoding);
  if (xml->doctype != NULL)
    printf("Doctype ... %s\n", xml->doctype);  
  xml_node_show(xml->root, 0, line_num);
  printf("\n");
}

/* 本処理 */
void xml_main(xml_t *xml) {
  node_t *node;
  xml_version(xml);
  xml_doctype(xml);  
  node = alloc(sizeof(node_t));
  xml->root = node;
  xml_parse(node, get_term());
  /* xml_node_debug(xml->root, 0); */
  xml->show(xml);
}

int main(int argc, char **argv) {
  xml_t xml;
  char *filename;
  FILE *fp;
  char ch;

  xml.show = show_format_line;
  while ((ch = getopt(argc, argv, "d")) != -1) {
    switch(ch) {
    case 'd':
      xml.show = show_format_dot;
      break;
    default:
      usage();
      break;
    }
  }
  argc -= optind;
  argv += optind;

  if (argc != 1) usage();
  
  filename = argv[0];
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("ファイルが開けません.\n");
    exit(1);
  }
  init_ch(fp);
  xml.filename = filename;
  xml_main(&xml);
  fclose(fp);

  return 0;
}
