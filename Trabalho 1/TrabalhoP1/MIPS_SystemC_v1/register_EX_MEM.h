#ifndef REGISTER_EX_MEM_H
#define REGISTER_EX_MEM_H

/**
 *
 * register_EX_MEM_t module interface.
 */

#include <systemc.h>

#include "registerT.h"

/**
 * register_EX_MEM_t.
 * register_EX_MEM_t module is the EX/MEM pipeline register.
 */

SC_MODULE(register_EX_MEM_t) {

	// Ports

	sc_in  < bool > clk;
	sc_in  < bool > reset;
	sc_in  < bool > enable;

	sc_in  < sc_uint<32> > ALUout_EX, rtData_EX;
	sc_out < sc_uint<32> > ALUout_MEM, rtData_MEM;

	sc_in  < sc_uint<5> > rdest_EX;
	sc_out < sc_uint<5> > rdest_MEM;

	sc_in  < bool > MemRead_EX, MemWrite_EX, MemtoReg_EX, RegWrite_EX;
	sc_out < bool > MemRead_MEM, MemWrite_MEM, MemtoReg_MEM, RegWrite_MEM;

	sc_in  < sc_uint<32> > PC_EX;    // only for visualization purposes
	sc_out < sc_uint<32> > PC_MEM;   // only for visualization purposes
	sc_in  < bool > valid_EX;        // only for visualization purposes
	sc_out < bool > valid_MEM;       // only for visualization purposes

	// Modules

	registerT < sc_uint<32> > *aluOut, *rtData;
	registerT < sc_uint<5> >  *rdest;
	registerT < bool > *MemRead, *MemWrite, *MemtoReg, *RegWrite;

	registerT < sc_uint<32> > *PC;        // only for visualization purposes
	registerT < bool > *valid;            // only for visualization purposes

	SC_CTOR(register_EX_MEM_t) {

		aluOut = new registerT < sc_uint<32> > ("aluOut");
		aluOut->din(ALUout_EX);
		aluOut->dout(ALUout_MEM);
		aluOut->clk(clk);
		aluOut->enable(enable);
		aluOut->reset(reset);

		rtData = new registerT < sc_uint<32> > ("rtData");
		rtData->din(rtData_EX);
		rtData->dout(rtData_MEM);
		rtData->clk(clk);
		rtData->enable(enable);
		rtData->reset(reset);

		rdest = new registerT < sc_uint<5> > ("rdest");
		rdest->din(rdest_EX);
		rdest->dout(rdest_MEM);
		rdest->clk(clk);
		rdest->enable(enable);
		rdest->reset(reset);

		MemRead = new registerT < bool >("MemRead");
		MemRead->din(MemRead_EX);
		MemRead->dout(MemRead_MEM);
		MemRead->clk(clk);
		MemRead->enable(enable);
		MemRead->reset(reset);

		MemWrite = new registerT < bool >("MemWrite");
		MemWrite->din(MemWrite_EX);
		MemWrite->dout(MemWrite_MEM);
		MemWrite->clk(clk);
		MemWrite->enable(enable);
		MemWrite->reset(reset);

		MemtoReg = new registerT < bool >("MemtoReg");
		MemtoReg->din(MemtoReg_EX);
		MemtoReg->dout(MemtoReg_MEM);
		MemtoReg->clk(clk);
		MemtoReg->enable(enable);
		MemtoReg->reset(reset);

		RegWrite = new registerT < bool >("RegWrite");
		RegWrite->din(RegWrite_EX);
		RegWrite->dout(RegWrite_MEM);
		RegWrite->clk(clk);
		RegWrite->enable(enable);
		RegWrite->reset(reset);

		// Visualization only
		PC = new registerT < sc_uint<32> > ("PC");
		PC->din(PC_EX);
		PC->dout(PC_MEM);
		PC->clk(clk);
		PC->enable(enable);
		PC->reset(reset);

		valid = new registerT < bool > ("valid");
		valid->din(valid_EX);
		valid->dout(valid_MEM);
		valid->clk(clk);
		valid->enable(enable);
		valid->reset(reset);
	}
};

#endif
