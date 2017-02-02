#include "regbank.h"

/**
 * instantiates the 32 registers and initializes values to 0.
 */
void regbank::init_regs()
{
    memory.clear();
    for(int i=0;i<32;i++) memory.push_back(0);
}

/**
 * Used for debugging.
 */
void regbank::dump()
{
    for(int i=0;i<16;i++) printf(" r%2d",i);
    printf("\n");
    for(int i=0;i<16;i++) printf("%4d",(int)memory[i]);
    printf("\n");
    for(int i=0;i<16;i++) printf(" r%2d",i+16);
    printf("\n");
    for(int i=0;i<16;i++) printf("%4d",(int)memory[i+16]);
    printf("\n");
}

/**
 * Callback for behaviour of \c register bank module.
 */
void regbank::regbank_access()
{
    if(reset.read()==true) {
	    init_regs();
    }

    if(clk.event() && clk.read()==1 && write.read()) {
        if(addin.read()<32 && addin.read()>0){
           memory[addin.read()]=din.read();
	}
    }

    if(add1.read()<32 && add1.read()>=0) {
       if(write.read() == false ||
          write.read() == true && addin.read()!=add1.read())
           dout1.write(memory[add1.read()]);
       else
           dout1.write(din.read());
    }

    if(add2.read()<32 && add2.read()>=0) {
       if(write.read() == false ||
          write.read() == true && addin.read()!=add2.read())
           dout2.write(memory[add2.read()]);
       else
           dout2.write(din.read());
    }
}
