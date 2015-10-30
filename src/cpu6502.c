#include "cpu6502.h"
#include "debug.h"

static inline void adc(struct cpu* cpu, word_t m)
{
    uint16_t t;
    t = cpu->acc + m + BIT(cpu->psw, PSW_C);
    SET_BIT(cpu->psw, PSW_Z, 0 == t);
    //SET_BIT(cpu->psw, PSW_V, BIT(cpu->acc, 7) != BIT(t, 7));
    //SET_BIT(cpu->psw, PSW_N, BIT(t, 7));
    if (BIT(cpu->psw, PSW_D)) {
        if ( (cpu->acc & 0x0F) + (m & 0x0F) + BIT(cpu->psw, PSW_C) > 9)
            t += 6;
        SET_BIT(cpu->psw, PSW_N, BIT(t, 7));
        
        //the two operands have same sign, and the sum gets a different sign.
        SET_BIT(cpu->psw, PSW_V, !((cpu->acc ^ m) & 0x80) && ((cpu->acc ^ t) & 0x80) );
        if(t > 0x99)
            t += 96;    //becareful
        SET_BIT(cpu->psw, PSW_C, t > 0x99);
    } else {
        SET_BIT(cpu->psw, PSW_N, BIT(t, 7));
        SET_BIT(cpu->psw, PSW_V, !((cpu->acc ^ m) & 0x80) && ((cpu->acc ^ t) & 0x80) );
        SET_BIT(cpu->psw, PSW_C, t > 0xFF);
    }
    cpu->acc = (word_t)t;
}

static inline void and(struct cpu* cpu, word_t m)
{
    cpu->acc &= m;
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->acc, 7));
    SET_BIT(cpu->psw, PSW_Z, 0 == cpu->acc);
}

static inline word_t asl(struct cpu* cpu, word_t b)
{
    SET_BIT(cpu->psw, PSW_C, BIT(b, 7));
    b = (b << 1) & 0xFF;
    SET_BIT(cpu->psw, PSW_N, BIT(b, 7));
    SET_BIT(cpu->psw, PSW_Z, 0 == b);
    return b;
}

static inline void bit(struct cpu* cpu, word_t m)
{
    word_t t;
    t = cpu->acc & m;
    SET_BIT(cpu->psw, PSW_N, BIT(t, 7));
    SET_BIT(cpu->psw, PSW_V, BIT(t, 6));
    SET_BIT(cpu->psw, PSW_Z, 0 == t);
}

static inline void brk(struct cpu* cpu)
{
    ++cpu->pc;
    PUSH(cpu, H(cpu->pc));
    PUSH(cpu, L(cpu->pc));
    SET_BIT(cpu->psw, PSW_B, 1);
    PUSH(cpu, cpu->psw);
    SET_BIT(cpu->psw, PSW_I, 1);
    cpu->pc = read_2byte(MEM_SIZE - 1 - 1);
}

static inline 
void branch(struct cpu* cpu, byte flag, byte b, int8_t offset)
{
    if (b == BIT(cpu->psw, flag)) {
        if ( (cpu->pc & 0xFF00) != ((cpu->pc + offset) & 0xFF00) )
            cpu->clk += 2;
        else
            cpu->clk += 1;
        cpu->pc += offset;
    }
}

static inline void cmp(struct cpu* cpu, word_t m)
{
    uint16_t t;
    t = cpu->acc - m;
    SET_BIT(cpu->psw, PSW_N, BIT(t, 7));
    SET_BIT(cpu->psw, PSW_C, cpu->acc >= m);
    SET_BIT(cpu->psw, PSW_Z, 0 == t);
}

static inline void cpx(struct cpu* cpu, word_t m)
{
    uint16_t t;
    t = cpu->xr - m;
    SET_BIT(cpu->psw, PSW_N, BIT(t, 7));
    SET_BIT(cpu->psw, PSW_C, cpu->xr >= m);
    SET_BIT(cpu->psw, PSW_Z, 0x00 == t);
}

static inline void cpy(struct cpu* cpu, word_t m)
{
    uint16_t t;
    t = cpu->yr - m;
    SET_BIT(cpu->psw, PSW_N, BIT(t, 7));
    SET_BIT(cpu->psw, PSW_C, cpu->yr >= m);
    SET_BIT(cpu->psw, PSW_Z, 0x00 == t);
}

static inline word_t dec(struct cpu* cpu, word_t m)
{
    --m;
    SET_BIT(cpu->psw, PSW_N, BIT(m, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == m);
    return m;
}

static inline void dex(struct cpu* cpu)
{
    --cpu->xr;
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->xr, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == cpu->xr);
}

static inline void dey(struct cpu* cpu)
{
    --cpu->yr;
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->yr, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == cpu->yr);
}

static inline void eor(struct cpu* cpu, word_t m)
{
    m ^= cpu->acc;
    SET_BIT(cpu->psw, PSW_N, BIT(m, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == m);
    cpu->acc = m;
}

static inline word_t inc(struct cpu* cpu, word_t m)
{
    ++m;
    SET_BIT(cpu->psw, PSW_N, BIT(m, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == m);
    return m;
}

static inline void inx(struct cpu* cpu)
{
    ++cpu->xr;
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->xr, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == cpu->xr);
}

static inline void iny(struct cpu* cpu)
{
    ++cpu->yr;
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->yr, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == cpu->yr);
}


static inline void jsr(struct cpu* cpu, addr_t addr)
{
    --cpu->pc;
    PUSH(cpu, H(cpu->pc));
    PUSH(cpu, L(cpu->pc));
    cpu->pc = addr;
}

static inline void lda(struct cpu* cpu, word_t m)
{
    cpu->acc = m;
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->acc, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == cpu->acc);
}

