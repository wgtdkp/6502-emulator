#include "utility.h"
#include "token.h"
#include "memory.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

#define IS_DIGIT(ch)    ('0' <= (ch) && '9' >= (ch))
#define IS_NUMBER(ch)   (IS_DIGIT(ch)                   \
                      || ('A' <= (ch) && (ch) <= 'F')    \
                      || '<' == (ch)                     \
                      || '>' == (ch)                     \
                      || 'O' == (ch)                     \
                      || '$' == (ch)                     \
                      || '%' == (ch))

#define IS_LETTER(ch)   (('A' <= (ch) && 'Z' >= (ch)) \
                        || '_' == (ch))

#define IS_BLANK(ch)    (' ' == (ch)    \
                      || '\t' == (ch)   \
                      || '\r' == (ch))

#define IS_PUNCTUATION(ch)  (',' == (ch)    \
                          || '#' == (ch)    \
                          || '*' == (ch)    \
                          || '=' == (ch)    \
                          || '(' == (ch)    \
                          || ')' == (ch)    \
                          || '\n' == (ch)) //'\n' is need to be added into a token list to mark the end of line

#define IS_COMMENTS(ch) (';' == (ch))

static int parse_number(const char* liter);;

/*
 * @return: the length of new string.
 */
int trim(char* str)
{
  int begin, end;
  int i, j;
  for (i = 0; 0 != str[i] && IS_BLANK(str[i]);++i) {}
  begin = i;
  for (; 0 != str[i]; ++i) {}
    for (--i; i >= 0 && IS_BLANK(str[i]);--i) {}
  end = i + 1;
  for (i = 0, j = begin; j < end; ++i, ++j)
    str[i] = str[j];
  str[i] = 0;
  return MAX(0, end - begin);
}

/*
as this function use the pointer directly(does not make a copy), it is
not the destroy_token that should free liter.so don't try to create a 
token that its liter is not in the file buffer.
*/
struct token_node* create_token(int type, const char* liter, size_t len)
{
  struct token_node* tok;
  tok = malloc(sizeof(struct token_node));
  if (NULL == tok) {
    error("memory running out!\n");
    return NULL;
  }
  tok->type = type;
  tok->len = len;
  tok->liter = liter;
  tok->next = NULL;
  return tok;
}

void destroy_token(struct token_node** head)
{
  struct token_node* p;
  struct token_node odd;
  odd.next = *head;
  for (p = odd.next; NULL != p; p = nextp) {
    struct token_node* nextp = p->next;
    free(p);
  }
  *head = NULL;
}

void token_append(struct token_list* tk_list, struct token_node* tok_node)
{
  if (NULL == tk_list || NULL == tok_node)
    return;
  if (NULL == tk_list->head)
    tk_list->head = tok_node;
  else
    tk_list->tail->next = tok_node;
  tk_list->tail = tok_node;
}

const struct token_node* token_offset(const struct token_node* head, size_t n)
{
   const struct token_node* p = head;
  for (; 0 != n && NULL != p; --n)
    p = p->next;
  return p;
}

void print_token(const struct token_node* tk)
{
  for (size_t i = 0; i < tk->len; ++i)
    printf("%c", tk->liter[i]);
}

static bool check_number(const char* str, size_t len)
{
  char num_type = 0;
  int stat = 0;
  for (size_t i = 0; i < len; i++) {
    if ('<' == str[i] || '>' == str[i]) {
      if (0 != stat)
        return false;
      stat = '<';
    } else if ('$' == str[i] || 'O' == str[i] || '%' == str[i]) {
      if ('$' == stat || '0' == stat)
        return false;
      num_type = str[i];
      stat = '$';
    } else if (IS_DIGIT(str[i])) {
      if (('%' == num_type) && ('0' != str[i] && '1' != str[i]))
        return false;
      else if (('O' == num_type) && !IS_DIGIT(str[i]))
        return false;
      stat = '0';
    } else if ('A' <= str[i] && 'F' >= str[i]) {
      if ('$' != num_type)
        return false;
      stat = '0';
    } else {
      break;
    }
  }
  return '0' == stat;    
}

static int parse_number(const char* liter)
{
  size_t i = 0;
  int res;
  char hl = 0;
  int base = 10;

  if ('<' == liter[i]) {
    hl = 'L';
    ++i;
  } else if ('>' == liter[i]) {
    hl = 'H';
    ++i;
  }

  if ('$' == liter[i]) {
    base = 16;
    ++i;
  } else if ('%' == liter[i]) {
    base = 2;
    ++i;
  } else if ('O' == liter[i]) {
    base = 8;
    ++i;
  }

  res = strtol(liter + i, NULL, base);
  if ('L' == hl)
    res = L(res);
  else if ('H' == hl)
    res = H(res);
  return res;
}

enum Status {
  STATUS_INVALID = 0,
  STATUS_LETTER,
  STATUS_PUNCTUATION,
  STATUS_NUMBER,
  STATUS_BLANK,
  STATUS_COMMENTS,
  STATUS_PRAGMA,
};

