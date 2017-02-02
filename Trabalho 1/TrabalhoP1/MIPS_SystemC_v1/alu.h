#ifndef ALUMOD_H
#define ALUMOD_H

/**
 *
 * ALU module interface.
 */

#include <systemc.h>

/**
 * ALU module.
 * ALU module models the integer ALU of the MIPS. It can perform 6 operations:
 * %add, subtract, %and, or, set on equal
 *   - input ports
 *   	- \c sc_uint<32> \c din1	- first operand
 *   	- \c sc_uint<32> \c din2	- second operand
 *   	- \c sc_uint<4>  \c op	- selects operation
 *   - output ports
 *   	- \c sc_unint<32> \c dout	- result
 */

SC_MODULE(alu) {

  sc_in< sc_uint<32> >  din1;
  sc_in< sc_uint<32> >  din2;
  sc_in< sc_uint<4> >  op;
  sc_in< sc_uint<5> >  shamt;

  sc_out< sc_uint<32> > dout;


  SC_CTOR(alu)
     {
      SC_METHOD(calc);
      sensitive << din1 << din2 << op << shamt;
    }

  void calc();
};

#endif
