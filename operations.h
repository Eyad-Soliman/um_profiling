/*
 *  operations.h
 *  Karla Armoush & Eyad Soliman
 *  COMP 40 HW 4: um
 *  11/09/25
 *
 * UM's operations interface
 */
#include "registers.h"
#include "memory.h"

/* enumeration corresponding to 14 instructions and their opcodes */
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

void conditional_move(Um_registers ra, Um_registers rb, Um_registers rc);

void segmented_load(Memory mem,
                    Um_registers ra, Um_registers rb, Um_registers rc);

void segmented_store(Memory mem,
                     Um_registers ra, Um_registers rb, Um_registers rc);

void addition(Um_registers ra, Um_registers rb, Um_registers rc);

void multiplication(Um_registers ra, Um_registers rb, Um_registers rc);

void division(Um_registers ra, Um_registers rb, Um_registers rc);

void nand(Um_registers ra, Um_registers rb, Um_registers rc);

void halt(Memory mem);

void map_segment(Memory mem, Um_registers rb, Um_registers rc);

void unmap_segment(Memory mem, Um_registers rc);

void output(Um_registers rc);

void input(Um_registers rc);

void load_program(Memory mem, Um_registers rb, Um_registers rc,
                  uint32_t **pc, uint32_t **program_base);

void load_value(Um_registers ra, uint32_t value);
