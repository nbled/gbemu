#ifndef GBEMU_CPU_HPP
#define GBEMU_CPP_HPP

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string>

#include "cpu/Registers.hpp"
#include "cpu/Instruction.hpp"
#include "memory/MemoryMap.hpp"

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

    /* Wrapper for memory functions to count cycles */
    inline uint8_t LoadByteCycled(uint16_t address)
    {
        this->cycles += 4;
        return this->mmap.LoadByte(address);
    }

    inline void WriteByteCycled(uint16_t address, uint8_t byte)
    {
        this->cycles += 4;
        return this->mmap.WriteByte(address, byte);
    }

    inline uint8_t LoadHalfWordCycled(uint16_t address)
    {
        this->cycles += 8;
        return this->mmap.LoadHalfWord(address);
    }

    inline void WriteHalfWordCycled(uint16_t address, uint16_t hw)
    {
        this->cycles += 8;
        this->mmap.WriteHalfWord(address, hw);
    }

    /* Helper functions to manipulate PC */
    inline uint8_t FetchByte(void)
    {
        return this->LoadByteCycled(this->registers.pc++);
    }

    inline uint16_t FetchHalfWord(void)
    {
        uint16_t halfWord = this->LoadHalfWordCycled(this->registers.pc);
        this->registers.pc += 2;
        return halfWord;
    }

    inline void SetPCCycled(uint16_t pc)
    {
        this->cycles += 4;
        this->registers.pc = pc;
    }

    /* Arithmetic & logic implementations */
    void AAdd(uint8_t value, bool carry);
    void ASub(uint8_t value, bool carry);
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

    /* Stack implementation */
    inline void Push(uint16_t value)
    {
        this->registers.sp = this->registers.sp - 2;
        this->WriteHalfWordCycled(this->registers.sp, value);
    }

    inline uint16_t Pop(void)
    {
        uint16_t value = this->LoadHalfWordCycled(this->registers.sp);
        this->registers.sp = this->registers.sp + 2;
        return value;
    }

    /* 8-bit registers functions */
    inline uint8_t GetByteRegister(uint8_t id)
    {
        if (id == 6)
            return this->LoadByteCycled(this->registers.GetHL());
        return (&this->registers.b)[id];
    }

    inline void SetByteRegister(uint8_t id, uint8_t n)
    {
        if (id == 6)
            this->WriteByteCycled(this->registers.GetHL(), n);
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
                break;
        }

        /* We should not reach here */
        throw std::runtime_error("Invalid halfword register id in instruction");
    }

    inline void SetHalfWordRegister(uint8_t id, uint16_t hw, bool use_af) {
        switch (id) {
            case 0: this->registers.SetBC(hw); break;
            case 1: this->registers.SetDE(hw); break;
            case 2: this->registers.SetHL(hw); break;
            case 3: 
                if (use_af) 
                    this->registers.SetAF(hw & 0xfff0);
                else
                    this->registers.sp = hw;
                break;
            default:
                throw std::runtime_error("Invalid halfword register id in instruction");
        }
    }

    inline void IncHL(void) { this->registers.SetHL(this->registers.GetHL() + 1); }
    inline void DecHL(void) { this->registers.SetHL(this->registers.GetHL() - 1); }

    bool IsConditionSatisfied(enum Condition cc)
    {
        switch (cc) {
            case NZ:    return !this->registers.GetZero();
            case Z:     return this->registers.GetZero();
            case NC:    return !this->registers.GetCarry();
            case C:     return this->registers.GetCarry();
            default:
                break;
        }

        return false;
    }

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

    Instruction DecodeNextInstruction() const;

public:
    CPU(MemoryInterface& gbMMap);
    void Reset(void);
    uint64_t Step(void);

    inline uint16_t GetPC(void) const
    {
        return this->registers.pc;
    }

    inline void SetPC(uint16_t pc)
    {
        this->registers.pc = pc;
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
        /* FIXME: lots of copies :( */
        Instruction instr = this->DecodeNextInstruction();
        printf("%04X: ", this->GetPC());
        std::vector<uint8_t> bytecode = instr.GetBytecode();
        for (uint8_t byte : bytecode) {
            printf("%02X ", byte);
        }
        printf("\t%s\n", instr.GetString().c_str());

        this->registers.Dump();
        printf("CYCLES: %ld\n\n", this->cycles);
    }
};

};

#endif