#include "registerP.h"

/**
 * \c registerP module callback function.
 */

void registerP::entry()
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
