
#include "term.h"
#include "io.h"
#include "base.h"

#define COMMENT "comment"

/* 次の要素 */
term_t *term_next = NULL;

/* XML の version とエンコーディングを読み込む */
void set_encoding(int *version, char **encoding) {
  char line[MAXLINE];
  char text[MAXLINE];
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
    ret = sscanf(line, "<?xml version=\"%lf\" encoding=\"%s\" ?>", version, text);
    if (ret < 0) {
        err_exit("illegal version format");
    }
  }
  else {
    err_exit("'<'が抜けています.\n");
  }
  *encoding = str(text);
}

/* doctype宣言の読込 */
char *get_doctype() {
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
  return str(line);
}

TInfo_t *get_info() {
  TInfo_t *info;
  info = (TInfo_t *)alloc(TInfo_t);
  set_encoding(&info->encoding, &info->version);
  info->doctype = get_doctype();
  return info;
}

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
  prop->key = str(key)
  prop->value = str(value)
  prop->next = curr_prop;
  prop->prev = NULL;
  return prop;
}

token_t *get_token() {
  char line[MAXNAME] = "";
  TAttr *attr, *tmp;
  char ch;
  int counter = -1;

  if (attr_next != NULL) {
    tmp = attr_next;
    attr_next = NULL;
    return tmp;
  }
  
  attr = alloc(sizeof(attr_t));
  ch = getch();
  /* 空白の読み捨て */
  while (isspace(ch)) {
    ch = getch();
  }

  if (ch == '<') {
    ch = getch();
    /* attr end */
    if (ch == '/') {
      attr->type = TOKEN_END;
      ch = getch();
    }
    /* comment */
    else if (ch  == '!') {
      if (expectch("--")) {
        token->type = TOKEN_COMMENT;
        attr->name = str(COMMENT);
        attr = get_comment();
        while (true) {
          ch = getch();
          if (expectch("-->")) break;
        }
        token->type = TOKEN_COMMENT;
      }
    }
    /* attr start */
    else {
      token->type = TOKEN_START;
    }
    while (true) {
      if (ch == '>') break;
      if (ch == '/') {
        attr->type = TOKEN_ELEMENT;
        getch(); //読み捨て
        break;
      }
      
      /* 属性を入力 */
      if (ch == ' ') {
        attr->property = get_property(attr->property);
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
    attr->name = (char *)alloc(sizeof(char) * (strlen(line) + 1));
    strncpy(attr->name, line, strlen(line) + 1);
  }
  else if (ch == EOF) {
    attr->type = TOKEN_EOF;
  }
  else if (isprint(ch)) {
    attr->type = TOKEN_CONTENT;
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
    attr->name = (char *)alloc(sizeof(char) * (strlen(line) + 1));
    strncpy(attr->name, line, strlen(line) + 1);
  }
  else {
    /* ありえないことが起こった */
    err_exit("英数字以外の文字は読み込めません.\n");
  }
  return attr;
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
