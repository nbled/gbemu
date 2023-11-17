#ifndef GBEMU_MEMORY_INTERFACE_HPP
#define GBEMU_MEMORY_INTERFACE_HPP

#include <cstdint>

namespace GameBoy
{

class MemoryInterface
{
public:
    virtual ~MemoryInterface() {}

    virtual void Load(uint16_t address, uint8_t *bytes, uint16_t size) const = 0;
    virtual uint8_t LoadByte(uint16_t address) const = 0;
    virtual uint16_t LoadHalfWord(uint16_t address) const = 0;

    virtual void Write(uint16_t address, const uint8_t *bytes, uint16_t size) = 0;
    virtual void WriteByte(uint16_t address, const uint8_t byte) = 0;
    virtual void WriteHalfWord(uint16_t address, uint16_t halfword) = 0;
};

};

#endif