#include "cpu.h"
#include <stdio.h>
#include <string.h>

#define DATA_LEN 6

unsigned char cpu_ram_read(struct cpu *cpu, unsigned char address)
{
  return cpu->ram[address];
}

void cpu_ram_write(struct cpu *cpu, unsigned char address, unsigned char data)
{
  cpu->ram[address] = data;
}

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu)
{
  char data[DATA_LEN] = {
      // From print8.ls8
      0b10000010, // LDI R0,8
      0b00000000,
      0b00001000,
      0b01000111, // PRN R0
      0b00000000,
      0b00000001 // HLT
  };

  int address = 0;

  for (int i = 0; i < DATA_LEN; i++)
  {
    cpu->ram[address++] = data[i];
  }

  // TODO: Replace this with something less hard-coded
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op)
  {
  case ALU_MUL:
    // TODO
    break;

    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction
  unsigned char ir, operandA, operandB;

  while (running)
  {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    ir = cpu_ram_read(cpu, cpu->pc);

    // 2. Figure out how many operands this next instruction requires
    operandA = cpu_ram_read(cpu, cpu->pc + 1);
    operandB = cpu_ram_read(cpu, cpu->pc + 2);

    // 3. Get the appropriate value(s) of the operands following this instruction
    // int add_to_pc = (ir >> 6) + 1;

    printf("TRACE: %02X: %02X %02X %02X\n", cpu->pc, ir, operandA, operandB);

    // 4. switch() over it to decide on a course of action.
    switch (ir)
    {
    case LDI:
      cpu->registers[operandA] = operandB;
      cpu->pc += 3;
      break;

    case PRN:
      printf("%d\n", cpu->registers[operandA]);
      // printf("8\n");
      cpu->pc += 2;
      break;

    case HLT:
      running = 0;
      break;
    }

    // 5. Do whatever the instruction should do according to the spec.

    // 6. Move the PC to the next instruction.
    // cpu->pc += add_to_pc;
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->pc = 0;
  memset(cpu->ram, 0, sizeof(cpu->ram));
  memset(cpu->registers, 0, sizeof(cpu->registers));
}