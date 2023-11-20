#include "Cartridge.hpp"

namespace GameBoy
{

Cartridge::Cartridge() : isLoaded(false)
{
}

Cartridge::Cartridge(const std::string& name)
{
    this->Open(name);
}

Cartridge::~Cartridge()
{
    this->Close();
}

void Cartridge::Open(const std::string& name)
{
    /* Open file and get its size */
    std::ifstream handler;
    handler.open(name, std::ifstream::ate | std::ifstream::binary);
    if (handler.fail())
        throw std::runtime_error("Cartridge file not found");
    
    uint32_t size = handler.tellg();
    if (size < 0x150)
        throw std::runtime_error("Cartridge too small");

    handler.seekg(0);

    /* Get file contents */
    this->contents = new uint8_t [size];
    handler.read(reinterpret_cast<char*>(contents), size);
    handler.close();
    this->rawHeader = (struct RawCartridgeHeader*) &this->contents[0x100];

    /* Parse contents */
    this->ParseType();
    this->ParseRAMSize();
    this->ParseROMSize();

    /* Loading finished */
    this->isLoaded = true;
}

void Cartridge::Close(void)
{
    if (this->isLoaded) {
        delete [] this->contents;
        this->isLoaded= false;
    }
}

void Cartridge::ParseType()
{
    this->hasRAM = false;
    this->hasTimer = false;

    switch (this->rawHeader->cartridgeType) {
        case 0x00:
            this->mbc = MemoryBankController::NoMBC;
            break;
        case 0x01:
            this->mbc = MemoryBankController::MBC1;
            break;
        case 0x02:
        case 0x03: /* Don't care about the +BATTERY */
            this->mbc = MemoryBankController::MBC1;
            this->hasRAM = true;
            break;
        case 0x05:
        case 0x06:
            this->mbc = MemoryBankController::MBC2;
            break;
        case 0x08:
        case 0x09:
            this->mbc = MemoryBankController::NoMBC;
            this->hasRAM = true;
            break;
        case 0x0B:
            this->mbc = MemoryBankController::MMM01;
            break;
        case 0x0C:
        case 0x0D:
            this->mbc = MemoryBankController::MMM01;
            this->hasRAM = true;
            break;
        case 0x0F:
            this->mbc = MemoryBankController::MBC3;
            this->hasTimer = true;
            break;
        case 0x10:
            this->mbc = MemoryBankController::MBC3;
            this->hasTimer = true;
            this->hasRAM = true;
            break;
        case 0x11:
            this->mbc = MemoryBankController::MBC3;
            break;
        case 0x12:
        case 0x13:
            this->mbc = MemoryBankController::MBC3;
            this->hasRAM = true;
            break;
        case 0x19:
            this->mbc = MemoryBankController::MBC5;
            break;
        case 0x1A:
        case 0x1B:
            this->mbc = MemoryBankController::MBC5;
            this->hasRAM = true;
            break;
        default:
            throw std::runtime_error("Unimplementend cartridge type");
    }
}

void Cartridge::ParseRAMSize()
{
    if (this->rawHeader->ramSize == 0) {
        this->nRAMBanks = 0;
    } else if (this->rawHeader->ramSize == 0x02) {
        this->nRAMBanks = 1;
    } else if (this->rawHeader->ramSize == 0x03) {
        this->nRAMBanks = 4;
    } else if (this->rawHeader->ramSize == 0x04) {
        this->nRAMBanks = 16;
    } else if (this->rawHeader->ramSize == 0x05) {
        this->nRAMBanks = 8;
    } else {
        throw std::runtime_error("Unrecognized \"RAM SIZE\" field of the cartridge");
    }
}

void Cartridge::ParseROMSize()
{
    if (this->rawHeader->romSize > 0x08)
        throw std::runtime_error("Unrecognized \"ROM SIZE\" field of the cartridge");

    this->nROMBanks = 1 << (1 + this->rawHeader->romSize);
}

};