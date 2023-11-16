#include "GameBoyCPU.hpp"

namespace GameBoy 
{

CPU::CPU(MMap &gbMMap) : mmap(gbMMap) 
{
    this->Reset();
}

void CPU::Reset() 
{
    this->registers.pc = 0x0100;
    this->registers.sp = 0xfffe;
    this->cycles = 0;
}

void CPU::Step()
{
    uint8_t opcode = this->FetchByte();
    std::cout << std::hex << (int)opcode << std::endl;

    /* 8 bits loads */
    if ((opcode & 0xc0) == 0x40) {
        /* LD r, r' | r = r' */
        uint8_t dstId = (opcode & 0x38) >> 3;
        uint8_t srcId = (opcode & 0x07);

        *this->GetOperandPointer(dstId) = *this->GetOperandPointer(srcId);
    } else if ((opcode & 0xc7) == 0x06) {
        /* LD r, n | r = n */
        uint8_t dstId = (opcode & 0x38) >> 3;
        uint8_t n = this->FetchByte();

        *this->GetOperandPointer(dstId) = n;
    } else if (opcode == 0x0a) {
        /* LD a, (bc) */
        this->registers.a = this->mmap.LoadByte(this->GetBC());
    } else if (opcode == 0x1a) {
        /* LD a, (de) */
        this->registers.a = this->mmap.LoadByte(this->GetDE());
    } else if (opcode == 0x02) {
        /* LD (bc), a */
        this->mmap.WriteByte(this->GetBC(), this->registers.a);
    } else if (opcode == 0x12) {
        /* LD (de), a */
        this->mmap.WriteByte(this->GetDE(), this->registers.a);
    } else if (opcode == 0xfa) {
        /* LD A, (nn) */
        uint16_t nn = this->FetchHalfWord();
        this->registers.a = this->mmap.LoadByte(nn);
    } else if (opcode == 0xea) {
        /* LD (nn), a */
        uint16_t nn = this->FetchHalfWord();
        this->mmap.WriteByte(nn, this->registers.a);
    } else if (opcode == 0xf2) {
        /* LDH a, (c) */
        this->registers.a = this->mmap.LoadByte(0xff00 + this->registers.c);
    } else if (opcode == 0xe2) {
        /* LDH (c), a */
        this->mmap.WriteByte(0xff00 + this->registers.c, this->registers.a);
    } else if (opcode == 0xe0) {
        /* LDH a, (n) */
        uint8_t n = this->FetchByte();
        this->registers.a = this->mmap.LoadByte(0xff00 + n);
    } else if (opcode == 0xf0) {
        /* LDH (n), a */
        uint8_t n = this->FetchByte();
        this->mmap.WriteByte(0xff00 + n, this->registers.a);
    } else if (opcode == 0x3a) {
        /* LD a, (HL-) */
        this->registers.a = this->mmap.LoadByte(this->GetHL());
        this->DecHL();
    } else if (opcode == 0x32) {
        /* LD (HL-), a */
        this->mmap.WriteByte(this->GetHL(), this->registers.a);
        this->DecHL();
    } else if (opcode == 0x2a) {
        /* LD a, (HL+) */
        this->registers.a = this->mmap.LoadByte(this->GetHL());
        this->IncHL();
    } else if (opcode == 0x22) {
        /* LD (HL+), a */
        this->mmap.WriteByte(this->GetHL(), this->registers.a);
        this->IncHL();
    }

    /* 8-bit ALU */
    else if ((opcode & 0xf8) == 0x80) {
        /* ADD a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AAdd(*this->GetOperandPointer(srcId));
    } else if (opcode == 0xc6) {
        /* ADD a, n */
        uint8_t n = this->mmap.LoadByte(this->registers.pc++);
        this->AAdd(n);
    } else if ((opcode & 0xf8) == 0x88) {
        /* ADC a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AAdd(*this->GetOperandPointer(srcId) + this->GetCarry());
    } else if (opcode == 0xce) {
        /* ADC a, n */
        uint8_t n = this->mmap.LoadByte(this->registers.pc++);
        this->AAdd(n + this->GetCarry());
    } else if ((opcode & 0xf8) == 0x90) {
        /* SUB a, r */
        uint8_t srcId = (opcode & 0x07);
        this->ASub(*this->GetOperandPointer(srcId));
    } else if (opcode == 0xd6) {
        /* SUB a, n */
        uint8_t n = this->mmap.LoadByte(this->registers.pc++);
        this->ASub(n);
    } else if ((opcode & 0xf8) == 0x98) {
        /* SBC a, r */
        uint8_t srcId = (opcode & 0x07);
        this->ASub(*this->GetOperandPointer(srcId) + this->GetCarry());
    } else if (opcode == 0xde) {
        /* SBC a, n */
        uint8_t n = this->mmap.LoadByte(this->registers.pc++);
        this->ASub(n + this->GetCarry());
    } else if ((opcode & 0xf8) == 0xa0) {
        /* AND a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AAnd(*this->GetOperandPointer(srcId));
    } else if (opcode == 0xe6) {
        /* AND a, n */
        uint8_t n = this->mmap.LoadByte(this->registers.pc++);
        this->AAnd(n);
    } else if ((opcode & 0xf8) == 0xa8) {
        /* XOR a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AXor(*this->GetOperandPointer(srcId));
    } else if (opcode == 0xee) {
        /* XOR a, n */
        uint8_t n = this->mmap.LoadByte(this->registers.pc++);
        this->AXor(n);
    } else if ((opcode & 0xf8) == 0xb0) {
        /* OR a, r */
        uint8_t srcId = (opcode & 0x07);
        this->AOr(*this->GetOperandPointer(srcId));
    } else if (opcode == 0xf6) {
        /* OR a, n */
        uint8_t n = this->mmap.LoadByte(this->registers.pc++);
        this->AOr(n);
    } else if ((opcode & 0xf8) == 0xb8) {
        /* CP a, r */
        uint8_t srcId = (opcode & 0x07);
        this->ACp(*this->GetOperandPointer(srcId));
    } else if (opcode == 0xfe) {
        /* CP a, n */
        uint8_t n = this->mmap.LoadByte(this->registers.pc++);
        this->ACp(n);
    } else if ((opcode & 0xc7) == 0x04) {
        /* INC r */
        uint8_t dstId = (opcode & 0x38) >> 3;
        this->Inc(this->GetOperandPointer(dstId)); 
    } else if ((opcode & 0xc7) == 0x05) {
        /* DEC r */
        uint8_t dstId = (opcode & 0x38) >> 3;
        this->Dec(this->GetOperandPointer(dstId));
    } else if (opcode == 0x27) {
        /* DAA */
        this->DAA();
    } else if (opcode == 0x2f) {
        /* CPL */
        this->CPL();
    } else if (opcode == 0x37) {
        /* CCF */
        this->SetSubstract(0);
        this->SetHalfCarry(0);
        if (this->GetCarry())
            this->SetCarry(0);
        else
            this->SetCarry(1);
    } else if (opcode == 0x3f) {
        /* SCF */
        this->SetSubstract(0);
        this->SetHalfCarry(0);
        this->SetCarry(1);
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
        this->mmap.WriteHalfWord(nn, this->registers.sp);
    } else if (opcode == 0xf9) {
        /* LD SP, HL */
        this->SetSP(this->GetHL());
    } else if (opcode == 0xf8) {
        /* LDHL SP, n */
        int8_t n = (int8_t) this->FetchByte();
        this->SetHalfCarry((((this->GetSP() & 0xff) + (n & 0xff)) & 0x100) >> 8);

        uint32_t tmp = this->GetSP() + n;
        this->SetCarry((tmp & 0x10000) >> 16);

        this->SetZero(0);
        this->SetSubstract(0);

        this->SetHL(tmp);
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

        this->SetHalfCarry((((this->GetSP() & 0xff) + (n & 0xff)) & 0x100) >> 8);

        uint32_t tmp = this->GetHL() + n;
        this->SetCarry((tmp & 0x10000) >> 16);

        this->SetZero(0);
        this->SetSubstract(0);

        this->SetHL(tmp);
    } else if (opcode == 0xe8) {
        /* ADD SP, n */
        int8_t n = (int8_t) this->mmap.LoadByte(this->registers.pc++);
        this->SetHalfCarry((((this->GetSP() & 0xff) + (n & 0xff)) & 0x100) >> 8);

        uint32_t tmp = this->GetSP() + n;
        this->SetCarry((tmp & 0x10000) >> 16);

        this->SetZero(0);
        this->SetSubstract(0);

        this->SetSP(tmp);
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
        uint16_t nn;

        this->mmap.Load(this->registers.pc, (uint8_t*) &nn, sizeof(nn));
        this->registers.pc += 2;
  
        this->registers.pc = nn;
    } else if ((opcode & 0xe7) == 0xc2) {
        /* JP cc, nn */
        uint8_t cc = (opcode & 0x18) >> 3;
        uint16_t nn = this->mmap.LoadHalfWord(this->registers.pc);

        this->JumpAbsoluteConditional((enum Condition) cc, nn);
    } else if (opcode == 0xe9) {
        /* JP (HL) */
        this->registers.pc = this->mmap.LoadHalfWord(this->GetHL());
    } else if (opcode == 0x18) {
        /* JR n */
        uint8_t n = this->FetchByte();
        this->registers.pc += n;
    } else if ((opcode & 0xe7) == 0x20) {
        /* JR cc, n */
        uint8_t cc = (opcode & 0x18) >> 3;
        uint8_t n = this->FetchByte();

        this->JumpRelativeConditional((enum Condition) cc, n);
    } else if (opcode == 0x12) {
        /* CALL nn */
        uint16_t nn = this->FetchHalfWord();
        this->Push(this->registers.pc);
        this->registers.pc = nn;
    } else if ((opcode & 0xe7) == 0xc4) {
        /* CALL cc, nn */
        uint8_t cc = (opcode & 0x18) >> 3;
        uint16_t nn = this->mmap.LoadHalfWord(this->registers.pc);

        this->Push(this->registers.pc);
        this->JumpAbsoluteConditional((enum Condition) cc, nn);
    } else if (opcode == 0xc9) {
        /* RET */
        this->registers.pc = this->Pop();
    } else if ((opcode & 0xe7) == 0xc0) {
        /* RET cc */
        uint8_t cc = (opcode & 0x18) >> 3;

        switch (cc) {
            case NZ:
                if (!this->GetZero())
                    this->registers.pc = this->Pop();
                break;
            case Z:
                if (this->GetZero())
                    this->registers.pc = this->Pop();
                break;
            case NC:
                if (!this->GetCarry())
                    this->registers.pc = this->Pop();
                break;
            case C:
                if (this->GetCarry())
                    this->registers.pc = this->Pop();
                break;
            default:
                break;
        }        
    } else if (opcode == 0xd9) {
        this->registers.pc = this->Pop();
        this->interrupts = true;
    }

    /* Misc & System */
    else if (opcode == 0x00) {
        /* NOP */
        ;
    } else if (opcode == 0x76) {
        /* HALT */
        this->status = STATUS_HALTED;
    } else if (opcode == 0xF3) {
        /* DI */
        this->interrupts = false;
    } else if (opcode == 0xFB) {
        /* EI */
        this->interrupts = true;
    }
}

void CPU::DumpState(void)
{
    std::cout << "A = " << std::hex << (int) this->registers.a << std::endl;
    std::cout << "B = " << std::hex << (int) this->registers.b << std::endl;
    std::cout << "C = " << std::hex << (int) this->registers.c << std::endl;
    std::cout << "D = " << std::hex << (int) this->registers.d << std::endl;
    std::cout << "E = " << std::hex << (int) this->registers.e << std::endl;
    std::cout << "H = " << std::hex << (int) this->registers.h << std::endl;
    std::cout << "L = " << std::hex << (int) this->registers.l << std::endl;

    std::cout << "F = " << std::hex << (int) this->registers.f << " (";
    if (this->registers.f & 0x80)
        std::cout << "Z";
    else
        std::cout << "-";
    if (this->registers.f & 0x40)
        std::cout << "N";
    else
        std::cout << "-";
    if (this->registers.f & 0x20)
        std::cout << "H";
    else
        std::cout << "-";
    if (this->registers.f & 0x10)
        std::cout << "C";
    else
        std::cout << "-";

    std::cout << ")" << std::endl;

    std::cout.width(4);
    std::cout << "SP = " << std::hex << (int) this->registers.sp << std::endl;
    std::cout << "PC = " << std::hex << (int) this->registers.pc << std::endl;
}

void CPU::AAdd(uint8_t value)
{
    this->SetHalfCarry((((this->registers.a & 0xf) + (value & 0xf)) & 0x10) >> 4);
    this->SetSubstract(0);

    uint16_t tmp = this->registers.a + value;
    this->SetCarry((tmp & 0x100) >> 8);

    this->registers.a = tmp & 0xff;

    this->SetZero(this->registers.a == 0);
}

void CPU::ASub(uint8_t value)
{
    this->SetHalfCarry((value & 0xf) > (this->registers.a & 0xf));
    this->SetCarry(value > this->registers.a);
    this->SetSubstract(1);

    this->registers.a -= value;

    if (this->registers.a == 0)
        this->SetZero(1);
    else
        this->SetZero(0);
}

void CPU::AAnd(uint8_t value)
{
    this->registers.a &= value;

    if (this->registers.a == 0)
        this->SetZero(1);
    else
        this->SetZero(0);
    
    this->SetSubstract(0);
    this->SetHalfCarry(1);
    this->SetCarry(0);
}

void CPU::AOr(uint8_t value)
{
    this->registers.a |= value;

    if (this->registers.a == 0)
        this->SetZero(1);
    else
        this->SetZero(0);
    
    this->SetHalfCarry(0);
    this->SetSubstract(0);
    this->SetCarry(0);
}

void CPU::AXor(uint8_t value)
{
    this->registers.a ^= value;

    if (this->registers.a == 0)
        this->SetZero(1);
    else
        this->SetZero(0);

    this->SetHalfCarry(0);
    this->SetSubstract(0);
    this->SetCarry(0);
}

void CPU::ACp(uint8_t value)
{
    this->SetHalfCarry((value & 0xf) > (this->registers.a & 0xf));
    this->SetCarry(value > this->registers.a);
    this->SetSubstract(1);

    if (this->registers.a - value == 0)
        this->SetZero(1);
    else
        this->SetZero(0);
}

void CPU::Inc(uint8_t *operand)
{
    if (((*operand & 0xf) + 1) & 0x10)
        this->SetHalfCarry(1);
    else
        this->SetHalfCarry(0);
    this->SetSubstract(0);

    *operand = *operand + 1;
    if (*operand == 0)
        this->SetZero(1);
    else
        this->SetZero(0);
}

void CPU::Dec(uint8_t *operand)
{
    this->SetHalfCarry((1 & 0xf) > (this->registers.a & 0xf));
    this->SetSubstract(1);

    *operand = *operand - 1;

    if (*operand == 0)
        this->SetZero(1);
    else
        this->SetZero(0);
}

void CPU::DAA(void)
{
    uint8_t c = this->GetCarry();
    this->SetCarry(0);
    
    uint8_t a = this->registers.a;
    if ((a & 0x0f) > 0x09 || this->GetHalfCarry())
        a += 0x06;
    if ((a & 0xf0) > 0x90 || c) {
        a += 0x60;
        this->SetCarry(1);
    }

    this->SetHalfCarry(0);
    if (a == 0)
        this->SetZero(1);
    else
        this->SetZero(0);
    
    this->registers.a = a;
}

void CPU::CPL(void)
{
    this->registers.a = ~this->registers.a;
    this->SetSubstract(1);
    this->SetHalfCarry(1);
}

void CPU::JumpAbsoluteConditional(enum CPU::Condition cc, uint16_t address)
{
    switch (cc) {
        case NZ:
            if (!this->GetZero())
                this->registers.pc = address;
            break;
        case Z:
            if (this->GetZero())
                this->registers.pc = address;
            break;
        case NC:
            if (!this->GetCarry())
                this->registers.pc = address;
            break;
        case C:
            if (this->GetCarry())
                this->registers.pc = address;
            break;
        default:
            break;
    }
}

};
