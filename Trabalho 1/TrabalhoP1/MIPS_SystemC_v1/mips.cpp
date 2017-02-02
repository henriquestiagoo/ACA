//mips.cpp

/**
 *
 * MIPS module implementation.
 */

#include "mips.h"

/**
 * buils IF stage components
 */
void mips::buildIF(void)
{
      // Program Counter
      PCreg = new registerP ("PCregister");
      PCreg->din(NPC);
      PCreg->dout(PC);
      PCreg->clk(clk);
      PCreg->reset(reset);
      PCreg->enable(PCWrite);

      // Instruction Memory
      instmem = new imem ("imem");
      instmem->addr(PC);
      instmem->inst(inst);

      // Adds 4 to Program Counter
      add4 = new add ("add4");
      add4->op1(PC);
      add4->op2(const4);
      add4->res(PC4);

      // Selects Next Program Counter Value
      mPC = new mux2< sc_uint<32> > ("mPC");
      mPC->sel(PCSrc);
      mPC->din0(PC4);
      mPC->din1(Target);
      mPC->dout(NPC);
}

/**
 * buils ID stage components
 */
void mips::buildID(void)
{
      // Instruction decoding
      decod = new decode("decoder");
      decod->inst(inst_ID);
      decod->rs(rs);
      decod->rt(rt);
      decod->rd(rd);
      decod->imm(imm);
      decod->opcode(opcode);
      decod->shamt(shamt);
      decod->funct(funct);

      // 16 to 32 bit signed extension of immediate
      e32 = new ext("ext");
      e32->din(imm);
      e32->dout(imm32);

      // shift left 2 imm_ext
      sl2 = new shiftl2("sl2");
      sl2->din(imm32);
      sl2->dout(offset);

      // Adds Branch Immediate to Program Counter + 4
      addbr = new add ("addbr");
      addbr->op1(PC4_ID);
      addbr->op2(offset);
      addbr->res(Target);

      // Register Bank
      rbank = new regbank ("regbank");
      rbank->add1(rs);
      rbank->add2(rt);
      rbank->dout1(rsdata);
      rbank->dout2(rtdata);
      rbank->addin(rdest_WB);
      rbank->din(destdata_WB);
      rbank->write(RegWrite_WB);
      rbank->clk(clk);
      rbank->reset(reset);

      // Selects register 1 contents for equality test
      mreg1 = new mux4< sc_uint<32> > ("mreg1");
      mreg1->sel(fwdIDA);
      mreg1->din0(rsdata);
      mreg1->din1(ALUout);
      mreg1->din2(ALUout_MEM);
      mreg1->din3(destdata_WB);
      mreg1->dout(rsdataTest);

      // Selects register 2 contents for equality test
      mreg2 = new mux4< sc_uint<32> > ("mreg2");
      mreg2->sel(fwdIDB);
      mreg2->din0(rtdata);
      mreg2->din1(ALUout);
      mreg2->din2(ALUout_MEM);
      mreg2->din3(destdata_WB);
      mreg2->dout(rtdataTest);

      // Test for equality register contents
      eq = new equality ("eq");
      eq->op1(rsdataTest);
      eq->op2(rtdataTest);
      eq->result(equal);

      // Enables Branch
      aeq = new andgate ("aeq");
      aeq->din1(Branch);
      aeq->din2(equal);
      aeq->dout(PCSrc);

      // Main Control
      ctrl = new control ("control");
      ctrl->opcode(opcode);
      ctrl->Branch(Branch);
      ctrl->RegDst(RegDst);
      ctrl->ALUSrc(ALUSrc);
      ctrl->ALUOp(ALUOp);
      ctrl->MemRead(MemRead);
      ctrl->MemWrite(MemWrite);
      ctrl->MemtoReg(MemtoReg);
      ctrl->RegWrite(RegWrite);

      // Forwarding Unit
      fwd = new forward ("forward");
      fwd->Branch(Branch);
      fwd->RegRs_ID(rs);
      fwd->RegRs_EX(RegRs_EX);
      fwd->RegRt_ID(rt);
      fwd->RegRt_EX(RegRt_EX);
      fwd->RegDest_EX(rdest);
      fwd->RegDest_MEM(rdest_MEM);
      fwd->RegDest_WB(rdest_WB);
      fwd->ALUSrc_EX(ALUSrc_EX);
      fwd->RegWrite_EX(RegWrite_EX);
      fwd->RegWrite_MEM(RegWrite_MEM);
      fwd->RegWrite_WB(RegWrite_WB);
      fwd->MemWrite_MEM(MemWrite_MEM);
      fwd->fwdIDA(fwdIDA);
      fwd->fwdIDB(fwdIDB);
      fwd->fwdEXA(fwdEXA);
      fwd->fwdEXB(fwdEXB);
      fwd->fwdMEM(fwdMEM);

      // Interlocking Unit
      intlock = new interlock ("interlock");
      intlock->Branch(Branch);
      intlock->RegRs_ID(rs);
      intlock->RegRt_ID(rt);
      intlock->RegDest_EX(rdest);
      intlock->RegDest_MEM(rdest_MEM);
      intlock->MemRead(MemRead);
      intlock->MemRead_EX(MemRead_EX);
      intlock->MemRead_MEM(MemRead_MEM);
      intlock->MemWrite(MemWrite);
      intlock->enable_PC(PCWrite);
      intlock->enable_IFID(IFIDWrite);
      intlock->sel_NOP(sel_NOP);
      // alterado
      intlock->opcode(opcode);
      intlock->funct(funct);
      intlock->clk(clk);


      // Selects NOP instruction
      mNOP0 = new mux2< bool > ("mNOP0");
      mNOP0->sel(sel_NOP);
      mNOP0->din0(RegDst);
      mNOP0->din1(const0);
      mNOP0->dout(RegDst_ID);
      mNOP1 = new mux2< bool > ("mNOP1");
      mNOP1->sel(sel_NOP);
      mNOP1->din0(ALUSrc);
      mNOP1->din1(const0);
      mNOP1->dout(ALUSrc_ID);
      mNOP23 = new mux2< sc_uint<2> > ("mNOP23");
      mNOP23->sel(sel_NOP);
      mNOP23->din0(ALUOp);
      mNOP23->din1(const0_2);
      mNOP23->dout(ALUOp_ID);
      mNOP4 = new mux2< bool > ("mNOP4");
      mNOP4->sel(sel_NOP);
      mNOP4->din0(MemRead);
      mNOP4->din1(const0);
      mNOP4->dout(MemRead_ID);
      mNOP5 = new mux2< bool > ("mNOP5");
      mNOP5->sel(sel_NOP);
      mNOP5->din0(MemWrite);
      mNOP5->din1(const0);
      mNOP5->dout(MemWrite_ID);
      mNOP6 = new mux2< bool > ("mNOP6");
      mNOP6->sel(sel_NOP);
      mNOP6->din0(MemtoReg);
      mNOP6->din1(const0);
      mNOP6->dout(MemtoReg_ID);
      mNOP7 = new mux2< bool > ("mNOP7");
      mNOP7->sel(sel_NOP);
      mNOP7->din0(RegWrite);
      mNOP7->din1(const0);
      mNOP7->dout(RegWrite_ID);
}

