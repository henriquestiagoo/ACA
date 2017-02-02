#ifndef REGBANKMOD_H
#define REGBANKMOD_H

/**
 *
 * Register bank module interface.
 */

#include <systemc.h>

#include "mem32.h"

/**
 * Register bank module.
 * Register bank module models the integer register bank of MIPS.
 * Synchronous on writes, assinchronous on reads.
 *
 *   - input ports
 *   	- \c sc_uint<5> \c add1		- identification of first register to be read
 *   	- \c sc_uint<5> \c add2		- identification of second register to be read
 *   	- \c sc_uint<5> \c addin	- identification of register to be written
 *   	- \c sc_uint<32> \c din  	- value to be written
 *   	- \c bool \c write		- write enable
 *   	- \c bool \c clk		- clock
 *   	- \c bool \c reset		- reset
 *   - output ports
 *   	- \c sc_uint<32> \c dout1	- value stored in first register
 *   	- \c sc_uint<32> \c dout2	- value stored in second register
 */

class regbank : public sc_module, public mem32
{
  public:
    sc_in< sc_uint<5> >  add1;
    sc_in< sc_uint<5> >  add2;
    sc_in< sc_uint<5> >  addin;
    sc_in< sc_uint<32> >  din;
    sc_in<bool> write;
    sc_in<bool> clk;
    sc_in<bool> reset;

    sc_out< sc_uint<32> >  dout1;
    sc_out< sc_uint<32> >  dout2;

    SC_CTOR(regbank)
     {
      SC_METHOD(regbank_access);
      sensitive << add1 << add2 << addin << din << write << reset;
      sensitive_pos(clk);

      init_regs();
      }

    void init_regs();
    void dump();
    void regbank_access();
};

#endif
