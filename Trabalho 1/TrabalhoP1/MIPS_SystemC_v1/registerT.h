#ifndef REGISTERTMOD_H
#define REGISTERTMOD_H

/**
 *
 * registerT template interface.
 */

#include <systemc.h>

/**
 * registerT template.
 * registerT template implements a variable width parallel register.
 * The type of data is selected by the template class T.
 * Synchronous on writes (positive edge-triggered), assinchronous on reset.
 *
 *   - input ports
 *   	- \c T \c din		- input
 *   	- \c bool \c reset	- reset
 *   	- \c bool \c enable	- enable
 *   	- \c bool \c clk	- clock
 *   - output ports
 *   	- \c T \c dout		- output
 */

template <class T> class registerT : public sc_module
{
  public:

  sc_in< T >  din;
  sc_out< T > dout;

  sc_in< bool > reset;
  sc_in< bool > enable;
  sc_in< bool > clk;

  T val;

  SC_CTOR(registerT)
    {
      SC_METHOD(entry);
      sensitive_pos << clk;
      val=0;
    }

  void entry();
};


template <class T> void registerT<T>::entry()
{
    if(reset) {
       val=0;
       dout.write(val);
    }
    else if(clk.event() && clk==1 && enable==1) {
            val=din.read();
            dout.write(val);
    }
}

#endif
