#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "type.h"
#include <stdint.h>

#define PSW_C   (0)
#define PSW_Z   (1)
#define PSW_I   (2)
#define PSW_D   (3)
#define PSW_B   (4)
#define PSW_R   (5) //reserve
#define PSW_V   (6)
#define PSW_N   (7)

/*******INSTRUCTION DEFINES*******/

/******ADC******/
#define ADC_IMM     (0x69)
#define ADC_ZERO    (0x65)
#define ADC_ZERO_X  (0x75)
#define ADC_ABS     (0x6D)
#define ADC_ABS_X   (0x7D)
#define ADC_ABS_Y   (0x79)
#define ADC_IND_X   (0x61)
#define ADC_IND_Y   (0x71)

/******AND******/
#define AND_IMM     (0x29)
#define AND_ZERO    (0x25)
#define AND_ZERO_X  (0x35)
#define AND_ABS     (0x2D)
#define AND_ABS_X   (0x3D)
#define AND_ABS_Y   (0x39)
#define AND_IND_X   (0x21)
#define AND_IND_Y   (0x31)

/******ASL******/
#define ASL_ACC     (0x0A)
#define ASL_ZERO    (0x06)
#define ASL_ZERO_X  (0x16)
#define ASL_ABS     (0x0E)
#define ASL_ABS_X   (0x1E)

/******BCC******/
#define BCC         (0x90)

/******BCS******/
#define BCS         (0xB0)

/******BEQ******/
#define BEQ         (0xF0)

/******BIT******/
#define BIT_ZERO    (0x24)
#define BIT_ABS     (0x2C)

/******BMI******/
#define BMI         (0x30)

/******BNE******/
#define BNE         (0xD0)

/******BPL******/
#define BPL         (0x10)

/******BRK******/
#define BRK         (0x00)

/******BVC******/
#define BVC         (0x50)

/******BVS******/
#define BVS         (0x70)

/******CLC******/
#define CLC         (0x18)

/******CLD******/
#define CLD         (0xD8)

/******CLI******/
#define CLI         (0x58)

/******CLV******/
#define CLV         (0xB8)

/******CMP******/
#define CMP_IMM     (0xC9)
#define CMP_ZERO    (0xC5)
#define CMP_ZERO_X  (0xD5)
#define CMP_ABS     (0xCD)
#define CMP_ABS_X   (0xDD)
#define CMP_ABS_Y   (0xD9)
#define CMP_IND_X   (0xC1)
#define CMP_IND_Y   (0xD1)

/******CPX******/
#define CPX_IMM     (0xE0)
#define CPX_ZERO    (0xE4)
#define CPX_ABS     (0xEC)

/******CPY******/
#define CPY_IMM     (0xC0)
#define CPY_ZERO    (0xC4)
#define CPY_ABS     (0xCC)

/******DEC******/
#define DEC_ZERO    (0xC6)
#define DEC_ZERO_X  (0xD6)
#define DEC_ABS     (0xCE)
#define DEC_ABS_X   (0xDE)

/******DEX******/
#define DEX         (0xCA)

/******DEY******/
#define DEY         (0x88)

/******EOR******/
#define EOR_IMM     (0x49)
#define EOR_ZERO    (0x45)
#define EOR_ZERO_X  (0x55)
#define EOR_ABS     (0x4D)
#define EOR_ABS_X   (0x5D)
#define EOR_ABS_Y   (0x59)
#define EOR_IND_X   (0x41)
#define EOR_IND_Y   (0x51)

/******INC******/
#define INC_ZERO    (0xE6)
#define INC_ZERO_X  (0xF6)
#define INC_ABS     (0xEE)
#define INC_ABS_X   (0xFE)

/******INX******/
#define INX     (0xE8)

/******INY******/
#define INY     (0xC8)

/******JMP******/
#define JMP_ABS     (0x4C)
#define JMP_IND     (0x6C)

/******JSR******/
#define JSR         (0x20)

