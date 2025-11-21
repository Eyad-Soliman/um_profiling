/*
 *  registers.c
 *  Karla Armoush & Eyad Soliman
 *  COMP 40 HW 4: um
 *  11/09/25
 *
 * Implementation of registers interface 
 * 
 */

#include "registers.h"

uint32_t registers[num_registers] = {0};

/* name: get_register
 * purpose: returns the value contained in register[register_ID]
 * arguments: 
 *          register_ID - the ID of the register to extract value from
 * returns: 
 *          uint32_t value inside register
 * effects: none
 * checked errors: none
 */
uint32_t get_register(Um_registers register_ID) 
{
        return registers[register_ID];
}

/* name: put_register
 * purpose: stores a value in specified register
 * arguments: 
 *          register_ID - the ID of the register to store value in
 *          value - the value to store
 * returns: n/a
 * effects: none
 * checked errors: none
 */
void put_register(Um_registers register_ID, uint32_t value)
{
        registers[register_ID] = value;
}