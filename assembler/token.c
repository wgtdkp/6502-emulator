#include "utility.h"
#include "token.h"
#include "memory.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

#define IS_DIGIT(ch)    ('0' <= (ch) && '9' >= (ch))
#define IS_NUMBER(ch) (IS_DIGIT(ch) \
                    || ('A' <= (ch) && (ch) <= 'F') \
                    || '<' == (ch) \
                    || '>' == (ch) \
                    || 'O' == (ch) \
                    || '$' == (ch) \
                    || '%' == (ch))
#define IS_LETTER(ch)   (('A' <= (ch) && 'Z' >= (ch)) \
                        || '_' == (ch))
#define IS_BLANK(ch)    (' ' == (ch) \
                    || '\t' == (ch) \
                    || '\n' == (ch) \
                    || '\r' == (ch))
#define IS_PUNCTUATION(ch) (',' == (ch) \
                        || '#' == (ch) \
                        || '*' == (ch) \
                        || '=' == (ch)) \
                        || '(' == (ch) \
                        || ')' == (ch) 
#define IS_COMMENTS(ch) (';' == (ch))

static const char valid_chars[] = ".()#<>$%O,;*=";

static bool is_valid_char(char ch)
{
    size_t i;
    for (i = 0; 0 != valid_chars[i]; i++) {
        if (valid_chars[i] == ch)
            return true;
    }
    return false;
}

/*
 * @return: the length of new string.
 */
int trim(char* str)
{
    int begin, end;
    int i, j;
    for (i = 0; 0 != str[i] && IS_BLANK(str[i]);) ++i;
    begin = i;
	for (; 0 != str[i]; i++);
    for (--i; i >= 0 && IS_BLANK(str[i]);) --i;
	end = i + 1;
    for (i = 0, j = begin; j < end; i++, j++)
        str[i] = str[j];
    str[i] = 0;

    return MAX(0, end - begin);
}

struct token_node* create_token(int type, const char* liter, size_t len)
{
    struct token_node* tok;
    tok = (struct token_node*)malloc(sizeof(struct token_node));
    if (NULL == tok)
        return NULL;
    tok->liter = (char*)malloc(sizeof(char) * (len + 1));
    if (NULL == tok->liter) {
        free(tok);
        return NULL;
    }
    tok->type = type;
    tok->len = len;
    memcpy(tok->liter, liter, len);
	tok->liter[len] = 0;
    tok->next = NULL;
    return tok;
}

void destroy_token(struct token_node** head)
{
    struct token_node* p;
    struct token_node odd;
    odd.next = *head;
    for (p = odd.next; NULL != p;) {
		struct token_node* tmp = p->next;
		free(p->liter);
        free(p);
		p = tmp;
    }
    *head = NULL;
}

void token_append(struct token_list* tok_list, struct token_node* tok_node)
{
    if (NULL == tok_list || NULL == tok_node)
        return;
    if (NULL == tok_list->head)
        tok_list->head = tok_node;
    else
        tok_list->tail->next = tok_node;
    tok_list->tail = tok_node;
    ++tok_list->size;
}

struct token_node* token_offset(struct token_node* head, size_t n)
{
    struct token_node* p = head;
    for (; 0 != n && NULL != p; n--)
        p = p->next;
    return p;
}


/*
 *@return: negative, error; 0, no error;
 */
