#include <iostream>
#include "Cartridge.hpp"

int8_t DumpROMInformations(const std::string& filename)
{
    GameBoy::Cartridge rom;

    try {
        rom.Open(filename);
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    std::cout << rom.GetTitle() << std::endl;
    
    std::cout << "| Features: " << GameBoy::EnumToString(rom.GetMBC());
    if (rom.HasRAM())
        std::cout << "+RAM";
    if (rom.HasTimer())
        std::cout << "+TIMER";
    std::cout << std::endl;

    uint8_t nRAMBanks = rom.NumberOfRAMBanks();
    uint8_t nROMBanks = rom.NumberOfROMBanks();
    
    std::cout << "| RAM Banks: " << std::to_string(nRAMBanks) << " (" << std::to_string(nRAMBanks * 16) << " KiB)" << std::endl;
    std::cout << "| ROM Banks: " << std::to_string(nROMBanks) << " (" << std::to_string(nROMBanks * 16) << " KiB)" << std::endl;

    std::cout << "| CGB: ";
    if (rom.IsCGBModeEnabled())
        std::cout << "Yes" << std::endl;
    else
        std::cout << "No" << std::endl;

    std::cout << "| SGB: ";
    if (rom.IsSGBModeEnabled())
        std::cout << "Yes" << std::endl;
    else
        std::cout << "No" << std::endl;

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "usage: ./ROMExplorer <rom file(s)>" << std::endl;
        return 0;
    }

    for (uint32_t i = 0; i < (uint32_t) argc - 1; i++) {
        DumpROMInformations(argv[i + 1]);
        std::cout << std::endl;
    }

    return 0;
}