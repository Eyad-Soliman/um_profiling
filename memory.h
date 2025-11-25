/*
 *  memory.h
 *  Karla Armoush & Eyad Soliman
 *  COMP 40 HW 4: um
 *  11/09/25
 *
 * Interface for the UM's memory abstraction
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <uarray.h>
#include "assert.h"
#include "seq.h"
#include "uarray.h"


/* 32bit words so each word is 4 bytes */
#define word_size 4
struct Memory {
        Seq_T segments;
        Seq_T ids;
};
typedef struct Memory *Memory;

/* memory object that will store segments and segment IDs */


Memory load_segment0(UArray_T program);

void load_segment(Memory mem, uint32_t segment_ID, uint32_t **program_base);

//UArray_T get_segment(Memory mem, uint32_t segment_ID);

#define get_segment(mem, segment_ID) Seq_get(mem->segments, segment_ID)
#define get_word(mem, segment_ID, word_offset) *(uint32_t *)UArray_at(get_segment(mem, segment_ID), word_offset)
#define put_word(mem, segment_ID, word_offset, word) *(uint32_t *)UArray_at(get_segment(mem, segment_ID), word_offset) = word
// uint32_t get_word(Memory mem, uint32_t segment_ID, uint32_t word_offset);

// void put_word(Memory mem, uint32_t segment_ID,
//               uint32_t word_offset, uint32_t word);

// returns ID that segment was mapped to
uint32_t map(Memory mem, uint32_t segment_size); 

void unmap(Memory mem, uint32_t segment_ID);

void memory_free(Memory mem);

