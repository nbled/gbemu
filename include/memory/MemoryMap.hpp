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
    MemoryMap() {
        this->AddSegment(new MemorySegment("WRAM0", 0xC000, 0xD000, GameBoy::MemorySegment::Permissions::ReadWrite));
        this->AddSegment(new MemorySegment("WRAM1", 0xD000, 0xE000, GameBoy::MemorySegment::Permissions::ReadWrite));
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
        /*if (address >= 0xFF00 && address < 0xFF7F)
            printf("read access to I/O register %04X\n", address);
        */
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
        /*if (address >= 0xFF00 && address < 0xFF7F)
            printf("write access to I/O register %04X\n", address);
        */
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