#include "type.h"
#include "parser.h"
#include "utility.h"
#include "token.h"
#include "symbol.h"
#include "codegen.h"
#include "hex_handler.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define MAX_LINE_LEN    (255)
#define TERMINAL    (-4)

#define NOT_INST    (0xFF)
#define INST_NUM    (56)
#define MODE_NUM    (13)

#define TO_LITER_CODE(str)  (str)

static addr_t pc = 0;

static const byte liters[INST_NUM][4] = {
  TO_LITER_CODE("ADC"),
  TO_LITER_CODE("AND"),
  TO_LITER_CODE("ASL"),
  TO_LITER_CODE("BCC"),
  TO_LITER_CODE("BCS"),

  TO_LITER_CODE("BEQ"),
  TO_LITER_CODE("BIT"),
  TO_LITER_CODE("BMI"),
  TO_LITER_CODE("BNE"),
  TO_LITER_CODE("BPL"),

  TO_LITER_CODE("BRK"),
  TO_LITER_CODE("BVC"),
  TO_LITER_CODE("BVS"),
  TO_LITER_CODE("CLC"),
  TO_LITER_CODE("CLD"),

  TO_LITER_CODE("CLI"),
  TO_LITER_CODE("CLV"),
  TO_LITER_CODE("CMP"),
  TO_LITER_CODE("CPX"),
  TO_LITER_CODE("CPY"),

  TO_LITER_CODE("DEC"),
  TO_LITER_CODE("DEX"),
  TO_LITER_CODE("DEY"),
  TO_LITER_CODE("EOR"),
  TO_LITER_CODE("INC"),

  TO_LITER_CODE("INX"),
  TO_LITER_CODE("INY"),
  TO_LITER_CODE("JMP"),
  TO_LITER_CODE("JSR"),
  TO_LITER_CODE("LDA"),

  TO_LITER_CODE("LDX"),
  TO_LITER_CODE("LDY"),
  TO_LITER_CODE("LSR"),
  TO_LITER_CODE("NOP"),
  TO_LITER_CODE("ORA"),

  TO_LITER_CODE("PHA"),
  TO_LITER_CODE("PHP"),
  TO_LITER_CODE("PLA"),
  TO_LITER_CODE("PLP"),
  TO_LITER_CODE("ROL"),

  TO_LITER_CODE("ROR"),
  TO_LITER_CODE("RTI"),
  TO_LITER_CODE("RTS"),
  TO_LITER_CODE("SBC"),
  TO_LITER_CODE("SEC"),

  TO_LITER_CODE("SED"),
  TO_LITER_CODE("SEI"),
  TO_LITER_CODE("STA"),
  TO_LITER_CODE("STX"),
  TO_LITER_CODE("STY"),

  TO_LITER_CODE("TAX"),
  TO_LITER_CODE("TAY"),
  TO_LITER_CODE("TSX"),
  TO_LITER_CODE("TXA"),
  TO_LITER_CODE("TXS"),

  TO_LITER_CODE("TYA"),
};

