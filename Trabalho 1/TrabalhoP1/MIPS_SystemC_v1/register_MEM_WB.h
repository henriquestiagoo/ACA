#ifndef REGISTER_MEM_WB_H
#define REGISTER_MEM_WB_H

/**
 *
 * register_MEM_WB_t module interface.
 */

#include <systemc.h>

#include "registerT.h"

/**
 * register_MEM_WB_t module.
 * register_MEM_WB_t module is the MEM/WB pipeline register.
 */

SC_MODULE(register_MEM_WB_t) {

	// Ports

	sc_in  < bool > clk;
	sc_in  < bool > reset;
	sc_in  < bool > enable;

	sc_in  < sc_uint<32> > ALUout_MEM, memdata_MEM;
	sc_out < sc_uint<32> > ALUout_WB, memdata_WB;

	sc_in  < sc_uint<5> > rdest_MEM;
	sc_out < sc_uint<5> > rdest_WB;

	sc_in  < bool > MemtoReg_MEM, RegWrite_MEM;
	sc_out < bool > MemtoReg_WB, RegWrite_WB;

	sc_in  < sc_uint<32> > PC_MEM;   // only for visualization purposes
	sc_out < sc_uint<32> > PC_WB;    // only for visualization purposes
	sc_in  < bool > valid_MEM;       // only for visualization purposes
	sc_out < bool > valid_WB;        // only for visualization purposes

	// Modules

	registerT < sc_uint<32> > *aluOut, *memOut;
	registerT < sc_uint<5> > *rdest;
	registerT < bool > *MemtoReg, *RegWrite;

	registerT < sc_uint<32> > *PC;        // only for visualization purposes
	registerT < bool > *valid;            // only for visualization purposes

	SC_CTOR(register_MEM_WB_t) {

		aluOut = new registerT < sc_uint<32> > ("aluOut");
		aluOut->din(ALUout_MEM);
		aluOut->dout(ALUout_WB);
		aluOut->clk(clk);
		aluOut->enable(enable);
		aluOut->reset(reset);

		memOut = new registerT < sc_uint<32> > ("memOut");
		memOut->din(memdata_MEM);
		memOut->dout(memdata_WB);
		memOut->clk(clk);
		memOut->enable(enable);
		memOut->reset(reset);

		rdest = new registerT < sc_uint<5> > ("rdest");
		rdest->din(rdest_MEM);
		rdest->dout(rdest_WB);
		rdest->clk(clk);
		rdest->enable(enable);
		rdest->reset(reset);

		MemtoReg = new registerT < bool >("MemtoReg");
		MemtoReg->din(MemtoReg_MEM);
		MemtoReg->dout(MemtoReg_WB);
		MemtoReg->clk(clk);
		MemtoReg->enable(enable);
		MemtoReg->reset(reset);

		RegWrite = new registerT < bool >("RegWrite");
		RegWrite->din(RegWrite_MEM);
		RegWrite->dout(RegWrite_WB);
		RegWrite->clk(clk);
		RegWrite->enable(enable);
		RegWrite->reset(reset);

		PC = new registerT < sc_uint<32> > ("PC");
		PC->din(PC_MEM);
		PC->dout(PC_WB);
		PC->clk(clk);
		PC->enable(enable);
		PC->reset(reset);

		valid = new registerT < bool > ("valid");
		valid->din(valid_MEM);
		valid->dout(valid_WB);
		valid->clk(clk);
		valid->enable(enable);
		valid->reset(reset);
	}
};

#endif