static bool check_number(const char* str, size_t len)
{
    char num_type = 0;
    int stat = 0;
    size_t i;
    for (i = 0; i < len; i++) {
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
    return ('0' == stat);    
}

enum Status {
    STATUS_INVALID = 0,
    STATUS_LETTER,
    STATUS_PUNCTUATION,
    STATUS_NUMBER,
    STATUS_BLANK,
    STATUS_COMMENTS,
};

static enum Status next_status(enum Status current_status, char ch)
{
    enum Status next_status;
    if (STATUS_COMMENTS == current_status)
        return STATUS_COMMENTS;

    if (IS_LETTER(ch))
        next_status = STATUS_LETTER;
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

static int letter_type(const char* liter, int len)
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

int tokenize(struct token_list* tok_list, char* line, int line_num)
{
    enum Status status;
    size_t i, len;
    size_t token_begin, token_end;
    //len = trim(line);
    
    token_begin = 0, token_end  = 0;
    status = STATUS_INVALID;
	len = strlen(line);
	str_toupper(line);
    /*careful: it seems an error to let "i <= len", 
      but we need one more execution to flush the last token into token_list */
    for (i = 0; i <= len; i++) {
		struct token_node* tok_node;
        switch (status) {
        case STATUS_LETTER:
            if (!IS_LETTER(line[i]) && !IS_DIGIT(line[i])) {
                token_end = i;
                tok_node = create_token(TOKEN_LABEL, line + token_begin, token_end - token_begin);
				tok_node->type = letter_type(tok_node->liter, tok_node->len);
				token_append(tok_list, tok_node);
                token_begin = i;
				status = next_status(status, line[i]);
            }
            break;
        case STATUS_PUNCTUATION:
            token_end = i;
            tok_node = create_token(line[token_begin], line + token_begin, token_end - token_begin);
            token_append(tok_list, tok_node);
            token_begin = i;
            status = next_status(status, line[i]);
            break;
        case STATUS_NUMBER:
            if (!IS_NUMBER(line[i])) {
                token_end = i;
                if (!check_number(line + token_begin, token_end - token_begin)) {
                    error("invalid number format at line %d, column %d.\n", line_num, i + 1);
                    return -2;
                }
                tok_node = create_token(TOKEN_NUMBER, line + token_begin, token_end - token_begin);
                token_append(tok_list, tok_node);
                token_begin = i;
				status = next_status(status, line[i]);
            }
            break;
        case STATUS_BLANK:
            if (!IS_BLANK(line[i])) {
                token_begin = i;
				status = next_status(status, line[i]);
            }
            break;
        case STATUS_COMMENTS:
            //once status is in comments, it will always be in comments
            break;
        case STATUS_INVALID:
            token_begin = i;
            status = next_status(status, line[i]);
			if (STATUS_INVALID == status && 0 != line[i]) {
				error("invalid format at line %d\n", line_num);
				return -3;
			}
            break;
        }
    }

    return 0;
}

/*
int tokenize(struct token_list* tok_list, char* line, int line_num)
{
    size_t i, len;

    len = trim(line);
    str_toupper(line);
    for (i = 0; i < len;) {
        struct token_node* tok_node;
        if (IS_NUMBER(line[i])) {
            size_t len_tok = 0;
            if (0 != check_number(line + i, &len_tok)) {
                error("invalid number format at line %d, column %d.\n", line_num, i + 1);
                return -2;
            }

            tok_node = create_token(TOKEN_NUMBER, line + i, len_tok);
            token_append(tok_list, tok_node);
            i += len_tok;
        } else if (IS_LETTER(line[i])) {
            size_t begin, end;
            for (begin = i; i < len && !IS_BLANK(line[i]); i++) {
				if (!IS_LETTER(line[i]) && !IS_DIGIT(line[i]))
					goto ERROR;
			}
            end = i;
            tok_node = create_token(TOKEN_LABEL, line + begin, end - begin);
            if (1 == tok_node->len) {
                if ('A' == tok_node->liter[0])
                    tok_node->type = TOKEN_SYMB_A;
                else if ('X' == tok_node->liter[0])
                    tok_node->type = TOKEN_SYMB_X;
                else if ('Y' == tok_node->liter[0])
                    tok_node->type = TOKEN_SYMB_Y;
            } else if (3 == tok_node->len && is_inst(tok_node->liter))
                tok_node->type = TOKEN_INST;
            token_append(tok_list, tok_node);
        } else if (';' == line[i]) {
            i = len;
        } else if ('.' == line[i]) {
            size_t begin, end;
            int token_type = TOKEN_NULL;
            ++i;
            for (begin = i; i < len && IS_LETTER(line[i]); i++) {}
            end = i;
            if (3 != end - begin && 4 != end - begin)
                goto ERROR;
            if (0 == strncmp("END", line + begin, 3))
                token_type = TOKEN_PRAGMA_END;
            else if (0 == strncmp("BYTE", line + begin, 4))
                token_type = TOKEN_PRAGMA_BYTE;
            else if (0 == strncmp("WORD", line + begin, 4))
                token_type = TOKEN_PRAGMA_WORD;
            else 
                goto ERROR;
            tok_node = create_token(token_type, line + begin, end - begin);
            token_append(tok_list, tok_node);
        } else if (is_valid_char(line[i])) {
            tok_node = create_token(line[i], line + i, 1);
            token_append(tok_list, tok_node);
            ++i;
		} else if (IS_BLANK(line[i])) {
			++i;
		} else {
            error("invalid character '%d' at line %d, column %d.\n", 
                line[i], line_num, i + 1);
            return -1;
        }
    }
    return 0;
ERROR:
    error("invalid format at line %d\n", line_num);
    return -3;
}
*/

void str_toupper(char* str)
{
    size_t i;
    for (i = 0; 0 != str[i]; i++) {
        if ('a' <= str[i] && 'z' >= str[i])
            str[i] = 'A' + str[i] - 'a';
    }
}
