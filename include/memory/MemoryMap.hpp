#ifndef GBEMU_MMAP_HPP
#define GBEMU_MMAP_HPP

#include <cstdint>
#include <vector>

#include "memory/MemoryInterface.hpp"
#include "memory/MemorySegment.hpp"

namespace GameBoy
{

class MemoryMap : public MemoryInterface
{
private:
    std::vector<MemorySegment*> segments;

public:
    MemoryMap()
    {
        this->AddSegment(new MemorySegment("RAM", 0x0000, 0xFFFF, MemorySegment::Permissions::ReadWrite));
    }

    void AddSegment(MemorySegment *segment)
    {
        segments.push_back(segment);
    }

    MemorySegment* GetSegment(uint16_t address) const
    {
        /* TODO: use smart pointers */
        for (MemorySegment *segment : this->segments) {
            if (segment->ContainsAddress(address))
                return segment;
        }
        return nullptr;
    }

    void Load(uint16_t address, uint8_t *bytes, uint16_t size) const
    {
        /* TODO: see for cross segments accesses */
        MemorySegment *segment = this->GetSegment(address);
        if (segment)
            segment->Load(address, bytes, size);
    }

    inline uint8_t LoadByte(uint16_t address) const
    {
        uint8_t byte;
        this->Load(address, &byte, sizeof(byte));
        return byte;
    }

    inline uint16_t LoadHalfWord(uint16_t address) const
    {
        uint16_t halfword;
        this->Load(address, (uint8_t*) &halfword, sizeof(halfword));
        return halfword;
    }

    void Write(uint16_t address, const uint8_t *bytes, uint16_t size)
    {
        MemorySegment *segment = this->GetSegment(address);
        if (segment)
            segment->Write(address, bytes, size);
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