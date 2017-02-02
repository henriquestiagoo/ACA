
// alu.cpp

#include "alu.h"

/**
 * \c alu module callback function.
 */
void alu::calc()
{
    sc_uint<32> a=din1.read();
    sc_uint<32> b=din2.read();
    sc_uint<5> sh = shamt.read();
    sc_int<32> asign;
    sc_int<32> bsign;

    asign=a;
    bsign=b;

    sc_uint<32> res;

    switch (op.read())
    {
       case 0: res = a & b;    // and
               break;
       case 1: res = a | b;    // or
               break;
       case 2: res = asign + bsign;    // add
               break;
       case 6: res = asign - bsign;    // subtract
               break;
       case 7: res = (asign < bsign);  // set on less than
               break;
       case 8: 
              res = ( b << sh ) | (b >> (32 - sh));
              break;
       case 9: 
              res =0;
              static int nibblebits[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

              while(b != 0){
                res += nibblebits[ b & 0x0F]; // %16
                b = b >> 4;
              }

              break;
       default: res = 0;       // non implemented operation
               break;
    }

    dout.write(res);
}
