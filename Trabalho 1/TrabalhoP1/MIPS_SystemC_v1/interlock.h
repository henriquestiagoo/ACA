#ifndef INTERLOCKMOD_H
#define INTERLOCKMOD_H

/**
 *
 * interlock module interface.
 */

#include <systemc.h>

/**
 * interlock module.
 * interlock module is the stall insertion unit.
 *
 *   - input ports
 *      - \c bool \c Branch                - branch instruction
 *      - \c sc_uint<5> \c RegRs_ID        - register Rs (ID stage)
 *      - \c sc_uint<5> \c RegRt_ID        - register Rt (ID stage)
 *      - \c sc_uint<5> \c RegDest_EX      - destination register (EX stage)
 *      - \c sc_uint<5> \c RegDest_MEM     - destination register (MEM stage)
 *      - \c bool \c MemRead               - control signal identifying that a memory location is to be read (ID stage)
 *      - \c bool \c MemRead_EX            - control signal identifying that a memory location is to be read (EX stage)
 *      - \c bool \c MemRead_MEM           - control signal identifying that a memory location is to be read (MEM stage)
 *      - \c bool \c MemWrite              - control signal identifying that a memory location is to be written (ID stage)
 *   - output ports
 *   	- \c bool \c enable_PC             - enables update of PC register
 *   	- \c bool \c enable_IFID           - enables update of IF/ID pipeline register
 *      - \c bool \c sel_NOP               - selects NOP instruction
 */

SC_MODULE(interlock)
{
  public:
    sc_in< bool > Branch;
    sc_in< sc_uint<5> > RegRs_ID;
    sc_in< sc_uint<5> > RegRt_ID, RegDest_EX, RegDest_MEM;
    // alterado
    sc_in< sc_uint<6> > opcode;
    sc_in< sc_uint<6> > funct;
    sc_in< bool > clk;
    //
    sc_in< bool > MemRead, MemRead_EX, MemRead_MEM, MemWrite;
    sc_out< bool > enable_PC, enable_IFID, sel_NOP;

    sc_uint<5> count = 0;

    SC_CTOR(interlock)
    {
        SC_METHOD(insert_stall);
        sensitive << Branch
		  << RegRs_ID
                  << RegRt_ID << RegDest_EX << RegDest_MEM
                  << MemRead << MemRead_EX << MemRead_MEM << MemWrite
                  << opcode << funct;
        // clk
        sensitive_pos << clk;
   }

   void insert_stall();
};

#endif
