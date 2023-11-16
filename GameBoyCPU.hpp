#ifndef GBEMU_CPU_HPP
#define GBEMU_CPP_HPP

#include <cstdint>
#include <iostream>
#include <iomanip>

#include "GameBoyMMap.hpp"

namespace GameBoy 
{

class CPU
{
private:
    /* TODO: rework the structure to handle instr. mask */
    enum Condition {
        NZ  = 0,
        Z   = 1,
        NC  = 2,
        C   = 3
    };

    enum {
        STATUS_RUNNING,
        STATUS_HALTED,
        STATUS_STOPPED
    } status;

    struct {
        uint8_t b;  /* 000 */
        uint8_t c;  /* 001 */
        uint8_t d;  /* 010 */
        uint8_t e;  /* 011 */
        uint8_t h;  /* 100 */
        uint8_t l;  /* 101 */
        uint8_t null0;
        uint8_t a;  /* 111 */
        uint8_t f;

        uint16_t sp;
        uint16_t pc;
    } registers;

    bool interrupts;

    MMap& mmap;
    uint64_t cycles;

    /* Helper functions to manipulate PC */
    inline uint8_t FetchByte(void)
    {
        return this->mmap.LoadByte(this->registers.pc++);
    }

    inline uint16_t FetchHalfWord(void)
    {
        uint16_t halfWord = this->mmap.LoadHalfWord(this->registers.pc);
        this->registers.pc += 2;
        return halfWord;
    }

    /* Arithmetic & logic implementations */
    void AAdd(uint8_t value);
    void ASub(uint8_t value);
    void AAnd(uint8_t value);
    void AOr(uint8_t value);
    void AXor(uint8_t value);
    void ACp(uint8_t value);
    void Inc(uint8_t *operand);
    void Dec(uint8_t *operand);
    void DAA(void);
    void CPL(void);

    /* Jump implemntation */
    void JumpAbsoluteConditional(enum Condition cc, uint16_t address);

    inline void JumpRelativeConditional(enum Condition cc, uint8_t offset)
    {
        this->JumpAbsoluteConditional(cc, this->registers.pc + offset);
    }

    /* Stack implementation */
    inline void Push(uint16_t value)
    {
        this->SetSP(this->GetSP() - 2);
        this->mmap.WriteHalfWord(this->registers.sp, value);
    }

    inline uint16_t Pop(void)
    {
        uint16_t value = this->mmap.LoadHalfWord(this->registers.sp);
        this->SetSP(this->GetSP() + 2);
        return value;
    }

    /* 8-bit registers functions */
    inline uint8_t* GetOperandPointer(uint8_t id)
    {
        if (id == 6)
            return this->mmap.GetBytePointer(this->GetHL());
        return &(&this->registers.b)[id];
    }

    /* 16-bit registers functions */
    inline uint16_t GetBC(void) const { return (this->registers.b << 8) | (this->registers.c); }
    inline uint16_t GetDE(void) const { return (this->registers.d << 8) | (this->registers.e); }
    inline uint16_t GetHL(void) const { return (this->registers.h << 8) | (this->registers.l); }
    inline uint16_t GetSP(void) const { return this->registers.sp; }
    inline uint16_t GetAF(void) const { return (this->registers.a << 8) | (this->registers.f); }

    inline void SetBC(const uint16_t bc) { 
        this->registers.b = (bc & 0xff00) >> 8;
        this->registers.c = bc & 0x00ff;
    }

    inline void SetDE(const uint16_t de) { 
        this->registers.d = (de & 0xff00) >> 8;
        this->registers.e = de & 0x00ff;
    }

    inline void SetHL(const uint16_t hl) { 
        this->registers.h = (hl & 0xff00) >> 8;
        this->registers.l = hl & 0x00ff;
    }

    inline void SetSP(const uint16_t sp) {
        this->registers.sp = sp;
    }

    inline void SetAF(const uint16_t af) { 
        this->registers.a = (af & 0xff00) >> 8;
        this->registers.f = af & 0x00ff;
    }
    
    inline uint16_t GetHalfWordRegister(uint8_t id, bool use_af) const {
        switch (id) {
            case 0: return this->GetBC();
            case 1: return this->GetDE();
            case 2: return this->GetHL();
            case 3: 
                if (use_af)
                    return this->GetAF();
                else
                    return this->GetSP();
            default:
                std::cout << "error" << std::endl;
                break;
        }
    }

    inline void SetHalfWordRegister(uint8_t id, uint16_t hw, bool use_af) {
        switch (id) {
            case 0: return this->SetBC(hw);
            case 1: return this->SetDE(hw);
            case 2: return this->SetHL(hw);
            case 3: 
                if (use_af) 
                    return this->SetAF(hw);
                else
                    return this->SetSP(hw);
            default:
                std::cout << "error" << std::endl;
                break;
        }
    }

    inline void IncHL(void) { this->SetHL(this->GetHL() + 1); }
    inline void DecHL(void) { this->SetHL(this->GetHL() - 1); }

    /* Flags related functions */
    inline void SetFlag(uint8_t pos, bool bit)
    {
        if (bit)
            this->registers.f |= (1 << pos);
        else
            this->registers.f &= ~(1 << pos);
    }

    inline bool GetFlag(uint8_t pos) const
    {
        return (this->registers.f & (1 << pos)) >> (8 - pos);
    }

    inline void SetCarry(bool c)     { this->SetFlag(4, c); }
    inline void SetHalfCarry(bool h) { this->SetFlag(5, h); }
    inline void SetSubstract(bool n) { this->SetFlag(6, n); }
    inline void SetZero(bool z)      { this->SetFlag(7, z); }

    inline bool GetCarry(void)      const { return GetFlag(4); }
    inline bool GetHalfCarry(void)  const { return GetFlag(5); }
    inline bool GetSubstract(void)  const { return GetFlag(6); }
    inline bool GetZero(void)       const { return GetFlag(7); }

public:
    CPU(MMap& gbMMap);
    void Reset(void);
    void Step(void);

    void DumpState(void);

    inline uint16_t GetPC(void) const
    {
        return this->registers.pc;
    }
};

};

#endif