#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
void cpu_load(struct cpu *cpu, char *argv)
{
  // char data[DATA_LEN] = {
  //     // From print8.ls8
  //     0b10000010, // LDI R0,8
  //     0b00000000,
  //     0b00001000,
  //     0b01000111, // PRN R0
  //     0b00000000,
  //     0b00000001 // HLT
  // };

  // int address = 0;

  // for (int i = 0; i < DATA_LEN; i++)
  // {
  //   cpu->ram[address++] = data[i];
  // }

  // TODO: Replace this with something less hard-coded
  FILE *fp;
  char line[1024];

  int address = 0;

  fp = fopen(argv, "r");

  while (fgets(line, sizeof line, fp) != NULL)
  {
    // printf("%s", line);
    if (line[0] == '\n' || line[0] == '#')
    {
      printf("Ignoring this line.\n");
      continue;
    }

    unsigned char b;
    b = strtoul(line, NULL, 2);
    // printf("%02X\n", b);

    cpu_ram_write(cpu, address++, b);
  }

  fclose(fp);
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
    cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
    break;

    // TODO: implement more ALU ops
  case ALU_ADD:
    cpu->registers[regA] = cpu->registers[regA] + cpu->registers[regB];
    break;

  case ALU_CMP:
    if (cpu->registers[regA] == cpu->registers[regB])
    {
      cpu->registers[7] = 1;
      cpu->registers[6] = 0;
      cpu->registers[5] = 0;
    }
    else if (cpu->registers[regA] < cpu->registers[regB])
    {
      cpu->registers[7] = 0;
      cpu->registers[6] = 0;
      cpu->registers[5] = 1;
    }
    else
    {
      cpu->registers[7] = 0;
      cpu->registers[6] = 1;
      cpu->registers[5] = 0;
    }
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction
  unsigned char ir, operandA, operandB;

  int SP = 7; // this is the stack pointer as states in the specs

  while (running)
  {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    ir = cpu_ram_read(cpu, cpu->pc);

    // 2. Figure out how many operands this next instruction requires
    operandA = cpu_ram_read(cpu, cpu->pc + 1);
    operandB = cpu_ram_read(cpu, cpu->pc + 2);

    // 3. Get the appropriate value(s) of the operands following this instruction

    int add_to_pc = (ir >> 6) + 1; // this shift over 6 - the first two digits represent the number of arguments. It gets us the number of bytes that the instruction has arguments. Then we add one more byte on for the instruction itself

    // printf("TRACE: %02X: %02X %02X %02X\n", cpu->pc, ir, operandA, operandB);

    // 4. switch() over it to decide on a course of action.
    switch (ir)
    {
    case LDI:
      cpu->registers[operandA] = operandB;
      break;

    case PRN:
      printf("%d\n", cpu->registers[operandA]);
      break;

    case MUL:
      alu(cpu, ALU_MUL, operandA, operandB);
      break;

    case ADD:
      alu(cpu, ALU_ADD, operandA, operandA);
      break;

    case PUSH:
      cpu->registers[SP]--;
      cpu_ram_write(cpu, cpu->registers[SP], cpu->registers[operandA]);
      break;

    case POP:
      cpu->registers[operandA] = cpu_ram_read(cpu, cpu->registers[SP]);
      cpu->registers[SP]++;
      break;

    case CALL:
      add_to_pc = 0;
      cpu_ram_write(cpu, cpu->registers[SP], cpu->pc + 2);
      cpu->pc = 24;
      break;

    case RET:
      add_to_pc = 0;
      cpu->pc = cpu_ram_read(cpu, cpu->registers[SP]);
      break;

    case CMP:
      alu(cpu, ALU_CMP, operandA, operandB);
      break;

    case JEQ:
      if (cpu->registers[7] == 1)
      {
        cpu->pc = cpu->registers[operandA];
        add_to_pc = 0;
      }
      break;

    case JNE:
      if (cpu->registers[7] == 0)
      {
        cpu->pc = cpu->registers[operandA];
        add_to_pc = 0;
      }
      break;

    case JMP:
      cpu->pc = cpu->registers[operandA];
      add_to_pc = 0;
      break;

    case HLT:
      running = 0;
      break;
    }

    // 5. Do whatever the instruction should do according to the spec.

    // 6. Move the PC to the next instruction.
    cpu->pc += add_to_pc;
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  int SP = 7; // this is the stack pointer as states in the specs

  cpu->pc = 0;
  memset(cpu->ram, 0, sizeof(cpu->ram));
  memset(cpu->registers, 0, sizeof(cpu->registers));
  cpu->registers[SP] = 0xF4;
}