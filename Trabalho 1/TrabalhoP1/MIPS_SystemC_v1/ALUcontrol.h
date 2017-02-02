#ifndef ALUCONTROLMOD_H
#define ALUCONTROLMOD_H

/**
 *
 * ALU control module interface.
 */

#include <systemc.h>

/**
 * ALU control module.
 * ALU control module models the ALU control unit of MIPS.
 *   - input ports
 *      - \c sc_uint<6> \c funct - instruction funct field
 *      - \c sc_uint<2> \c ALUOp - basic operation
 *   - output ports
 *   	- \c sc_uint<4> \c op	 - ALU operation
 */

SC_MODULE(ALUcontrol) {

  sc_in< sc_uint<6> >  funct;
  sc_in< sc_uint<2> >  ALUOp;

  sc_out< sc_uint<4> > op;

  SC_CTOR(ALUcontrol)
     {
      SC_METHOD(entry);
      sensitive << funct << ALUOp;
    }

  void entry();
};

#endif