static const byte op_codes[INST_NUM][MODE_NUM] = {
/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*ADC*/ {0xFF, 0x69, 0x65, 0x75, 0xFF, 0x6D, 0x7D, 0x79, 0xFF, 0xFF, 0x61, 0x71, 0xFF},
/*AND*/ {0xFF, 0x69, 0x65, 0x75, 0xFF, 0x6D, 0x7D, 0x79, 0xFF, 0xFF, 0x61, 0x71, 0xFF},
/*ASL*/ {0x0A, 0xFF, 0x06, 0x16, 0xFF, 0x0E, 0x1E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*BCC*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x90, 0xFF, 0xFF, 0xFF},
/*BCS*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB0, 0xFF, 0xFF, 0xFF},

/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*BEQ*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF},
/*BIT*/ {0xFF, 0xFF, 0x24, 0xFF, 0xFF, 0x2C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*BMI*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x30, 0xFF, 0xFF, 0xFF},
/*BNE*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xD0, 0xFF, 0xFF, 0xFF},
/*BPL*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x10, 0xFF, 0xFF, 0xFF},

/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*BRK*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF},
/*BVC*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x50, 0xFF, 0xFF, 0xFF},
/*BVS*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x70, 0xFF, 0xFF, 0xFF},
/*CLC*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x18, 0xFF, 0xFF, 0xFF, 0xFF},
/*CLD*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xD8, 0xFF, 0xFF, 0xFF, 0xFF},

/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*CLI*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x58, 0xFF, 0xFF, 0xFF, 0xFF},
/*CLV*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF},
/*CMP*/ {0xFF, 0xC9, 0xC5, 0xD5, 0xFF, 0xCD, 0xDD, 0xD9, 0xFF, 0xFF, 0xC1, 0xD1, 0xFF},
/*CPX*/ {0xFF, 0xE0, 0xE4, 0xFF, 0xFF, 0xEC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*CPY*/ {0xFF, 0xC0, 0xC4, 0xFF, 0xFF, 0xCC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},

/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*DEC*/ {0xFF, 0xFF, 0xC6, 0xD6, 0xFF, 0xCE, 0xDE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*DEX*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCA, 0xFF, 0xFF, 0xFF, 0xFF},
/*DEY*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x88, 0xFF, 0xFF, 0xFF, 0xFF},
/*EOR*/ {0xFF, 0x49, 0x45, 0x55, 0xFF, 0x4D, 0x5D, 0x59, 0xFF, 0xFF, 0x41, 0x51, 0xFF},
/*INC*/ {0xFF, 0xFF, 0xE6, 0xF6, 0xFF, 0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},

/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*INX*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE8, 0xFF, 0xFF, 0xFF, 0xFF},
/*INY*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC8, 0xFF, 0xFF, 0xFF, 0xFF},
/*JMP*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0X4C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0X6C},
/*JSR*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*LDA*/ {0xFF, 0xA9, 0xA5, 0xB5, 0xFF, 0xAD, 0xBD, 0xB9, 0xFF, 0xFF, 0xA1, 0xB1, 0xFF},

/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*LDX*/ {0xFF, 0xA2, 0xA6, 0xFF, 0xB6, 0xAE, 0xFF, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*LDY*/ {0xFF, 0xA0, 0xA4, 0xB4, 0xFF, 0xAC, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*LSR*/ {0x4A, 0xFF, 0x46, 0x56, 0xFF, 0x4E, 0x5E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*NOP*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEA, 0xFF, 0xFF, 0xFF, 0xFF},
/*ORA*/ {0xFF, 0x09, 0x05, 0x15, 0xFF, 0x0D, 0x1D, 0x19, 0xFF, 0xFF, 0x01, 0x11, 0xFF},

/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*PHA*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x48, 0xFF, 0xFF, 0xFF, 0xFF},
/*PHP*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x08, 0xFF, 0xFF, 0xFF, 0xFF},
/*PLA*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x68, 0xFF, 0xFF, 0xFF, 0xFF},
/*PLP*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x28, 0xFF, 0xFF, 0xFF, 0xFF},
/*ROL*/ {0x2A, 0xFF, 0x26, 0x36, 0xFF, 0x2E, 0x3E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},

/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*ROR*/ {0x6A, 0xFF, 0x66, 0x76, 0xFF, 0x6E, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*RTI*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x40, 0xFF, 0xFF, 0xFF, 0xFF},
/*RTS*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x60, 0xFF, 0xFF, 0xFF, 0xFF},
/*SBC*/ {0xFF, 0xE9, 0xE5, 0xF5, 0xFF, 0xED, 0xFD, 0xF9, 0xFF, 0xFF, 0xE1, 0xF1, 0xFF},
/*SEC*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x38, 0xFF, 0xFF, 0xFF, 0xFF},

/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*SED*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF},
/*SEI*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x78, 0xFF, 0xFF, 0xFF, 0xFF},   
/*STA*/ {0xFF, 0xFF, 0x85, 0x95, 0xFF, 0x8D, 0x9D, 0x99, 0xFF, 0xFF, 0x81, 0x91, 0xFF},
/*STX*/ {0xFF, 0xFF, 0x86, 0xFF, 0x96, 0x8E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*STY*/ {0xFF, 0xFF, 0x84, 0x94, 0xFF, 0x8C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},

/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*TAX*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF},
/*TAY*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xA8, 0xFF, 0xFF, 0xFF, 0xFF},
/*TSX*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBA, 0xFF, 0xFF, 0xFF, 0xFF},
/*TXA*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x8A, 0xFF, 0xFF, 0xFF, 0xFF},
/*TXS*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9A, 0xFF, 0xFF, 0xFF, 0xFF},

/********{ACC, IMM,  ZERO, Z_X,  Z_Y,  ABS, ABS_X, ABS_Y, IMP, REL, IND_X, IND_Y, IND}*/
/*TYA*/ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x98, 0xFF, 0xFF, 0xFF, 0xFF},
};

