#ifndef GBEMU_CARTRIDGE_HPP
#define GBEMU_CARTRIDGE_HPP

#include <cstdint>
#include <string>
#include <fstream>

namespace GameBoy
{

const uint8_t NintendoLogo[] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
};

enum MemoryBankController {
    NoMBC,
    MBC1,
    MBC2,
    MBC3,
    MBC5,
    MBC6,
    MBC7,
    MMM01,
    M161,
    HuC1,
    HuC3,
};

inline std::string EnumToString(enum MemoryBankController mbc)
{
    switch (mbc) {
        case NoMBC:     return "ROM";
        case MBC1:      return "MBC1";
        case MBC2:      return "MBC2";
        case MBC3:      return "MBC3";
        case MBC5:      return "MBC5";
        case MBC6:      return "MBC6";
        case MBC7:      return "MBC7";
        case MMM01:     return "MM01";
        case M161:      return "M161";
        case HuC1:      return "HuC1";
        case HuC3:      return "HuC3";
        default:
            break;
    }

    return "";
}

struct RawCartridgeHeader {
    uint8_t entryPoint[4];          /* 0x100 */
    uint8_t logo[48];               /* 0x104 */
    uint8_t title[11];              /* 0x134 */
    uint8_t manufacturerCode[4];    /* 0x13F */
    uint8_t cbgFlag;                /* 0x143 */
    uint8_t licenseeCode[2];        /* 0x144 */
    uint8_t sbgFlag;                /* 0x146 */
    uint8_t cartridgeType;          /* 0x147 */
    uint8_t romSize;                /* 0x148 */
    uint8_t ramSize;                /* 0x149 */
    uint8_t destinationCode;        /* 0x14A */
    uint8_t oldLicenseeCode;        /* 0x14B */
    uint8_t maskRom;                /* 0x14C */
    uint8_t headerChecksum;         /* 0x14D */
    uint16_t globalChecksum;        /* 0x14E */
};

class Cartridge
{
private:
    struct RawCartridgeHeader *rawHeader;
    uint8_t *contents;

    enum MemoryBankController mbc;
    uint32_t ramSize;
    uint32_t romSize;

    bool hasRAM;
    bool hasTimer;

    uint8_t nROMBanks;
    uint8_t nRAMBanks;

    void ParseType();
    void ParseRAMSize();
    void ParseROMSize();

public:
    Cartridge(const std::string& name);
    ~Cartridge();

    inline const std::string GetTitle(void)
    {
        const std::string s = reinterpret_cast<char*>(this->rawHeader->title);
        return s;
    }

    inline enum MemoryBankController GetMBC(void)
    {
        return this->mbc;
    }

    inline bool HasRAM(void)    { return this->hasRAM; }
    inline bool HasTimer(void)  { return this->hasTimer; }

    inline uint8_t NumberOfRAMBanks(void) { return this->nRAMBanks; }
    inline uint8_t NumberOfROMBanks(void) { return this->nROMBanks; }

    inline bool IsCGBModeEnabled(void)
    {
        return (this->rawHeader->cbgFlag == 0x80) || (this->rawHeader->cbgFlag == 0xC0);
    }

    inline bool IsSGBModeEnabled(void)
    {
        return (this->rawHeader->sbgFlag == 0x03);
    }
};

};

#endif