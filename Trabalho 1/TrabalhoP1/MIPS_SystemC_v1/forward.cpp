#include "forward.h"

/**
 * Callback for the forward_value of \c forwarding module.
 */
void forward::forward_value()
{
        //ID data hazards
        if (Branch.read() && RegWrite_EX.read() && (RegDest_EX.read() != 0) &&
            (RegDest_EX.read() == RegRs_ID.read()))
           fwdIDA.write(1);
           else if (Branch.read() && RegWrite_MEM.read() && (RegDest_MEM.read() != 0) &&
                    (RegDest_MEM.read() == RegRs_ID.read()))
                   fwdIDA.write(2);
                   else if (Branch.read() && RegWrite_WB.read() && (RegDest_WB.read() != 0) &&
                            (RegDest_WB.read() == RegRs_ID.read()))
                           fwdIDA.write(3);
                           else fwdIDA.write(0);
        if (Branch.read() && RegWrite_EX.read() && (RegDest_EX.read() != 0) &&
            (RegDest_EX.read() == RegRt_ID.read()))
           fwdIDB.write(1);
           else if (Branch.read() && RegWrite_MEM.read() && (RegDest_MEM.read() != 0) &&
                    (RegDest_MEM.read() == RegRt_ID.read()))
                   fwdIDB.write(2);
                   else if (Branch.read() && RegWrite_WB.read() && (RegDest_WB.read() != 0) &&
                            (RegDest_WB.read() == RegRs_ID.read()))
                           fwdIDB.write(3);
                           else fwdIDB.write(0);

        //EX data hazards
        if (RegWrite_MEM.read() && (RegDest_MEM.read() != 0) &&
            (RegDest_MEM.read() == RegRs_EX.read()))
           fwdEXA.write(2);
           else if (RegWrite_WB.read() && (RegDest_WB.read() != 0) &&
                    (RegDest_WB.read() == RegRs_EX.read()))
                   fwdEXA.write(3);
                   else fwdEXA.write(0);
        if (RegWrite_MEM.read() && (RegDest_MEM.read() != 0) &&
            (RegDest_MEM.read() == RegRt_EX.read()) && !ALUSrc_EX.read())
           fwdEXB.write(2);
           else if (RegWrite_WB.read() && (RegDest_WB.read() != 0) &&
                    (RegDest_WB.read() == RegRt_EX.read()) && !ALUSrc_EX.read())
                   fwdEXB.write(3);
                   else if (ALUSrc_EX.read())
                           fwdEXB.write(1);
                           else fwdEXB.write(0);

        //MEM data hazards
        if (MemWrite_MEM.read() && RegWrite_WB.read() &&
            (RegDest_MEM.read() == RegDest_WB.read()))
           fwdMEM.write(1);
           else fwdMEM.write(0);
}
