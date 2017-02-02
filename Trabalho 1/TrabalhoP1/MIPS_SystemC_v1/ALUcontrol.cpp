#include "ALUcontrol.h"

/**
 * \c ALUcontrol module callback function.
 */
void ALUcontrol::entry()
{
  switch(ALUOp.read()) {
    case 0: // lw - sw
            op.write(2);
	    break;
    case 2: // R - format
            switch(funct.read()) {
               case 32: // add
                        op.write(2);
                        break;
               case 34: //subtract
                        op.write(6);
                        break;
               case 36: // and
                        op.write(0);
                        break;
               case 37: // or
                        op.write(1);
                        break;
               case 42: // slt
                        op.write(7);
                        break;
               case 48: //lrl
                        op.write(8);
                        break;
               case 49: //pct
                        op.write(9);
                        break;
               default: // do nothing
                        op.write(15);
                        break;
            }
	    break;
    default:// do nothing
            op.write(15);
	    break;
    }
}
