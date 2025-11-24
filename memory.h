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
typedef struct Memory *Memory;

Memory load_segment0(UArray_T program);

void load_segment(Memory mem, uint32_t segment_ID, uint32_t **program_base);

//UArray_T get_segment(Memory mem, uint32_t segment_ID);

uint32_t get_word(Memory mem, uint32_t segment_ID, uint32_t word_offset);

void put_word(Memory mem, uint32_t segment_ID,
              uint32_t word_offset, uint32_t word);

// returns ID that segment was mapped to
uint32_t map(Memory mem, uint32_t segment_size); 

void unmap(Memory mem, uint32_t segment_ID);

void memory_free(Memory mem);

