#include "control.h"

/**
 * \c control module callback function.
 */
void control::entry()
{
  switch(opcode.read()) {
    case 0: // R-format
            Branch.write(0);
            RegDst.write(1);
            ALUSrc.write(0);
            ALUOp.write(2);
	    MemRead.write(0);
	    MemWrite.write(0);
            RegWrite.write(1);
            MemtoReg.write(0);
	    break;
    case  4: // beq
            Branch.write(1);
            RegDst.write(0);
            ALUSrc.write(0);
            ALUOp.write(1);
	    MemRead.write(0);
	    MemWrite.write(0);
            RegWrite.write(0);
            MemtoReg.write(0);
	    break;
    case 35: // lw
            Branch.write(0);
            RegDst.write(0);
            ALUSrc.write(1);
            ALUOp.write(0);
            MemRead.write(1);
            MemWrite.write(0);
            RegWrite.write(1);
            MemtoReg.write(1);
	    break;
    case 43: // sw
            Branch.write(0);
            RegDst.write(0);
            ALUSrc.write(1);
            ALUOp.write(0);
            MemRead.write(0);
            MemWrite.write(1);
            RegWrite.write(0);
            MemtoReg.write(0);
	    break;
   default: // Unknown opcode
            fprintf(stderr,"ERROR: Illegal opcode\n");
	    //assert(0);
	    break;
    }
}
