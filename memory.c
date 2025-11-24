/*
 *  memory.h
 *  Karla Armoush & Eyad Soliman
 *  COMP 40 HW 4: um
 *  11/09/25
 *
 * Implementation of the UM's memory abstraction
 */

#include "memory.h"

/* memory object that will store segments and segment IDs */
struct Memory {
        Seq_T segments;
        Seq_T ids;
};

/* name: load_segment0
 * purpose: create the Memory abstraction and install the program as segment 0
 * arguments: UArray_T representing the program holding the words to execute in
 *            big endian order.
 * returns: a Memory object with exactly one mapped segment (ID 0).
 * effects: allocates Memory and two Hanson's sequences (storing words and IDs)
 * checked errors: asserts program is not NULL and all malloc/Seq_new calls 
 *                 succeed.
 * notes: segment 0 is stored at index 0 of mem->segments; mem->ids is empty
 */
Memory load_segment0(UArray_T program)
{
        assert(program != NULL);

        Memory mem = malloc(sizeof(*mem));
        assert(mem != NULL);
        mem->segments = Seq_new(0);
        mem->ids = Seq_new(0);

        assert(mem->segments != NULL);
        assert(mem->ids != NULL);
        
        Seq_addhi(mem->segments, program);
        
        return mem;
}

/* name: map
 * purpose: allocate a segment whose elements are initialized to zero of the 
 *          requested length and assign it an ID.
 * arguments: mem - Memory instance
 *            segment_size - number of 32-bit words to allocate
 * returns: segment ID assigned to the new segment.
 * effects: may allocate a new UArray_T, may modify mem->segments and mem->ids
 * checked errors: asserts mem is not NULL and asserts UArray_new succeeds
 * notes: reuses an ID from mem->ids if available, otherwise appends segment
 */
uint32_t map(Memory mem, uint32_t segment_size) // returns assigned ID of seg
{
        assert(mem != NULL);

        /* initialize a zero-ed segment of passed size */
        UArray_T segment = UArray_new(segment_size, word_size);
        assert(segment != NULL);
        int segment_length = UArray_length(segment);
        for (int i = 0; i < segment_length; i++) {
                uint32_t *word_p = UArray_at(segment, i);
                *word_p = 0;
        }

        uint32_t id;
        /* case where we have an ID we can reuse */
        if (Seq_length(mem->ids) > 0) {
                id = (uint32_t)(uintptr_t)Seq_remlo(mem->ids);
                Seq_put(mem->segments, id, segment);
                return id;
        }

        /* case where we must assign the segment a new ID */
        id = Seq_length(mem->segments);
        Seq_addhi(mem->segments, segment);

        return id;

}

/* name: unmap
 * purpose: free a previously-mapped segment and recycle its ID
 * arguments: mem - Memory instance
 *            segment_ID - ID of the segment to unmap (must be mapped)
 * returns: n/a
 * effects: frees the UArray_T at mem->segments[segment_ID];
 *          inserts segment_ID at the end of mem->ids for future reuse
 * checked errors: asserts mem and the segment pointer are not NULL
 * notes: never intended for segment 0; caller must guarantee correctness
 */
void unmap(Memory mem, uint32_t segment_ID)
{
        assert(mem != NULL);
    
        /* extract the segment to unmap and put NULL in its place */
        UArray_T segment = Seq_put(mem->segments, segment_ID, NULL);
        assert(segment != NULL);
        UArray_free(&segment);

        /* save ID for possible reuse */
        Seq_addhi(mem->ids, (void *)(uintptr_t)segment_ID);
}

/* name: get_segment
 * purpose: obtain the UArray_T that represents a segment
 * arguments: mem - Memory instance
 *            segment_ID - ID of the desired segment
 * returns: pointer to the segment's UArray_T
 * effects: none
 * checked errors: asserts mem is not NULL
 * notes: none
 */
UArray_T get_segment(Memory mem, uint32_t segment_ID)
{
        assert(mem != NULL);

        return Seq_get(mem->segments, segment_ID);
}

/* name: get_word
 * purpose: fetch a 32-bit word from a given segment and offset
 * arguments: mem - Memory instance
 *            segment_ID - segment to index
 *            word_offset - index within the segment
 * returns: 32-bit word stored at that location
 * effects: non
 * checked errors: asserts mem is not NULL
 * notes: none
 */
uint32_t get_word(Memory mem, uint32_t segment_ID, uint32_t word_offset)
{
        assert(mem != NULL);
        
        uint32_t *word_p = UArray_at(get_segment(mem, segment_ID),
                                     word_offset);
        return *word_p;
}

/* name: put_word
 * purpose: store a 32-bit word into a given segment and offset
 * arguments: mem - Memory instance
 *            segment_ID - segment to update
 *            word_offset - index within the segment
 *            word - value to write
 * returns: n/a
 * effects: overwrites the specified word in memory
 * checked errors: asserts mem is not NULL
 * notes: none
 */
void put_word(Memory mem, uint32_t segment_ID,
              uint32_t word_offset, uint32_t word)
{
        assert(mem != NULL);
        
        uint32_t *word_p = UArray_at(get_segment(mem, segment_ID),
                                     word_offset);
        *word_p = word;
}

/* name: memory_free
 * purpose: release all heap storage owned by a Memory instance
 * arguments: mem - Memory instance
 * returns: n/a
 * effects: frees every mapped segment, the two sequence, and the Memory struct
 * checked errors: asserts mem is not NULL
 * notes: none
 */
void memory_free(Memory mem)
{
        assert(mem != NULL);
        
        /* FREE DATA INSIDE SEGMENT SEQUENCE */
        int sequence_length = Seq_length(mem->segments);
        for (int i = 0; i < sequence_length; i++) {
                UArray_T segment = Seq_get(mem->segments, i);
                if (segment != NULL) UArray_free(&segment);
        }
        Seq_free(&mem->segments);
        
        Seq_free(&mem->ids);
        free(mem);
}

/* name: load_segment
 * purpose: duplicate an existing segment and install the copy as segment 0
 * arguments: mem - Memory instance
 *            segment_ID - ID of the segment to duplicate (may be 0)
 * returns: n/a
 * effects: allocates a new UArray_T copy, replaces mem->segments[0] with it,
 *          frees the old segment 0, leaves other segments unchanged
 * checked errors: asserts mem is not NULL, asserts segment exists, asserts 
 *                 memory allocations did not fail.
 * notes: implements the load value instruction. if the segment id is 0, the 
 *        function still gets a deep copy of the segment.    
 */
void load_segment(Memory mem, uint32_t segment_ID)
{
        assert(mem != NULL);

        /* gets segment to duplicate */
        UArray_T segment = get_segment(mem, segment_ID);
        assert(segment != NULL);

        /* initializes a new segment and copies values */
        UArray_T duplicate = UArray_new(UArray_length(segment), word_size);
        assert(duplicate != NULL);
        int segment_length = UArray_length(duplicate);
        for (int i = 0; i < segment_length; i++) {
                uint32_t *word_p = UArray_at(duplicate, i);
                *word_p = *(uint32_t *)UArray_at(segment, i);
        }
        
        /* puts duplicate segment into segment 0 and abandons old segment 0 */
        UArray_T abandon = Seq_put(mem->segments, 0, duplicate);
        UArray_free(&abandon);
}





