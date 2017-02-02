#ifndef REGISTERPMOD_H
#define REGISTERPMOD_H

/**
 *
 * RegisterP module interface.
 */

#include <systemc.h>

/**
 * RegisterP module.
 * RegisterP module implements a 32 bit parallel register.
 * Synchronous on writes (positive edge-triggered), assinchronous on reset.
 *
 *   - input ports
 *   	- \c sc_uint<32> \c din		- input
 *   	- \c bool \c reset		- reset
 *   	- \c bool \c clk		- clock
 *   - output ports
 *   	- \c sc_uint<32> \c dout	- output
 */

SC_MODULE(registerP) {

  sc_in< sc_uint<32> >  din;
  sc_out< sc_uint<32> > dout;

  sc_in< bool > reset;
  sc_in< bool > clk;
  sc_in< bool > enable;

  sc_uint<32> val;

  SC_CTOR(registerP)
    {
      SC_METHOD(entry);
      sensitive << reset;
      sensitive_pos << clk;
      val=0;
    }

  void entry();
};

#endif
