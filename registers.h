/*
 *  registers.h
 *  Karla Armoush & Eyad Soliman
 *  COMP 40 HW 4: um
 *  11/09/25
 * 
 * registers interface
 *
 */
#include <stdint.h>

#define num_registers 8

/* enumeration for 8 registers */
typedef enum Um_registers {r0 = 0, r1, r2, r3, r4, r5, r6, r7} Um_registers;

uint32_t get_register(Um_registers register_ID);
void put_register(Um_registers register_ID, uint32_t value);
