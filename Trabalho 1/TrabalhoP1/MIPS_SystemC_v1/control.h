#ifndef CONTROLMOD_H
#define CONTROLMOD_H

/**
 *
 * Control module interface.
 */

#include <systemc.h>

/**
 * Control module.
 * Control module models the control unit of MIPS.
 *   - input ports
 *   	- \c sc_uint<6> \c opcode - instruction opcode field
 *   - output ports
 *      - \c bool \c Branch       - signal a beq instruction
 *   	- \c bool \c RegDst	  - selects if rd or rt is to be written
 *      - \c bool \c ALUSrc       - selects ALU second operand
 *      - \c sc_uint<2> \c ALUOp  - selects ALU basic operation
 *   	- \c bool \c MemRead	  - enables reading from memory
 *   	- \c bool \c MemWrite	  - enables writing into memory
 *   	- \c bool \c MemtoReg	  - selects whether memory or register contents are to be written into the destination register
 *      - \c bool \c RegWrite     - enables writing into the destination register
 */

SC_MODULE(control) {

  sc_in< sc_uint<6> >  opcode;
  sc_out< bool >  Branch;
  sc_out< bool >  RegDst;
  sc_out< bool >  ALUSrc;
  sc_out< sc_uint<2> >  ALUOp;
  sc_out< bool >  MemRead;
  sc_out< bool >  MemWrite;
  sc_out< bool >  MemtoReg;
  sc_out< bool >  RegWrite;

  SC_CTOR(control)
     {
      SC_METHOD(entry);
      sensitive << opcode;
    }

  void entry();
};

#endif
