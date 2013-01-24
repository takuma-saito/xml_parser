
#include "term.h"
#include "io.h"

/* 次の要素 */
term_t *term_next = NULL;

/* プロパティを得る */
static prop_t *get_property() {
  char key[MAXLINE] = "", value[MAXLINE] = "";
  char ch;
  prop_t *prop;
  int pos = -1;

  /* key を得る */
  while (1) {
    ch = getch();
    if (MAXLINE - 1 < pos) {
      err_exit("error key length\n");
    }
    if (ch == '=') {
      break;
    }
    else {
      key[++pos] = ch;
    }
  }
  key[++pos] = '\0';
  pos = -1;

  /* valueを得る */
  if ((ch = getch()) != '"') {
    err_exit("'\"'がありません. トークンエラー.\n");
  }
  while (1) {
    ch = getch();
    if (MAXLINE - 1 < pos) {
      err_exit("error value length");
    }
    if (ch == '"') {
      break;
    }
    else {
      value[++pos] = ch;
    }
  }
  value[++pos] = '\0';

  prop= malloc(sizeof(prop_t));
  prop->key = malloc(sizeof(char) * (strlen(key) + 1));
  prop->value = malloc(sizeof(char) * (strlen(value) + 1));
  prop->next = NULL;
  prop->prev = NULL;
  strcpy(prop->key, key);
  strcpy(prop->value, value);
  return prop;
}

term_t *get_term(void) {
  char line[MAXNAME] = "";
  term_t *term, *tmp;
  char ch;
  int counter = -1;
  prop_t *prop = NULL;

  if (term_next != NULL) {
    tmp = term_next;
    term_next = NULL;
    return tmp;
  }
  
  term = alloc(sizeof(term));
  ch = getch();
  /* 空白の読み捨て */
  while (isspace(ch)) {
    ch = getch();
  }

  if (ch == '<') {
    if ((ch = getch()) == '/') {
      term->type = TERM_END;
      ch = getch();
    }
    else {
      term->type = TERM_START;
    }
    while (true) {
      if (ch == '>') {
        for (prop = term->property; prop != NULL; prop = prop->prev);
        term->property = prop;
        break;
      }
      if (ch == '/') {
        term->type = TERM_ELEMENT;
        getch(); //読み捨て
        break;
      }
      
      /* 属性を入力 */
      if (ch == ' ') {
        term->property = get_property();
        term->property->prev = term->property;
        term->property = term->property->next;
      }
      
      if (MAXNAME > ++counter) {
        line[counter] = ch;
      }
      else {
        err_exit(">が見つかりません.文法エラーです.");
      }
      ch = getch();
    }
    line[++counter] = '\0';
    term->name = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(term->name, line);
  }
  else if (ch == EOF) {
    term->type = TERM_EOF;
  }
  else if (isprint(ch)) {
    term->type = TERM_CONTENT;
    while (1) {      
      if (ch == '<') {
        pushch(ch);
        break;
      }
      if (MAXNAME > ++counter) {
        line[counter] = ch;
      }
      else {
        err_exit(">の閉じ忘れがあります\n");
      }
      ch = getch();
    }
    line[++counter] = '\0';
    term->name = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(term->name, line);
  }
  else {
    /* ありえないことが起こった */
    err_exit("英数字以外の文字は読み込めません.\n");
  }
  return term;
}

/* 次の単語のタイプを予測する */
bool expect_term(type_t type) {
  if (term_next == NULL) {
    term_next = get_term();
  }
  return term_next->type == type;
}

type_t next_term_type() {
  if (term_next == NULL) {
    term_next = get_term();
  }
  return term_next->type;
}
