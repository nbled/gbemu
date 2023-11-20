#ifndef GBEMU_MEMORYSEGMENT_HPP
#define GBEMU_MEMORYSEGMENT_HPP

#include <cstdint>
#include <string>
#include <stdexcept>

#include "memory/MemoryInterface.hpp"

namespace GameBoy
{

class MemorySegment : public MemoryInterface
{
public:
    enum class Permissions {
        Read      = 1,
        Write     = 2,
        ReadWrite = 3,
    };

private:
    std::string name;
    uint16_t begin;
    uint16_t end;
    Permissions perms;

    uint8_t *memory;

    /* Anonymous means allocated when the object was created */
    bool isAnonymous;

public:
    MemorySegment(
        const std::string& name, 
        uint16_t begin, uint16_t end, 
        Permissions perms)
    : name(name), begin(begin), end(end), perms(perms), isAnonymous(true)
    {
        this->memory = new uint8_t [end - begin];
    }

    MemorySegment(
        const std::string& name, 
        uint16_t begin, uint16_t end, 
        Permissions perms,
        uint8_t *memory)
    : name(name), begin(begin), end(end), perms(perms), memory(memory), isAnonymous(false)
    {
    }

    ~MemorySegment()
    {
        if (this->isAnonymous)
            delete [] this->memory;
    }

    inline bool ContainsAddress(uint16_t address) const
    {
        if (this->begin <= address && address < this->end)
            return true;
        return false;
    }

    void Load(uint16_t address, uint8_t *bytes, uint16_t size) const
    {
        for (uint16_t i = 0; i < size; i++) {
            if (!this->ContainsAddress(address + i))
                throw std::runtime_error("Address out of bounds in segment");

            bytes[i] = this->memory[address + i];
        }
    }

    inline uint8_t LoadByte(uint16_t address) const
    {
        uint8_t byte;
        this->Load(address, &byte, sizeof(byte));
        return byte;
    }

    uint16_t LoadHalfWord(uint16_t address) const
    {
        uint16_t halfword;
        this->Load(address, (uint8_t*) &halfword, sizeof(halfword));
        return halfword;
    }

    void Write(uint16_t address, const uint8_t *bytes, uint16_t size)
    {
        for (uint16_t i = 0; i < size; i++) {
            if (address + i >= 0xffff)
                break;
            
            if (!this->ContainsAddress(address + i))
                throw std::runtime_error("Address out of bounds in segment");

            this->memory[address + i] = bytes[i];
        }
    }

    inline void WriteByte(uint16_t address, const uint8_t byte)
    {
        this->Write(address, &byte, sizeof(byte));
    }

    inline void WriteHalfWord(uint16_t address, uint16_t halfword) 
    {
        this->Write(address, (const uint8_t*) &halfword, sizeof(halfword));
    }
};

};

#endif