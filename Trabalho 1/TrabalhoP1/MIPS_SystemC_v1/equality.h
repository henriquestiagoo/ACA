#ifndef EQUALITYMOD_H
#define EQUALITYMOD_H

/**
 *
 * Equality test module interface.
 */

#include <systemc.h>

/**
 * equality test module.
 * equality test module compares to 32-bit operands for equality.
 *   - input ports
 *   	- \c sc_uint<32> \c op1 - first operand
 *   	- \c sc_uint<32> \c op2 - second operand
 *   - output ports
 *      - \c bool \c result     - result
 */

SC_MODULE(equality) {

  sc_in< sc_uint<32> > op1, op2;

  sc_out< bool > result;

  SC_CTOR(equality)
     {
      SC_METHOD(entry);
      sensitive << op1 << op2;
    }

  void entry();
};

#endif
