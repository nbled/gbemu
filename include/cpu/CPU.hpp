#ifndef GBEMU_CPU_HPP
#define GBEMU_CPP_HPP

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string>

#include "cpu/Registers.hpp"
#include "cpu/MMap.hpp"

namespace GameBoy 
{

class CPU
{
public:
    enum Status {
        StatusRunning,
        StatusHalted,
        StatusStopped
    };

private:
    enum Condition {
        NZ  = 0,
        Z   = 1,
        NC  = 2,
        C   = 3
    };

    /* CPU Status */
    enum Status status;
    bool interrupts;
    uint64_t cycles;

    MemoryInterface& mmap;
    Registers registers;

    /* Last executed instruction name */
    std::string lastInstr;
    /* Last executed instruction size */
    uint8_t lastInstrSize;
    /* Last executed instruction pc */
    uint16_t lastPC;

    /* Helper functions to manipulate PC */
    inline uint8_t FetchByte(void)
    {
        this->lastInstrSize++;
        return this->mmap.LoadByte(this->registers.pc++);
    }

    inline uint16_t FetchHalfWord(void)
    {
        this->lastInstrSize += 2;
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
    void Inc(uint8_t id);
    void Dec(uint8_t id);
    void DAA(void);
    void CPL(void);

    /* Rotate implementations */
    void RotateLeft(uint8_t id, bool through_carry);
    void RotateRight(uint8_t id, bool through_carry);

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
    inline uint8_t GetByteRegister(uint8_t id)
    {
        if (id == 6)
            return this->mmap.LoadByte(this->registers.GetHL());
        return (&this->registers.b)[id];
    }

    inline void SetByteRegister(uint8_t id, uint8_t n)
    {
        if (id == 6)
            return this->mmap.WriteByte(this->registers.GetHL(), n);
        (&this->registers.b)[id] = n;
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

    /* Display Functions */
    inline const std::string GetOperandFormat(uint8_t id) const
    {
        switch (id) {
            case 0: return "B";
            case 1: return "C";
            case 2: return "D";
            case 3: return "E";
            case 4: return "H";
            case 5: return "L";
            case 6: return "(HL)";
            case 7: return "A";
            default:
                break;
        }
        return "";
    }

    inline const std::string GetHalfWordFormat(uint8_t id, bool use_af) 
    {
        switch (id) {
            case 0: return "BC";
            case 1: return "DE";
            case 2: return "HL";
            case 3: 
                if (use_af)
                    return "AF";
                else
                    return "SP";
            default:
                break;
        }
        return "";
    }

    inline const std::string GetConditionFormat(enum Condition cc)
    {
        switch (cc) {
            case NZ:    return "NZ";
            case Z:     return "Z";
            case NC:    return "NC";
            case C:     return "C";
            default:
                break;
        }
        return "";
    }

public:
    CPU(MemoryInterface& gbMMap);
    void Reset(void);
    void Step(void);

    inline uint16_t GetPC(void) const
    {
        return this->registers.pc;
    }

    inline enum Status GetStatus(void) const
    {
        return this->status;
    }

    inline Registers& GetRegisters(void)
    {
        return this->registers;
    }

    inline bool InterruptsEnabled(void) const
    {
        return this->interrupts;
    }

    inline void SetInterruptsSwitch(bool b)
    {
        this->interrupts = b;
    }

    inline void SetStatus(enum Status status)
    {
        this->status = status;
    }

    void Dump(void) const {
        /* TODO: fix the current system, we should be able to decode instruction before executing it */
        printf("%04X: ", this->lastPC);
        for (uint16_t address = this->lastPC; address < this->lastPC + this->lastInstrSize; address++) {
            printf("%02X ", this->mmap.LoadByte(address));
        }
        printf("\t%s\n", this->lastInstr.c_str());

        this->registers.Dump();
    }
};

};

#endif