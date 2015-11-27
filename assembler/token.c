#include "utility.h"
#include "token.h"
#include "memory.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

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
	--i;
    for (; i >= 0 && IS_BLANK(str[i]);) --i;
	end = i;
    for (i = 0, j = begin; j <= end; i++, j++)
        str[i] = str[j];
    str[i] = 0;

    return MAX(0, end - begin + 1);
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
static int check_number(const char* str, size_t* len)
{
    char num_type = 0;
    int stat = 0;
    size_t i;
    for (i = 0; 0 != str[i]; i++) {
        if ('<' == str[i] || '>' == str[i]) {
            if (0 != stat)
                return -2;
            stat = '<';
        } else if ('$' == str[i] || 'O' == str[i] || '%' == str[i]) {
            if ('$' == stat || '0' == stat)
                return -3;
            num_type = str[i];
            stat = '$';
        } else if (IS_NUM(str[i])) {
            if (('%' == num_type) && ('0' != str[i] && '1' != str[i]))
                return -4;
            else if (('O' == num_type) && !IS_NUM(str[i]))
                return -5;
            stat = '0';
        } else if ('A' <= str[i] && 'F' >= str[i]) {
            if ('$' != num_type)
                return -6;
            stat = '0';
        } else {
            break;
        }
    }
    if ('0' != stat)
        return -7;
	*len = i;
    return 0;
}

int tokenize(struct token_list* tok_list, char* line, int line_num)
{
    size_t i, len;

    len = trim(line);
    str_toupper(line);
    for (i = 0; i < len;) {
        struct token_node* tok_node;
        if (IS_NUM_PREFIX(line[i])) {
            size_t len_tok = 0;
            if (0 != check_number(line + i, &len_tok)) {
                error("invalid number format at line %d, column %d.\n", line_num, i + 1);
                return -2;
            }

            tok_node = create_token(TOKEN_NUM, line + i, len_tok);
            token_append(tok_list, tok_node);
            i += len_tok;
        } else if (IS_LETTER(line[i])) {
            size_t begin, end;
            for (begin = i; i < len && !IS_BLANK(line[i]); i++) {
				if (!IS_LETTER(line[i]) && !IS_NUM(line[i]))
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

void str_toupper(char* str)
{
    size_t i;
    for (i = 0; 0 != str[i]; i++) {
        if ('a' <= str[i] && 'z' >= str[i])
            str[i] = 'A' + str[i] - 'a';
    }
}
