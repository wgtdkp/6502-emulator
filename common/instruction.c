#include "instruction.h"

static const struct inst is[IS_SIZE] = {
/*********ADC*********/
    [ADC_IMM].len = 2,
    [ADC_IMM].cycle = 2,
    
    [ADC_ZERO].len = 2,
    [ADC_ZERO].cycle = 3,

    [ADC_ZERO_X].len = 2,
    [ADC_ZERO_X].cycle = 4,

    [ADC_ABS].len = 3,
    [ADC_ABS].cycle = 4,
    
    [ADC_ABS_X].len = 3,
    [ADC_ABS_X].cycle = 4,

    [ADC_ABS_Y].len = 3,
    [ADC_ABS_Y].cycle = 4,

    [ADC_IND_X].len = 2,
    [ADC_IND_X].cycle = 6,
    
    [ADC_IND_Y].len = 2,
    [ADC_IND_Y].cycle = 5,

    /******AND******/
    [AND_IMM].len = 2,
    [AND_IMM].cycle = 2,
    
    [AND_ZERO].len = 2,
    [AND_ZERO].cycle = 2,

    [AND_ZERO_X].len = 2,
    [AND_ZERO_X].cycle = 3,

    [AND_ABS].len = 3,
    [AND_ABS].cycle = 4,
    
    [AND_ABS_X].len = 3,
    [AND_ABS_X].cycle = 4,

    [AND_ABS_Y].len = 3,
    [AND_ABS_Y].cycle = 4,

    [AND_IND_X].len = 2,
    [AND_IND_X].cycle = 6,
    
    [AND_IND_Y].len = 2,
    [AND_IND_Y].cycle = 5,

    /******ASL******/
    [ASL_ACC].len = 1,
    [ASL_ACC].cycle = 2,

    [ASL_ZERO].len = 2,
    [ASL_ZERO].cycle = 5,

    [ASL_ZERO_X].len = 2,
    [ASL_ZERO_X].cycle = 6,

    [ASL_ABS].len = 3,
    [ASL_ABS].cycle = 6,
    
    [ASL_ABS_X].len = 3,
    [ASL_ABS_X].cycle = 7,

    /******BCC******/
    [BCC].len = 2,
    [BCC].cycle = 2,

    /******BCS******/
    [BCS].len = 2,
    [BCS].cycle = 2,

    /******BEQ******/
    [BEQ].len = 2,
    [BEQ].cycle = 2,

    /******BIT******/
    [BIT_ZERO].len = 2,
    [BIT_ZERO].cycle = 3,

    [BIT_ABS].len = 3,
    [BIT_ABS].cycle = 4,

    /******BMI******/
    [BMI].len = 2,
    [BMI].cycle = 2,

    /******BNE******/
    [BNE].len = 2,
    [BNE].cycle = 2,

    /******BPL******/
    [BPL].len = 2,
    [BPL].cycle = 2,

    /******BRK******/
    [BRK].len = 1,
    [BRK].cycle = 7,

    /******BVC******/
    [BVC].len = 2,
    [BVC].cycle = 2,

    /******BVS******/
    [BVS].len = 2,
    [BVS].cycle = 2,

    /******CLC******/
    [CLC].len = 1,
    [CLC].cycle = 2,

    /******CLD******/
    [CLD].len = 1,
    [CLD].cycle = 2,

    /******CLI******/
    [CLI].len = 1,
    [CLI].cycle = 2,

    /******CLV******/
    [CLV].len = 1,
    [CLV].cycle = 2,
    
    /*********CMP*********/
    [CMP_IMM].len = 2,
    [CMP_IMM].cycle = 2,
    
    [CMP_ZERO].len = 2,
    [CMP_ZERO].cycle = 3,

    [CMP_ZERO_X].len = 2,
    [CMP_ZERO_X].cycle = 4,

    [CMP_ABS].len = 3,
    [CMP_ABS].cycle = 4,
    
    [CMP_ABS_X].len = 3,
    [CMP_ABS_X].cycle = 4,

    [CMP_ABS_Y].len = 3,
    [CMP_ABS_Y].cycle = 4,

    [CMP_IND_X].len = 2,
    [CMP_IND_X].cycle = 6,
    
    [CMP_IND_Y].len = 2,
    [CMP_IND_Y].cycle = 5,

    /*********CPX*********/
    [CPX_IMM].len = 2,
    [CPX_IMM].cycle = 2,
    
    [CPX_ZERO].len = 2,
    [CPX_ZERO].cycle = 3,

    [CPX_ABS].len = 3,
    [CPX_ABS].cycle = 4,

    /*********CPY*********/
    [CPY_IMM].len = 2,
    [CPY_IMM].cycle = 2,
    
    [CPY_ZERO].len = 2,
    [CPY_ZERO].cycle = 3,

    [CPY_ABS].len = 3,
    [CPY_ABS].cycle = 4,

    /*********DEC*********/
    [DEC_ZERO].len = 2,
    [DEC_ZERO].cycle = 5,

    [DEC_ZERO_X].len = 2,
    [DEC_ZERO_X].cycle = 6,

    [DEC_ABS].len = 3,
    [DEC_ABS].cycle = 6,
    
    [DEC_ABS_X].len = 3,
    [DEC_ABS_X].cycle = 7,

    /******DEX******/
    [DEX].len = 1,
    [DEX].cycle = 2,

    /******DEY******/
    [DEY].len = 1,
    [DEY].cycle = 2,

    /*********EOR*********/
    [EOR_IMM].len = 2,
    [EOR_IMM].cycle = 2,
    
    [EOR_ZERO].len = 2,
    [EOR_ZERO].cycle = 3,

    [EOR_ZERO_X].len = 2,
    [EOR_ZERO_X].cycle = 4,

    [EOR_ABS].len = 3,
    [EOR_ABS].cycle = 4,
    
    [EOR_ABS_X].len = 3,
    [EOR_ABS_X].cycle = 4,

    [EOR_ABS_Y].len = 3,
    [EOR_ABS_Y].cycle = 4,

    [EOR_IND_X].len = 2,
    [EOR_IND_X].cycle = 6,
    
    [EOR_IND_Y].len = 2,
    [EOR_IND_Y].cycle = 5,

    /*********INC*********/
    [INC_ZERO].len = 2,
    [INC_ZERO].cycle = 5,

    [INC_ZERO_X].len = 2,
    [INC_ZERO_X].cycle = 6,

    [INC_ABS].len = 3,
    [INC_ABS].cycle = 6,
    
    [INC_ABS_X].len = 3,
    [INC_ABS_X].cycle = 7,

    /******INX******/
    [INX].len = 1,
    [INX].cycle = 2,

    /******INY******/
    [INY].len = 1,
    [INY].cycle = 2,

    /******JMP******/
    [JMP_ABS].len = 3,
    [JMP_ABS].cycle = 3,

    [JMP_IND].len = 3,
    [JMP_IND].cycle = 5,

    /******JSR******/
    [JSR].len = 3,
    [JSR].cycle = 6,

    /*********LDA*********/
    [LDA_IMM].len = 2,
    [LDA_IMM].cycle = 2,
    
    [LDA_ZERO].len = 2,
    [LDA_ZERO].cycle = 3,

    [LDA_ZERO_X].len = 2,
    [LDA_ZERO_X].cycle = 4,

    [LDA_ABS].len = 3,
    [LDA_ABS].cycle = 4,
    
    [LDA_ABS_X].len = 3,
    [LDA_ABS_X].cycle = 4,

    [LDA_ABS_Y].len = 3,
    [LDA_ABS_Y].cycle = 4,

    [LDA_IND_X].len = 2,
    [LDA_IND_X].cycle = 6,
    
    [LDA_IND_Y].len = 2,
    [LDA_IND_Y].cycle = 5,

    /*********LDX*********/
    [LDX_IMM].len = 2,
    [LDX_IMM].cycle = 2,
    
    [LDX_ZERO].len = 2,
    [LDX_ZERO].cycle = 3,

    [LDX_ZERO_Y].len = 2,
    [LDX_ZERO_Y].cycle = 4,

    [LDX_ABS].len = 2,
    [LDX_ABS].cycle = 4,

    [LDX_ABS_Y].len = 2,
    [LDX_ABS_Y].cycle = 4,

    /*********LDY*********/
    [LDY_IMM].len = 2,
    [LDY_IMM].cycle = 2,
    
    [LDY_ZERO].len = 2,
    [LDY_ZERO].cycle = 3,

    [LDY_ZERO_X].len = 2,
    [LDY_ZERO_X].cycle = 4,

    [LDY_ABS].len = 2,
    [LDY_ABS].cycle = 4,

    [LDY_ABS_X].len = 2,
    [LDY_ABS_X].cycle = 4,

    /*********LSR*********/
    [LSR_ACC].len = 1,
    [LSR_ACC].cycle = 2,
    
    [LSR_ZERO].len = 2,
    [LSR_ZERO].cycle = 5,

    [LSR_ZERO_X].len = 2,
    [LSR_ZERO_X].cycle = 6,

    [LSR_ABS].len = 3,
    [LSR_ABS].cycle = 6,
    
    [LSR_ABS_X].len = 3,
    [LSR_ABS_X].cycle = 7,

    /*********NOP*********/
    [NOP].len = 1,
    [NOP].cycle = 2,

    /*********ORA*********/
    [ORA_IMM].len = 2,
    [ORA_IMM].cycle = 2,
    
    [ORA_ZERO].len = 2,
    [ORA_ZERO].cycle = 2,

    [ORA_ZERO_X].len = 2,
    [ORA_ZERO_X].cycle = 3,

    [ORA_ABS].len = 3,
    [ORA_ABS].cycle = 4,
    
    [ORA_ABS_X].len = 3,
    [ORA_ABS_X].cycle = 4,

    [ORA_ABS_Y].len = 3,
    [ORA_ABS_Y].cycle = 4,

    [ORA_IND_X].len = 2,
    [ORA_IND_X].cycle = 6,
    
    [ORA_IND_Y].len = 2,
    [ORA_IND_Y].cycle = 5,

    /*********PHA*********/
    [PHA].len = 1,
    [PHA].cycle = 3,

    /*********PHP*********/
    [PHP].len = 1,
    [PHP].cycle = 3,

    /*********PLA*********/
    [PLA].len = 1,
    [PLA].cycle = 4,

    /*********PLP*********/
    [PLP].len = 1,
    [PLP].cycle = 4,

    /*********ROL*********/
    [ROL_ACC].len = 1,
    [ROL_ACC].cycle = 2,
    
    [ROL_ZERO].len = 2,
    [ROL_ZERO].cycle = 5,

    [ROL_ZERO_X].len = 2,
    [ROL_ZERO_X].cycle = 6,

    [ROL_ABS].len = 3,
    [ROL_ABS].cycle = 6,
    
    [ROL_ABS_X].len = 3,
    [ROL_ABS_X].cycle = 7,

    /*********ROR*********/
    [ROR_ACC].len = 1,
    [ROR_ACC].cycle = 2,
    
    [ROR_ZERO].len = 2,
    [ROR_ZERO].cycle = 5,

    [ROR_ZERO_X].len = 2,
    [ROR_ZERO_X].cycle = 6,

    [ROR_ABS].len = 3,
    [ROR_ABS].cycle = 6,
    
    [ROR_ABS_X].len = 3,
    [ROR_ABS_X].cycle = 7,

    /*********RTI*********/
    [RTI].len = 1,
    [RTI].cycle = 6,

    /*********RTS*********/
    [RTS].len = 1,
    [RTS].cycle = 6,

    /*********SBC*********/
    [SBC_IMM].len = 2,
    [SBC_IMM].cycle = 2,
    
    [SBC_ZERO].len = 2,
    [SBC_ZERO].cycle = 3,

    [SBC_ZERO_X].len = 2,
    [SBC_ZERO_X].cycle = 4,

    [SBC_ABS].len = 3,
    [SBC_ABS].cycle = 4,
    
    [SBC_ABS_X].len = 3,
    [SBC_ABS_X].cycle = 4,

    [SBC_ABS_Y].len = 3,
    [SBC_ABS_Y].cycle = 4,

    [SBC_IND_X].len = 2,
    [SBC_IND_X].cycle = 6,
    
    [SBC_IND_Y].len = 2,
    [SBC_IND_Y].cycle = 5,

    /*********SEC*********/
    [SEC].len = 1,
    [SEC].cycle = 2,

    /*********SED*********/
    [SED].len = 1,
    [SED].cycle = 2,

    /*********SEI*********/
    [SEI].len = 1,
    [SEI].cycle = 2,

    /*********STA*********/
    [STA_ZERO].len = 2,
    [STA_ZERO].cycle = 3,

    [STA_ZERO_X].len = 2,
    [STA_ZERO_X].cycle = 4,

    [STA_ABS].len = 3,
    [STA_ABS].cycle = 4,
    
    [STA_ABS_X].len = 3,
    [STA_ABS_X].cycle = 5,

    [STA_ABS_Y].len = 3,
    [STA_ABS_Y].cycle = 5,

    [STA_IND_X].len = 2,
    [STA_IND_X].cycle = 6,
    
    [STA_IND_Y].len = 2,
    [STA_IND_Y].cycle = 6,

    /*********STX*********/
    [STX_ZERO].len = 2,
    [STX_ZERO].cycle = 3,

    [STX_ZERO_Y].len = 2,
    [STX_ZERO_Y].cycle = 4,

    [STX_ABS].len = 3,
    [STX_ABS].cycle = 4,

    /*********STY*********/
    [STY_ZERO].len = 2,
    [STY_ZERO].cycle = 3,

    [STY_ZERO_X].len = 2,
    [STY_ZERO_X].cycle = 4,

    [STY_ABS].len = 3,
    [STY_ABS].cycle = 4,

    /*********TAX*********/
    [TAX].len = 1,
    [TAX].cycle = 2,

    /*********TAY*********/
    [TAY].len = 1,
    [TAY].cycle = 2,

    /*********TSX*********/
    [TSX].len = 1,
    [TSX].cycle = 2,

    /*********TXA*********/
    [TXA].len = 1,
    [TXA].cycle = 2,

    /*********TXS*********/
    [TXS].len = 1,
    [TXS].cycle = 2,

    /*********TYA*********/
    [TYA].len = 1,
    [TYA].cycle = 2,
};

const struct inst* get_inst(uint8_t op_code)
{
    return &is[op_code];
}