static int pass(const struct token_list* tk_list, FILE* code_file);

#define INST_LITER_TO_INT(liter) (liter[0] << 16 | liter[1] << 8 | liter[2])
/*
 *@return:-1 stands failure;
 */
int find_inst(const char* liter)
{
  uint32_t x = INST_LITER_TO_INT(liter);
  int begin = 0, end = INST_NUM - 1;
  while (begin <= end) {
    size_t middle = (end - begin) / 2 + begin;
    uint32_t mid_val = INST_LITER_TO_INT(liters[middle]);
    if (x == mid_val)
      return middle;
    else if (x > mid_val)
      begin = middle + 1;
    else
      end = middle - 1;
  }
  return -1;
}

static bool match(struct token_list* tk_list, ...)
{
  byte type;
  va_list args;
  va_start(args, tk_list);
  type = va_arg(args, uint32_t);
  struct token_node* p;
  for (p = tk_list->head; p != tk_list->tail; p = p->next) {
    if (TOKEN_LABEL == p->type && TOKEN_NUMBER == type) {//treats equal
      struct symb_node* label;
      label = symb_find(p->liter, p->len);
      if (NULL != label) {
        p->data = label->data;
      }
    } else if (0 == type || type != p->type) {
      return false;
    }
    type = va_arg(args, uint32_t);
  }
  return (0 == type && tk_list->tail == p);   //both of them are NULL(0)
}

static int build_symb_tb(const struct token_list* tk_list)
{
  size_t line_num;
  ASSERT(tk_list != 0 && tk_list->head != 0);
  struct token_node* head = tk_list->head;
  struct token_node* tail = head;
  line_num = 1;
  for (; head != NULL;) {
    struct token_list tk_line;
    while ('\n' != tail->type) 
      tail = tail->next;
    tk_line.head = head;
    tk_line.tail = tail;

    if (match(&tk_line, TOKEN_LABEL, '=', TOKEN_NUMBER, TOKEN_NULL)) {
      //TODO: insert into symbol table
      if (NULL != symb_find(head->liter, head->len)) {
        error("Line %d, symbol redefined.\n", line_num);
        print_symb_tb();
        return -1;
      }
      symb_insert(head->liter, head->len, token_offset(head, 2)->data);
    } else if (TOKEN_LABEL == head->type) {
      //TODO: insert into symbol table
      //TODO: check if any instruction followed
      if (NULL != symb_find(head->liter, head->len)) {
        error("Line %d, symbol redefined.\n", line_num);
        return -1;
      }
      symb_insert(head->liter, head->len, pc);   //wrong data! pc is always zero
      //return gen_inst(inst, head->next, line_num);
    }
    ++line_num;
    while (NULL != tail && '\n' == tail->type) {
      tail = tail->next;
    }
    head = tail;
  }
  print_symb_tb();
  return 0;	
}

static inline bool is_branch(struct token_node* inst)
{
  if ('B' != inst->liter[0])
    return false;
  if ('I' == inst->liter[1] || 'R' == inst->liter[1])
    return false;
  return true;
}

