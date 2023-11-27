#include <cstring>

#include "gbit/lib/tester.h"

#include "cpu/CPU.hpp"
#include "test/MockMMap.hpp"

static GameBoy::MockMMap *mmap;
static GameBoy::CPU *cpu;

static void CPUInit(size_t tester_instruction_mem_size,
                       uint8_t *tester_instruction_mem)
{
    mmap = new GameBoy::MockMMap(tester_instruction_mem, tester_instruction_mem_size);
    cpu = new GameBoy::CPU(*mmap);
}

static void CPUSetState(struct state *state)
{
    mmap->nMemoryAccesses = 0;

    GameBoy::Registers& regs = cpu->GetRegisters();
    regs.SetAF(state->reg16.AF);
    regs.SetBC(state->reg16.BC);
    regs.SetDE(state->reg16.DE);
    regs.SetHL(state->reg16.HL);

    regs.sp = state->SP;
    regs.pc = state->PC;

    if (state->halted)
        cpu->SetStatus(GameBoy::CPU::StatusHalted);
    else
        cpu->SetStatus(GameBoy::CPU::StatusRunning);
    
    cpu->SetInterruptsSwitch(state->interrupts_master_enabled);
}

static void CPUGetState(struct state *state)
{
    state->num_mem_accesses = mmap->nMemoryAccesses;
    memcpy(state->mem_accesses, mmap->memoryAccesses, sizeof(mmap->memoryAccesses));

    GameBoy::Registers& regs = cpu->GetRegisters();
    state->reg16.AF = regs.GetAF();
    state->reg16.BC = regs.GetBC();
    state->reg16.DE = regs.GetDE();
    state->reg16.HL = regs.GetHL();

    state->SP = regs.sp;
    state->PC = regs.pc;
    
    state->halted = (cpu->GetStatus() != GameBoy::CPU::StatusRunning);
    state->interrupts_master_enabled = cpu->InterruptsEnabled();
}

static int CPUStep(void)
{
    cpu->Step();
    return 0;
}

struct tester_operations ops = {
    .init       = CPUInit,
    .set_state  = CPUSetState,
    .get_state  = CPUGetState,
    .step       = CPUStep,
};

struct tester_flags flags = {
    .keep_going_on_mismatch         = 1,
    .enable_cb_instruction_testing  = 1,
    .print_tested_instruction       = 0,
    .print_verbose_inputs           = 0,
};

int main(int argc, char *argv[])
{
    if (argc == 2)
        if (!strcmp(argv[1], "step"))
            flags.keep_going_on_mismatch = 0;

    return tester_run(&flags, &ops);
}