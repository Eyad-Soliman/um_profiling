/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction word = 0;
        word = Bitpack_newu(word, 4, 28, op);
        word = Bitpack_newu(word, 3, 0, rc);
        word = Bitpack_newu(word, 3, 3, rb);
        word = Bitpack_newu(word, 3, 6, ra);
        return word;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction word = 0;
        word = Bitpack_newu(word, 4, 28, LV);
        word = Bitpack_newu(word, 3, 25, ra);
        word = Bitpack_newu(word, 25, 0, val);
        return word;
}


/* Wrapper functions for each of the instructions */

static inline Um_instruction conditionl_move(Um_register a, Um_register b, Um_register c) 
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction segmented_load(Um_register a, Um_register b, Um_register c) 
{
        return three_register(SLOAD, a, b, c);
}

static inline Um_instruction segmented_store(Um_register a, Um_register b, Um_register c) 
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction multiplication(Um_register a, Um_register b, Um_register c) 
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction division(Um_register a, Um_register b, Um_register c) 
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction bitwise_nand(Um_register a, Um_register b, Um_register c) 
{
        return three_register(NAND, a, b, c);
}

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

static inline Um_instruction map_segment(Um_register b, Um_register c) 
{
        return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction unmap_segment(Um_register c) 
{
        return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction output(Um_register c) 
{
        return three_register(OUT, 0, 0, c);
}

static inline Um_instruction input(Um_register c) 
{
        return three_register(IN, 0, 0, c);
}

static inline Um_instruction load_program(Um_register b, Um_register c) 
{
        return three_register(LOADP, 0, b, c);
}

static inline Um_instruction load_value(Um_register a, uint32_t value) 
{
        return loadval(a, value);
}

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}


void build_LV_and_output_test(Seq_T stream)
{
        append(stream, loadval(r0, 'H'));
        append(stream, output(r0));
        append(stream, loadval(r1, 'e'));
        append(stream, output(r1));
        append(stream, loadval(r2, 'l'));
        append(stream, output(r2));
        append(stream, loadval(r3, 'l'));
        append(stream, output(r3));
        append(stream, loadval(r4, 'o'));
        append(stream, output(r4));
        append(stream, loadval(r5, '4'));
        append(stream, output(r5));
        append(stream, loadval(r6, '0'));
        append(stream, output(r6));
        append(stream, loadval(r7, '\n'));
        append(stream, output(r7));
        append(stream, halt());
}

void build_io_test(Seq_T stream)
{
        append(stream, input(r0));
        append(stream, input(r1));
        append(stream, input(r2));
        append(stream, input(r3));
        append(stream, input(r4));
        append(stream, input(r5));
        append(stream, input(r6));
        append(stream, input(r7));

        append(stream, output(r0));
        append(stream, output(r1));
        append(stream, output(r2));
        append(stream, output(r3));
        append(stream, output(r4));
        append(stream, output(r5));
        append(stream, output(r6));
        append(stream, output(r7));
        
        append(stream, halt());
}

/* testing using special inputs from /dev/urandom (chars outside the printable
ASCII range) */
void build_special_io_test(Seq_T stream)
{
        append(stream, input(r0));
        append(stream, input(r1));
        append(stream, input(r2));
        append(stream, input(r3));
        append(stream, input(r4));
        append(stream, input(r5));
        append(stream, input(r6));
        append(stream, input(r7));

        append(stream, output(r0));
        append(stream, output(r1));
        append(stream, output(r2));
        append(stream, output(r3));
        append(stream, output(r4));
        append(stream, output(r5));
        append(stream, output(r6));
        append(stream, output(r7));

        append(stream, input(r0));
        append(stream, input(r1));
        append(stream, input(r2));
        append(stream, input(r3));
        append(stream, input(r4));
        append(stream, input(r5));
        append(stream, input(r6));
        append(stream, input(r7));

        append(stream, output(r0));
        append(stream, output(r1));
        append(stream, output(r2));
        append(stream, output(r3));
        append(stream, output(r4));
        append(stream, output(r5));
        append(stream, output(r6));
        append(stream, output(r7));
        
        append(stream, halt());
}


void build_arithmetic_test(Seq_T stream)
{
        // c
        append(stream, loadval(r0, 90));
        append(stream, loadval(r1, 9));
        append(stream, add(r2, r0, r1));
        append(stream, output(r2));

        // a
        append(stream, loadval(r0, 5));
        append(stream, loadval(r1, 19));
        append(stream, loadval(r2, 2));
        append(stream, multiplication(r3, r0, r1));
        append(stream, add(r4, r3, r2));
        append(stream, output(r4));

        // t
        append(stream, loadval(r0, 220));
        append(stream, loadval(r1, 4));
        append(stream, loadval(r2, 2));
        append(stream, loadval(r3, 6));
        append(stream, division(r4, r0, r1));
        append(stream, multiplication(r4, r4, r2));
        append(stream, add(r4, r4, r3));
        append(stream, output(r4));

        append(stream, loadval(r0, 10));
        append(stream, output(r0));

        append(stream, halt());
}

void build_divide_truncate_test(Seq_T stream)
{
    append(stream, loadval(r1, 100));
    append(stream, loadval(r2, 3));
    append(stream, division(r0, r1, r2));
    append(stream, output(r0));
    append(stream, loadval(r0, '\n'));
    append(stream, output(r0));
    append(stream, halt());
}

void build_divide_0_test(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 0));
        append(stream, division(r0, r1, r2));
        append(stream, halt());
}