static inline void ldx(struct cpu* cpu, word_t m)
{
    SET_BIT(cpu->psw, PSW_N, BIT(m, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == m);
    cpu->xr = m;
}

static inline void ldy(struct cpu* cpu, word_t m)
{
    SET_BIT(cpu->psw, PSW_N, BIT(m, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == m);
    cpu->yr = m;
}

static inline word_t lsr(struct cpu* cpu, word_t m)
{
    SET_BIT(cpu->psw, PSW_N, 0);
    SET_BIT(cpu->psw, PSW_C, BIT(m, 0));
    m >>= 1;
    SET_BIT(cpu->psw, PSW_Z, 0 == m);
    return m;
}

static inline void ora(struct cpu* cpu, word_t m)
{
    m |= cpu->acc;
    SET_BIT(cpu->psw, PSW_N, BIT(m, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == m);
    cpu->acc = m;
}

static inline void pla(struct cpu* cpu)
{
    cpu->acc = POP(cpu);
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->acc, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == cpu->acc);
}

static inline word_t rol(struct cpu* cpu, word_t b)
{
    word_t t;
    t = BIT(b, 7);
    b <<= 1;
    b |= BIT(cpu->psw, PSW_C);
    SET_BIT(cpu->psw, PSW_C, t);
    SET_BIT(cpu->psw, PSW_Z, 0x00 == b);
    SET_BIT(cpu->psw, PSW_N, BIT(b, 7));
    return b;
}

static inline word_t ror(struct cpu* cpu, word_t b)
{
    word_t t;
    t = BIT(b, 0);
    b >>= 1;
    b |= BIT(cpu->psw, PSW_C) << 7;
    SET_BIT(cpu->psw, PSW_C, t);
    SET_BIT(cpu->psw, PSW_Z, 0x00 == b);
    SET_BIT(cpu->psw, PSW_N, BIT(b, 7));
    return b;
}

static inline void rti(struct cpu* cpu)
{
    word_t l, h;
    cpu->psw = POP(cpu);
    l = POP(cpu);
    h = POP(cpu);
    cpu->pc = ((addr_t)h << 8) | l;
}

static inline void rts(struct cpu* cpu)
{
    word_t l, h;
    l = POP(cpu);
    h = POP(cpu);
    cpu->pc = (((addr_t)h << 8) | l) + 1;
}

static inline void sbc(struct cpu* cpu, word_t m)
{
    uint16_t t;
    t = cpu->acc - m - !BIT(cpu->psw, PSW_C);
    SET_BIT(cpu->psw, PSW_N, BIT(t, 7));
    SET_BIT(cpu->psw, PSW_Z, 0 == (word_t)t);

    //the two operands have different sign and the sign of sum if diff to acc
    SET_BIT(cpu->psw, PSW_V,  ((cpu->acc ^ t) & 0x80) && ((cpu->acc ^ m) & 0x80) );
    if (BIT(cpu->psw, PSW_D)) {
        if (((cpu->acc & 0x0F) - !BIT(cpu->psw, PSW_C)) < (m & 0x0F))
            t -= 6;
        if (t > 0x99)
            t -= 0x60;
    }
    SET_BIT(cpu->psw, PSW_C, t < 0x100);
    cpu->acc = (word_t)t;
}

static inline void tax(struct cpu* cpu)
{
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->acc, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == cpu->acc);
    cpu->xr = cpu->acc;
}

static inline void tay(struct cpu* cpu)
{
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->acc, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == cpu->acc);
    cpu->yr = cpu->acc;
}

static inline void tsx(struct cpu* cpu)
{
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->sp, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == cpu->sp);
    cpu->xr = cpu->sp;
}

static inline void txa(struct cpu* cpu)
{
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->xr, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == cpu->xr);
    cpu->acc = cpu->xr;
}

static inline void tya(struct cpu* cpu)
{
    SET_BIT(cpu->psw, PSW_N, BIT(cpu->yr, 7));
    SET_BIT(cpu->psw, PSW_Z, 0x00 == cpu->yr);
    cpu->acc = cpu->yr;
}

//int cpu6502_run(struct cpu* cpu, mem_t* mem){ return 0;}

