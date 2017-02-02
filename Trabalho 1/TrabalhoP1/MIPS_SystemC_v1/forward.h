#ifndef FORWARDMOD_H
#define FORWARDMOD_H

/**
 *
 * forwarding module interface.
 */

#include <systemc.h>

/**
 * forwarding module.
 * forwarding module solves some RAW exceptions.
 *
 *   - input ports
 *      - \c bool \c Branch                - branch instruction
 *      - \c sc_uint<5> \c RegRs_ID        - register Rs (ID stage)
 *      - \c sc_uint<5> \c RegRs_EX        - register Rs (EX stage)
 *   	- \c sc_uint<5> \c RegRt_ID	   - register Rt (ID stage)
 *      - \c sc_uint<5> \c RegRt_EX        - register Rt (EX stage)
 *      - \c sc_uint<5> \c RegDest_EX      - id of register to be written (EX stage)
 *      - \c sc_uint<5> \c RegDest_MEM     - id of register to be written (MEM stage)
 *   	- \c sc_uint<5> \c RegDest_WB	   - id of register to be written (WB stage)
 *      - \c bool \c ALUSrc_EX             - control signal identifying second ALU operand (EX stage)
 *      - \c bool \c RegWrite_EX           - control signal identifying that a register is to be written (EX stage)
 *      - \c bool \c RegWrite_MEM          - control signal identifying that a register is to be written (MEM stage)
 *      - \c bool \c RegWrite_WB           - control signal identifying that a register is to be written (WB stage)
 *      - \c bool \c MemWrite_MEM          - control signal identifying that a memory location is to be written (MEM stage)
 *   - output ports
 *      - \c sc_uint<2> \c fwdIDA          - selects input for first register operand (ID stage)
 *      - \c sc_uint<2> \c fwdIDB          - selects input for second register operand (ID stage)
 *      - \c sc_uint<2> \c fwdEXA          - selects input for first ALU operand (EX stage)
 *      - \c sc_uint<2> \c fwdEXB          - selects input for second ALU operand (EX stage)
 *   	- \c bool \c fwdMEM                - selects input for data to be written in memory (MEM stage)
 */

SC_MODULE(forward)
{
  public:
    sc_in< bool > Branch;
    sc_in< sc_uint<5> > RegRs_ID, RegRs_EX;
    sc_in< sc_uint<5> > RegRt_ID, RegRt_EX;
    sc_in< sc_uint<5> > RegDest_EX, RegDest_MEM, RegDest_WB;
    sc_in< bool > ALUSrc_EX;
    sc_in< bool > RegWrite_EX, RegWrite_MEM, RegWrite_WB;
    sc_in< bool > MemWrite_MEM;

    sc_out< sc_uint<2> > fwdIDA, fwdIDB, fwdEXA, fwdEXB;
    sc_out< bool > fwdMEM;

    SC_CTOR(forward)
    {
        SC_METHOD(forward_value);
        sensitive << RegRs_ID << RegRs_EX
                  << RegRt_ID << RegRt_EX
                  << RegDest_EX << RegDest_MEM << RegDest_WB
		  << Branch << ALUSrc_EX
		  << RegWrite_EX << RegWrite_MEM << RegWrite_WB
		  << MemWrite_MEM;
    }

    void forward_value();
};

#endif