void build_nand_test(Seq_T stream)
{
        append(stream, input(r0));
        append(stream, load_value(r1, 0x0F));
        append(stream, bitwise_nand(r2, r0, r1));
        append(stream, bitwise_nand(r2, r2, r2));

        append(stream, load_value(r3, 48));
        append(stream, add(r4, r2, r3));

        append(stream, output(r4));
        append(stream, halt());
}

void build_cmov_zero_test(Seq_T stream)
{
        /* r2 is 0 so condition should not happen */
        append(stream, loadval(r0, 'a')); 
        append(stream, loadval(r1, 'b'));
        append(stream, loadval(r2, 0));       

        append(stream, conditionl_move(r0, r1, r2));  /* r0 should stay a */
        append(stream, output(r0));                  
        append(stream, halt());
}

void build_cmov_non_zero_test(Seq_T stream)
{
        /* r2 is not 0 so condition should happen */
        append(stream, loadval(r0, 'a')); 
        append(stream, loadval(r1, 'b'));
        append(stream, loadval(r2, 1));       

        append(stream, conditionl_move(r0, r1, r2));  /* r0 becomes b */
        append(stream, output(r0));                  
        append(stream, halt());
}

void build_load_program_test(Seq_T stream)
{
        append(stream, loadval(r0, 0));
        append(stream, loadval(r1, 6));
        /* stay in segment 0 but jump to word 7 to skip the halts */
        append(stream, load_program(r0, r1));

        append(stream, halt());
        append(stream, halt());
        append(stream, halt());

        /* output to show we've jumped the halts */
        append(stream, loadval(r0, 'X'));
        append(stream, output(r0));
        append(stream, halt());
}

void build_map_unmap_test(Seq_T stream)
{
        append(stream, loadval(r2, 50));
        append(stream, loadval(r3, 100));
        append(stream, map_segment(r0, r2)); /* r0 has segment ID 1 */
        append(stream, map_segment(r1, r3)); /* r1 has segment ID 2 */

        append(stream, loadval(r5, 10));
        append(stream, loadval(r6, 48));

        /* outputs 1 and 2 separated by a new line */
        append(stream, add(r7, r0, r6));
        append(stream, output(r7));
        append(stream, output(r5));
        append(stream, add(r7, r1, r6));
        append(stream, output(r7));
        append(stream, output(r5));

        append(stream, unmap_segment(r0)); /* unmaps r0 which has ID 1 */

        append(stream, loadval(r2, 5)); /* maps segment of size 5 */
        append(stream, map_segment(r0, r2)); /* should reuse segment ID 1 */
        
        /* outputs reused segment, which is 1 */
        append(stream, add(r7, r0, r6));
        append(stream, output(r7));
        append(stream, output(r5));

        append(stream, halt());
}

void build_sstore_sload_test(Seq_T stream)
{
        append(stream, loadval(r1, 5));
        append(stream, map_segment(r0, r1)); /* r0 has segment ID 1 */

        append(stream, loadval(r1, 4));
        append(stream, loadval(r2, 97));
        append(stream, segmented_store(r0, r1, r2)); /* puts 'a' in m[1][4] */
        append(stream, segmented_load(r3, r0, r1)); /* extracts m[1][4] into r3 */

        append(stream, loadval(r4, 10));
        append(stream, output(r3));
        append(stream, output(r4));
        append(stream, halt());
}

void build_load_segment_test(Seq_T stream)
{
        append(stream, loadval(r1, 5));
        append(stream, map_segment(r0, r1)); /* r0 has segment ID 1 */

        append(stream, loadval(r1, 7));
        append(stream, loadval(r2, 128));
        
        /* multiples 7 by 128 4 times to create a 32 bit word containing halt 
        instruction */
        append(stream, multiplication(r1, r1, r2));
        append(stream, multiplication(r1, r1, r2));
        append(stream, multiplication(r1, r1, r2));
        append(stream, multiplication(r1, r1, r2));

        /* stores in our mapped segment the halt instruction at word offset 4 */
        append(stream, loadval(r3, 4));
        append(stream, segmented_store(r0, r3, r1));

        /* loads mapped segment and jumps program counter to word offset 4 */
        append(stream, load_program(r0, r3));

}

