#ifndef GBEMU_REGISTERS_HPP
#define GBEMU_REGISTERS_HPP

#include <cstdint>
#include <cstdio>

namespace GameBoy 
{

class Registers
{
public:
    Registers() : b(0), c(0), d(0), e(0), h(0), l(0), a(0), f(0), sp(0), pc(0) {}

    uint8_t b;  /* 000 */
    uint8_t c;  /* 001 */
    uint8_t d;  /* 010 */
    uint8_t e;  /* 011 */
    uint8_t h;  /* 100 */
    uint8_t l;  /* 101 */
    uint8_t null0;
    uint8_t a;  /* 111 */
    uint8_t f;

    uint16_t sp;
    uint16_t pc;

    /* 16-bit registers functions */
    inline uint16_t GetBC(void) const { return (this->b << 8) | (this->c); }
    inline uint16_t GetDE(void) const { return (this->d << 8) | (this->e); }
    inline uint16_t GetHL(void) const { return (this->h << 8) | (this->l); }
    inline uint16_t GetAF(void) const { return (this->a << 8) | (this->f); }

    inline void SetBC(const uint16_t bc) { 
        this->b = (bc & 0xff00) >> 8;
        this->c = bc & 0x00ff;
    }

    inline void SetDE(const uint16_t de) { 
        this->d = (de & 0xff00) >> 8;
        this->e = de & 0x00ff;
    }

    inline void SetHL(const uint16_t hl) { 
        this->h = (hl & 0xff00) >> 8;
        this->l = hl & 0x00ff;
    }

    inline void SetAF(const uint16_t af) { 
        this->a = (af & 0xff00) >> 8;
        this->f = af & 0x00ff;
    }

    /* Flags related functions */
    inline void SetFlag(uint8_t pos, bool bit)
    {
        if (bit)
            this->f |= (1 << pos);
        else
            this->f &= ~(1 << pos);
    }

    inline bool GetFlag(uint8_t pos) const
    {
        return (this->f & (1 << pos)) >> (8 - pos);
    }

    inline void SetCarry(bool c)     { this->SetFlag(4, c); }
    inline void SetHalfCarry(bool h) { this->SetFlag(5, h); }
    inline void SetSubstract(bool n) { this->SetFlag(6, n); }
    inline void SetZero(bool z)      { this->SetFlag(7, z); }

    inline bool GetCarry(void)      const { return GetFlag(4); }
    inline bool GetHalfCarry(void)  const { return GetFlag(5); }
    inline bool GetSubstract(void)  const { return GetFlag(6); }
    inline bool GetZero(void)       const { return GetFlag(7); }

    /* Display */
    void Dump(void) const
    {
        printf("A = %02X F = %02X (AF = %04X)\n", this->a, this->f, this->GetAF());
        printf("B = %02X C = %02X (BC = %04X)\n", this->b, this->c, this->GetBC());
        printf("D = %02X E = %02X (DE = %04X)\n", this->d, this->e, this->GetDE());
        printf("H = %02X L = %02X (HL = %04X)\n", this->h, this->l, this->GetHL());

        printf("FLAGS = ");
        if (this->GetZero())
            printf("Z");
        else
            printf("-");
        if (this->GetSubstract())
            printf("N");
        else
            printf("-");

        if (this->GetHalfCarry())
            printf("H");
        else
            printf("-");
        if (this->GetCarry())
            printf("C");
        else
            printf("-");
        printf("\n");

        printf("SP = %04X\n", this->sp);
        printf("PC = %04X\n", this->pc);
    }
};

};

#endif