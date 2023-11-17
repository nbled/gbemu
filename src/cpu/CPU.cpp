#include "cpu/CPU.hpp"

namespace GameBoy 
{

CPU::CPU(MemoryInterface &mmap) : mmap(mmap) 
{
    this->Reset();
}

void CPU::Reset() 
{
    this->registers.pc = 0x0100;
    this->registers.sp = 0xfffe;
    this->registers.f = 0;

    this->cycles = 0;

    /* TODO: fix lastInstrSize to handle jump correctly */
    this->lastInstr = "";
    this->lastPC = 0;
    this->lastInstrSize = 0;

    this->status = StatusRunning;
}

void CPU::Step()
{
    this->lastInstrSize = 0;
    this->lastPC = this->registers.pc;

    uint8_t opcode = this->FetchByte();

    /* 8 bits loads */
    if ((opcode & 0xc0) == 0x40 && opcode != 0x76) {
        /* LD r, r' | r = r' */
        uint8_t dstId = (opcode & 0x38) >> 3;
        uint8_t srcId = (opcode & 0x07);

        this->SetByteRegister(dstId, this->GetByteRegister(srcId));
        this->lastInstr = "LD " + this->GetOperandFormat(dstId) + ", " + this->GetOperandFormat(srcId);
    } else if ((opcode & 0xc7) == 0x06) {
        /* LD r, n | r = n */
        uint8_t dstId = (opcode & 0x38) >> 3;
        uint8_t n = this->FetchByte();

        this->SetByteRegister(dstId, n);
        this->lastInstr = "LD " + this->GetOperandFormat(dstId) + ", " + std::to_string(n);
    } else if (opcode == 0x0a) {
        /* LD a, (bc) */
        this->registers.a = this->mmap.LoadByte(this->registers.GetBC());
        this->lastInstr = "LD a, (bc)";
    } else if (opcode == 0x1a) {
        /* LD a, (de) */
        this->registers.a = this->mmap.LoadByte(this->registers.GetDE());
        this->lastInstr = "LD a, (de)";
    } else if (opcode == 0x02) {
        /* LD (bc), a */
        this->mmap.WriteByte(this->registers.GetBC(), this->registers.a);
        this->lastInstr = "LD (bc), a";
    } else if (opcode == 0x12) {
        /* LD (de), a */
        this->mmap.WriteByte(this->registers.GetDE(), this->registers.a);
        this->lastInstr = "LD (de), a";
    } else if (opcode == 0xfa) {
        /* LD A, (nn) */
        uint16_t nn = this->FetchHalfWord();
        this->registers.a = this->mmap.LoadByte(nn);
        this->lastInstr = "LD a, (" + std::to_string(nn) + ")";
    } else if (opcode == 0xea) {
        /* LD (nn), a */
        uint16_t nn = this->FetchHalfWord();
        this->mmap.WriteByte(nn, this->registers.a);
        this->lastInstr = "LD (" + std::to_string(nn) + "), a";
    } else if (opcode == 0xf2) {
        /* LDH a, (c) */
        this->registers.a = this->mmap.LoadByte(0xff00 + this->registers.c);
        this->lastInstr = "LDH a, (c)";
    } else if (opcode == 0xe2) {
        /* LDH (c), a */
        this->mmap.WriteByte(0xff00 + this->registers.c, this->registers.a);
        this->lastInstr = "LDH (c), a";
    } else if (opcode == 0xe0) {
        /* LDH a, (n) */
        uint8_t n = this->FetchByte();
        this->registers.a = this->mmap.LoadByte(0xff00 + n);
        this->lastInstr = "LDH a, (" + std::to_string(n) + ")";
    } else if (opcode == 0xf0) {
        /* LDH (n), a */
        uint8_t n = this->FetchByte();
        this->mmap.WriteByte(0xff00 + n, this->registers.a);
        this->lastInstr = "LDH (" + std::to_string(n) + "), a";
    } else if (opcode == 0x3a) {
        /* LD a, (HL-) */
        this->registers.a = this->mmap.LoadByte(this->registers.GetHL());
        this->DecHL();
        this->lastInstr = "LD a, (HL-)";
    } else if (opcode == 0x32) {
        /* LD (HL-), a */
        this->mmap.WriteByte(this->registers.GetHL(), this->registers.a);
        this->DecHL();
        this->lastInstr = "LD (HL-), a";
    } else if (opcode == 0x2a) {
        /* LD a, (HL+) */
        this->registers.a = this->mmap.LoadByte(this->registers.GetHL());
        this->IncHL();
        this->lastInstr = "LD a, (HL+)";
    } else if (opcode == 0x22) {
        /* LD (HL+), a */
        this->mmap.WriteByte(this->registers.GetHL(), this->registers.a);
        this->IncHL();
        this->lastInstr = "LD (HL+), a";
    }

    /* 8-bit ALU */
    else if ((opcode & 0xf8) == 0x80) {
        /* ADD a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AAdd(this->GetByteRegister(srcId));
        this->lastInstr = "ADD a, " + this->GetOperandFormat(srcId);
    } else if (opcode == 0xc6) {
        /* ADD a, n */
        uint8_t n = this->FetchByte();
        this->AAdd(n);
        this->lastInstr = "ADD a, " + std::to_string(n);
    } else if ((opcode & 0xf8) == 0x88) {
        /* ADC a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AAdd(this->GetByteRegister(srcId) + this->registers.GetCarry());
        this->lastInstr = "ADC a, " + this->GetOperandFormat(srcId);
    } else if (opcode == 0xce) {
        /* ADC a, n */
        uint8_t n = this->FetchByte();
        this->AAdd(n + this->registers.GetCarry());
        this->lastInstr = "ADC a, " + std::to_string(n);
    } else if ((opcode & 0xf8) == 0x90) {
        /* SUB a, r */
        uint8_t srcId = (opcode & 0x07);
        this->ASub(this->GetByteRegister(srcId));
        this->lastInstr = "SUB a, " + this->GetOperandFormat(srcId);
    } else if (opcode == 0xd6) {
        /* SUB a, n */
        uint8_t n = this->FetchByte();
        this->ASub(n);
        this->lastInstr = "SUB a, " + std::to_string(n);
    } else if ((opcode & 0xf8) == 0x98) {
        /* SBC a, r */
        uint8_t srcId = (opcode & 0x07);
        this->ASub(this->GetByteRegister(srcId) + this->registers.GetCarry());
        this->lastInstr = "SBC a, " + this->GetOperandFormat(srcId);
    } else if (opcode == 0xde) {
        /* SBC a, n */
        uint8_t n = this->FetchByte();
        this->ASub(n + this->registers.GetCarry());
        this->lastInstr = "SBC a, " + std::to_string(n);
    } else if ((opcode & 0xf8) == 0xa0) {
        /* AND a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AAnd(this->GetByteRegister(srcId));
        this->lastInstr = "AND a, " + this->GetOperandFormat(srcId);
    } else if (opcode == 0xe6) {
        /* AND a, n */
        uint8_t n = this->FetchByte();
        this->AAnd(n);
        this->lastInstr = "AND a, " + std::to_string(n);
    } else if ((opcode & 0xf8) == 0xa8) {
        /* XOR a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AXor(this->GetByteRegister(srcId));
        this->lastInstr = "XOR a, " + this->GetOperandFormat(srcId);
    } else if (opcode == 0xee) {
        /* XOR a, n */
        uint8_t n = this->FetchByte();
        this->AXor(n);
        this->lastInstr = "XOR a, " + std::to_string(n);
    } else if ((opcode & 0xf8) == 0xb0) {
        /* OR a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AOr(this->GetByteRegister(srcId));
        this->lastInstr = "OR a, " + this->GetOperandFormat(srcId);
    } else if (opcode == 0xf6) {
        /* OR a, n */
        uint8_t n = this->FetchByte();
        this->AOr(n);
        this->lastInstr = "OR a, " + std::to_string(n);
    } else if ((opcode & 0xf8) == 0xb8) {
        /* CP a, r */
        uint8_t srcId = (opcode & 0x07);
        this->ACp(this->GetByteRegister(srcId));
        this->lastInstr = "CP a, " + this->GetOperandFormat(srcId);
    } else if (opcode == 0xfe) {
        /* CP a, n */
        uint8_t n = this->FetchByte();
        this->ACp(n);
        this->lastInstr = "CP a, " + std::to_string(n);
    } else if ((opcode & 0xc7) == 0x04) {
        /* INC r */
        uint8_t dstId = (opcode & 0x38) >> 3;
        this->Inc(dstId); 
        this->lastInstr = "INC " + this->GetOperandFormat(dstId);
    } else if ((opcode & 0xc7) == 0x05) {
        /* DEC r */
        uint8_t dstId = (opcode & 0x38) >> 3;
        this->Dec(dstId);
        this->lastInstr = "DEC " + this->GetOperandFormat(dstId);
    } else if (opcode == 0x27) {
        /* DAA */
        this->DAA();
        this->lastInstr = "DAA";
    } else if (opcode == 0x2f) {
        /* CPL */
        this->CPL();
        this->lastInstr = "CPL";
    } else if (opcode == 0x37) {
        /* CCF */
        this->registers.SetSubstract(0);
        this->registers.SetHalfCarry(0);
        if (this->registers.GetCarry())
            this->registers.SetCarry(0);
        else
            this->registers.SetCarry(1);
        this->lastInstr = "CCF";
    } else if (opcode == 0x3f) {
        /* SCF */
        this->registers.SetSubstract(0);
        this->registers.SetHalfCarry(0);
        this->registers.SetCarry(1);
        this->lastInstr = "SCF";
    }

    /* 16-bit loads */
    else if ((opcode & 0xcf) == 0x01) {
        /* LD rr, nn */
        uint8_t dstId = (opcode & 0x30) >> 4;
        uint16_t nn = this->FetchHalfWord();
        this->SetHalfWordRegister(dstId, nn, false);
        this->lastInstr = "LD " + this->GetHalfWordFormat(dstId, false) + ", " + std::to_string(nn);
    } else if (opcode == 0x08) {
        /* LD (nn), SP */
        uint16_t nn = this->FetchHalfWord();
        this->mmap.WriteHalfWord(nn, this->registers.sp);
        this->lastInstr = "LD (" + std::to_string(nn) + "), SP";
    } else if (opcode == 0xf9) {
        /* LD SP, HL */
        this->registers.sp = this->registers.GetHL();
        this->lastInstr = "LD SP, HL";
    } else if (opcode == 0xf8) {
        /* LDHL SP, n */
        int8_t n = (int8_t) this->FetchByte();
        this->registers.SetHalfCarry((((this->registers.sp & 0xff) + (n & 0xff)) & 0x100) >> 8);

        uint32_t tmp = this->registers.sp + n;
        this->registers.SetCarry((tmp & 0x10000) >> 16);

        this->registers.SetZero(0);
        this->registers.SetSubstract(0);

        this->registers.SetHL(tmp);

        this->lastInstr = "LDHL SP, " + std::to_string(n);
    } else if ((opcode & 0xcf) == 0xc5) {
        /* PUSH rr */
        uint8_t srcId = (opcode & 0x30) >> 4;
        uint16_t nn = this->GetHalfWordRegister(srcId, true);
        this->Push(nn);
        this->lastInstr = "PUSH " + this->GetHalfWordFormat(srcId, true);
    } else if ((opcode & 0xcf) == 0xc1) {
        /* POP rr */
        uint8_t dstId = (opcode & 0x30) >> 4;
        this->SetHalfWordRegister(dstId, this->Pop(), true);
        this->lastInstr = "POP " + this->GetHalfWordFormat(dstId, true);
    }

    /* 16-bits ALU */
    else if ((opcode & 0xcf) == 0x09) {
        /* ADD HL, rr */
        uint8_t dstId = (opcode & 0x30) >> 4;
        uint16_t n = this->GetHalfWordRegister(dstId, false);

        this->registers.SetHalfCarry((((this->registers.sp & 0xff) + (n & 0xff)) & 0x100) >> 8);

        uint32_t tmp = this->registers.GetHL() + n;
        this->registers.SetCarry((tmp & 0x10000) >> 16);

        this->registers.SetZero(0);
        this->registers.SetSubstract(0);

        this->registers.SetHL(tmp);

        this->lastInstr = "ADD HL, " + this->GetHalfWordFormat(dstId, false);
    } else if (opcode == 0xe8) {
        /* ADD SP, n */
        int8_t n = (int8_t) this->FetchByte();
        this->registers.SetHalfCarry((((this->registers.sp & 0xff) + (n & 0xff)) & 0x100) >> 8);

        uint32_t tmp = this->registers.sp + n;
        this->registers.SetCarry((tmp & 0x10000) >> 16);

        this->registers.SetZero(0);
        this->registers.SetSubstract(0);

        this->registers.sp = tmp;

        this->lastInstr = "ADD SP, " + std::to_string(n);
    } else if ((opcode & 0xcf) == 0x03) {
        /* INC rr */
        uint8_t dstId = (opcode & 0x30) >> 4;
        this->SetHalfWordRegister(dstId, this->GetHalfWordRegister(dstId, false) + 1, false);
        this->lastInstr = "INC " + this->GetHalfWordFormat(dstId, false);
    } else if ((opcode & 0xcf) == 0x0b) {
        /* DEC rr */
        uint8_t dstId = (opcode & 0x30) >> 4;
        this->SetHalfWordRegister(dstId, this->GetHalfWordRegister(dstId, false) - 1, false);
        this->lastInstr = "DEC " + this->GetHalfWordFormat(dstId, false);
    }

    /* Control flow */
    else if (opcode == 0xc3) {
        /* JP nn */
        uint16_t nn;

        this->mmap.Load(this->registers.pc, (uint8_t*) &nn, sizeof(nn));
        this->registers.pc += 2;
  
        this->registers.pc = nn;

        this->lastInstr = "JP " + std::to_string(nn);
    } else if ((opcode & 0xe7) == 0xc2) {
        /* JP cc, nn */
        enum Condition cc = (enum Condition) ((opcode & 0x18) >> 3);
        uint16_t nn = this->FetchHalfWord();

        this->JumpAbsoluteConditional(cc, nn);
        this->lastInstr = "JP " + this->GetConditionFormat(cc) + ", " + std::to_string(nn);
    } else if (opcode == 0xe9) {
        /* JP (HL) */
        this->registers.pc = this->mmap.LoadHalfWord(this->registers.GetHL());
        this->lastInstr = "JP (HL)";
    } else if (opcode == 0x18) {
        /* JR n */
        uint8_t n = this->FetchByte();
        this->registers.pc += n;
        this->lastInstr = "JR " + std::to_string(n);
    } else if ((opcode & 0xe7) == 0x20) {
        /* JR cc, n */
        enum Condition cc = (enum Condition) ((opcode & 0x18) >> 3);
        uint8_t n = this->FetchByte();

        this->JumpRelativeConditional(cc, n);
        this->lastInstr = "JR" + this->GetConditionFormat(cc) + ", " + std::to_string(n);
    } else if (opcode == 0xcd) {
        /* CALL nn */
        uint16_t nn = this->FetchHalfWord();
        this->Push(this->registers.pc);
        this->registers.pc = nn;
        this->lastInstr = "CALL " + std::to_string(nn);
    } else if ((opcode & 0xe7) == 0xc4) {
        /* CALL cc, nn */
        enum Condition cc = (enum Condition) ((opcode & 0x18) >> 3);
        uint16_t nn = this->FetchHalfWord();

        this->Push(this->registers.pc);
        this->JumpAbsoluteConditional((enum Condition) cc, nn);
        this->lastInstr = "CALL " + this->GetConditionFormat(cc) + ", " + std::to_string(nn);
    } else if (opcode == 0xc9) {
        /* RET */
        this->registers.pc = this->Pop();
        this->lastInstr = "RET";
    } else if ((opcode & 0xe7) == 0xc0) {
        /* RET cc */
        enum Condition cc = (enum Condition) ((opcode & 0x18) >> 3);

        switch (cc) {
            case NZ:
                if (!this->registers.GetZero())
                    this->registers.pc = this->Pop();
                break;
            case Z:
                if (this->registers.GetZero())
                    this->registers.pc = this->Pop();
                break;
            case NC:
                if (!this->registers.GetCarry())
                    this->registers.pc = this->Pop();
                break;
            case C:
                if (this->registers.GetCarry())
                    this->registers.pc = this->Pop();
                break;
            default:
                break;
        }

        this->lastInstr = "RET " + this->GetConditionFormat(cc);
    } else if (opcode == 0xd9) {
        /* RETI */
        this->registers.pc = this->Pop();
        this->interrupts = true;
        this->lastInstr = "RETI";
    } else if ((opcode & 0xc7) == 0xc7) {
        /* RST n */
        uint8_t n = (opcode & 0x38) >> 3;
        uint8_t offset = n * 8;

        this->registers.pc = offset;
        this->lastInstr = "RST " + std::to_string(offset);
    }

    /* Shifts & Rotates */
    else if (opcode == 0x07) {
        /* RLCA */
        this->RotateLeft(7, false);
        this->lastInstr = "RLCA";
    } else if (opcode == 0x17) {
        /* RLA */
        this->RotateLeft(7, true);
        this->lastInstr = "RLA";
    } else if (opcode == 0x0F) {
        /* RRCA */ 
        this->RotateRight(7, false);
        this->lastInstr = "RRCA"; 
    } else if (opcode == 0x1F) {
        /* RRA */
        this->RotateRight(7, true);
        this->lastInstr = "RRA";
    } else if (opcode == 0xCB) {
        opcode = this->FetchByte();

        if ((opcode & 0xf8) == 0x20) {
            /* SLA n */
            uint8_t dstId = (opcode & 0x07);
            uint8_t reg = this->GetByteRegister(dstId);

            this->registers.SetCarry((reg & 0x80) >> 7);

            reg <<= 1;
            this->registers.SetZero(reg == 0);
            this->registers.SetHalfCarry(0);
            this->registers.SetSubstract(0);

            this->SetByteRegister(dstId, reg);

            this->lastInstr = "SLA " + this->GetOperandFormat(dstId);
        } else if ((opcode & 0xf8) == 0x28) {
            /* SRA n */
            uint8_t dstId = (opcode & 0x07);
            uint8_t reg = this->GetByteRegister(dstId);

            this->registers.SetCarry((reg & 0x80) >> 7);

            reg = (reg & 0x80) | (reg >> 1);
            this->registers.SetZero(reg == 0);
            this->registers.SetHalfCarry(0);
            this->registers.SetSubstract(0);

            this->SetByteRegister(dstId, reg);

            this->lastInstr = "SRA " + this->GetOperandFormat(dstId);
        } else if ((opcode & 0xf8) == 0x38) {
            /* SRL n */
            uint8_t dstId = (opcode & 0x07);
            uint8_t reg = this->GetByteRegister(dstId);

            this->registers.SetCarry(reg & 0x01);
            
            reg >>= 1;
            this->registers.SetZero(reg == 0);
            this->registers.SetHalfCarry(0);
            this->registers.SetSubstract(0);

            this->SetByteRegister(dstId, reg);

            this->lastInstr = "SRL " + this->GetOperandFormat(dstId);
        } else if ((opcode & 0xf8) == 0x00) {
            /* RLC n */
            uint8_t dstId = (opcode & 0x07);
            this->RotateLeft(dstId, false);
            this->lastInstr = "RLC " + this->GetOperandFormat(dstId);
        } else if ((opcode & 0xf8) == 0x10) {
            /* RL n */
            uint8_t dstId = (opcode & 0x07);
            this->RotateLeft(dstId, true);
            this->lastInstr = "RL " + this->GetOperandFormat(dstId);
        } else if ((opcode & 0xf8) == 0x08) {
            /* RRC n */
            uint8_t dstId = (opcode & 0x07);
            this->RotateRight(dstId, false);
            this->lastInstr = "RRC " + this->GetOperandFormat(dstId);
        } else if ((opcode & 0xf8) == 0x18) {
            /* RR n */
            uint8_t dstId = (opcode & 0x07);
            this->RotateRight(dstId, true);
            this->lastInstr = "RR " + this->GetOperandFormat(dstId);
        }

        /* Bit manipulation */
        else if ((opcode & 0xc0) == 0x40) {
            /* BIT b, n */
            uint8_t b = (opcode & 0x38) >> 3;
            uint8_t dstId = (opcode & 0x07);
            uint8_t reg = this->GetByteRegister(dstId);

            this->registers.SetZero(!(reg & (1 << b)));
            this->registers.SetSubstract(0);
            this->registers.SetHalfCarry(1);

            this->lastInstr = "BIT " + std::to_string(b) + ", " + this->GetOperandFormat(dstId);
        } else if ((opcode & 0xc0) == 0xc0) {
            /* SET b, n */
            uint8_t b = (opcode & 0x38) >> 3;
            uint8_t dstId = (opcode & 0x07);
            uint8_t reg = this->GetByteRegister(dstId);

            reg |= (1 << b);
            
            this->SetByteRegister(dstId, reg);

            this->lastInstr = "SET " + std::to_string(b) + ", " + this->GetOperandFormat(dstId);
        } else if ((opcode & 0xc0) == 0x80) {
            /* RES b, n */
            uint8_t b = (opcode & 0x38) >> 3;
            uint8_t dstId = (opcode & 0x07);
            uint8_t reg = this->GetByteRegister(dstId);

            reg &= ~(1 << b);

            this->SetByteRegister(dstId, reg);

            this->lastInstr = "RES " + std::to_string(b) + ", " + this->GetOperandFormat(dstId);
        }

        /* TODO: fail here */
    }

    
    /* Misc & System */
    else if (opcode == 0x00) {
        /* NOP */
        this->lastInstr = "NOP";
    } else if (opcode == 0x76) {
        /* HALT */
        this->status = StatusHalted;
        this->lastInstr = "HALT";
    } else if (opcode == 0xF3) {
        /* DI */
        this->interrupts = false;
        this->lastInstr = "DI";
    } else if (opcode == 0xFB) {
        /* EI */
        this->interrupts = true;
        this->lastInstr = "EI";
    } else if (opcode == 0x10) {
        opcode = this->FetchByte();
        if (opcode == 0) {
            this->status = StatusStopped;
            this->lastInstr = "STOP";
        } 
        /* TODO: fail here */
    }

    /* TODO: fail here */
    //this->Dump();
}

void CPU::AAdd(uint8_t value)
{
    this->registers.SetHalfCarry((((this->registers.a & 0xf) + (value & 0xf)) & 0x10) >> 4);
    this->registers.SetSubstract(0);

    uint16_t tmp = this->registers.a + value;
    this->registers.SetCarry((tmp & 0x100) >> 8);

    this->registers.a = tmp & 0xff;

    this->registers.SetZero(this->registers.a == 0);
}

void CPU::ASub(uint8_t value)
{
    this->registers.SetHalfCarry((value & 0xf) > (this->registers.a & 0xf));
    this->registers.SetCarry(value > this->registers.a);
    this->registers.SetSubstract(1);

    this->registers.a -= value;

    if (this->registers.a == 0)
        this->registers.SetZero(1);
    else
        this->registers.SetZero(0);
}

void CPU::AAnd(uint8_t value)
{
    this->registers.a &= value;

    if (this->registers.a == 0)
        this->registers.SetZero(1);
    else
        this->registers.SetZero(0);
    
    this->registers.SetSubstract(0);
    this->registers.SetHalfCarry(1);
    this->registers.SetCarry(0);
}

void CPU::AOr(uint8_t value)
{
    this->registers.a |= value;

    if (this->registers.a == 0)
        this->registers.SetZero(1);
    else
        this->registers.SetZero(0);
    
    this->registers.SetHalfCarry(0);
    this->registers.SetSubstract(0);
    this->registers.SetCarry(0);
}

void CPU::AXor(uint8_t value)
{
    this->registers.a ^= value;

    if (this->registers.a == 0)
        this->registers.SetZero(1);
    else
        this->registers.SetZero(0);

    this->registers.SetHalfCarry(0);
    this->registers.SetSubstract(0);
    this->registers.SetCarry(0);
}

void CPU::ACp(uint8_t value)
{
    this->registers.SetHalfCarry((value & 0xf) > (this->registers.a & 0xf));
    this->registers.SetCarry(value > this->registers.a);
    this->registers.SetSubstract(1);

    if (this->registers.a - value == 0)
        this->registers.SetZero(1);
    else
        this->registers.SetZero(0);
}

void CPU::Inc(uint8_t id)
{
    uint8_t reg = this->GetByteRegister(id);

    if (((reg & 0xf) + 1) & 0x10)
        this->registers.SetHalfCarry(1);
    else
        this->registers.SetHalfCarry(0);
    this->registers.SetSubstract(0);

    reg++;
    if (reg == 0)
        this->registers.SetZero(1);
    else
        this->registers.SetZero(0);

    this->SetByteRegister(id, reg);
}

void CPU::Dec(uint8_t id)
{
    uint8_t reg = this->GetByteRegister(id);

    this->registers.SetHalfCarry((1 & 0xf) > (reg & 0xf));
    this->registers.SetSubstract(1);

    reg--;
    if (reg == 0)
        this->registers.SetZero(1);
    else
        this->registers.SetZero(0);

    this->SetByteRegister(id, reg);
}

void CPU::DAA(void)
{
    uint8_t c = this->registers.GetCarry();
    this->registers.SetCarry(0);
    
    uint8_t a = this->registers.a;
    if ((a & 0x0f) > 0x09 || this->registers.GetHalfCarry())
        a += 0x06;
    if ((a & 0xf0) > 0x90 || c) {
        a += 0x60;
        this->registers.SetCarry(1);
    }

    this->registers.SetHalfCarry(0);
    if (a == 0)
        this->registers.SetZero(1);
    else
        this->registers.SetZero(0);
    
    this->registers.a = a;
}

void CPU::CPL(void)
{
    this->registers.a = ~this->registers.a;
    this->registers.SetSubstract(1);
    this->registers.SetHalfCarry(1);
}

void CPU::RotateLeft(uint8_t id, bool through_carry)
{
    uint8_t reg = this->GetByteRegister(id);
    uint8_t carry = this->registers.GetCarry();

    this->registers.SetCarry((reg & 0x80) >> 7);

    if (through_carry)
        reg = (reg << 1) | carry;
    else
        reg = (reg << 1) | (reg & 0x01); 

    this->registers.SetZero(reg == 0);
    this->registers.SetSubstract(0);
    this->registers.SetHalfCarry(0);

    this->SetByteRegister(id, reg);
}

void CPU::RotateRight(uint8_t id, bool through_carry)
{
    uint8_t reg = this->GetByteRegister(id);
    uint8_t carry = this->registers.GetCarry();
    
    this->registers.SetCarry(reg & 0x01);

    if (through_carry)
        reg = (carry << 7) | (reg >> 1);
    else
        reg = ((reg & 0x01) << 7) | (reg >> 1); 

    this->registers.SetZero(reg == 0);
    this->registers.SetSubstract(0);
    this->registers.SetHalfCarry(0);

    this->SetByteRegister(id, reg);
}

void CPU::JumpAbsoluteConditional(enum CPU::Condition cc, uint16_t address)
{
    switch (cc) {
        case NZ:
            if (!this->registers.GetZero())
                this->registers.pc = address;
            break;
        case Z:
            if (this->registers.GetZero())
                this->registers.pc = address;
            break;
        case NC:
            if (!this->registers.GetCarry())
                this->registers.pc = address;
            break;
        case C:
            if (this->registers.GetCarry())
                this->registers.pc = address;
            break;
        default:
            break;
    }
}

};
