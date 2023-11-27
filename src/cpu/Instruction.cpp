#include "cpu/Instruction.hpp"

namespace GameBoy 
{

std::vector<uint8_t> Instruction::GetBytecode() const
{
    std::vector<uint8_t> bytecode;
    if (this->isCB)
        bytecode.push_back(0xCB);
    bytecode.push_back(opcode);

    switch (this->baseInstruction->GetArgumentType()) {
        case Immediate8:
            bytecode.push_back(this->arg8);
            break;
        case Immediate16:
            bytecode.push_back(this->arg16 & 0xff);
            bytecode.push_back((this->arg16 >> 8) & 0xff);
            break;
        case NoImmediate:
        default:
            break;
    }

    return bytecode;
}

std::string Instruction::GetString() const
{
    uint8_t strArgSize = 0;
    switch (this->baseInstruction->GetArgumentType()) {
        case NoImmediate:   strArgSize = 0; break;
        case Immediate8:    strArgSize = 2; break;
        case Immediate16:   strArgSize = 4; break;
        default:
            break;
    }

    const std::string& format = this->baseInstruction->GetFormat();

    uint8_t strTotalSize = format.size() + strArgSize;
    /* TODO: use unique_ptr */
    uint8_t *tmp = new uint8_t [strTotalSize + 1];
    tmp[strTotalSize] = 0;

    switch (this->baseInstruction->GetArgumentType()) {
        case NoImmediate:
            std::strncpy((char*) tmp, format.c_str(), strTotalSize);
            break;
        case Immediate8:    
            std::snprintf((char*) tmp, strTotalSize, format.c_str(), this->arg8);
            break;
        case Immediate16:
            std::snprintf((char*) tmp, strTotalSize, format.c_str(), this->arg16);
            break;
        default:
            break;
    }

    std::string s((char*) tmp);

    return s;
}

};