static enum Status next_status(enum Status current_status, char ch)
{
  enum Status next_status;
  if ('\n' == ch)
    return STATUS_PUNCTUATION;
  else if (STATUS_COMMENTS == current_status)
    return STATUS_COMMENTS;

  if (IS_LETTER(ch))
    next_status = STATUS_LETTER;
  else if ('.' == ch)
    next_status = STATUS_PRAGMA;
  else if (IS_PUNCTUATION(ch))
    next_status = STATUS_PUNCTUATION;
  else if (IS_NUMBER(ch))
    next_status = STATUS_NUMBER;
  else if (IS_BLANK(ch))
    next_status = STATUS_BLANK;
  else if (IS_COMMENTS(ch))
    next_status = STATUS_COMMENTS;
  else
    next_status = STATUS_INVALID;
  return next_status;
}

static int letter_type(const char* liter, size_t len)
{
  if (3 == len && is_inst(liter)) {
    return TOKEN_INST;
  } else if (1 == len) {
    if ('A' == liter[0])
      return TOKEN_SYMB_A;
    else if ('X' == liter[0])
      return TOKEN_SYMB_X;
    else if ('Y' == liter[0])
      return TOKEN_SYMB_Y;
  }
  return TOKEN_LABEL;
}

static int pragma_type(const char* liter, size_t len)
{
  if (0 == strncmp(".BYTE", liter, 5))
    return TOKEN_PRAGMA_BYTE;
  else if (0 == strncmp(".WORD", liter, 5))
    return TOKEN_PRAGMA_WORD;
  else if (0 == strncmp(".END", liter, 4))
    return TOKEN_PRAGMA_END;
  return -1;
}

int tokenize(struct token_list* tk_list, char* file_buffer)
{
  enum Status status;
   line_num;
  size_t token_begin, token_end;
  token_begin = 0, token_end  = 0;
  status = STATUS_INVALID;
  str_toupper(file_buffer);
  /*
   * Careful: it seems an error to let "i <= len", 
   * but we need one more execution to flush the last token into token list.
   */
  size_t line_num = 1;
  for (size_t i = 0; ; ++i) {
    struct token_node* tok_node;
    switch (status) {
    case STATUS_LETTER:
      if (!IS_LETTER(file_buffer[i]) && !IS_DIGIT(file_buffer[i])) {
        token_end = i;
        tok_node = create_token(TOKEN_LABEL, file_buffer + token_begin, token_end - token_begin);
        tok_node->type = letter_type(tok_node->liter, tok_node->len);
        token_append(tk_list, tok_node);
        token_begin = i;
        status = next_status(status, file_buffer[i]);
      }
      break;
    case STATUS_PRAGMA:
      if (!IS_LETTER(file_buffer[i])) {
        int type;
        token_end = i;
        type = pragma_type(file_buffer + token_begin, token_end - token_begin);
        if (type < 0) {
          error("invalid pragma ad line %d\n", line_num);
          return -4;
        }
        tok_node = create_token(type, file_buffer + token_begin, token_end - token_begin);
        token_append(tk_list, tok_node);
        token_begin = i;
        status = next_status(status, file_buffer[i]);
      }
      break;
    case STATUS_PUNCTUATION:
      token_end = i;
      tok_node = create_token(file_buffer[token_begin], file_buffer + token_begin, token_end - token_begin);
      token_append(tk_list, tok_node);
      token_begin = i;
      status = next_status(status, file_buffer[i]);
      break;
    case STATUS_NUMBER:
      if (!IS_NUMBER(file_buffer[i])) {
        token_end = i;
        if (!check_number(file_buffer + token_begin, token_end - token_begin)) {
          error("invalid number format at line %d\n", line_num);
          return -2;
        }
        tok_node = create_token(TOKEN_NUMBER, file_buffer + token_begin, token_end - token_begin);
        tok_node->data = parse_number(tok_node->liter);
        token_append(tk_list, tok_node);
        token_begin = i;
        status = next_status(status, file_buffer[i]);
      }
      break;
    case STATUS_BLANK:
      if (!IS_BLANK(file_buffer[i])) {
        token_begin = i;
        status = next_status(status, file_buffer[i]);
      }
      break;
    case STATUS_COMMENTS:
      //once status is in comments, it will always be in comments
      if ('\n' == file_buffer[i]) {
        token_begin = i;
        status = next_status(status, file_buffer[i]);
      }
      break;
    case STATUS_INVALID:
      token_begin = i;
      status = next_status(status, file_buffer[i]);
      if (STATUS_INVALID == status && 0 != file_buffer[i]) {
        error("invalid format at line %d\n", line_num);
        return -3;
      }
      break;
    }
    if (0 == file_buffer[i])
      break;
    else if ('\n' == file_buffer[i])
      ++line_num;
  }
  return 0;
}

void str_toupper(char* str)
{
  for (size_t i = 0; 0 != str[i]; ++i) {
    if ('a' <= str[i] && 'z' >= str[i])
      str[i] = 'A' + str[i] - 'a';
  }
}
