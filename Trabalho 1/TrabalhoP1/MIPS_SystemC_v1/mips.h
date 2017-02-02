//mips.h

#ifndef MIPSMOD_H
#define MIPSMOD_H

/**
 *
 * MIPS module interface.
 */

#include <systemc.h>

#include "add.h"
#include "alu.h"
#include "ALUcontrol.h"
#include "control.h"
#include "decode.h"
#include "dmem.h"
#include "equality.h"
#include "ext.h"
#include "forward.h"
#include "gates.h"
#include "imem.h"
#include "interlock.h"
#include "mux2.h"
#include "mux4.h"
#include "registerP.h"
#include "regbank.h"
#include "shiftl2.h"

#include "registerT.h"
#include "register_IF_ID.h"
#include "register_ID_EX.h"
#include "register_EX_MEM.h"
#include "register_MEM_WB.h"

/**
 * MIPS module.
 * MIPS module is the main module of the MIPS simulator.
 * Instruction memory, register bank, ALU, data memory, etc are instatiated
 * and interconnected inside this module.
 *
 *   - input ports
 *   	- \c bool \c reset	- reset
 *   	- \c bool \c clk	- clock
 *
 * \image MIPS_datapathPipe4.png "architecture of mips" width=15cm
 */

SC_MODULE(mips) {

   // Ports
   sc_in < bool > clk;
   sc_in < bool > reset;

   // Modules
   // IF
   registerP *PCreg;             // PC register
   imem *instmem;                // instruction memory
   add *add4;                    // adds 4 to PC
   mux2< sc_uint<32> > *mPC;     // selects Next PC from Target or PC + 4

   //ID
   decode *decod;                // instruction decoder
   ext *e32;                     // sign extends imm to 32 bits
   shiftl2 *sl2;                 // shift left 2 imm signed extension
   add *addbr;                   // adds slft2sxtimm to PC +4
   regbank *rbank;               // register bank
   mux4< sc_uint<32> > *mreg1;   // selects register 1 contents for equality test
   mux4< sc_uint<32> > *mreg2;   // selects register 2 contents for equality test
   equality *eq;                 // equality test of register contents
   andgate *aeq;                 // beq instruction and equal value anding
   control *ctrl;                // main control
   forward *fwd;                 // forwarding unit
   interlock *intlock;           // interlocking unit
   mux2< bool > *mNOP0,          // selects NOP from normal instruction
                *mNOP1,
                *mNOP4,
                *mNOP5,
                *mNOP6,
                *mNOP7;
   mux2< sc_uint<2> > *mNOP23;

   //EXE
   alu *ALU;                     // ALU
   mux4< sc_uint<32> > *mop1;    // selects first ALU operand
   mux4< sc_uint<32> > *mop2;    // selects second ALU operand
   ALUcontrol *aluctrl;          // ALU control
   mux2< sc_uint<5> > *mDst;     // selects destination register

   //MEM
   dmem *datamem;                // data memory
   mux2< sc_uint<32> > *mmemin;  // selects data to be written into memory

   //WB
   mux2< sc_uint<32> > *mreg;    // selects data to be written into a register

   //pipeline registers
   register_IF_ID_t *reg_IF_ID;
   register_ID_EX_t *reg_ID_EX;
   register_EX_MEM_t *reg_EX_MEM;
   register_MEM_WB_t *reg_MEM_WB;

   // Signals

   // IF
   sc_signal < sc_uint<32> > PC,       // Program Counter
                             NPC,      // Next Program Counter
			     PC4;      // PC + 4
   sc_signal < sc_uint<32> > inst;     // current instruction

   //ID
   sc_signal < sc_uint<32> > inst_ID,  // current instruction (ID stage)
                             PC4_ID;   // current next program counter (ID stage)
   // instruction fields
   sc_signal < sc_uint<5> > rs, rt, rd;
   sc_signal < sc_uint<16> > imm;
   sc_signal < sc_uint<6> > opcode;
   sc_signal < sc_uint<5> > shamt;
   sc_signal < sc_uint<6> > funct;
   // generic signals
   sc_signal < sc_uint<32> > rsdata,   // contents of register rs
                             rtdata;   // contents of register rt
   sc_signal < sc_uint<32> > rsdataTest,   // contents of register rs for comparision
                             rtdataTest;   // contents of register rt for comparision
   sc_signal < sc_uint<32> > imm32;    // imm sign extended
   sc_signal < sc_uint<32> > offset;   // imm sign extended
   sc_signal < sc_uint<32> > Target;   // branch target address
   sc_signal <bool> PCSrc;             // selection between PC+4 and branch target address
   sc_signal <bool> equal;             // equality signal
   // control signals
   sc_signal <bool> MemRead, MemWrite, MemtoReg;
   sc_signal <bool> RegWrite, RegDst;
   sc_signal <bool> ALUSrc;
   sc_signal < sc_uint<2> > ALUOp;
   sc_signal <bool> MemRead_ID, MemWrite_ID, MemtoReg_ID;
   sc_signal <bool> RegWrite_ID, RegDst_ID;
   sc_signal <bool> ALUSrc_ID;
   sc_signal < sc_uint<2> > ALUOp_ID;
   sc_signal <bool> Branch;
   // forwarding signals
   sc_signal < sc_uint<2> > fwdIDA, fwdIDB, fwdEXA, fwdEXB;
   sc_signal <bool> fwdMEM;
   // interlocking signals
   sc_signal <bool> PCWrite, IFIDWrite, sel_NOP;
   // the following two signals are not used by the architecture
   // they are used only for visualization purposes
   sc_signal < sc_uint<32> > PC_ID;      // PC of instruction in ID
   sc_signal < bool > valid_ID;          // true if there is an instruction in ID

   //EX
   // instruction fields
   sc_signal < sc_uint<5> > shamt_EX;
   sc_signal < sc_uint<6> > funct_EX;
   // generic signals
   sc_signal < sc_uint<32> > rsdata_EX,   // contents of register rs
                             rtdata_EX;   // contents of register rt
   sc_signal < sc_uint<32> > imm_EX;      // imm sign extended
   sc_signal < sc_uint<5> > RegRs_EX,     // rs
                            RegRt_EX,     // rt
                            RegRd_EX;     // rd
   sc_signal < sc_uint<5> > rdest;        // destination register
   // ALU signals
   sc_signal < sc_uint<32> > ALUin1,   // ALU first operand
                             ALUin2,   // ALU second operand
                             ALUout;   // ALU output
   sc_signal < sc_uint<4> > op;        // ALU operation
   // control signals
   sc_signal <bool> MemRead_EX, MemWrite_EX, MemtoReg_EX;
   sc_signal <bool> RegWrite_EX, RegDst_EX;
   sc_signal <bool> ALUSrc_EX;
   sc_signal < sc_uint<2> > ALUOp_EX;
   // the following two signals are not used by the architecture
   // they are used only for visualization purposes
   sc_signal < sc_uint<32> > PC_EX;     // PC of instruction in ID
   sc_signal < bool > valid_EX;         // true if there is an instruction in ID

   //MEM
   // generic signals
   sc_signal < sc_uint<32> > rtdata_MEM;  // contents of register rt
   sc_signal < sc_uint<5> > rdest_MEM;    // destination register
   sc_signal < sc_uint<32> > destdata;    // data memory input
   sc_signal < sc_uint<32> > memdata;     // data memory output
   sc_signal < sc_uint<32> > ALUout_MEM;  // ALU output
   // control signals
   sc_signal < sc_uint<5> > WriteReg_MEM;
   sc_signal <bool> MemRead_MEM, MemWrite_MEM, MemtoReg_MEM;
   sc_signal <bool> RegWrite_MEM;
   // the following two signals are not used by the architecture
   // they are used only for visualization purposes
   sc_signal < sc_uint<32> > PC_MEM;
   sc_signal < bool > valid_MEM;

   //WB
   // generic signals
   sc_signal < sc_uint<5> > rdest_WB;     // destination register
   sc_signal < sc_uint<32> > memdata_WB;  // data memory output
   sc_signal < sc_uint<32> > ALUout_WB;   // ALU output
   sc_signal < sc_uint<32> > destdata_WB; // register contents to be written
   // control signals
   sc_signal < sc_uint<5> > WriteReg_wb;
   sc_signal <bool> MemtoReg_WB;
   sc_signal <bool> RegWrite_WB;
   // the following two signals are not used by the architecture
   // they are used only for visualization purposes
   sc_signal < sc_uint<32> > PC_WB;
   sc_signal < bool > valid_WB;

   //nonpipelined signals
   sc_signal < sc_uint<32> > const4;   // contant 4
   sc_signal < bool > const1;          // contant 1
   sc_signal < bool > const0;          // contant 0
   sc_signal < sc_uint<2> > const0_2;  // contant 0

   SC_CTOR(mips) {
       buildArchitecture();
   }

   ~mips(void);

   void buildArchitecture();

   void buildIF();
   void buildID();
   void buildEX();
   void buildMEM();
   void buildWB();
};

#endif