/******LDA******/
#define LDA_IMM     (0xA9)
#define LDA_ZERO    (0xA5)
#define LDA_ZERO_X  (0xB5)
#define LDA_ABS     (0xAD)
#define LDA_ABS_X   (0xBD)
#define LDA_ABS_Y   (0xB9)
#define LDA_IND_X   (0xA1)
#define LDA_IND_Y   (0xB1)

/******LDX******/
#define LDX_IMM     (0xA2)
#define LDX_ZERO    (0xA6)
#define LDX_ZERO_Y  (0xB6)
#define LDX_ABS     (0xAE)
#define LDX_ABS_Y   (0xBE)

/******LDY******/
#define LDY_IMM     (0xA0)
#define LDY_ZERO    (0xA4)
#define LDY_ZERO_X  (0xB4)
#define LDY_ABS     (0xAC)
#define LDY_ABS_X   (0xBC)

/******LSR******/
#define LSR_ACC     (0x4A)
#define LSR_ZERO    (0x46)
#define LSR_ZERO_X  (0x56)
#define LSR_ABS     (0x4E)
#define LSR_ABS_X   (0x5E)

/******NOP******/
#define NOP     (0xEA)

/******ORA******/
#define ORA_IMM     (0x09)
#define ORA_ZERO    (0x05)
#define ORA_ZERO_X  (0x15)
#define ORA_ABS     (0x0D)
#define ORA_ABS_X   (0x1D)
#define ORA_ABS_Y   (0x19)
#define ORA_IND_X   (0x01)
#define ORA_IND_Y   (0x11)

/******PHA******/
#define PHA     (0x48)

/******PHP******/
#define PHP     (0x08)

/******PLA******/
#define PLA     (0x68)

/******PLP******/
#define PLP     (0x28)

/******ROL******/
#define ROL_ACC     (0x2A)
#define ROL_ZERO    (0x26)
#define ROL_ZERO_X  (0x36)
#define ROL_ABS     (0x2E)
#define ROL_ABS_X   (0x3E)

/******ROR******/
#define ROR_ACC     (0x6A)
#define ROR_ZERO    (0x66)
#define ROR_ZERO_X  (0x76)
#define ROR_ABS     (0x6E)
#define ROR_ABS_X   (0x7E)

/******RTI******/
#define RTI     (0x40)

/******RTS******/
#define RTS     (0x60)

/******SBC******/
#define SBC_IMM     (0xE9)
#define SBC_ZERO    (0xE5)
#define SBC_ZERO_X  (0xF5)
#define SBC_ABS     (0xED)
#define SBC_ABS_X   (0xFD)
#define SBC_ABS_Y   (0xF9)
#define SBC_IND_X   (0xE1)
#define SBC_IND_Y   (0xF1)

/******SEC******/
#define SEC     (0x38)

/******SED******/
#define SED     (0xF8)

/******SEI******/
#define SEI     (0x78)

/******STA******/
#define STA_ZERO    (0x85)
#define STA_ZERO_X  (0x95)
#define STA_ABS     (0x8D)
#define STA_ABS_X   (0x9D)
#define STA_ABS_Y   (0x99)
#define STA_IND_X   (0x81)
#define STA_IND_Y   (0x91)

/******STX******/
#define STX_ZERO    (0x86)
#define STX_ZERO_Y  (0x96)
#define STX_ABS     (0x8E)

/******STY******/
#define STY_ZERO    (0x84)
#define STY_ZERO_X  (0x94)
#define STY_ABS     (0x8C)

/******TAX******/
#define TAX     (0xAA)

/******TAY******/
#define TAY     (0xA8)

/******TSX******/
#define TSX     (0xBA)

/******TXA******/
#define TXA     (0x8A)

/******TXS******/
#define TXS     (0x9A)

/******TYA******/
#define TYA     (0x98)

#define IS_SIZE (256)


struct inst {
    //word_t op_code; //operation code
    uint8_t cycle;  //instrcution cycle
    uint8_t len;     //instrcution length
};

extern const struct inst is[IS_SIZE];

#endif
