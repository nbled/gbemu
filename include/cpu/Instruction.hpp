#ifndef GBEMU_INSTRUCTION_HPP
#define GBEMU_INSTRUCTION_HPP

#include <cstring>
#include <string>
#include <vector>

namespace GameBoy
{

typedef enum {
    NoImmediate,
    Immediate8,
    Immediate16,
} ImmediateType;

class AbstractInstruction
{
private:
    std::string format;
    const ImmediateType argType;

public:
    AbstractInstruction() : format(""), argType(NoImmediate) {}
    AbstractInstruction(const std::string& format, const ImmediateType argType) 
    : format(format), argType(argType) {}

    inline const std::string& GetFormat() const         { return this->format; }
    inline ImmediateType GetArgumentType() const        { return this->argType; }
};

extern const AbstractInstruction InstructionTable[];
extern const AbstractInstruction InstructionTableCB[];

class Instruction
{
private:
    uint8_t opcode;
    const AbstractInstruction *baseInstruction;
    bool isCB;

    uint8_t arg8;
    uint16_t arg16;

public:
    Instruction(uint8_t opcode, bool isCB) 
    : opcode(opcode), isCB(isCB)
    {
        if (this->isCB)
            this->baseInstruction = &InstructionTableCB[opcode];
        else
            this->baseInstruction = &InstructionTable[opcode];
    }

    ~Instruction() { this->baseInstruction = nullptr; }

    std::vector<uint8_t> GetBytecode() const;
    std::string GetString() const;

    /* TODO: make Instruction inherit AbstractInstruction */
    const AbstractInstruction *GetBaseInstruction() { return this->baseInstruction; }

    inline uint8_t GetByteArgument() const { return this->arg8; }
    inline void SetByteArgument(uint8_t arg8) { this->arg8 = arg8; }

    inline uint16_t GetHalfwordArgument() const { return this->arg16; }
    inline void SetHalfwordArgument(uint16_t arg16) { this->arg16 = arg16; }
};

};

#endif