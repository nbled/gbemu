#include <fstream>

#include "GameBoyCPU.hpp"
#include "GameBoyMMap.hpp"

int main()
{
    GameBoy::MMap mmap;
    GameBoy::CPU cpu(mmap);

    std::ifstream in("test.gb", std::ifstream::ate | std::ifstream::binary);
    std::ifstream::pos_type size = in.tellg();
    in.seekg(0);

    uint8_t *code = new uint8_t[size];
    in.read(reinterpret_cast<char*>(code), size);

    mmap.Write(0x0, code, size);

    while (cpu.GetStatus() == GameBoy::CPU::Status::StatusRunning) {
        cpu.Dump();
        cpu.Step();
        std::cout << std::endl;
    }
    cpu.Dump();

    return 0;
}