/**
 * buils EX stage components
 */
void mips::buildEX(void)
{
      // Selects first operand of ALU
      mop1 = new mux4< sc_uint<32> > ("mOp1");
      mop1->sel(fwdEXA);
      mop1->din0(rsdata_EX);
      mop1->din1(rsdata_EX);
      mop1->din2(ALUout_MEM);
      mop1->din3(destdata_WB);
      mop1->dout(ALUin1);

      // Selects second operand of ALU
      mop2 = new mux4< sc_uint<32> > ("mOp2");
      mop2->sel(fwdEXB);
      mop2->din0(rtdata_EX);
      mop2->din1(imm_EX);
      mop2->din2(ALUout_MEM);
      mop2->din3(destdata_WB);
      mop2->dout(ALUin2);

      // ALU
      ALU = new alu("alu");
      ALU->din1(ALUin1);
      ALU->din2(ALUin2);
      ALU->op(op);
      ALU->dout(ALUout);
      ALU->shamt(shamt_EX);

      // ALU Control
      aluctrl = new ALUcontrol("ALUcontrol");
      aluctrl->funct(funct_EX);
      aluctrl->ALUOp(ALUOp_EX);
      aluctrl->op(op);

      // Selects destination register
      mDst = new mux2< sc_uint<5> > ("mDst");
      mDst->sel(RegDst_EX);
      mDst->din0(RegRt_EX);
      mDst->din1(RegRd_EX);
      mDst->dout(rdest);

}

