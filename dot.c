
/*******************************************
 * 階層構造をdot形式で表すためのモジュール *
 *******************************************/

#include "xml-parser.h"

/* キュー */
typedef struct queue_tag {
  struct queue_tag *next;
  node_t *node;
} queue_t;
queue_t queue_data = {NULL, NULL};
queue_t *queue_head = &queue_data;
queue_t *queue_tail = &queue_data;
static node_t *dequeue();
static void enqueue(node_t *node);

/* dot言語をファイルへ出力するための情報を格納 */
typedef struct dot_tag {
  struct dot_tag *next;
  char *line;
} dot_list_t;
dot_list_t dot_list = {NULL, NULL};
dot_list_t *curr_dot_list = &dot_list;
char dot_file_start[] =
  "digraph sample {\n" \
  "\tedge [color = \"#993333\", arrowtail = odot];\n";
char dot_file_end[] = "}\n";

/* キューから取り出す */
static node_t *dequeue() {
  queue_t *curr;
  node_t *node;
  if (queue_head->next == NULL) {
    return NULL;
  }
  curr = queue_head;
  queue_head = queue_head->next;
  node = queue_head->node;
  if (curr != &queue_data) {
    free(curr);
  }
  return node;
}

/* キューに加える */
static void enqueue(node_t *node) {
  queue_t *ptr = malloc(sizeof(queue_t));
  ptr->node = node;
  ptr->next = NULL;
  queue_tail->next = ptr;
  queue_tail = ptr;
}

/* dot_list に値を追加する */
static void add_list(node_t *ptr) {
  char line[MAXLINE] = "";
  curr_dot_list->next = malloc(sizeof(dot_list_t));
  snprintf(line, MAXLINE, "\t\"%s\" -> \"%s\";\n",
           ptr->parent->term->name, ptr->term->name);
  curr_dot_list->line = malloc(sizeof(char) * (strlen(line) + 1));
  strcpy(curr_dot_list->line, line);
  curr_dot_list = curr_dot_list->next;
  curr_dot_list->next = NULL;
}

/* dot_listを作る */
static void dot_create_list() {
  int depth = 0;
  while (1) {
    node_t *ptr = NULL, *node = dequeue();
    if (node == NULL) break;
    if (node->children != NULL) depth++;
    for (ptr = node->children; ptr != NULL; ptr = ptr->next) {
      switch (node->term->type) {
      case TERM_CONTENT:
        add_list(ptr);
        break;
      case TERM_ELEMENT:
      case TERM_START:
        add_list(ptr);
        enqueue(ptr);
        break;
      case TERM_END:
      case TERM_COMMENT:
      case TERM_EOF:
        break;
      }
    }
  }
}

/* listを削除する */
static void dot_delete_list() {
  dot_list_t *ptr = dot_list.next, *next;
  while (1) {
    if (ptr->next == NULL) break;
    next = ptr->next;
    free(ptr);
    ptr = next;
  }
}

/* xml解析木を幅優先探索で検索 */
void show_format_dot(xml_t *xml) {
  node_t *root = xml->root;
  dot_list_t *ptr;
  enqueue(root);
  dot_create_list();
  for (ptr = dot_list.next; ptr != NULL; ptr = ptr->next) {
    printf("%s", ptr->line);
  }
  dot_delete_list();

  return;
}