int cpu6502_run(struct cpu* cpu, mem_t* mem)
{

    const uint16_t period = 20;
    //reset pc
    cpu->pc = read_2byte(RESET_VECTOR_ADDR);
    print_cpu(cpu);

    for (; ;) {
        //uint8_t i;
        uint16_t inst = 0;
        byte data;
        addr_t addr;
        byte op_code = read_byte(cpu->pc++);
        cpu->clk += cpu->is[op_code].cycle;

        if (2 == cpu->is[op_code].len)
            inst = read_byte(cpu->pc);
        else if (3 == cpu->is[op_code].len)
            inst = read_2byte(cpu->pc);
        cpu->pc += cpu->is[op_code].len - 1;
        //printf("XR: %x  YR: %x\n", cpu->xr, cpu->yr);
        


        switch (op_code) {
        /******ADC******/
        case ADC_IMM:
            data = inst & MASK(byte);
            adc(cpu, data);
            break;
        case ADC_ZERO:
            data = read_byte(inst & MASK(byte));
            adc(cpu, data);
            break;
        case ADC_ZERO_X:
            data = read_byte((inst & MASK(byte)) + cpu->xr);
            adc(cpu, data);
            break;
        case ADC_ABS:
            data = read_byte(inst & MASK(addr_t));
            adc(cpu, data);
            break;
        case ADC_ABS_X:
            data = read_byte((inst & MASK(addr_t)) + cpu->xr);
            adc(cpu, data);
            break;
        case ADC_ABS_Y:
            data = read_byte((inst & MASK(addr_t)) + cpu->yr);
            adc(cpu, data);
            break;
        case ADC_IND_X:
            addr = read_2byte((inst & MASK(byte)) + cpu->xr);
            data = read_byte(addr);
            adc(cpu, data);
            break;
        case ADC_IND_Y:
            addr = read_2byte(inst & MASK(byte)) + cpu->yr;
            data = read_byte(addr);
            adc(cpu, data);
            break;

        /******AND******/
        case AND_IMM:
            data = inst & MASK(byte);
            and(cpu, data);
            break;
        case AND_ZERO:
            data = read_byte(inst & MASK(byte));
            and(cpu, data);
            break;
        case AND_ZERO_X:
            data = read_byte((inst & MASK(byte)) + cpu->xr);
            and(cpu, data);
            break;
        case AND_ABS:
            data = read_byte(inst & MASK(addr_t));
            and(cpu, data);
            break;
        case AND_ABS_X:
            data = read_byte((inst & MASK(addr_t)) + cpu->xr);
            and(cpu, data);
            break;
        case AND_ABS_Y:
            data = read_byte((inst & MASK(addr_t)) + cpu->yr);
            and(cpu, data);
            break;
        case AND_IND_X:
            addr = read_2byte((inst & MASK(byte)) + cpu->xr);
            data = read_byte(addr);
            and(cpu, data);
            break;
        case AND_IND_Y:
            addr = read_2byte(inst & MASK(byte)) + cpu->yr;
            data = read_byte(addr);
            and(cpu, data);
            break;

        /******ASL******/
        case ASL_ACC:
            cpu->acc = asl(cpu, cpu->acc);
            break;
        case ASL_ZERO:
            addr = inst & MASK(byte);
            data = asl(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case ASL_ZERO_X:
            addr = (inst & MASK(byte)) + cpu->xr;
            data = asl(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case ASL_ABS:
            addr = inst & MASK(addr_t);
            data = asl(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case ASL_ABS_X:
            addr = (inst & MASK(addr_t)) + cpu->xr;
            data = asl(cpu, read_byte(addr));
            write_byte(addr, data);
            break;    

        /******BCC******/
        case BCC:
            data = inst & MASK(word_t);
            branch(cpu, PSW_C, 0, data);
            break;

        /******BCS******/
        case BCS:
            data = inst & MASK(word_t);
            branch(cpu, PSW_C, 1, data);
            break;

        /******BEQ******/
        case BEQ:
            data = inst & MASK(word_t);
            branch(cpu, PSW_Z, 1, data);
            break;

        /******BIT******/
        case BIT_ZERO:
            data = read_byte(inst & MASK(word_t));
            bit(cpu, data);
            break;
        case BIT_ABS:
            data = read_byte(inst & MASK(addr_t));
            bit(cpu, data);
            break;

        /******BMI******/
        case BMI:
            data = inst & MASK(word_t);
            branch(cpu, PSW_N, 1, data);
            break;

        /******BNE******/
        case BNE:
            data = inst & MASK(word_t);
            branch(cpu, PSW_Z, 0, data);
            break;

        /******BPL******/
        case BPL:
            data = inst & MASK(word_t);
            branch(cpu, PSW_N, 0, data);
            break;

        /******BRK******/
        case BRK:
            brk(cpu);
            printf("program end.\n");
            while(1) {}
            break;

        /******BVC******/
        case BVC:
            data = inst & MASK(word_t);
            branch(cpu, PSW_V, 0x00, data);
            break;

        /******BVS******/
        case BVS:
            data = inst & MASK(word_t);
            branch(cpu, PSW_V, 0x01, data);
            break;

        /******Clear Flag******/
        case CLC:
            SET_BIT(cpu->psw, PSW_C, 0x00);
            break;
        case CLD:
            SET_BIT(cpu->psw, PSW_D, 0x00);
            break;
        case CLI:
            SET_BIT(cpu->psw, PSW_I, 0x00);
            break;
        case CLV:
            SET_BIT(cpu->psw, PSW_V, 0x00);
            break; 

        /******CMP******/
        case CMP_IMM:
            data = inst & MASK(byte);
            cmp(cpu, data);
            {
                int i;
                //for (i = 0x20; i < 0x28; i++) {
                //    printf("mem[%x] ", i);
                //}
                //printf("\n");
                for (i = 0x20; i < 0x28; i++) {
                    printf("%x ", read_byte(i));
                }
                printf("\n");
            }


            break;
        case CMP_ZERO:
            data = read_byte(inst & MASK(byte));
            cmp(cpu, data);
            break;
        case CMP_ZERO_X:
            data = read_byte((inst & MASK(byte)) + cpu->xr);
            cmp(cpu, data);
            break;
        case CMP_ABS:
            data = read_byte(inst & MASK(addr_t));
            cmp(cpu, data);
            break;
        case CMP_ABS_X:
            data = read_byte((inst & MASK(addr_t)) + cpu->xr);
            cmp(cpu, data);
            break;
        case CMP_ABS_Y:
            data = read_byte((inst & MASK(addr_t)) + cpu->yr);
            cmp(cpu, data);
            break;
        case CMP_IND_X:
            addr = read_2byte((inst & MASK(byte)) + cpu->xr);
            data = read_byte(addr);
            cmp(cpu, data);
            break;
        case CMP_IND_Y:
            addr = read_2byte(inst & MASK(byte)) + cpu->yr;
            data = read_byte(addr);
            cmp(cpu, data);
            break;

        /******CPX******/
        case CPX_IMM:
            data = inst & MASK(byte);
            cpx(cpu, data);
            break;
        case CPX_ZERO:
            data = read_byte(inst & MASK(byte));
            cpx(cpu, data);
            break;
        case CPX_ABS:
            data = read_byte(inst & MASK(addr_t));
            cpx(cpu, data);
            break;

        /******CPY******/
        case CPY_IMM:
            data = inst & MASK(byte);
            cpx(cpu, data);
            break;
        case CPY_ZERO:
            data = read_byte(inst & MASK(byte));
            cpx(cpu, data);
            break;
        case CPY_ABS:
            data = read_byte(inst & MASK(addr_t));
            cpx(cpu, data);
            break;

        /******DEC******/
        case DEC_ZERO:
            addr = inst & MASK(byte);
            data = dec(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case DEC_ZERO_X:
            addr = (inst & MASK(byte)) + cpu->xr;
            data = dec(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case DEC_ABS:
            addr = inst & MASK(addr_t);
            data = dec(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case DEC_ABS_X:
            addr = (inst & MASK(addr_t)) + cpu->xr;
            data = dec(cpu, read_byte(addr));
            write_byte(addr, data);
            break;

        /******DEX******/
        case DEX:
            dex(cpu);
            break;

        /******DEY******/
        case DEY:
            dey(cpu);
            break;

        /******EOR******/
        case EOR_IMM:
            data = inst & MASK(byte);
            eor(cpu, data);
            break;
        case EOR_ZERO:
            data = read_byte(inst & MASK(byte));
            eor(cpu, data);
            break;
        case EOR_ZERO_X:
            data = read_byte((inst & MASK(byte)) + cpu->xr);
            eor(cpu, data);
            break;
        case EOR_ABS:
            data = read_byte(inst & MASK(addr_t));
            eor(cpu, data);
            break;
        case EOR_ABS_X:
            data = read_byte((inst & MASK(addr_t)) + cpu->xr);
            eor(cpu, data);
            break;
        case EOR_ABS_Y:
            data = read_byte((inst & MASK(addr_t)) + cpu->yr);
            eor(cpu, data);
            break;
        case EOR_IND_X:
            addr = read_2byte((inst & MASK(byte)) + cpu->xr);
            data = read_byte(addr);
            eor(cpu, data);
            break;
        case EOR_IND_Y:
            addr = read_2byte(inst & MASK(byte)) + cpu->yr;
            data = read_byte(addr);
            eor(cpu, data);
            break;

        /******INC******/
        case INC_ZERO:
            addr = inst & MASK(byte);
            data = inc(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case INC_ZERO_X:
            addr = (inst & MASK(byte)) + cpu->xr;
            data = inc(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case INC_ABS:
            addr = inst & MASK(addr_t);
            data = inc(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case INC_ABS_X:
            addr = (inst & MASK(addr_t)) + cpu->xr;
            data = inc(cpu, read_byte(addr));
            write_byte(addr, data);
            break;

        /******INX******/
        case INX:
            inx(cpu);
            break;

        /******INY******/
        case INY:
            iny(cpu);
            break;

        /******JMP******/
        case JMP_ABS:
            addr = inst & MASK(addr_t);
            cpu->pc = addr;
            break;
        case JMP_IND:
            addr = read_2byte(inst & MASK(addr_t));
            cpu->pc = addr;
            break;

        /******JSR******/
        case JSR:
            addr = inst & MASK(addr_t);
            jsr(cpu, addr);
            break;

        /******LDA******/
        case LDA_IMM:
            data = inst & MASK(byte);
            lda(cpu, data);
            break;
        case LDA_ZERO:
            data = read_byte(inst & MASK(byte));
            lda(cpu, data);
            break;
        case LDA_ZERO_X:
            data = read_byte((inst & MASK(byte)) + cpu->xr);
            lda(cpu, data);
            break;
        case LDA_ABS:
            data = read_byte(inst & MASK(addr_t));
            lda(cpu, data);
            break;
        case LDA_ABS_X:
            data = read_byte((inst & MASK(addr_t)) + cpu->xr);
            lda(cpu, data);
            break;
        case LDA_ABS_Y:
            data = read_byte((inst & MASK(addr_t)) + cpu->yr);
            lda(cpu, data);
            break;
        case LDA_IND_X:
            addr = read_2byte((inst & MASK(byte)) + cpu->xr);
            data = read_byte(addr);
            lda(cpu, data);
            break;
        case LDA_IND_Y:
            addr = read_2byte(inst & MASK(byte)) + cpu->yr;
            data = read_byte(addr);
            lda(cpu, data);
            break; 

        /******LDX******/
        case LDX_IMM:
            data = inst & MASK(byte);
            ldx(cpu, data);
            break;
        case LDX_ZERO:
            data = read_byte(inst & MASK(byte));
            ldx(cpu, data);
            break;
        case LDX_ZERO_Y:
            data = read_byte((inst & MASK(byte)) + cpu->yr);
            lda(cpu, data);
            break;
        case LDX_ABS:
            data = read_byte(inst & MASK(addr_t));
            ldx(cpu, data);
            break;
        case LDX_ABS_Y:
            data = read_byte((inst & MASK(addr_t)) + cpu->yr);
            ldx(cpu, data);
            break;

        /******LDY******/
        case LDY_IMM:
            data = inst & MASK(byte);
            ldy(cpu, data);
            break;
        case LDY_ZERO:
            data = read_byte(inst & MASK(byte));
            ldy(cpu, data);
            break;
        case LDY_ZERO_X:
            data = read_byte((inst & MASK(byte)) + cpu->xr);
            ldy(cpu, data);
            break;
        case LDY_ABS:
            data = read_byte(inst & MASK(addr_t));
            ldy(cpu, data);
            break;
        case LDY_ABS_X:
            data = read_byte((inst & MASK(addr_t)) + cpu->xr);
            ldy(cpu, data);
            break;

        /******LSR******/
        case LSR_ACC:
            cpu->acc = lsr(cpu, cpu->acc);
            break;
        case LSR_ZERO:
            addr = inst & MASK(byte);
            data = lsr(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case LSR_ZERO_X:
            addr = (inst & MASK(byte)) + cpu->xr;
            data = lsr(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case LSR_ABS:
            addr = inst & MASK(addr_t);
            data = lsr(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case LSR_ABS_X:
            addr = (inst & MASK(addr_t)) + cpu->xr;
            data = lsr(cpu, read_byte(addr));
            write_byte(addr, data);
            break;

        /******NOP******/
        case NOP:
            break;

        /******ORA******/
        case ORA_IMM:
            data = inst & MASK(byte);
            ora(cpu, data);
            break;
        case ORA_ZERO:
            data = read_byte(inst & MASK(byte));
            ora(cpu, data);
            break;
        case ORA_ZERO_X:
            data = read_byte((inst & MASK(byte)) + cpu->xr);
            ora(cpu, data);
            break;
        case ORA_ABS:
            data = read_byte(inst & MASK(addr_t));
            ora(cpu, data);
            break;
        case ORA_ABS_X:
            data = read_byte((inst & MASK(addr_t)) + cpu->xr);
            ora(cpu, data);
            break;
        case ORA_ABS_Y:
            data = read_byte((inst & MASK(addr_t)) + cpu->yr);
            ora(cpu, data);
            break;
        case ORA_IND_X:
            addr = read_2byte((inst & MASK(byte)) + cpu->xr);
            data = read_byte(addr);
            ora(cpu, data);
            break;
        case ORA_IND_Y:
            addr = read_2byte(inst & MASK(byte)) + cpu->yr;
            data = read_byte(addr);
            ora(cpu, data);
            break;

        /******PHA******/
        case PHA:
            PUSH(cpu, cpu->acc);
            break;

        /******PHP******/
        case PHP:
            PUSH(cpu, cpu->psw);
            break;

        /******PLA******/
        case PLA:
            pla(cpu);
            break;

        /******PLP******/
        case PLP:
            cpu->psw = POP(cpu);
            break;

        /******ROL******/
        case ROL_ACC:
            cpu->acc = rol(cpu, cpu->acc);
            break;
        case ROL_ZERO:
            addr = inst & MASK(byte);
            data = rol(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case ROL_ZERO_X:
            addr = (inst & MASK(byte)) + cpu->xr;
            data = rol(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case ROL_ABS:
            addr = inst & MASK(addr_t);
            data = rol(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case ROL_ABS_X:
            addr = (inst & MASK(addr_t)) + cpu->xr;
            data = rol(cpu, read_byte(addr));
            write_byte(addr, data);
            break;

        /******ROR******/
        case ROR_ACC:
            cpu->acc = ror(cpu, cpu->acc);
            break;
        case ROR_ZERO:
            addr = inst & MASK(byte);
            data = ror(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case ROR_ZERO_X:
            addr = (inst & MASK(byte)) + cpu->xr;
            data = ror(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case ROR_ABS:
            addr = inst & MASK(addr_t);
            data = ror(cpu, read_byte(addr));
            write_byte(addr, data);
            break;
        case ROR_ABS_X:
            addr = (inst & MASK(addr_t)) + cpu->xr;
            data = ror(cpu, read_byte(addr));
            write_byte(addr, data);
            break;

        /******RTI******/
        case RTI:
            rti(cpu);
            break;

        /******RTS******/
        case RTS:
            rts(cpu);
            break;

        /******SBC******/
        case SBC_IMM:
            data = inst & MASK(byte);
            sbc(cpu, data);
            break;
        case SBC_ZERO:
            data = read_byte(inst & MASK(byte));
            sbc(cpu, data);
            break;
        case SBC_ZERO_X:
            data = read_byte((inst & MASK(byte)) + cpu->xr);
            sbc(cpu, data);
            break;
        case SBC_ABS:
            data = read_byte(inst & MASK(addr_t));
            sbc(cpu, data);
            break;
        case SBC_ABS_X:
            data = read_byte((inst & MASK(addr_t)) + cpu->xr);
            sbc(cpu, data);
            break;
        case SBC_ABS_Y:
            data = read_byte((inst & MASK(addr_t)) + cpu->yr);
            sbc(cpu, data);
            break;
        case SBC_IND_X:
            addr = read_2byte((inst & MASK(byte)) + cpu->xr);
            data = read_byte(addr);
            sbc(cpu, data);
            break;
        case SBC_IND_Y:
            addr = read_2byte(inst & MASK(byte)) + cpu->yr;
            data = read_byte(addr);
            sbc(cpu, data);
            break;

        /******SEC******/
        case SEC:
            SET_BIT(cpu->psw, PSW_C, 1);
            break;

        /******SED******/
        case SED:
            SET_BIT(cpu->psw, PSW_D, 1);
            break;

        /******SEI******/
        case SEI:
            SET_BIT(cpu->psw, PSW_I, 1);
            break;

        /******STA******/
        case STA_ZERO:
            addr = inst & MASK(byte);
            write_byte(addr, cpu->acc);
            break;
        case STA_ZERO_X:
            addr = (inst & MASK(byte)) + cpu->xr;
            write_byte(addr, cpu->acc);
            break;
        case STA_ABS:
            addr = inst & MASK(addr_t);
            write_byte(addr, cpu->acc);
            //printf("mem[%x] : %x\n", addr, read_byte(addr));
            break;
        case STA_ABS_X:
            addr = (inst & MASK(addr_t)) + cpu->xr;
            write_byte(addr, cpu->acc);
            break;
        case STA_ABS_Y:
            addr = (inst & MASK(addr_t)) + cpu->yr;
            write_byte(addr, cpu->acc);
            break;
        case STA_IND_X:
            addr = (inst & MASK(byte)) + cpu->xr;
            addr = read_2byte(addr);
            write_byte(addr, cpu->acc);
            break;
        case STA_IND_Y:
            addr = inst & MASK(byte);
            addr = read_2byte(addr) + cpu->yr;
            write_byte(addr, cpu->acc);
            break; 

        /******STX******/
        case STX_ZERO:
            addr = inst & MASK(byte);
            write_byte(addr, cpu->xr);
            break;
        case STX_ZERO_Y:
            addr = (inst & MASK(byte)) + cpu->yr;
            write_byte(addr, cpu->xr);
            break;
        case STX_ABS:
            addr = inst & MASK(addr_t);
            write_byte(addr, cpu->xr);
            break;

        /******STY******/
        case STY_ZERO:
            addr = inst & MASK(byte);
            write_byte(addr, cpu->yr);
            break;
        case STY_ZERO_X:
            addr = (inst & MASK(byte)) + cpu->xr;
            write_byte(addr, cpu->yr);
            break;
        case STY_ABS:
            addr = inst & MASK(addr_t);
            write_byte(addr, cpu->yr);
            break;

        /******TAX******/
        case TAX:
            tax(cpu);
            break;
        
        /******TAY******/
        case TAY:
            tay(cpu);
            break;

        /******TSX******/
        case TSX:
            tsx(cpu);
            break;

        /******TXA******/
        case TXA:
            txa(cpu);
            break;

        /******TXS******/
        case TXS:
            cpu->sp = cpu->xr;
            break;

        /******TYA******/
        case TYA:
            tya(cpu);
            break;
        default:
            printf("error instruction!!!\n");
            printf("op_code: %2x\n", op_code);
            printf("cpu->pc : %x\n", cpu->pc);
            while(1){}
            break;
        }
    }
}

struct cpu cpu6502 = {
    .psw = 0x20, //set the reserve bit
    .acc = 0x00,
    .xr = 0x00,
    .yr = 0x00,
    .sp = 0x01FF,   //init value of Stack Pointer
    .pc = 0x00,
    .clk = 0x0000,

    /*********ADC*********/
    .is[ADC_IMM].len = 2,
    .is[ADC_IMM].cycle = 2,
    
    .is[ADC_ZERO].len = 2,
    .is[ADC_ZERO].cycle = 3,

    .is[ADC_ZERO_X].len = 2,
    .is[ADC_ZERO_X].cycle = 4,

    .is[ADC_ABS].len = 3,
    .is[ADC_ABS].cycle = 4,
    
    .is[ADC_ABS_X].len = 3,
    .is[ADC_ABS_X].cycle = 4,

    .is[ADC_ABS_Y].len = 3,
    .is[ADC_ABS_Y].cycle = 4,

    .is[ADC_IND_X].len = 2,
    .is[ADC_IND_X].cycle = 6,
    
    .is[ADC_IND_Y].len = 2,
    .is[ADC_IND_Y].cycle = 5,

    /******AND******/
    .is[AND_IMM].len = 2,
    .is[AND_IMM].cycle = 2,
    
    .is[AND_ZERO].len = 2,
    .is[AND_ZERO].cycle = 2,

    .is[AND_ZERO_X].len = 2,
    .is[AND_ZERO_X].cycle = 3,

    .is[AND_ABS].len = 3,
    .is[AND_ABS].cycle = 4,
    
    .is[AND_ABS_X].len = 3,
    .is[AND_ABS_X].cycle = 4,

    .is[AND_ABS_Y].len = 3,
    .is[AND_ABS_Y].cycle = 4,

    .is[AND_IND_X].len = 2,
    .is[AND_IND_X].cycle = 6,
    
    .is[AND_IND_Y].len = 2,
    .is[AND_IND_Y].cycle = 5,

    /******ASL******/
    .is[ASL_ACC].len = 1,
    .is[ASL_ACC].cycle = 2,

    .is[ASL_ZERO].len = 2,
    .is[ASL_ZERO].cycle = 5,

    .is[ASL_ZERO_X].len = 2,
    .is[ASL_ZERO_X].cycle = 6,

    .is[ASL_ABS].len = 3,
    .is[ASL_ABS].cycle = 6,
    
    .is[ASL_ABS_X].len = 3,
    .is[ASL_ABS_X].cycle = 7,

    /******BCC******/
    .is[BCC].len = 2,
    .is[BCC].cycle = 2,

    /******BCS******/
    .is[BCS].len = 2,
    .is[BCS].cycle = 2,

    /******BEQ******/
    .is[BEQ].len = 2,
    .is[BEQ].cycle = 2,

    /******BIT******/
    .is[BIT_ZERO].len = 2,
    .is[BIT_ZERO].cycle = 3,

    .is[BIT_ABS].len = 3,
    .is[BIT_ABS].cycle = 4,

    /******BMI******/
    .is[BMI].len = 2,
    .is[BMI].cycle = 2,

    /******BNE******/
    .is[BNE].len = 2,
    .is[BNE].cycle = 2,

    /******BPL******/
    .is[BPL].len = 2,
    .is[BPL].cycle = 2,

    /******BRK******/
    .is[BRK].len = 1,
    .is[BRK].cycle = 7,

    /******BVC******/
    .is[BVC].len = 2,
    .is[BVC].cycle = 2,

    /******BVS******/
    .is[BVS].len = 2,
    .is[BVS].cycle = 2,

    /******CLC******/
    .is[CLC].len = 1,
    .is[CLC].cycle = 2,

    /******CLD******/
    .is[CLD].len = 1,
    .is[CLD].cycle = 2,

    /******CLI******/
    .is[CLI].len = 1,
    .is[CLI].cycle = 2,

    /******CLV******/
    .is[CLV].len = 1,
    .is[CLV].cycle = 2,
    
    /*********CMP*********/
    .is[CMP_IMM].len = 2,
    .is[CMP_IMM].cycle = 2,
    
    .is[CMP_ZERO].len = 2,
    .is[CMP_ZERO].cycle = 3,

    .is[CMP_ZERO_X].len = 2,
    .is[CMP_ZERO_X].cycle = 4,

    .is[CMP_ABS].len = 3,
    .is[CMP_ABS].cycle = 4,
    
    .is[CMP_ABS_X].len = 3,
    .is[CMP_ABS_X].cycle = 4,

    .is[CMP_ABS_Y].len = 3,
    .is[CMP_ABS_Y].cycle = 4,

    .is[CMP_IND_X].len = 2,
    .is[CMP_IND_X].cycle = 6,
    
    .is[CMP_IND_Y].len = 2,
    .is[CMP_IND_Y].cycle = 5,

    /*********CPX*********/
    .is[CPX_IMM].len = 2,
    .is[CPX_IMM].cycle = 2,
    
    .is[CPX_ZERO].len = 2,
    .is[CPX_ZERO].cycle = 3,

    .is[CPX_ABS].len = 3,
    .is[CPX_ABS].cycle = 4,

    /*********CPY*********/
    .is[CPY_IMM].len = 2,
    .is[CPY_IMM].cycle = 2,
    
    .is[CPY_ZERO].len = 2,
    .is[CPY_ZERO].cycle = 3,

    .is[CPY_ABS].len = 3,
    .is[CPY_ABS].cycle = 4,

    /*********DEC*********/
    .is[DEC_ZERO].len = 2,
    .is[DEC_ZERO].cycle = 5,

    .is[DEC_ZERO_X].len = 2,
    .is[DEC_ZERO_X].cycle = 6,

    .is[DEC_ABS].len = 3,
    .is[DEC_ABS].cycle = 6,
    
    .is[DEC_ABS_X].len = 3,
    .is[DEC_ABS_X].cycle = 7,

    /******DEX******/
    .is[DEX].len = 1,
    .is[DEX].cycle = 2,

    /******DEY******/
    .is[DEY].len = 1,
    .is[DEY].cycle = 2,

    /*********EOR*********/
    .is[EOR_IMM].len = 2,
    .is[EOR_IMM].cycle = 2,
    
    .is[EOR_ZERO].len = 2,
    .is[EOR_ZERO].cycle = 3,

    .is[EOR_ZERO_X].len = 2,
    .is[EOR_ZERO_X].cycle = 4,

    .is[EOR_ABS].len = 3,
    .is[EOR_ABS].cycle = 4,
    
    .is[EOR_ABS_X].len = 3,
    .is[EOR_ABS_X].cycle = 4,

    .is[EOR_ABS_Y].len = 3,
    .is[EOR_ABS_Y].cycle = 4,

    .is[EOR_IND_X].len = 2,
    .is[EOR_IND_X].cycle = 6,
    
    .is[EOR_IND_Y].len = 2,
    .is[EOR_IND_Y].cycle = 5,

    /*********INC*********/
    .is[INC_ZERO].len = 2,
    .is[INC_ZERO].cycle = 5,

    .is[INC_ZERO_X].len = 2,
    .is[INC_ZERO_X].cycle = 6,

    .is[INC_ABS].len = 3,
    .is[INC_ABS].cycle = 6,
    
    .is[INC_ABS_X].len = 3,
    .is[INC_ABS_X].cycle = 7,

    /******INX******/
    .is[INX].len = 1,
    .is[INX].cycle = 2,

    /******INY******/
    .is[INY].len = 1,
    .is[INY].cycle = 2,

    /******JMP******/
    .is[JMP_ABS].len = 3,
    .is[JMP_ABS].cycle = 3,

    .is[JMP_IND].len = 3,
    .is[JMP_IND].cycle = 5,

    /******JSR******/
    .is[JSR].len = 3,
    .is[JSR].cycle = 6,

    /*********LDA*********/
    .is[LDA_IMM].len = 2,
    .is[LDA_IMM].cycle = 2,
    
    .is[LDA_ZERO].len = 2,
    .is[LDA_ZERO].cycle = 3,

    .is[LDA_ZERO_X].len = 2,
    .is[LDA_ZERO_X].cycle = 4,

    .is[LDA_ABS].len = 3,
    .is[LDA_ABS].cycle = 4,
    
    .is[LDA_ABS_X].len = 3,
    .is[LDA_ABS_X].cycle = 4,

    .is[LDA_ABS_Y].len = 3,
    .is[LDA_ABS_Y].cycle = 4,

    .is[LDA_IND_X].len = 2,
    .is[LDA_IND_X].cycle = 6,
    
    .is[LDA_IND_Y].len = 2,
    .is[LDA_IND_Y].cycle = 5,

    /*********LDX*********/
    .is[LDX_IMM].len = 2,
    .is[LDX_IMM].cycle = 2,
    
    .is[LDX_ZERO].len = 2,
    .is[LDX_ZERO].cycle = 3,

    .is[LDX_ZERO_Y].len = 2,
    .is[LDX_ZERO_Y].cycle = 4,

    .is[LDX_ABS].len = 2,
    .is[LDX_ABS].cycle = 4,

    .is[LDX_ABS_Y].len = 2,
    .is[LDX_ABS_Y].cycle = 4,

    /*********LDY*********/
    .is[LDY_IMM].len = 2,
    .is[LDY_IMM].cycle = 2,
    
    .is[LDY_ZERO].len = 2,
    .is[LDY_ZERO].cycle = 3,

    .is[LDY_ZERO_X].len = 2,
    .is[LDY_ZERO_X].cycle = 4,

    .is[LDY_ABS].len = 2,
    .is[LDY_ABS].cycle = 4,

    .is[LDY_ABS_X].len = 2,
    .is[LDY_ABS_X].cycle = 4,

    /*********LSR*********/
    .is[LSR_ACC].len = 1,
    .is[LSR_ACC].cycle = 2,
    
    .is[LSR_ZERO].len = 2,
    .is[LSR_ZERO].cycle = 5,

    .is[LSR_ZERO_X].len = 2,
    .is[LSR_ZERO_X].cycle = 6,

    .is[LSR_ABS].len = 3,
    .is[LSR_ABS].cycle = 6,
    
    .is[LSR_ABS_X].len = 3,
    .is[LSR_ABS_X].cycle = 7,

    /*********NOP*********/
    .is[NOP].len = 1,
    .is[NOP].cycle = 2,

    /*********ORA*********/
    .is[ORA_IMM].len = 2,
    .is[ORA_IMM].cycle = 2,
    
    .is[ORA_ZERO].len = 2,
    .is[ORA_ZERO].cycle = 2,

    .is[ORA_ZERO_X].len = 2,
    .is[ORA_ZERO_X].cycle = 3,

    .is[ORA_ABS].len = 3,
    .is[ORA_ABS].cycle = 4,
    
    .is[ORA_ABS_X].len = 3,
    .is[ORA_ABS_X].cycle = 4,

    .is[ORA_ABS_Y].len = 3,
    .is[ORA_ABS_Y].cycle = 4,

    .is[ORA_IND_X].len = 2,
    .is[ORA_IND_X].cycle = 6,
    
    .is[ORA_IND_Y].len = 2,
    .is[ORA_IND_Y].cycle = 5,

    /*********PHA*********/
    .is[PHA].len = 1,
    .is[PHA].cycle = 3,

    /*********PHP*********/
    .is[PHP].len = 1,
    .is[PHP].cycle = 3,

    /*********PLA*********/
    .is[PLA].len = 1,
    .is[PLA].cycle = 4,

    /*********PLP*********/
    .is[PLP].len = 1,
    .is[PLP].cycle = 4,

    /*********ROL*********/
    .is[ROL_ACC].len = 1,
    .is[ROL_ACC].cycle = 2,
    
    .is[ROL_ZERO].len = 2,
    .is[ROL_ZERO].cycle = 5,

    .is[ROL_ZERO_X].len = 2,
    .is[ROL_ZERO_X].cycle = 6,

    .is[ROL_ABS].len = 3,
    .is[ROL_ABS].cycle = 6,
    
    .is[ROL_ABS_X].len = 3,
    .is[ROL_ABS_X].cycle = 7,

    /*********ROR*********/
    .is[ROR_ACC].len = 1,
    .is[ROR_ACC].cycle = 2,
    
    .is[ROR_ZERO].len = 2,
    .is[ROR_ZERO].cycle = 5,

    .is[ROR_ZERO_X].len = 2,
    .is[ROR_ZERO_X].cycle = 6,

    .is[ROR_ABS].len = 3,
    .is[ROR_ABS].cycle = 6,
    
    .is[ROR_ABS_X].len = 3,
    .is[ROR_ABS_X].cycle = 7,

    /*********RTI*********/
    .is[RTI].len = 1,
    .is[RTI].cycle = 6,

    /*********RTS*********/
    .is[RTS].len = 1,
    .is[RTS].cycle = 6,

    /*********SBC*********/
    .is[SBC_IMM].len = 2,
    .is[SBC_IMM].cycle = 2,
    
    .is[SBC_ZERO].len = 2,
    .is[SBC_ZERO].cycle = 3,

    .is[SBC_ZERO_X].len = 2,
    .is[SBC_ZERO_X].cycle = 4,

    .is[SBC_ABS].len = 3,
    .is[SBC_ABS].cycle = 4,
    
    .is[SBC_ABS_X].len = 3,
    .is[SBC_ABS_X].cycle = 4,

    .is[SBC_ABS_Y].len = 3,
    .is[SBC_ABS_Y].cycle = 4,

    .is[SBC_IND_X].len = 2,
    .is[SBC_IND_X].cycle = 6,
    
    .is[SBC_IND_Y].len = 2,
    .is[SBC_IND_Y].cycle = 5,

    /*********SEC*********/
    .is[SEC].len = 1,
    .is[SEC].cycle = 2,

    /*********SED*********/
    .is[SED].len = 1,
    .is[SED].cycle = 2,

    /*********SEI*********/
    .is[SEI].len = 1,
    .is[SEI].cycle = 2,

    /*********STA*********/
    .is[STA_ZERO].len = 2,
    .is[STA_ZERO].cycle = 3,

    .is[STA_ZERO_X].len = 2,
    .is[STA_ZERO_X].cycle = 4,

    .is[STA_ABS].len = 3,
    .is[STA_ABS].cycle = 4,
    
    .is[STA_ABS_X].len = 3,
    .is[STA_ABS_X].cycle = 5,

    .is[STA_ABS_Y].len = 3,
    .is[STA_ABS_Y].cycle = 5,

    .is[STA_IND_X].len = 2,
    .is[STA_IND_X].cycle = 6,
    
    .is[STA_IND_Y].len = 2,
    .is[STA_IND_Y].cycle = 6,

    /*********STX*********/
    .is[STX_ZERO].len = 2,
    .is[STX_ZERO].cycle = 3,

    .is[STX_ZERO_Y].len = 2,
    .is[STX_ZERO_Y].cycle = 4,

    .is[STX_ABS].len = 3,
    .is[STX_ABS].cycle = 4,

    /*********STY*********/
    .is[STY_ZERO].len = 2,
    .is[STY_ZERO].cycle = 3,

    .is[STY_ZERO_X].len = 2,
    .is[STY_ZERO_X].cycle = 4,

    .is[STY_ABS].len = 3,
    .is[STY_ABS].cycle = 4,

    /*********TAX*********/
    .is[TAX].len = 1,
    .is[TAX].cycle = 2,

    /*********TAY*********/
    .is[TAY].len = 1,
    .is[TAY].cycle = 2,

    /*********TSX*********/
    .is[TSX].len = 1,
    .is[TSX].cycle = 2,

    /*********TXA*********/
    .is[TXA].len = 1,
    .is[TXA].cycle = 2,

    /*********TXS*********/
    .is[TXS].len = 1,
    .is[TXS].cycle = 2,

    /*********TYA*********/
    .is[TYA].len = 1,
    .is[TYA].cycle = 2,
};