static int gen_inst(struct token_inst* inst, struct token_list* tk_list, size_t line_num)
{
  if (!tk_list || !tk_list->head)
    return 0; 

  if (TOKEN_INST == tk_list->head->type) {
    uint8_t mode;
    int inst_index = find_inst(tk_list->head->liter);

    if (match(tk_list, TOKEN_INST, TOKEN_NULL)) {
      mode = ADDR_IMP;
    } else if (match(tk_list, TOKEN_INST, TOKEN_SYMB_A, TOKEN_NULL)) {
      mode = ADDR_ACC;
    } else if (match(tk_list, TOKEN_INST, '#', TOKEN_NUMBER, TOKEN_NULL)) {
      mode = ADDR_IMM;
      inst->lo = (byte)(token_offset(tk_list->head, 2)->data);
    } else if (match(tk_list, TOKEN_INST, TOKEN_NUMBER, TOKEN_NULL)) {
      uint16_t data = token_offset(tk_list->head, 1)->data;
      if (0 == is_branch(tk_list->head)) {
        mode = ADDR_ABS;
        inst->operand = data;
      } else {
        mode = ADDR_REL;
        inst->lo = data - (pc + 2);	//branch instructions are all 2 bytes len
      }
    } else if (match(tk_list, TOKEN_INST, TOKEN_NUMBER, ',', TOKEN_SYMB_X, TOKEN_NULL)) {
      mode = ADDR_ABS_X;
      inst->operand = token_offset(tk_list->head, 1)->data;
    } else if (match(tk_list, TOKEN_INST, TOKEN_NUMBER, ',', TOKEN_SYMB_Y, TOKEN_NULL)) {
      mode = ADDR_ABS_Y;
      inst->operand = token_offset(tk_list->head, 1)->data;
    } else if (match(tk_list, TOKEN_INST, '*', TOKEN_NUMBER, TOKEN_NULL)) {
      mode = ADDR_ZERO;
      inst->lo = (byte)(token_offset(tk_list->head, 2)->data);
    } else if (match(tk_list, TOKEN_INST, '*', TOKEN_NUMBER, ',', TOKEN_SYMB_X, TOKEN_NULL)) {
      mode = ADDR_ZERO_X;
      inst->lo = (byte)(token_offset(tk_list->head, 2)->data);
    } else if (match(tk_list, TOKEN_INST, '*', TOKEN_NUMBER, ',', TOKEN_SYMB_Y, TOKEN_NULL)) {
      mode = ADDR_ZERO_Y;
      inst->lo = (byte)(token_offset(tk_list->head, 2)->data);
    } else if (match(tk_list, TOKEN_INST, '(', TOKEN_NUMBER, ',', TOKEN_SYMB_X, ')', TOKEN_NULL)) {
      mode = ADDR_IND_X;
      inst->lo = (byte)(token_offset(tk_list->head, 2)->data);
    } else if (match(tk_list, TOKEN_INST, '(', TOKEN_NUMBER, ')', ',', TOKEN_SYMB_Y, TOKEN_NULL)) {
      mode = ADDR_IND_Y;
      inst->lo = (byte)(token_offset(tk_list->head, 2)->data);
    } else if (match(tk_list, TOKEN_INST, '(', TOKEN_NUMBER, ')', TOKEN_NULL)) {
      mode = ADDR_IND;
      inst->operand = token_offset(tk_list->head, 2)->data;
    } else {
      error("Line %d, invalid format of instruction\n", line_num);
      return -2;
    }
    byte op_code = op_codes[inst_index][mode];
    if (NOT_INST == op_code)
      goto INST_ADDRESSING_ERROR;
    inst->op_code = op_code;
  } else if (match(tk_list, '*', '=', TOKEN_NUMBER, TOKEN_NULL)) {
      inst->op_code = PSEUDO_SPC;
      inst->operand = token_offset(tk_list->head, 2)->data;
    } else if (match(tk_list, TOKEN_PRAGMA_BYTE, TOKEN_NUMBER, TOKEN_NULL)) {
      inst->op_code = PRAGMA_BYTE;
      inst->lo = (uint8_t)(token_offset(tk_list->head, 1)->data);
    } else if (match(tk_list, TOKEN_PRAGMA_WORD, TOKEN_NUMBER, TOKEN_NULL)) {
      inst->op_code = PRAGMA_WORD;
      inst->operand = token_offset(tk_list->head, 1)->data;
    } else if (match(tk_list, TOKEN_PRAGMA_END, TOKEN_NULL)) {
    return TERMINAL;
  } else if (match(tk_list, TOKEN_LABEL, '=', TOKEN_NUMBER, TOKEN_NULL)) {
    //TODO: insert into symbol table
    uint16_t data = token_offset(tk_list->head, 2)->data;
    symb_insert(tk_list->head->liter, tk_list->head->len, data);
    inst->op_code = NOT_INST;
  } else if (match(tk_list, TOKEN_LABEL, '=', '*', TOKEN_NULL)) {
    //TODO: insert into symbol table
    symb_insert(tk_list->head->liter, tk_list->head->len, pc);
    inst->op_code = NOT_INST;
  }else if (TOKEN_LABEL == tk_list->head->type) {
    //TODO: insert into symbol table
    //TODO: check if any instruction followed
    symb_insert(tk_list->head->liter, tk_list->head->len, pc);
    tk_list->head = tk_list->head->next;
      return gen_inst(inst, tk_list, line_num);
  } else  {
    error("Line %d, invalid format\n", line_num);
    return -1;
  }
  return 0;
INST_ADDRESSING_ERROR:
  error("Line %d, instrction addressing mode error\n", line_num);
  return -2;
}

