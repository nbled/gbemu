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

    this->status = StatusRunning;
}

uint64_t CPU::Step()
{
    uint8_t opcode = this->FetchByte();

    /* 8 bits loads */
    if ((opcode & 0xc0) == 0x40 && opcode != 0x76) {
        /* LD r, r' | r = r' */
        uint8_t dstId = (opcode & 0x38) >> 3;
        uint8_t srcId = (opcode & 0x07);

        this->SetByteRegister(dstId, this->GetByteRegister(srcId));
    } else if ((opcode & 0xc7) == 0x06) {
        /* LD r, n | r = n */
        uint8_t dstId = (opcode & 0x38) >> 3;
        uint8_t n = this->FetchByte();

        this->SetByteRegister(dstId, n);
    } else if (opcode == 0x0a) {
        /* LD a, (bc) */
        this->registers.a = this->LoadByteCycled(this->registers.GetBC());
    } else if (opcode == 0x1a) {
        /* LD a, (de) */
        this->registers.a = this->LoadByteCycled(this->registers.GetDE());
    } else if (opcode == 0x02) {
        /* LD (bc), a */
        this->WriteByteCycled(this->registers.GetBC(), this->registers.a);
    } else if (opcode == 0x12) {
        /* LD (de), a */
        this->WriteByteCycled(this->registers.GetDE(), this->registers.a);
    } else if (opcode == 0xfa) {
        /* LD A, (nn) */
        uint16_t nn = this->FetchHalfWord();
        this->registers.a = this->LoadByteCycled(nn);
    } else if (opcode == 0xea) {
        /* LD (nn), a */
        uint16_t nn = this->FetchHalfWord();
        this->WriteByteCycled(nn, this->registers.a);
    } else if (opcode == 0xf2) {
        /* LDH a, (c) */
        this->registers.a = this->LoadByteCycled(0xff00 + this->registers.c);
    } else if (opcode == 0xe2) {
        /* LDH (c), a */
        this->WriteByteCycled(0xff00 + this->registers.c, this->registers.a);
    } else if (opcode == 0xf0) {
        /* LDH a, (n) */
        uint8_t n = this->FetchByte();
        this->registers.a = this->LoadByteCycled(0xff00 + n);
    } else if (opcode == 0xe0) {
        /* LDH (n), a */
        uint8_t n = this->FetchByte();
        this->WriteByteCycled(0xff00 + n, this->registers.a);
    } else if (opcode == 0x3a) {
        /* LD a, (HL-) */
        this->registers.a = this->LoadByteCycled(this->registers.GetHL());
        this->DecHL();
    } else if (opcode == 0x32) {
        /* LD (HL-), a */
        this->WriteByteCycled(this->registers.GetHL(), this->registers.a);
        this->DecHL();
    } else if (opcode == 0x2a) {
        /* LD a, (HL+) */
        this->registers.a = this->LoadByteCycled(this->registers.GetHL());
        this->IncHL();
    } else if (opcode == 0x22) {
        /* LD (HL+), a */
        this->WriteByteCycled(this->registers.GetHL(), this->registers.a);
        this->IncHL();
    }

    /* 8-bit ALU */
    else if ((opcode & 0xf8) == 0x80) {
        /* ADD a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AAdd(this->GetByteRegister(srcId), false);
    } else if (opcode == 0xc6) {
        /* ADD a, n */
        uint8_t n = this->FetchByte();
        this->AAdd(n, false);
    } else if ((opcode & 0xf8) == 0x88) {
        /* ADC a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AAdd(this->GetByteRegister(srcId), this->registers.GetCarry());
    } else if (opcode == 0xce) {
        /* ADC a, n */
        uint8_t n = this->FetchByte();
        this->AAdd(n, this->registers.GetCarry());
    } else if ((opcode & 0xf8) == 0x90) {
        /* SUB a, r */
        uint8_t srcId = (opcode & 0x07);
        this->ASub(this->GetByteRegister(srcId), false);
    } else if (opcode == 0xd6) {
        /* SUB a, n */
        uint8_t n = this->FetchByte();
        this->ASub(n, false);
    } else if ((opcode & 0xf8) == 0x98) {
        /* SBC a, r */
        uint8_t srcId = (opcode & 0x07);
        this->ASub(this->GetByteRegister(srcId), this->registers.GetCarry());
    } else if (opcode == 0xde) {
        /* SBC a, n */
        uint8_t n = this->FetchByte();
        this->ASub(n, this->registers.GetCarry());
    } else if ((opcode & 0xf8) == 0xa0) {
        /* AND a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AAnd(this->GetByteRegister(srcId));
    } else if (opcode == 0xe6) {
        /* AND a, n */
        uint8_t n = this->FetchByte();
        this->AAnd(n);
    } else if ((opcode & 0xf8) == 0xa8) {
        /* XOR a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AXor(this->GetByteRegister(srcId));
    } else if (opcode == 0xee) {
        /* XOR a, n */
        uint8_t n = this->FetchByte();
        this->AXor(n);
    } else if ((opcode & 0xf8) == 0xb0) {
        /* OR a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AOr(this->GetByteRegister(srcId));
    } else if (opcode == 0xf6) {
        /* OR a, n */
        uint8_t n = this->FetchByte();
        this->AOr(n);
    } else if ((opcode & 0xf8) == 0xb8) {
        /* CP a, r */
        uint8_t srcId = (opcode & 0x07);
        this->ACp(this->GetByteRegister(srcId));
    } else if (opcode == 0xfe) {
        /* CP a, n */
        uint8_t n = this->FetchByte();
        this->ACp(n);
    } else if ((opcode & 0xc7) == 0x04) {
        /* INC r */
        uint8_t dstId = (opcode & 0x38) >> 3;
        this->Inc(dstId); 
    } else if ((opcode & 0xc7) == 0x05) {
        /* DEC r */
        uint8_t dstId = (opcode & 0x38) >> 3;
        this->Dec(dstId);
    } else if (opcode == 0x27) {
        /* DAA */
        this->DAA();
    } else if (opcode == 0x2f) {
        /* CPL */
        this->CPL();
    } else if (opcode == 0x3f) {
        /* CCF */
        this->registers.SetSubstract(0);
        this->registers.SetHalfCarry(0);
        if (this->registers.GetCarry())
            this->registers.SetCarry(0);
        else
            this->registers.SetCarry(1);
    } else if (opcode == 0x37) {
        /* SCF */
        this->registers.SetSubstract(0);
        this->registers.SetHalfCarry(0);
        this->registers.SetCarry(1);
    }

    /* 16-bit loads */
    else if ((opcode & 0xcf) == 0x01) {
        /* LD rr, nn */
        uint8_t dstId = (opcode & 0x30) >> 4;
        uint16_t nn = this->FetchHalfWord();
        this->SetHalfWordRegister(dstId, nn, false);
    } else if (opcode == 0x08) {
        /* LD (nn), SP */
        uint16_t nn = this->FetchHalfWord();
        this->WriteHalfWordCycled(nn, this->registers.sp);
    } else if (opcode == 0xf9) {
        /* LD SP, HL */
        this->registers.sp = this->registers.GetHL();
    } else if (opcode == 0xf8) {
        /* LDHL SP, n */
        int8_t n = (int8_t) this->FetchByte();
        
        this->registers.SetHalfCarry((((this->registers.sp & 0xf) + (n & 0xf)) & 0x10) >> 4);
        this->registers.SetCarry((((this->registers.sp & 0xff) + (n & 0xff)) & 0x100) >> 8);
        this->registers.SetZero(0);
        this->registers.SetSubstract(0);

        this->registers.SetHL(this->registers.sp + n);
    } else if ((opcode & 0xcf) == 0xc5) {
        /* PUSH rr */
        uint8_t srcId = (opcode & 0x30) >> 4;
        uint16_t nn = this->GetHalfWordRegister(srcId, true);
        this->Push(nn);
    } else if ((opcode & 0xcf) == 0xc1) {
        /* POP rr */
        uint8_t dstId = (opcode & 0x30) >> 4;
        this->SetHalfWordRegister(dstId, this->Pop(), true);
    }

    /* 16-bits ALU */
    else if ((opcode & 0xcf) == 0x09) {
        /* ADD HL, rr */
        uint8_t dstId = (opcode & 0x30) >> 4;
        uint16_t n = this->GetHalfWordRegister(dstId, false);

        uint32_t tmp = this->registers.GetHL() + n;
        /* https://stackoverflow.com/questions/57958631/game-boy-half-carry-flag-and-16-bit-instructions-especially-opcode-0xe8 */
        uint8_t tmp_c = (uint16_t)(this->registers.l + (n & 0xff)) >> 8;

        this->registers.SetHalfCarry((((this->registers.h & 0xf) + ((n >> 8) & 0xf) + tmp_c) & 0x10) >> 4);
        this->registers.SetCarry((tmp & 0x10000) >> 16);
        this->registers.SetSubstract(0);

        this->registers.SetHL(tmp);
    } else if (opcode == 0xe8) {
        /* ADD SP, n */
        int8_t n = (int8_t) this->FetchByte();

        this->registers.SetHalfCarry((((this->registers.sp & 0xf) + (n & 0xf)) & 0x10) >> 4);
        this->registers.SetCarry((((this->registers.sp & 0xff) + (n & 0xff)) & 0x100) >> 8);
        this->registers.SetZero(0);
        this->registers.SetSubstract(0);
        
        this->registers.sp += n;
    } else if ((opcode & 0xcf) == 0x03) {
        /* INC rr */
        uint8_t dstId = (opcode & 0x30) >> 4;
        this->SetHalfWordRegister(dstId, this->GetHalfWordRegister(dstId, false) + 1, false);
    } else if ((opcode & 0xcf) == 0x0b) {
        /* DEC rr */
        uint8_t dstId = (opcode & 0x30) >> 4;
        this->SetHalfWordRegister(dstId, this->GetHalfWordRegister(dstId, false) - 1, false);
    }

    /* Control flow */
    else if (opcode == 0xc3) {
        /* JP nn */
        uint16_t nn = this->FetchHalfWord();
        this->SetPCCycled(nn);
    } else if ((opcode & 0xe7) == 0xc2) {
        /* JP cc, nn */
        enum Condition cc = (enum Condition) ((opcode & 0x18) >> 3);
        uint16_t nn = this->FetchHalfWord();

        if (this->IsConditionSatisfied(cc))
            this->SetPCCycled(nn);
    } else if (opcode == 0xe9) {
        /* JP HL */
        /* For unknown reason this takes 4 cycles instead of 8 ? */
        this->registers.pc = this->registers.GetHL();
    } else if (opcode == 0x18) {
        /* JR n */
        int8_t n = (int8_t) this->FetchByte();
        this->SetPCCycled(this->registers.pc + n);
    } else if ((opcode & 0xe7) == 0x20) {
        /* JR cc, n */
        enum Condition cc = (enum Condition) ((opcode & 0x18) >> 3);
        int8_t n = (int8_t) this->FetchByte();

        if (this->IsConditionSatisfied(cc))
            this->SetPCCycled(this->registers.pc + n);
    } else if (opcode == 0xcd) {
        /* CALL nn */
        uint16_t nn = this->FetchHalfWord();
        
        this->Push(this->registers.pc);
        this->SetPCCycled(nn);
    } else if ((opcode & 0xe7) == 0xc4) {
        /* CALL cc, nn */
        enum Condition cc = (enum Condition) ((opcode & 0x18) >> 3);
        uint16_t nn = this->FetchHalfWord();

        if (this->IsConditionSatisfied(cc)) {
            this->Push(this->registers.pc);
            this->SetPCCycled(nn);
        }
    } else if (opcode == 0xc9) {
        /* RET */
        this->SetPCCycled(this->Pop());
    } else if ((opcode & 0xe7) == 0xc0) {
        /* RET cc */
        enum Condition cc = (enum Condition) ((opcode & 0x18) >> 3);

        if (this->IsConditionSatisfied(cc))
            this->SetPCCycled(this->Pop());
    } else if (opcode == 0xd9) {
        /* RETI */
        this->SetPCCycled(this->Pop());
        this->interrupts = true;
    } else if ((opcode & 0xc7) == 0xc7) {
        /* RST n */
        uint8_t n = (opcode & 0x38) >> 3;
        uint8_t offset = n * 8;

        this->Push(this->registers.pc);
        this->SetPCCycled(offset);
    }

    /* Shifts & Rotates */
    else if (opcode == 0x07) {
        /* RLCA */
        this->RotateLeft(7, false);
        /* For some reason Z is untouched when the op. is on A */
        this->registers.SetZero(0);
    } else if (opcode == 0x17) {
        /* RLA */
        this->RotateLeft(7, true);
        this->registers.SetZero(0);
    } else if (opcode == 0x0F) {
        /* RRCA */ 
        this->RotateRight(7, false);
        this->registers.SetZero(0);
    } else if (opcode == 0x1F) {
        /* RRA */
        this->RotateRight(7, true);
        this->registers.SetZero(0);
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
        } else if ((opcode & 0xf8) == 0x28) {
            /* SRA n */
            uint8_t dstId = (opcode & 0x07);
            uint8_t reg = this->GetByteRegister(dstId);

            this->registers.SetCarry((reg & 0x01));

            reg = (reg & 0x80) | (reg >> 1);
            this->registers.SetZero(reg == 0);
            this->registers.SetHalfCarry(0);
            this->registers.SetSubstract(0);

            this->SetByteRegister(dstId, reg);
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
        } else if ((opcode & 0xf8) == 0x00) {
            /* RLC n */
            uint8_t dstId = (opcode & 0x07);
            this->RotateLeft(dstId, false);
        } else if ((opcode & 0xf8) == 0x10) {
            /* RL n */
            uint8_t dstId = (opcode & 0x07);
            this->RotateLeft(dstId, true);
        } else if ((opcode & 0xf8) == 0x08) {
            /* RRC n */
            uint8_t dstId = (opcode & 0x07);
            this->RotateRight(dstId, false);
        } else if ((opcode & 0xf8) == 0x18) {
            /* RR n */
            uint8_t dstId = (opcode & 0x07);
            this->RotateRight(dstId, true);
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
        } else if ((opcode & 0xc0) == 0xc0) {
            /* SET b, n */
            uint8_t b = (opcode & 0x38) >> 3;
            uint8_t dstId = (opcode & 0x07);
            uint8_t reg = this->GetByteRegister(dstId);

            reg |= (1 << b);
            
            this->SetByteRegister(dstId, reg);
        } else if ((opcode & 0xc0) == 0x80) {
            /* RES b, n */
            uint8_t b = (opcode & 0x38) >> 3;
            uint8_t dstId = (opcode & 0x07);
            uint8_t reg = this->GetByteRegister(dstId);

            reg &= ~(1 << b);

            this->SetByteRegister(dstId, reg);
        }

        /* Swap ? */
        else if ((opcode & 0xf8) == 0x30) {
            /* SWAP n */
            uint8_t dstId = (opcode & 0x07);
            uint8_t reg = this->GetByteRegister(dstId);
        
            reg = (reg << 4) | (reg >> 4);

            this->registers.SetZero(reg == 0);
            this->registers.SetSubstract(0);
            this->registers.SetHalfCarry(0);
            this->registers.SetCarry(0);

            this->SetByteRegister(dstId, reg);
        }
        
        else {
            throw std::runtime_error("Illegal Instruction");
        }
    }

    
    /* Misc & System */
    else if (opcode == 0x00) {
        /* NOP */
    } else if (opcode == 0x76) {
        /* HALT */
        this->status = StatusHalted;
    } else if (opcode == 0xF3) {
        /* DI */
        this->interrupts = false;
    } else if (opcode == 0xFB) {
        /* EI */
        this->interrupts = true;
    } else if (opcode == 0x10) {
        opcode = this->FetchByte();
        //if (opcode == 0) {
            this->status = StatusStopped;
        //} else {
        //    throw std::runtime_error("Illegal Instruction");
        //}
    }

    else {
        throw std::runtime_error("Illegal Instruction");
    }

    return this->cycles;
}

