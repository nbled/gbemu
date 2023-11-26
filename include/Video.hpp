#ifndef GBEMU_VIDEO_HPP
#define GBEMU_VIDEO_HPP

#include "memory/MemoryMap.hpp"

namespace GameBoy
{

class Video 
{
private:
    MemoryMap& mmap;
    
    uint8_t *vram;
    MemorySegment *vramSegment;

public:
    Video(MemoryMap& mmap) : mmap(mmap)
    {
        this->vram = new uint8_t [0x2000];
        this->vramSegment = new MemorySegment(
            "VRAM", 0x8000, 0xA000,
            GameBoy::MemorySegment::Permissions::ReadWrite,   
            this->vram
        );

        this->mmap.AddSegment(this->vramSegment);
    }

    ~Video()
    {
        delete this->vramSegment;
        delete [] this->vram;
    }
};

};

#endif