#ifndef GBEMU_CPU_HPP
#define GBEMU_CPP_HPP

#include <cstdint>
#include <iostream>
#include <iomanip>

#include "GameBoyRegisters.hpp"
#include "GameBoyMMap.hpp"

namespace GameBoy 
{

class CPU
{
private:
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

    bool interrupts;

    MMap& mmap;
    uint64_t cycles;

    Registers registers;

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
        this->registers.sp = this->registers.sp - 2;
        this->mmap.WriteHalfWord(this->registers.sp, value);
    }

    inline uint16_t Pop(void)
    {
        uint16_t value = this->mmap.LoadHalfWord(this->registers.sp);
        this->registers.sp = this->registers.sp + 2;
        return value;
    }

    /* 8-bit registers functions */
    inline uint8_t* GetOperandPointer(uint8_t id)
    {
        if (id == 6)
            return this->mmap.GetBytePointer(this->registers.GetHL());
        return &(&this->registers.b)[id];
    }
    
    inline uint16_t GetHalfWordRegister(uint8_t id, bool use_af) const {
        switch (id) {
            case 0: return this->registers.GetBC();
            case 1: return this->registers.GetDE();
            case 2: return this->registers.GetHL();
            case 3: 
                if (use_af)
                    return this->registers.GetAF();
                else
                    return this->registers.sp;
            default:
                std::cout << "error" << std::endl;
                break;
        }
    }

    inline void SetHalfWordRegister(uint8_t id, uint16_t hw, bool use_af) {
        switch (id) {
            case 0: this->registers.SetBC(hw); break;
            case 1: this->registers.SetDE(hw); break;
            case 2: this->registers.SetHL(hw); break;
            case 3: 
                if (use_af) 
                    this->registers.SetAF(hw);
                else
                    this->registers.sp = hw;
                break;
            default:
                std::cout << "error" << std::endl;
                break;
        }
    }

    inline void IncHL(void) { this->registers.SetHL(this->registers.GetHL() + 1); }
    inline void DecHL(void) { this->registers.SetHL(this->registers.GetHL() - 1); }

public:
    CPU(MMap& gbMMap);
    void Reset(void);
    void Step(void);

    inline uint16_t GetPC(void) const
    {
        return this->registers.pc;
    }

    void Dump(void) const {
        this->registers.Dump();
    }
};

};

#endif