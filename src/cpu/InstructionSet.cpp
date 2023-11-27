#include "cpu/Instruction.hpp"

namespace GameBoy
{

const AbstractInstruction InstructionTable[] = {
    {"NOP", NoImmediate},
    {"LD BC, 0x%04X", Immediate16},
    {"LD [BC], A", NoImmediate},
    {"INC BC", NoImmediate},
    {"INC B", NoImmediate},
    {"DEC B", NoImmediate},
    {"LD B, 0x%02X", Immediate8},
    {"RLCA", NoImmediate},
    {"LD [%04X], SP", Immediate16},
    {"ADD HL, BC", NoImmediate},
    {"LD A, [BC]", NoImmediate},
    {"DEC BC", NoImmediate},
    {"INC C", NoImmediate},
    {"DEC C", NoImmediate},
    {"LD C, 0x%02X", Immediate8},
    {"RRCA", NoImmediate},
    {"STOP 0x%02X", Immediate8},
    {"LD DE, 0x%04X", Immediate16},
    {"LD [DE], A", NoImmediate},
    {"INC DE", NoImmediate},
    {"INC D", NoImmediate},
    {"DEC D", NoImmediate},
    {"LD D, 0x%02X", Immediate8},
    {"RLA", NoImmediate},
    {"JR %02X", Immediate8},
    {"ADD HL, DE", NoImmediate},
    {"LD A, [DE]", NoImmediate},
    {"DEC DE", NoImmediate},
    {"INC E", NoImmediate},
    {"DEC E", NoImmediate},
    {"LD E, 0x%02X", Immediate8},
    {"RRA", NoImmediate},
    {"JR NZ, %02X", Immediate8},
    {"LD HL, 0x%04X", Immediate16},
    {"LD [HL+], A", NoImmediate},
    {"INC HL", NoImmediate},
    {"INC H", NoImmediate},
    {"DEC H", NoImmediate},
    {"LD H, 0x%02X", Immediate8},
    {"DAA", NoImmediate},
    {"JR Z, %02X", Immediate8},
    {"ADD HL, HL", NoImmediate},
    {"LD A, [HL+]", NoImmediate},
    {"DEC HL", NoImmediate},
    {"INC L", NoImmediate},
    {"DEC L", NoImmediate},
    {"LD L, 0x%02X", Immediate8},
    {"CPL", NoImmediate},
    {"JR NC, %02X", Immediate8},
    {"LD SP, 0x%04X", Immediate16},
    {"LD [HL-], A", NoImmediate},
    {"INC SP", NoImmediate},
    {"INC [HL]", NoImmediate},
    {"DEC [HL]", NoImmediate},
    {"LD [HL], 0x%02X", Immediate8},
    {"SCF", NoImmediate},
    {"JR C, %02X", Immediate8},
    {"ADD HL, SP", NoImmediate},
    {"LD A, [HL-]", NoImmediate},
    {"DEC SP", NoImmediate},
    {"INC A", NoImmediate},
    {"DEC A", NoImmediate},
    {"LD A, 0x%02X", Immediate8},
    {"CCF", NoImmediate},
    {"LD B, B", NoImmediate},
    {"LD B, C", NoImmediate},
    {"LD B, D", NoImmediate},
    {"LD B, E", NoImmediate},
    {"LD B, H", NoImmediate},
    {"LD B, L", NoImmediate},
    {"LD B, [HL]", NoImmediate},
    {"LD B, A", NoImmediate},
    {"LD C, B", NoImmediate},
    {"LD C, C", NoImmediate},
    {"LD C, D", NoImmediate},
    {"LD C, E", NoImmediate},
    {"LD C, H", NoImmediate},
    {"LD C, L", NoImmediate},
    {"LD C, [HL]", NoImmediate},
    {"LD C, A", NoImmediate},
    {"LD D, B", NoImmediate},
    {"LD D, C", NoImmediate},
    {"LD D, D", NoImmediate},
    {"LD D, E", NoImmediate},
    {"LD D, H", NoImmediate},
    {"LD D, L", NoImmediate},
    {"LD D, [HL]", NoImmediate},
    {"LD D, A", NoImmediate},
    {"LD E, B", NoImmediate},
    {"LD E, C", NoImmediate},
    {"LD E, D", NoImmediate},
    {"LD E, E", NoImmediate},
    {"LD E, H", NoImmediate},
    {"LD E, L", NoImmediate},
    {"LD E, [HL]", NoImmediate},
    {"LD E, A", NoImmediate},
    {"LD H, B", NoImmediate},
    {"LD H, C", NoImmediate},
    {"LD H, D", NoImmediate},
    {"LD H, E", NoImmediate},
    {"LD H, H", NoImmediate},
    {"LD H, L", NoImmediate},
    {"LD H, [HL]", NoImmediate},
    {"LD H, A", NoImmediate},
    {"LD L, B", NoImmediate},
    {"LD L, C", NoImmediate},
    {"LD L, D", NoImmediate},
    {"LD L, E", NoImmediate},
    {"LD L, H", NoImmediate},
    {"LD L, L", NoImmediate},
    {"LD L, [HL]", NoImmediate},
    {"LD L, A", NoImmediate},
    {"LD [HL], B", NoImmediate},
    {"LD [HL], C", NoImmediate},
    {"LD [HL], D", NoImmediate},
    {"LD [HL], E", NoImmediate},
    {"LD [HL], H", NoImmediate},
    {"LD [HL], L", NoImmediate},
    {"HALT", NoImmediate},
    {"LD [HL], A", NoImmediate},
    {"LD A, B", NoImmediate},
    {"LD A, C", NoImmediate},
    {"LD A, D", NoImmediate},
    {"LD A, E", NoImmediate},
    {"LD A, H", NoImmediate},
    {"LD A, L", NoImmediate},
    {"LD A, [HL]", NoImmediate},
    {"LD A, A", NoImmediate},
    {"ADD A, B", NoImmediate},
    {"ADD A, C", NoImmediate},
    {"ADD A, D", NoImmediate},
    {"ADD A, E", NoImmediate},
    {"ADD A, H", NoImmediate},
    {"ADD A, L", NoImmediate},
    {"ADD A, [HL]", NoImmediate},
    {"ADD A, A", NoImmediate},
    {"ADC A, B", NoImmediate},
    {"ADC A, C", NoImmediate},
    {"ADC A, D", NoImmediate},
    {"ADC A, E", NoImmediate},
    {"ADC A, H", NoImmediate},
    {"ADC A, L", NoImmediate},
    {"ADC A, [HL]", NoImmediate},
    {"ADC A, A", NoImmediate},
    {"SUB A, B", NoImmediate},
    {"SUB A, C", NoImmediate},
    {"SUB A, D", NoImmediate},
    {"SUB A, E", NoImmediate},
    {"SUB A, H", NoImmediate},
    {"SUB A, L", NoImmediate},
    {"SUB A, [HL]", NoImmediate},
    {"SUB A, A", NoImmediate},
    {"SBC A, B", NoImmediate},
    {"SBC A, C", NoImmediate},
    {"SBC A, D", NoImmediate},
    {"SBC A, E", NoImmediate},
    {"SBC A, H", NoImmediate},
    {"SBC A, L", NoImmediate},
    {"SBC A, [HL]", NoImmediate},
    {"SBC A, A", NoImmediate},
    {"AND A, B", NoImmediate},
    {"AND A, C", NoImmediate},
    {"AND A, D", NoImmediate},
    {"AND A, E", NoImmediate},
    {"AND A, H", NoImmediate},
    {"AND A, L", NoImmediate},
    {"AND A, [HL]", NoImmediate},
    {"AND A, A", NoImmediate},
    {"XOR A, B", NoImmediate},
    {"XOR A, C", NoImmediate},
    {"XOR A, D", NoImmediate},
    {"XOR A, E", NoImmediate},
    {"XOR A, H", NoImmediate},
    {"XOR A, L", NoImmediate},
    {"XOR A, [HL]", NoImmediate},
    {"XOR A, A", NoImmediate},
    {"OR A, B", NoImmediate},
    {"OR A, C", NoImmediate},
    {"OR A, D", NoImmediate},
    {"OR A, E", NoImmediate},
    {"OR A, H", NoImmediate},
    {"OR A, L", NoImmediate},
    {"OR A, [HL]", NoImmediate},
    {"OR A, A", NoImmediate},
    {"CP A, B", NoImmediate},
    {"CP A, C", NoImmediate},
    {"CP A, D", NoImmediate},
    {"CP A, E", NoImmediate},
    {"CP A, H", NoImmediate},
    {"CP A, L", NoImmediate},
    {"CP A, [HL]", NoImmediate},
    {"CP A, A", NoImmediate},
    {"RET NZ", NoImmediate},
    {"POP BC", NoImmediate},
    {"JP NZ, %04X", Immediate16},
    {"JP %04X", Immediate16},
    {"CALL NZ, %04X", Immediate16},
    {"PUSH BC", NoImmediate},
    {"ADD A, 0x%02X", Immediate8},
    {"RST $00", NoImmediate},
    {"RET Z", NoImmediate},
    {"RET", NoImmediate},
    {"JP Z, %04X", Immediate16},
    {"PREFIX", NoImmediate},
    {"CALL Z, %04X", Immediate16},
    {"CALL %04X", Immediate16},
    {"ADC A, 0x%02X", Immediate8},
    {"RST $08", NoImmediate},
    {"RET NC", NoImmediate},
    {"POP DE", NoImmediate},
    {"JP NC, %04X", Immediate16},
    {"ILLEGAL", NoImmediate},
    {"CALL NC, %04X", Immediate16},
    {"PUSH DE", NoImmediate},
    {"SUB A, 0x%02X", Immediate8},
    {"RST $10", NoImmediate},
    {"RET C", NoImmediate},
    {"RETI", NoImmediate},
    {"JP C, %04X", Immediate16},
    {"ILLEGAL", NoImmediate},
    {"CALL C, %04X", Immediate16},
    {"ILLEGAL", NoImmediate},
    {"SBC A, 0x%02X", Immediate8},
    {"RST $18", NoImmediate},
    {"LDH [%02X], A", Immediate8},
    {"POP HL", NoImmediate},
    {"LD [C], A", NoImmediate},
    {"ILLEGAL", NoImmediate},
    {"ILLEGAL", NoImmediate},
    {"PUSH HL", NoImmediate},
    {"AND A, 0x%02X", Immediate8},
    {"RST $20", NoImmediate},
    {"ADD SP, %02X", Immediate8},
    {"JP HL", NoImmediate},
    {"LD [%04X], A", Immediate16},
    {"ILLEGAL", NoImmediate},
    {"ILLEGAL", NoImmediate},
    {"ILLEGAL", NoImmediate},
    {"XOR A, 0x%02X", Immediate8},
    {"RST $28", NoImmediate},
    {"LDH A, [%02X]", Immediate8},
    {"POP AF", NoImmediate},
    {"LD A, [C]", NoImmediate},
    {"DI", NoImmediate},
    {"ILLEGAL", NoImmediate},
    {"PUSH AF", NoImmediate},
    {"OR A, 0x%02X", Immediate8},
    {"RST $30", NoImmediate},
    {"LD HL, SP + %02X", Immediate8},
    {"LD SP, HL", NoImmediate},
    {"LD A, [%04X]", Immediate16},
    {"EI", NoImmediate},
    {"ILLEGAL", NoImmediate},
    {"ILLEGAL", NoImmediate},
    {"CP A, 0x%02X", Immediate8},
    {"RST $38", NoImmediate},
};

const AbstractInstruction InstructionTableCB[] = {
    {"RLC B", NoImmediate},
    {"RLC C", NoImmediate},
    {"RLC D", NoImmediate},
    {"RLC E", NoImmediate},
    {"RLC H", NoImmediate},
    {"RLC L", NoImmediate},
    {"RLC [HL]", NoImmediate},
    {"RLC A", NoImmediate},
    {"RRC B", NoImmediate},
    {"RRC C", NoImmediate},
    {"RRC D", NoImmediate},
    {"RRC E", NoImmediate},
    {"RRC H", NoImmediate},
    {"RRC L", NoImmediate},
    {"RRC [HL]", NoImmediate},
    {"RRC A", NoImmediate},
    {"RL B", NoImmediate},
    {"RL C", NoImmediate},
    {"RL D", NoImmediate},
    {"RL E", NoImmediate},
    {"RL H", NoImmediate},
    {"RL L", NoImmediate},
    {"RL [HL]", NoImmediate},
    {"RL A", NoImmediate},
    {"RR B", NoImmediate},
    {"RR C", NoImmediate},
    {"RR D", NoImmediate},
    {"RR E", NoImmediate},
    {"RR H", NoImmediate},
    {"RR L", NoImmediate},
    {"RR [HL]", NoImmediate},
    {"RR A", NoImmediate},
    {"SLA B", NoImmediate},
    {"SLA C", NoImmediate},
    {"SLA D", NoImmediate},
    {"SLA E", NoImmediate},
    {"SLA H", NoImmediate},
    {"SLA L", NoImmediate},
    {"SLA [HL]", NoImmediate},
    {"SLA A", NoImmediate},
    {"SRA B", NoImmediate},
    {"SRA C", NoImmediate},
    {"SRA D", NoImmediate},
    {"SRA E", NoImmediate},
    {"SRA H", NoImmediate},
    {"SRA L", NoImmediate},
    {"SRA [HL]", NoImmediate},
    {"SRA A", NoImmediate},
    {"SWAP B", NoImmediate},
    {"SWAP C", NoImmediate},
    {"SWAP D", NoImmediate},
    {"SWAP E", NoImmediate},
    {"SWAP H", NoImmediate},
    {"SWAP L", NoImmediate},
    {"SWAP [HL]", NoImmediate},
    {"SWAP A", NoImmediate},
    {"SRL B", NoImmediate},
    {"SRL C", NoImmediate},
    {"SRL D", NoImmediate},
    {"SRL E", NoImmediate},
    {"SRL H", NoImmediate},
    {"SRL L", NoImmediate},
    {"SRL [HL]", NoImmediate},
    {"SRL A", NoImmediate},
    {"BIT 0, B", NoImmediate},
    {"BIT 0, C", NoImmediate},
    {"BIT 0, D", NoImmediate},
    {"BIT 0, E", NoImmediate},
    {"BIT 0, H", NoImmediate},
    {"BIT 0, L", NoImmediate},
    {"BIT 0, [HL]", NoImmediate},
    {"BIT 0, A", NoImmediate},
    {"BIT 1, B", NoImmediate},
    {"BIT 1, C", NoImmediate},
    {"BIT 1, D", NoImmediate},
    {"BIT 1, E", NoImmediate},
    {"BIT 1, H", NoImmediate},
    {"BIT 1, L", NoImmediate},
    {"BIT 1, [HL]", NoImmediate},
    {"BIT 1, A", NoImmediate},
    {"BIT 2, B", NoImmediate},
    {"BIT 2, C", NoImmediate},
    {"BIT 2, D", NoImmediate},
    {"BIT 2, E", NoImmediate},
    {"BIT 2, H", NoImmediate},
    {"BIT 2, L", NoImmediate},
    {"BIT 2, [HL]", NoImmediate},
    {"BIT 2, A", NoImmediate},
    {"BIT 3, B", NoImmediate},
    {"BIT 3, C", NoImmediate},
    {"BIT 3, D", NoImmediate},
    {"BIT 3, E", NoImmediate},
    {"BIT 3, H", NoImmediate},
    {"BIT 3, L", NoImmediate},
    {"BIT 3, [HL]", NoImmediate},
    {"BIT 3, A", NoImmediate},
    {"BIT 4, B", NoImmediate},
    {"BIT 4, C", NoImmediate},
    {"BIT 4, D", NoImmediate},
    {"BIT 4, E", NoImmediate},
    {"BIT 4, H", NoImmediate},
    {"BIT 4, L", NoImmediate},
    {"BIT 4, [HL]", NoImmediate},
    {"BIT 4, A", NoImmediate},
    {"BIT 5, B", NoImmediate},
    {"BIT 5, C", NoImmediate},
    {"BIT 5, D", NoImmediate},
    {"BIT 5, E", NoImmediate},
    {"BIT 5, H", NoImmediate},
    {"BIT 5, L", NoImmediate},
    {"BIT 5, [HL]", NoImmediate},
    {"BIT 5, A", NoImmediate},
    {"BIT 6, B", NoImmediate},
    {"BIT 6, C", NoImmediate},
    {"BIT 6, D", NoImmediate},
    {"BIT 6, E", NoImmediate},
    {"BIT 6, H", NoImmediate},
    {"BIT 6, L", NoImmediate},
    {"BIT 6, [HL]", NoImmediate},
    {"BIT 6, A", NoImmediate},
    {"BIT 7, B", NoImmediate},
    {"BIT 7, C", NoImmediate},
    {"BIT 7, D", NoImmediate},
    {"BIT 7, E", NoImmediate},
    {"BIT 7, H", NoImmediate},
    {"BIT 7, L", NoImmediate},
    {"BIT 7, [HL]", NoImmediate},
    {"BIT 7, A", NoImmediate},
    {"RES 0, B", NoImmediate},
    {"RES 0, C", NoImmediate},
    {"RES 0, D", NoImmediate},
    {"RES 0, E", NoImmediate},
    {"RES 0, H", NoImmediate},
    {"RES 0, L", NoImmediate},
    {"RES 0, [HL]", NoImmediate},
    {"RES 0, A", NoImmediate},
    {"RES 1, B", NoImmediate},
    {"RES 1, C", NoImmediate},
    {"RES 1, D", NoImmediate},
    {"RES 1, E", NoImmediate},
    {"RES 1, H", NoImmediate},
    {"RES 1, L", NoImmediate},
    {"RES 1, [HL]", NoImmediate},
    {"RES 1, A", NoImmediate},
    {"RES 2, B", NoImmediate},
    {"RES 2, C", NoImmediate},
    {"RES 2, D", NoImmediate},
    {"RES 2, E", NoImmediate},
    {"RES 2, H", NoImmediate},
    {"RES 2, L", NoImmediate},
    {"RES 2, [HL]", NoImmediate},
    {"RES 2, A", NoImmediate},
    {"RES 3, B", NoImmediate},
    {"RES 3, C", NoImmediate},
    {"RES 3, D", NoImmediate},
    {"RES 3, E", NoImmediate},
    {"RES 3, H", NoImmediate},
    {"RES 3, L", NoImmediate},
    {"RES 3, [HL]", NoImmediate},
    {"RES 3, A", NoImmediate},
    {"RES 4, B", NoImmediate},
    {"RES 4, C", NoImmediate},
    {"RES 4, D", NoImmediate},
    {"RES 4, E", NoImmediate},
    {"RES 4, H", NoImmediate},
    {"RES 4, L", NoImmediate},
    {"RES 4, [HL]", NoImmediate},
    {"RES 4, A", NoImmediate},
    {"RES 5, B", NoImmediate},
    {"RES 5, C", NoImmediate},
    {"RES 5, D", NoImmediate},
    {"RES 5, E", NoImmediate},
    {"RES 5, H", NoImmediate},
    {"RES 5, L", NoImmediate},
    {"RES 5, [HL]", NoImmediate},
    {"RES 5, A", NoImmediate},
    {"RES 6, B", NoImmediate},
    {"RES 6, C", NoImmediate},
    {"RES 6, D", NoImmediate},
    {"RES 6, E", NoImmediate},
    {"RES 6, H", NoImmediate},
    {"RES 6, L", NoImmediate},
    {"RES 6, [HL]", NoImmediate},
    {"RES 6, A", NoImmediate},
    {"RES 7, B", NoImmediate},
    {"RES 7, C", NoImmediate},
    {"RES 7, D", NoImmediate},
    {"RES 7, E", NoImmediate},
    {"RES 7, H", NoImmediate},
    {"RES 7, L", NoImmediate},
    {"RES 7, [HL]", NoImmediate},
    {"RES 7, A", NoImmediate},
    {"SET 0, B", NoImmediate},
    {"SET 0, C", NoImmediate},
    {"SET 0, D", NoImmediate},
    {"SET 0, E", NoImmediate},
    {"SET 0, H", NoImmediate},
    {"SET 0, L", NoImmediate},
    {"SET 0, [HL]", NoImmediate},
    {"SET 0, A", NoImmediate},
    {"SET 1, B", NoImmediate},
    {"SET 1, C", NoImmediate},
    {"SET 1, D", NoImmediate},
    {"SET 1, E", NoImmediate},
    {"SET 1, H", NoImmediate},
    {"SET 1, L", NoImmediate},
    {"SET 1, [HL]", NoImmediate},
    {"SET 1, A", NoImmediate},
    {"SET 2, B", NoImmediate},
    {"SET 2, C", NoImmediate},
    {"SET 2, D", NoImmediate},
    {"SET 2, E", NoImmediate},
    {"SET 2, H", NoImmediate},
    {"SET 2, L", NoImmediate},
    {"SET 2, [HL]", NoImmediate},
    {"SET 2, A", NoImmediate},
    {"SET 3, B", NoImmediate},
    {"SET 3, C", NoImmediate},
    {"SET 3, D", NoImmediate},
    {"SET 3, E", NoImmediate},
    {"SET 3, H", NoImmediate},
    {"SET 3, L", NoImmediate},
    {"SET 3, [HL]", NoImmediate},
    {"SET 3, A", NoImmediate},
    {"SET 4, B", NoImmediate},
    {"SET 4, C", NoImmediate},
    {"SET 4, D", NoImmediate},
    {"SET 4, E", NoImmediate},
    {"SET 4, H", NoImmediate},
    {"SET 4, L", NoImmediate},
    {"SET 4, [HL]", NoImmediate},
    {"SET 4, A", NoImmediate},
    {"SET 5, B", NoImmediate},
    {"SET 5, C", NoImmediate},
    {"SET 5, D", NoImmediate},
    {"SET 5, E", NoImmediate},
    {"SET 5, H", NoImmediate},
    {"SET 5, L", NoImmediate},
    {"SET 5, [HL]", NoImmediate},
    {"SET 5, A", NoImmediate},
    {"SET 6, B", NoImmediate},
    {"SET 6, C", NoImmediate},
    {"SET 6, D", NoImmediate},
    {"SET 6, E", NoImmediate},
    {"SET 6, H", NoImmediate},
    {"SET 6, L", NoImmediate},
    {"SET 6, [HL]", NoImmediate},
    {"SET 6, A", NoImmediate},
    {"SET 7, B", NoImmediate},
    {"SET 7, C", NoImmediate},
    {"SET 7, D", NoImmediate},
    {"SET 7, E", NoImmediate},
    {"SET 7, H", NoImmediate},
    {"SET 7, L", NoImmediate},
    {"SET 7, [HL]", NoImmediate},
    {"SET 7, A", NoImmediate},
};

};
