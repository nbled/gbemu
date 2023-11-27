#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <algorithm>

#include "cpu/CPU.hpp"
#include "memory/MemoryMap.hpp"
#include "Video.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "usage: ./LoadBlob <blob file>" << std::endl;
        return 0;
    }

    std::ifstream handler;
    handler.open(argv[1], std::ifstream::ate | std::ifstream::binary);
    if (handler.fail()) {
        std::cerr << "Blob file not found" << std::endl;
        return -1;
    }

    uint32_t size = handler.tellg();
    handler.seekg(0);

    /* Get file contents */
    uint8_t *contents = new uint8_t [size];
    handler.read(reinterpret_cast<char*>(contents), size);
    handler.close();

    /* Initialize emulator */
    GameBoy::MemoryMap mmap;
    GameBoy::CPU cpu(mmap);
    GameBoy::Video video(mmap);

    mmap.AddSegment(new GameBoy::MemorySegment(
        "ROM", 0x0000, size, 
        GameBoy::MemorySegment::Permissions::ReadWrite, 
        contents));
    cpu.SetPC(0x0000);

    std::vector<uint16_t> bps;
    bps.push_back(0x000C);
    
    while (cpu.GetStatus() == GameBoy::CPU::StatusRunning) {
        if (std::find(bps.begin(), bps.end(), cpu.GetPC()) != bps.end())
            break;
        cpu.Dump();
        cpu.Step();
        std::getchar();
    }

    while (cpu.GetStatus() == GameBoy::CPU::StatusRunning) {
        cpu.Dump();
        cpu.Step();
    }

    delete [] contents;
    return 0;
}