void CPU::AAdd(uint8_t value, bool carry)
{
    this->registers.SetHalfCarry((((this->registers.a & 0xf) + (value & 0xf) + carry) & 0x10) >> 4);
    this->registers.SetSubstract(0);

    uint16_t tmp = this->registers.a + value + carry;
    this->registers.SetCarry((tmp & 0x100) >> 8);

    this->registers.a = tmp & 0xff;

    this->registers.SetZero(this->registers.a == 0);
}

void CPU::ASub(uint8_t value, bool carry)
{
    this->registers.SetHalfCarry((value & 0xf) + carry > (this->registers.a & 0xf));
    this->registers.SetCarry(value + carry > this->registers.a);
    this->registers.SetSubstract(1);

    this->registers.a -= (value + carry);

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

/**
 * https://faculty.kfupm.edu.sa/COE/aimane/assembly/pagegen-68.aspx.htm
 * https://aplawrence.com/Basics/packedbcd.html
 */
void CPU::DAA(void)
{
    uint8_t c = this->registers.GetCarry();
    uint16_t a = this->registers.a;

    if (this->registers.GetSubstract()) {
        if (this->registers.GetHalfCarry())
            a += 0xfa;
        if (this->registers.GetCarry())
            a += 0xa0;
    } else {
        if ((a & 0x0f) > 0x09 || this->registers.GetHalfCarry())
            a += 0x06;
        
        if ((a & 0x1f0) > 0x90 || c) {
            a += 0x60;
            this->registers.SetCarry(1);
        } else {
            this->registers.SetCarry(0);
        }
    }

    a &= 0xff;

    this->registers.SetHalfCarry(0);
    this->registers.SetZero(a == 0);
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
    uint8_t msb = (reg & 0x80) >> 7;

    if (through_carry)
        reg = (reg << 1) | carry;
    else
        reg = (reg << 1) | msb; 

    this->registers.SetZero(reg == 0);
    this->registers.SetCarry(msb);    
    this->registers.SetSubstract(0);
    this->registers.SetHalfCarry(0);

    this->SetByteRegister(id, reg);
}

void CPU::RotateRight(uint8_t id, bool through_carry)
{
    uint8_t reg = this->GetByteRegister(id);
    uint8_t carry = this->registers.GetCarry();
    uint8_t lsb = reg & 0x01;

    if (through_carry)
        reg = (carry << 7) | (reg >> 1);
    else
        reg = (lsb << 7) | (reg >> 1); 

    this->registers.SetZero(reg == 0);
    this->registers.SetCarry(lsb);
    this->registers.SetSubstract(0);
    this->registers.SetHalfCarry(0);

    this->SetByteRegister(id, reg);
}

Instruction CPU::DecodeNextInstruction() const
{
    uint8_t fakePC = this->GetPC();
    uint8_t opcode = this->mmap.LoadByte(fakePC++);

    bool isCB = false;

    if (opcode == 0xCB) {
        isCB = true;
        opcode = this->mmap.LoadByte(fakePC++);
    }

    Instruction instr(opcode, isCB);
    switch (instr.GetBaseInstruction()->GetArgumentType()) {
        case Immediate8:
            instr.SetByteArgument(this->mmap.LoadByte(fakePC++));
            break;
        case Immediate16:
            instr.SetHalfwordArgument(this->mmap.LoadHalfWord(fakePC));
            fakePC += 2;
            break;
        case NoImmediate:
        default:
            break;
    }

    return instr;
}

};
