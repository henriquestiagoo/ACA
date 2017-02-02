#ifndef REG_ID_EX_H
#define REG_ID_EX_H

/**
 *
 * register_ID_EX_t module interface.
 */

#include <systemc.h>
#include "registerT.h"

/**
 * register_ID_EX_t module.
 * register_ID_EX_t module is the ID/EXE pipeline register.
 */

SC_MODULE(register_ID_EX_t) {

	// Ports

	sc_in  < bool > clk;
	sc_in  < bool > reset;
	sc_in  < bool > enable;

	sc_in  < sc_uint<32> > rsData_ID, rtData_ID, imm_ID;
	sc_out < sc_uint<32> > rsData_EX, rtData_EX, imm_EX;

	sc_in  < sc_uint<5> > RegRs_ID, RegRt_ID, RegRd_ID;
	sc_out < sc_uint<5> > RegRs_EX, RegRt_EX, RegRd_EX;

	sc_in  < bool > MemRead_ID, MemWrite_ID, MemtoReg_ID, RegWrite_ID, RegDst_ID;
	sc_out < bool > MemRead_EX, MemWrite_EX, MemtoReg_EX, RegWrite_EX, RegDst_EX;

	sc_in  < bool > ALUSrc_ID;
	sc_out < bool > ALUSrc_EX;

	sc_in  < sc_uint<2> > ALUOp_ID;
	sc_out < sc_uint<2> > ALUOp_EX;

        sc_in  < sc_uint<6> > funct_ID;
        sc_out < sc_uint<6> > funct_EX;

        sc_in  < sc_uint<5> > shamt_ID;
        sc_out < sc_uint<5> > shamt_EX;

	sc_in  < sc_uint<32> > PC_ID;   // only for visualization purposes
	sc_out < sc_uint<32> > PC_EX;   // only for visualization purposes
	sc_in  < bool > valid_ID;       // only for visualization purposes
	sc_out < bool > valid_EX;       // only for visualization purposes

	// Modules

	registerT < sc_uint<32> > *rsData,*rtData,*imm;
        registerT < sc_uint<5> > *RegRs, *RegRt, *RegRd;
	registerT < bool > *MemRead, *MemWrite, *MemtoReg, *RegWrite, *RegDst, *ALUSrc;
	registerT < sc_uint<2> > *ALUOp;
        registerT < sc_uint<6> > *functR;
        registerT < sc_uint<5> > *shamtR;

	registerT < sc_uint<32> > *PC;      // only for visualization purposes
	registerT < bool > *valid;          // only for visualization purposes

	SC_CTOR(register_ID_EX_t) {

		rsData = new registerT < sc_uint<32> > ("rsData");
		rsData->din(rsData_ID);
		rsData->dout(rsData_EX);
		rsData->clk(clk);
		rsData->enable(enable);
		rsData->reset(reset);

                rtData = new registerT < sc_uint<32> >("rtData");
                rtData->din(rtData_ID);
                rtData->dout(rtData_EX);
                rtData->clk(clk);
                rtData->enable(enable);
                rtData->reset(reset);

                imm = new registerT < sc_uint<32> >("imm");
                imm->din(imm_ID);
                imm->dout(imm_EX);
                imm->clk(clk);
                imm->enable(enable);
                imm->reset(reset);

                RegRs = new registerT < sc_uint<5> >("RegRs");
                RegRs->din(RegRs_ID);
                RegRs->dout(RegRs_EX);
                RegRs->clk(clk);
                RegRs->enable(enable);
                RegRs->reset(reset);

                RegRt = new registerT < sc_uint<5> >("RegRt");
                RegRt->din(RegRt_ID);
                RegRt->dout(RegRt_EX);
                RegRt->clk(clk);
                RegRt->enable(enable);
                RegRt->reset(reset);

                RegRd = new registerT < sc_uint<5> >("RegRd");
                RegRd->din(RegRd_ID);
                RegRd->dout(RegRd_EX);
                RegRd->clk(clk);
                RegRd->enable(enable);
                RegRd->reset(reset);

		MemRead = new registerT < bool >("MemRead");
		MemRead->din(MemRead_ID);
		MemRead->dout(MemRead_EX);
		MemRead->clk(clk);
		MemRead->enable(enable);
		MemRead->reset(reset);

		MemWrite = new registerT < bool >("MemWrite");
		MemWrite->din(MemWrite_ID);
		MemWrite->dout(MemWrite_EX);
		MemWrite->clk(clk);
		MemWrite->enable(enable);
		MemWrite->reset(reset);

		MemtoReg = new registerT < bool >("MemtoReg");
		MemtoReg->din(MemtoReg_ID);
		MemtoReg->dout(MemtoReg_EX);
		MemtoReg->clk(clk);
		MemtoReg->enable(enable);
		MemtoReg->reset(reset);

		RegWrite = new registerT < bool >("RegWrite");
		RegWrite->din(RegWrite_ID);
		RegWrite->dout(RegWrite_EX);
		RegWrite->clk(clk);
		RegWrite->enable(enable);
		RegWrite->reset(reset);

		ALUSrc = new registerT < bool >("ALUSrc");
		ALUSrc->din(ALUSrc_ID);
		ALUSrc->dout(ALUSrc_EX);
		ALUSrc->clk(clk);
		ALUSrc->enable(enable);
		ALUSrc->reset(reset);

		ALUOp = new registerT < sc_uint<2> >("ALUOp");
		ALUOp->din(ALUOp_ID);
		ALUOp->dout(ALUOp_EX);
		ALUOp->clk(clk);
		ALUOp->enable(enable);
		ALUOp->reset(reset);

                RegDst = new registerT < bool >("RegDst");
                RegDst->din(RegDst_ID);
                RegDst->dout(RegDst_EX);
                RegDst->clk(clk);
                RegDst->enable(enable);
                RegDst->reset(reset);

                functR = new registerT < sc_uint<6> >("funct");
                functR->din(funct_ID);
                functR->dout(funct_EX);
                functR->clk(clk);
                functR->enable(enable);
                functR->reset(reset);

                shamtR = new registerT < sc_uint<5> >("shamt");
                shamtR->din(shamt_ID);
                shamtR->dout(shamt_EX);
                shamtR->clk(clk);
                shamtR->enable(enable);
                shamtR->reset(reset);

		PC = new registerT < sc_uint<32> >("PC");
		PC->din(PC_ID);
		PC->dout(PC_EX);
		PC->clk(clk);
		PC->enable(enable);
		PC->reset(reset);

		valid = new registerT < bool >("valid");
		valid->din(valid_ID);
		valid->dout(valid_EX);
		valid->clk(clk);
		valid->enable(enable);
		valid->reset(reset);
	}
};

#endif
