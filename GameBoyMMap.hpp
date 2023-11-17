#ifndef GBEMU_MMAP_HPP
#define GBEMU_MMAP_HPP

#include <cstdint>
#include "GameBoyMemoryInterface.hpp"

namespace GameBoy
{

class MMap : public MemoryInterface
{
private:
    uint8_t *memory;

public:
    MMap()
    {
        this->memory = new uint8_t [0xFFFF];
    }

    ~MMap()
    {
        delete this->memory;
    }

    uint8_t LoadByte(uint16_t address) const
    {
        return this->memory[address];
    }

    void WriteByte(uint16_t address, const uint8_t byte)
    {
        this->memory[address] = byte;
    }

    uint16_t LoadHalfWord(uint16_t address) const
    {
        uint16_t halfword;
        this->Load(address, (uint8_t*) &halfword, sizeof(halfword));
        return halfword;
    }

    void WriteHalfWord(uint16_t address, uint16_t halfword) 
    {
        this->Write(address, (const uint8_t*) &halfword, sizeof(halfword));
    }

    void Load(uint16_t address, uint8_t *bytes, uint16_t size) const
    {
        for (uint16_t i = 0; i < size; i++) {
            if (address + i >= 0xffff)
                break;
            bytes[i] = this->memory[address + i];
        }        
    }

    void Write(uint16_t address, const uint8_t *bytes, uint16_t size)
    {
        for (uint16_t i = 0; i < size; i++) {
            if (address + i >= 0xffff)
                break;
            this->memory[address + i] = bytes[i];
        }
    }
};

};

#endif