static char* file_buffer = NULL;
static bool load_file(FILE* src_file)
{
  long file_len;
  fseek(src_file, 0, SEEK_SET);
  fseek(src_file, 0, SEEK_END );
  file_len = ftell(src_file);
  fseek(src_file, 0, SEEK_SET );

  file_buffer = (char*)malloc(sizeof(char) * (file_len + 2));
  if (NULL == file_buffer)
    return false;

  file_len = fread(file_buffer, sizeof(char), file_len, src_file);
  file_buffer[file_len] = '\n';   //always append a new line at the end
  file_buffer[file_len + 1] = 0;
  return true;
}

/*compile asm file to hex file*/
int parse(const char* output, const char* input)
{
  int err;
  struct token_list tk_list = {.head = NULL, .tail = NULL};
  FILE *src_file = NULL, *code_file = NULL;
  src_file = fopen(input, "r");
  code_file = fopen(output, "w");
  if (NULL == src_file || NULL == code_file) {
    printf("open file '%s' failed\n", NULL == src_file? output: input);
    fclose(src_file);
    fclose(code_file);
    return -1;
  }
  if (false == load_file(src_file)) {
    error("file '%s' is too big!\n", input);
    return -2;
  }
  fclose(src_file);

  err = tokenize(&tk_list, file_buffer);
  if (0 != err) goto END;

    /******construct symbol table******/
  err = build_symb_tb(&tk_list);
  if (0 != err) goto END;

  err = pass(&tk_list, NULL);
  err = pass(&tk_list, code_file);

END:
  free(file_buffer);
  destroy_token(&tk_list.head);
  destroy_symb_tb();
  fclose(code_file);
  return err;
}

static int pass(const struct token_list* tk_list, FILE* code_file)
{
  int err = 0;
  size_t line_num;
  struct token_node* head = tk_list->head;
  struct token_node* tail = head;
  line_num = 0;
  for (; head != NULL;) {
    struct token_list tk_line;
    struct token_inst inst;
    while ('\n' != tail->type) 
      tail = tail->next;
    tk_line.head = head;
    tk_line.tail = tail;
    if (tk_line.head == tk_line.tail)
      goto END_LOOP;
    err = gen_inst(&inst, &tk_line, line_num);
    if (TERMINAL == err)
      break;
    else if (0 != err)
      return err;
    if (NOT_INST == inst.op_code)
      goto END_LOOP;
    if (PSEUDO_SPC == inst.op_code) {
      pc = inst.addr;
    } else {
      uint8_t code[3];
      uint8_t code_len = gen_code(code, &inst);
      ASSERT(1 <= code_len && code_len <= 3);
      if (NULL != code_file)
        dump_code(code_file, code, code_len, pc);
      pc += (addr_t)code_len;
    }
  END_LOOP:
    while (NULL != tail && '\n' == tail->type) {
      ++line_num;
      tail = tail->next;
    }
    head = tail;
  }
  if (NULL != code_file)
    dump_end(code_file);
  return err;
}

