#include "GameBoyCPU.hpp"
#include "GameBoyMMap.hpp"

int main()
{
    GameBoy::MMap mmap;
    GameBoy::CPU cpu(mmap);

    uint8_t code[] = {
        0x26, 0x01,
        0x2E, 0x10,
        0x06, 0xAA,
        0x46, 0x7F,
        0x7F, 0x7F,
        0x7F, 0x7F,
        0x7F, 0x7F,
        0x7F, 0x7F,
        0xBB, 0xBB
    };
    uint16_t size = 17;
    uint16_t end = 0x107;
    mmap.Write(0x100, code, size);

    while (cpu.GetPC() != end) {
        cpu.DumpState();
        cpu.Step();
        std::cout << std::endl;
    }
    cpu.DumpState();

    return 0;
}