/**
 * buils MEM stage components
 */
void mips::buildMEM(void)
{
      // Data Memory
      datamem = new dmem ("datamem");
      datamem->addr(ALUout_MEM);
      datamem->din(destdata);
      datamem->dout(memdata);
      datamem->wr(MemWrite_MEM);
      datamem->rd(MemRead_MEM);
      datamem->clk(clk);

      // Selects memory data input
      mmemin = new mux2< sc_uint<32> > ("mmemin");
      mmemin->sel(fwdMEM);
      mmemin->din0(rtdata_MEM);
      mmemin->din1(destdata_WB);
      mmemin->dout(destdata);
}

/**
 * buils WB stage components
 */
void mips::buildWB(void)
{
      // Selects contents to be written in a register
      mreg = new mux2< sc_uint<32> > ("mreg");
      mreg->sel(MemtoReg_WB);
      mreg->din0(ALUout_WB);
      mreg->din1(memdata_WB);
      mreg->dout(destdata_WB);
}

/**
 * Instantiates the pipeline registers and calls other functions to
 * buils stage specific components
 */
void mips::buildArchitecture(void){

      const4 = 4;
      const1 = 1;
      const0 = 0;
      const0_2 = 0;

      buildIF();

      //register_IF_ID
      reg_IF_ID = new register_IF_ID_t("reg_IF_ID");
      reg_IF_ID->inst_IF(inst);
      reg_IF_ID->inst_ID(inst_ID);
      reg_IF_ID->PC4_IF(PC4);
      reg_IF_ID->PC4_ID(PC4_ID);
      reg_IF_ID->PC_IF(PC);
      reg_IF_ID->PC_ID(PC_ID);
      reg_IF_ID->valid_IF(const1);
      reg_IF_ID->valid_ID(valid_ID);
      reg_IF_ID->clk(clk);
      reg_IF_ID->reset(reset);
      reg_IF_ID->enable(IFIDWrite);


      buildID();

      //register_ID_EX
      reg_ID_EX = new register_ID_EX_t("reg_ID_EX");
      reg_ID_EX->rsData_ID(rsdata);
      reg_ID_EX->rsData_EX(rsdata_EX);
      reg_ID_EX->rtData_ID(rtdata);
      reg_ID_EX->rtData_EX(rtdata_EX);
      reg_ID_EX->imm_ID(imm32);
      reg_ID_EX->imm_EX(imm_EX);
      reg_ID_EX->RegRs_ID(rs);
      reg_ID_EX->RegRs_EX(RegRs_EX);
      reg_ID_EX->RegRt_ID(rt);
      reg_ID_EX->RegRt_EX(RegRt_EX);
      reg_ID_EX->RegRd_ID(rd);
      reg_ID_EX->RegRd_EX(RegRd_EX);
      reg_ID_EX->MemRead_ID(MemRead_ID);
      reg_ID_EX->MemRead_EX(MemRead_EX);
      reg_ID_EX->MemWrite_ID(MemWrite_ID);
      reg_ID_EX->MemWrite_EX(MemWrite_EX);
      reg_ID_EX->MemtoReg_ID(MemtoReg_ID);
      reg_ID_EX->MemtoReg_EX(MemtoReg_EX);
      reg_ID_EX->RegWrite_ID(RegWrite_ID);
      reg_ID_EX->RegWrite_EX(RegWrite_EX);
      reg_ID_EX->ALUSrc_ID(ALUSrc_ID);
      reg_ID_EX->ALUSrc_EX(ALUSrc_EX);
      reg_ID_EX->ALUOp_ID(ALUOp_ID);
      reg_ID_EX->ALUOp_EX(ALUOp_EX);
      reg_ID_EX->RegDst_ID(RegDst_ID);
      reg_ID_EX->RegDst_EX(RegDst_EX);
      reg_ID_EX->funct_ID(funct);
      reg_ID_EX->funct_EX(funct_EX);
      reg_ID_EX->shamt_ID(shamt);
      reg_ID_EX->shamt_EX(shamt_EX);
      reg_ID_EX->PC_ID(PC_ID);
      reg_ID_EX->PC_EX(PC_EX);
      reg_ID_EX->valid_ID(valid_ID);
      reg_ID_EX->valid_EX(valid_EX);
      reg_ID_EX->clk(clk);
      reg_ID_EX->reset(reset);
      reg_ID_EX->enable(const1);

      buildEX();

      //register_EX_MEM
      reg_EX_MEM = new register_EX_MEM_t("reg_EX_MEM");
      reg_EX_MEM->ALUout_EX(ALUout);
      reg_EX_MEM->ALUout_MEM(ALUout_MEM);
      reg_EX_MEM->rtData_EX(rtdata_EX);
      reg_EX_MEM->rtData_MEM(rtdata_MEM);
      reg_EX_MEM->rdest_EX(rdest);
      reg_EX_MEM->rdest_MEM(rdest_MEM);
      reg_EX_MEM->MemRead_EX(MemRead_EX);
      reg_EX_MEM->MemRead_MEM(MemRead_MEM);
      reg_EX_MEM->MemWrite_EX(MemWrite_EX);
      reg_EX_MEM->MemWrite_MEM(MemWrite_MEM);
      reg_EX_MEM->MemtoReg_EX(MemtoReg_EX);
      reg_EX_MEM->MemtoReg_MEM(MemtoReg_MEM);
      reg_EX_MEM->RegWrite_EX(RegWrite_EX);
      reg_EX_MEM->RegWrite_MEM(RegWrite_MEM);
      reg_EX_MEM->PC_EX(PC_EX);
      reg_EX_MEM->PC_MEM(PC_MEM);
      reg_EX_MEM->valid_EX(valid_EX);
      reg_EX_MEM->valid_MEM(valid_MEM);
      reg_EX_MEM->clk(clk);
      reg_EX_MEM->reset(reset);
      reg_EX_MEM->enable(const1);

      buildMEM();

      //register_mem_wb
      reg_MEM_WB = new register_MEM_WB_t("reg_MEM_WB");
      reg_MEM_WB->ALUout_MEM(ALUout_MEM);
      reg_MEM_WB->ALUout_WB(ALUout_WB);
      reg_MEM_WB->memdata_MEM(memdata);
      reg_MEM_WB->memdata_WB(memdata_WB);
      reg_MEM_WB->rdest_MEM(rdest_MEM);
      reg_MEM_WB->rdest_WB(rdest_WB);
      reg_MEM_WB->MemtoReg_MEM(MemtoReg_MEM);
      reg_MEM_WB->MemtoReg_WB(MemtoReg_WB);
      reg_MEM_WB->RegWrite_MEM(RegWrite_MEM);
      reg_MEM_WB->RegWrite_WB(RegWrite_WB);
      reg_MEM_WB->PC_MEM(PC_MEM);
      reg_MEM_WB->PC_WB(PC_WB);
      reg_MEM_WB->valid_MEM(valid_MEM);
      reg_MEM_WB->valid_WB(valid_WB);
      reg_MEM_WB->clk(clk);
      reg_MEM_WB->reset(reset);
      reg_MEM_WB->enable(const1);

      buildWB();
   }

mips::~mips(void)
{
      delete PCreg;
      delete instmem;
      delete add4;
      delete mPC;

      delete e32;
      delete sl2;
      delete addbr;
      delete rbank;
      delete mreg1;
      delete mreg2;
      delete eq;
      delete aeq;
      delete ctrl;
      delete fwd;
      delete intlock;
      delete mNOP0;
      delete mNOP1;
      delete mNOP23;
      delete mNOP4;
      delete mNOP5;
      delete mNOP6;
      delete mNOP7;

      delete mop1;
      delete mop2;
      delete ALU;
      delete aluctrl;
      delete mDst;

      delete datamem;
      delete mmemin;

      delete mreg;
}
