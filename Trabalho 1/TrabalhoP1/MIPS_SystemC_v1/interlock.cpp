#include "interlock.h"

/**
 * Callback for the stall insertion of \c interlock module.
  */
void interlock::insert_stall()
{
  // alterado
  
  // pct
  if(opcode.read()== 0 && funct.read()== 49){

      if(clk.event() && clk==1)
      {
        count--;
      }

      if(count == 0){
        count = 4;
        enable_PC.write(1);
        enable_IFID.write(1);
        sel_NOP.write(0);
      }

      else{

        enable_PC.write(0);
        enable_IFID.write(0);
        sel_NOP.write(1);
      }
      
    }

	else if (MemRead_EX.read() && !MemWrite.read() &&
            ((RegDest_EX.read() == RegRs_ID.read()) ||
             ((RegDest_EX.read() == RegRt_ID.read()) && !MemRead.read())))
           { enable_PC.write(0);
             enable_IFID.write(0);
             sel_NOP.write(1);
           }
           else if (Branch.read() && MemRead_MEM.read() &&
                    ((RegDest_MEM.read() == RegRs_ID.read()) ||
                     (RegDest_MEM.read() == RegRt_ID.read())))
                   { enable_PC.write(0);
                     enable_IFID.write(0);
                     sel_NOP.write(1);
                   }
                   else { enable_PC.write(1);
                          enable_IFID.write(1);
                          sel_NOP.write(0);
                        }
}


 //clk.event() && clk==1 && enable==1