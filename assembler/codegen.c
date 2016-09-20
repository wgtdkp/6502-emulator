#include "codegen.h"
#include "parser.h"
#include "token.h"

uint8_t gen_code(byte code[3], const struct token_inst* tk_inst)
{
  uint8_t code_len;
  if (NULL == code)
    return 0;
  if (PRAGMA_BYTE == tk_inst->op_code) {
    code_len = 1;
  } else if (PRAGMA_WORD == tk_inst->op_code) {
    code_len = 2;
  } else {
    code_len = get_inst(tk_inst->op_code)->len;
  }

  if (PRAGMA_BYTE == tk_inst->op_code) {
    code_len = 1;
    code[0] = tk_inst->lo;
  } else if (PRAGMA_WORD == tk_inst->op_code) {
    code_len = 2;
    code[0] = tk_inst->lo;
    code[1] = tk_inst->hi;
  } else {
    code_len = get_inst(tk_inst->op_code)->len;
    code[0] = tk_inst->op_code;
    if (2 <= code_len)
      code[1] = tk_inst->lo;
    if (3 <= code_len)
      code[2] = tk_inst->hi;
  }
  return code_len;
}
