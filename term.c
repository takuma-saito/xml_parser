
#include "term.h"
#include "io.h"
#include "base.h"

#define COMMENT "comment"

/* 次の要素 */
term_t *term_next = NULL;

/* プロパティを得る */
static prop_t *get_property(prop_t *curr_prop) {
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

  prop = (prop_t *)alloc(sizeof(prop_t));
  prop->key = (char *)alloc(sizeof(char) * (strlen(key) + 1));
  prop->value = (char *)alloc(sizeof(char) * (strlen(value) + 1));
  prop->next = curr_prop;
  prop->prev = NULL;
  strncpy(prop->key, key, strlen(key) + 1);
  strncpy(prop->value, value, strlen(value) + 1);
  return prop;
}

term_t *get_term(void) {
  char line[MAXNAME] = "";
  term_t *term, *tmp;
  char ch;
  int counter = -1;

  if (term_next != NULL) {
    tmp = term_next;
    term_next = NULL;
    return tmp;
  }
  
  term = alloc(sizeof(term_t));
  ch = getch();
  /* 空白の読み捨て */
  while (isspace(ch)) {
    ch = getch();
  }

  if (ch == '<') {
    ch = getch();
    /* term end */
    if (ch == '/') {
      term->type = TERM_END;
      ch = getch();
    }
    /* comment */
    /* else if (ch  == '!') { */
    /*   if (expectch("--")) { */
    /*     term->type = TERM_COMMENT; */
    /*     term->name = str(COMMENT); */
    /*     term = get_comment(); */
    /*     while (true) { */
    /*       ch = getch(); */
    /*       if (expectch("-->")) break; */
    /*     } */
    /*     term->type = TERM_COMMENT; */
    /*   } */
    /* } */
    /* term start */
    else {
      term->type = TERM_START;
    }
    while (true) {
      if (ch == '>') break;
      if (ch == '/') {
        term->type = TERM_ELEMENT;
        getch(); //読み捨て
        break;
      }
      
      /* 属性を入力 */
      if (ch == ' ') {
        term->property = get_property(term->property);
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
    term->name = (char *)alloc(sizeof(char) * (strlen(line) + 1));
    strncpy(term->name, line, strlen(line) + 1);
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
    term->name = (char *)alloc(sizeof(char) * (strlen(line) + 1));
    strncpy(term->name, line, strlen(line) + 1);
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
