/*
 *  um.c
 *  Karla Armoush & Eyad Soliman
 *  COMP 40 HW 4: um
 *  11/09/25
 *
 * Universal Machine program that proceses .um
 * files and executes instructions
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "assert.h"
#include "operations.h"
#include "bitpack.h"

/* defines a struct object which contains member values representing
   the bitfields that each UM instruction can contain */
typedef struct instruction {
        Um_opcode opcode;
        Um_registers ra, rb, rc;
        uint32_t value;
} instruction;

void read_program(FILE *fp, UArray_T *program_p);
uint32_t read_word(FILE *fp);
void execute_operation(Memory mem, instruction *instr, uint32_t *pc);
void decode_instruction(uint32_t word, instruction *instr);

 /* main
 *
 * Initializes the UM memory with the program and manages the
 * fetch, decode, execute loop
 *
 * Parameters:
 *      int argc: The number of arguments passed on the command line
 *      char *argv[]: An array of the arhumnets passed on the command line
 *
 * Return: none
 *
 * Expects
 *      argc is equal to 2
 *
 * Notes: CRE if fails to open file
 *        The Memory abstraction itself is free upon encountering a halt 
 */
int main(int argc, char *argv[])
{
        if (argc != 2) {
                fprintf(stderr, "Usage: %s [filename].um\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        struct stat file_info;

        if(stat(argv[1], &file_info) == -1) {
                fprintf(stderr, "Error: stat\n");
                exit(EXIT_FAILURE);
        }

        FILE *fp = fopen(argv[1], "rb");
        assert(fp != NULL);

        UArray_T program = UArray_new(file_info.st_size / 4, word_size);
        read_program(fp, &program);
        fclose(fp);
        Memory mem = load_segment0(program);

        uint32_t pc = 0;
        instruction instr;

        while (1) {
                uint32_t word = get_word(mem, 0, pc);
                decode_instruction(word, &instr);

                execute_operation(mem, &instr, &pc); // note: pass instr by reference to mitigate ecopies
        }
        
        return EXIT_SUCCESS;
}

/* name: read_program
 * purpose: reads the contents of the binary .um file and stores the 
            instructions in a UArray representing segment 0
 * arguments: fp - file pointer object to read from
 *            program_p - pointer to segment that will store program
 * returns: n/a
 * effects: stores 32bit instructions in the UArray
 * checked errors: asserts file and the UArray pointers are not NULL
 * notes: n/a
 */
void read_program(FILE *fp, UArray_T *program_p)
{
        assert(fp != NULL);
        assert(program_p != NULL);

        int program_length = UArray_length(*program_p);
        for (int i = 0; i < program_length; i++) {
                uint32_t *word_p = UArray_at(*program_p, i);
                *word_p = read_word(fp);
        }
}

/* name: read_word
 * purpose: reads one individual 32bit instruction in big-endian
 * arguments: fp - file pointer object to read from
 * returns: uint32_t word representing an instruction
 * effects: n/a
 * checked errors: asserts file object is not NULL
 *                 asserts that an EOF is never prematurely reached
 * notes: uses bitpack operations to correctly format bit fields in the word
 */
uint32_t read_word(FILE *fp) 
{
        assert(fp != NULL);
        uint64_t word = 0ull;
        /* read bytes and pack starting at highest-order byte (Big-Endian) */
        for (int i = 0; i < 4; i++) {
                int c = getc(fp);
                /* CRE if insufficient amount of bytes in file */
                assert(c != EOF); 
                word = Bitpack_newu(word, 8, 24 - (i * 8), (uint64_t)c);
        }

        return (uint32_t)word;
}

/* name: execute_operation
 * purpose: takes a single instruction and carries out the encoded operation
 * arguments: mem - memory abstraction object
 *            instr - 32bit instruction
 *            *pc - reference to the program counter
 * returns: none
 * effects: increments program counter (unless LV operation), frees memory
 *          abstraction object if halt instruction is provided
 * checked errors: memory is not NULL and program counter pointer is not NULL
 * notes: n/a
 */
void execute_operation(Memory mem, instruction *instr, uint32_t *pc)
{
        assert(mem != NULL);
        assert(pc != NULL);

        switch (instr->opcode) {
                case CMOV:
                        conditional_move(instr->ra, instr->rb, instr->rc);
                        break;
                case SLOAD:
                        segmented_load(mem, instr->ra, instr->rb, instr->rc);
                        break;
                case SSTORE:
                        segmented_store(mem, instr->ra, instr->rb, instr->rc);
                        break;
                case ADD:
                        addition(instr->ra, instr->rb, instr->rc);
                        break;
                case MUL:
                        multiplication(instr->ra, instr->rb, instr->rc);
                        break;
                case DIV:
                        division(instr->ra, instr->rb, instr->rc);
                        break;
                case NAND:
                        nand(instr->ra, instr->rb, instr->rc);
                        break;
                case HALT:
                        halt(mem);
                        break;
                case ACTIVATE:
                        map_segment(mem, instr->rb, instr->rc);
                        break;
                case INACTIVATE:
                        unmap_segment(mem, instr->rc);
                        break;
                case OUT:
                        output(instr->rc);
                        break;
                case IN:
                        input(instr->rc);
                        break;
                case LOADP:
                        load_program(mem, instr->rb, instr->rc, pc);
                        return;
                case LV:
                        load_value(instr->ra, instr->value);
                        break;
        }

        (*pc)++;

}

/* name: decode_instruction
 * purpose: takes 32bit word and parses fields to initialize instruction object
 * arguments: word - 32bit instruction
 *            *instr - reference to an instruction object
 * returns: none
 * effects: initializes the instructions member variables with values
 *          corresponding to fields in the 32bit word
 * checked errors: asserts that the instruction reference is not NULL and
 *                 that decoded opcodes are between 0-13
 * notes: bitfield corresponding to register A is handled different for LV
 */
void decode_instruction(uint32_t word, instruction *instr)
{
        assert(instr != NULL);

        instr->opcode = word >> 28;
        assert(instr->opcode < 14);

        if (instr->opcode == LV) {
                instr->ra = ((word >> 25) & 7);
        } else {
                instr->ra = ((word >> 6) & 7);
        }
        
        instr->rb = ((word >> 3) & 7);
        instr->rc = (word & 7);
        instr->value =  (word & ((1ul << 25) - 1));

}