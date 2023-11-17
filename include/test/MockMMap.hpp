#ifndef GBEMU_MOCK_MMAP_HPP
#define GBEMU_MOCK_MMAP_HPP

#include <cstdint>
#include "gbit/lib/tester.h"
#include "cpu/MemoryInterface.hpp"

namespace GameBoy
{

class MockMMap : public MemoryInterface
{
public:
    uint8_t *iMemory;
    size_t iMemorySize;
    int nMemoryAccesses;
    struct mem_access memoryAccesses[16];
    uint8_t invalidMemoryAccess = 0xaa;

    MockMMap(uint8_t *iMemory, uint32_t iMemorySize)
    {
        this->iMemory = iMemory;
        this->iMemorySize = iMemorySize;
        this->nMemoryAccesses = 0;
    }

    uint8_t LoadByte(uint16_t address) const
    {
        if (address >= this->iMemorySize)
            return this->invalidMemoryAccess;
        return this->iMemory[address];
    }

    void WriteByte(uint16_t address, const uint8_t byte)
    {
        struct mem_access *access = &this->memoryAccesses[this->nMemoryAccesses++];
        access->type = MEM_ACCESS_WRITE;
        access->addr = address;
        access->val = byte;
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
        for (uint16_t i = 0; i < size; i++)
            bytes[i] = this->LoadByte(address + i);
    }

    void Write(uint16_t address, const uint8_t *bytes, uint16_t size)
    {
        for (uint16_t i = 0; i < size; i++)
            this->WriteByte(address + i, bytes[i]);
    }
};

};

#endif