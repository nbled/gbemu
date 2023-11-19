#include <iostream>
#include "Cartridge.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "usage: ./ROMExplorer <rom file>" << std::endl;
        return 0;
    }

    GameBoy::Cartridge rom;

    try {
        rom.Open(argv[1]);
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
    
    std::cout << "| RAM Banks: " << std::to_string(nRAMBanks) << " (" << std::to_string(nRAMBanks * 32) << " KiB)" << std::endl;
    std::cout << "| ROM Banks: " << std::to_string(nROMBanks) << " (" << std::to_string(nROMBanks * 32) << " KiB)" << std::endl;

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