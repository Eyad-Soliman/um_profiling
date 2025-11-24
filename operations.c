/*
 *  operations.c
 *  Karla Armoush & Eyad Soliman
 *  COMP 40 HW 4: um
 *  11/09/25
 *
 * Implementation of UM's operations
 * 
 */

#include "operations.h"

/* name: conditional_move
 * purpose: copy the value in r[B] into r[A] only if r[C] is non-zero.
 * arguments: ra - destination register
 *            rb - source register
 *            rc - condition register
 * returns: n/a
 * effects: may modify r[A]
 * checked errors: none
 * notes: instruction is a no-op if r[C] == 0
 */
void conditional_move(Um_registers ra, Um_registers rb, Um_registers rc)
{
        if (get_register(rc) != 0) put_register(ra, get_register(rb));
}

/* name: segmented_load
 * purpose: load a word from m[r[B]][r[C]] into register r[A].
 * arguments: mem – memory structure
 *            ra – destination register
 *            rb – register holding segment ID
 *            rc – register holding word offset
 * returns: n/a
 * effects: modifies r[A]
 * checked errors: CRE if mem is NULL
 * notes: allowed to segfault if accessing an unmapped segment or out-of-bounds
 */
void segmented_load(Memory mem,
                    Um_registers ra, Um_registers rb, Um_registers rc)
{
        assert(mem != NULL);
        put_register(ra, get_word(mem, get_register(rb), get_register(rc)));
}

/* name: segmented_store
 * purpose: store r[C] into memory segment m[r[A]] at offset r[B].
 * arguments: mem - memory structure
 *            ra – register holding segment ID
 *            rb – register holding word offset
 *            rc – register holding value to store
 * returns: n/a
 * effects: writes to memory segment
 * checked errors: CRE if mem is NULL
 * notes: allowed failure on unmapped or out-of-bounds segments
 */
void segmented_store(Memory mem,
                     Um_registers ra, Um_registers rb, Um_registers rc)
{   
        assert(mem != NULL);
        put_word(mem, get_register(ra), get_register(rb), get_register(rc));
}

/* name: addition
 * purpose: compute r[B] + r[C] modulo 2^32 and store in r[A].
 * arguments: ra, rb, rc - register IDs
 * returns: n/a
 * effects: modifies r[A]
 * checked errors: none
 * notes: unsigned 32-bit wraparound is intentional
 */
void addition(Um_registers ra, Um_registers rb, Um_registers rc)
{
        put_register(ra, (get_register(rb) + get_register(rc)));
}

/* name: multiplication
 * purpose: compute r[B] * r[C] modulo 2^32 and store in r[A].
 * arguments: ra, rb, rc - register IDs
 * returns: n/a
 * effects: modifies r[A]
 * checked errors: none
 */
void multiplication(Um_registers ra, Um_registers rb, Um_registers rc)
{
        put_register(ra, (get_register(rb) * get_register(rc)));
}

/* name: division
 * purpose: compute r[B] / r[C] and store quotient in r[A].
 * arguments: ra, rb, rc - register IDs
 * returns: n/a
 * effects: modifies r[A]
 * checked errors: none
 * notes: allowed failure if r[C] == 0
 */
void division(Um_registers ra, Um_registers rb, Um_registers rc)
{
        put_register(ra, (get_register(rb) / get_register(rc)));
}

/* name: nand
 * purpose: compute bitwise NOT of (r[B] AND r[C]) and store in r[A].
 * arguments: ra, rb, rc - register IDs
 * returns: n/a
 * effects: modifies r[A]
 * checked errors: none
 */
void nand(Um_registers ra, Um_registers rb, Um_registers rc)
{
        put_register(ra, ~(get_register(rb) & get_register(rc)));
}

/* name: halt
 * purpose: free all memory and terminate execution.
 * arguments: mem - memory structure
 * returns: never returns
 * effects: frees all allocated memory and exits program
 * checked errors: CRE if mem is NULL
 */
void halt(Memory mem) {
        assert(mem != NULL);
        memory_free(mem);
        exit(EXIT_SUCCESS);
}

/* name: map_segment
 * purpose: create a new segment of length r[C], initialize to all zeros,
 *          store assigned segment ID into r[B].
 * arguments: mem - memory structure
 *            rb - destination register for segment ID
 *            rc - register giving segment size
 * returns: n/a
 * effects: allocates new memory segment, modifies r[B]
 * checked errors: CRE if mem is NULL
 */
void map_segment(Memory mem, Um_registers rb, Um_registers rc)
{
        assert(mem != NULL);
        put_register(rb, map(mem, get_register(rc)));
}

/* name: unmap_segment
 * purpose: unmap the segment whose ID is in r[C].
 * arguments: mem - memory structure
 *            rc - register holding segment ID
 * returns: n/a
 * effects: frees memory segment, records ID for reuse
 * checked errors: CRE if mem is NULL
 * notes: allowed failure if attempting to unmap segment 0 or unmapped segment
 */
void unmap_segment(Memory mem, Um_registers rc)
{
        assert(mem != NULL);
        unmap(mem, get_register(rc));
}

/* name: output
 * purpose: write the low 8 bits of r[C] to stdout as a character.
 * arguments: rc - register holding character value
 * returns: n/a
 * effects: writes to stdout
 * checked errors: CRE if r[C] > 255
 */
void output(Um_registers rc)
{
        uint32_t value = get_register(rc);
        assert(value <= 255);

        putchar(value);
}

/* name: input
 * purpose: read a character from stdin and store it in r[C].
 * arguments: rc - destination register
 * returns: n/a
 * effects: modifies r[C], may read from stdin
 * checked errors: none
 * notes: stores 0xFFFFFFFF if EOF
 */
void input(Um_registers rc)
{
        int value = getchar();

        if (value == EOF) {
                put_register(rc, ~(uint32_t)0);
        } else {
                assert(value >= 0 && value <= 255);
                put_register(rc, value);
        }
}

/* name: load_program
 * purpose: replace segment 0 with a duplicate of segment m[r[B]],
 *          then set PC to the value in r[C].
 * arguments: mem - memory structure
 *            rb - register holding segment ID to load
 *            rc - register holding new PC
 *            pc - pointer to program counter
 * returns: n/a
 * effects: overwrites segment 0; updates PC; alloc/free operations
 * checked errors: CRE if mem or pc is NULL
 * notes: if r[B] == 0, only PC is updated
 */
void load_program(Memory mem, Um_registers rb, Um_registers rc, uint32_t **pc,
                  uint32_t **program_base)
{
        assert(mem != NULL);
        assert(pc != NULL);
        if (get_register(rb) != 0) {
                load_segment(mem, get_register(rb), program_base);
        } 

        *pc = *program_base + get_register(rc);
}   

/* name: load_value
 * purpose: load a 25-bit immediate constant into register r[A].
 * arguments: ra - destination register
 *            value - 25-bit unsigned constant
 * returns: n/a
 * effects: modifies r[A]
 * checked errors: none
 */
void load_value(Um_registers ra, uint32_t value)
{
        put_register(ra, value);
}