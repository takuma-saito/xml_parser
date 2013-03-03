
/**********************************************
 * xml-parser - xmlを解析して構文木を出力する *
 * 目的 - xmlの構造を簡単俯瞰するためのツール *
 **********************************************/

#include "xml-parser.h"
#include "token.h"
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
node_t *new_next_node(node_t *node, token_t *token) {
  node_t *new;
  new = alloc(sizeof(node_t));
  new->prev = node;
  new->parent = node->parent;
  new->token = token;
  node->next = new;
  return new;
}

/* ノードを prev 方向に進める */
node_t *new_prev_node(node_t *node, token_t *token) {
  node_t *new;
  new = alloc(sizeof(node_t));
  new->next = node;
  new->parent = node->parent;
  new->token = token;
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
/* node->token: 空 */
void xml_parse(node_t *node, token_t *curr_token) {
  token_t *next_token;
  /* printf("node->token->name: %s\n", node->token->name); */
  /* printf("node->token->type: %d\n", node->token->type); */
  /* puts(""); */
  /* fflush(stdout); */
  switch(curr_token->type) {
  case TOKEN_START:
    next_token = get_token();
    node->token = curr_token;
    node->children = new_children_node(node);
    node = node->children;
    xml_parse(node, next_token);
    break;
  case TOKEN_END:
    next_token = get_token(); 
    switch (next_token->type) {
    case TOKEN_END:
      node = node->parent;
      break;
    case TOKEN_START:      
      node = new_next_node(node, next_token);
      break;
    case TOKEN_EOF:
      return;
    default:
      break;
    }
    xml_parse(node, next_token);
    break;
  case TOKEN_CONTENT:
    node->token = curr_token;
    node = node->parent;
    xml_parse(node, get_token());
    break;
  case TOKEN_ELEMENT:
    if (node->token == NULL) {
      node->token = curr_token;
    }
    else {
      node = new_next_node(node, curr_token);
    }
    xml_parse(node, get_token());
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
    switch(ptr->token->type) {
    case TOKEN_ELEMENT:
    case TOKEN_CONTENT:
      printf("[content] %d -> '%s'\n", depth, ptr->token->name);
      break;
    case TOKEN_START:
      printf("[node:%d] %d -> %s\n", ptr->token->type, depth, ptr->token->name);
      for (prop = ptr->token->property; prop != NULL; prop = prop->next) {
        printf("[prop]      - [%s] %s\n", prop->key, prop->value);
      } 
      xml_node_debug(ptr->children, depth + 1);
      break;
    default:
      printf("[debug]: token->name -> %s\n", ptr->token->name);
      printf("[debug]: token->type -> %d\n", ptr->token->type);
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
    switch(ptr->token->type) {
    case TOKEN_ELEMENT:
    case TOKEN_CONTENT:          /* 終端 */
      PRINT("  |\n");
      PRINT("   -- %s%s%s\n", GREEN, ptr->token->name, C_END);
      break;
    case TOKEN_START:              /* 節 */
      PRINT("  |\n");
      PRINT("   -- %s\n", ptr->token->name);
      for (prop = ptr->token->property; prop != NULL; prop = prop->next) {
        PRINT("      - [%s] %s\n", prop->key, prop->value);
      } 
      /* 隣の要素がなければ, ラインを引かない */
      line_num[depth] = (ptr->next != NULL) ? true : false;
      xml_node_show(ptr->children, depth + 1, line_num);
      break;
    case TOKEN_EOF:
    case TOKEN_COMMENT:
      break;
    default:
      /* ありえないことが起こった */
      printf("%d\n", ptr->token->type);
      err_exit("unkown error\n");
      break;
    }
  }
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
  TInfo_t *info = xml->info;
  line_num = alloc_line(xml->root);
  printf("Show xml tree ... %s\n", xml->filename);
  if (info->encoding != NULL)
    printf("Version ... %2.1lf  Encoding ... %s\n", info->version, info->encoding);
  if (info->doctype != NULL)
    printf("Doctype ... %s\n", info->doctype);
  xml_node_show(xml->root, 0, line_num);
  printf("\n");
}

/* 本処理 */
void xml_main(xml_t *xml) {
  node_t *node;
  node = alloc(sizeof(node_t));
  xml->info = get_info();
  xml->root = node;
  xml_parse(node, get_token());
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
