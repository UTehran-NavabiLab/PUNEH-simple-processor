/*******************************************************************************/
//	Filename:		Datapath.v
//	Project:		PUNEH : Processor that is Useful and New for Education of Hardware
//  Version:		1.0
//	Created by: 	Mohammad Roeintan
//	Approved by:	Nooshin Nosrati
//	Date:			6 June 2021
//  Copyright (C) 2021 University of Teheran
//  This source file may be used and distributed without
//  restriction provided that this copyright statement is not
//  removed from the file and that any derivative work contains
//  the original copyright notice and the associated disclaimer.

//******************************************************************************/
//	File content description:
//	Datapath of the PUNEH processor                                 
/*******************************************************************************/

module Datapath (clk, rst, dataBus, AND, NOT, SHF, ADD, MUL, sel1_ARU,
		 selIMM_LGU, selMEM_LGU, ldAC, zeroAC, selIMM_AC, selMEM_AC,
		 selARU_AC, selLGU_AC, ldPC, selINC_PC, selMEM_PC, selIMM_PC,
		 ldIN, selINC_IN, selMEM_IN, conOF, SE12bits, SE4bits, ldIR,
		 ldOF, selPC_OF, selIMM_OF, selIN_ADR, selPC_ADR, selAC_MEM,
		 selIR_ADR, seldataBus, INC1, INC2, ldSR, selSET_SR, selLGU_SR,
		 selARU_SR, enSKP, addrBus, IRout, selMO_ARU, selIN_MEM, selPC_MEM, LSB0E);
	
	input clk, rst, AND, NOT, ADD, MUL, sel1_ARU, selMO_ARU, selIMM_LGU,
	      selMEM_LGU, ldAC, zeroAC, selIMM_AC, selMEM_AC,  selARU_AC,
	      selLGU_AC, ldPC, selINC_PC, selMEM_PC, selIMM_PC, ldIN,
	      selINC_IN, selMEM_IN, conOF, SE12bits, SE4bits, ldIR, ldOF,
	      selPC_OF, selIMM_OF, selIN_ADR, selPC_ADR, selAC_MEM, selIR_ADR,
	      seldataBus, INC1, INC2, selSET_SR, selLGU_SR, selARU_SR,selIN_MEM, selPC_MEM, LSB0E;
	inout  [15:0] dataBus;
	input  [1:0]  SHF;
	input  [3:0]  ldSR;
	output [15:0] addrBus, IRout;
	output enSKP;
		
	wire Z1, Z2, N1, N2, C, V;
	wire [1:0]  toINC;
	wire [3:0]  toOF, OFout, toSR, exp, obs;
	wire [11:0] w;
	wire [15:0] ARU_AC, LGU_AC, MEM_AC, IMM_AC, AC_MEM, MO, MO_ARU, toAC,
		    PC_MEM, toPC, toIN, IN_MEM, INC_PC, INC2_out, toBuff;

	// Arithmetic Units
	LGU LGU1 (AC_MEM, MO, LGU_AC, AND, NOT, SHF, Z1, N1);
	ARU ARU1 (AC_MEM, MO_ARU, ARU_AC, ADD, MUL, Z2, N2, C, V);

	// MUXes
	Mux2to1 #(16) mux1  (16'd1, MO, sel1_ARU, selMO_ARU, MO_ARU);
	Mux2to1 #(16) mux2  (IMM_AC, MEM_AC, selIMM_LGU, selMEM_LGU, MO);
	Mux4to1 #(16) mux3  (IMM_AC, MEM_AC, ARU_AC, LGU_AC, selIMM_AC,
			     selMEM_AC, selARU_AC, selLGU_AC, toAC);
	Mux2to1 #(4)  mux4  (PC_MEM[15:12], IMM_AC[3:0], selPC_OF, selIMM_OF, toOF);  
	Mux4to1 #(16) mux5  (INC_PC, MEM_AC, IMM_AC, 16'bz, selINC_PC,
			     selMEM_PC, selIMM_PC, 1'bz, toPC);
	Mux2to1 #(16) mux6  (INC2_out, MEM_AC, selINC_IN, selMEM_IN, toIN);
	Mux4to1 #(16) mux7  (IN_MEM, INC_PC, AC_MEM, 16'bz, selIN_MEM, 
			     selPC_MEM, selAC_MEM, 1'bz, toBuff);
	Mux4to1 #(16) mux8  (IN_MEM, IMM_AC, PC_MEM, 16'bz, selIN_ADR,
			     selIR_ADR, selPC_ADR, 1'bz, addrBus);
	Mux2to1 #(2)  mux9  (2'd2, 2'd1, INC2, INC1, toINC);
	Mux4to1 #(4)  mux10 (IRout[3:0], {Z1, N1, 2'bz}, {Z2, N2, C, V}, 4'bz, selSET_SR,
			     selLGU_SR, selARU_SR, 1'bz, toSR);

	// Registers
	Register #(16) AC (clk, rst, toAC, AC_MEM, ldAC, zeroAC);
	Register #(16) IR (clk, rst, MEM_AC, IRout, ldIR, 1'b0);
	Register #(16) PC (clk, rst, toPC, PC_MEM, ldPC, 1'b0);
	Register #(16) IN (clk, rst, toIN, IN_MEM, ldIN, 1'b0);
	Register #(4)  OF (clk, rst, toOF, OFout, ldOF, 1'b0);
	Register #(1)  SR_Z (clk, rst, toSR[3], w[3], ldSR[3], 1'b0);
	Register #(1)  SR_N (clk, rst, toSR[2], w[2], ldSR[2], 1'b0);
	Register #(1)  SR_C (clk, rst, toSR[1], w[1], ldSR[1], 1'b0);
	Register #(1)  SR_V (clk, rst, toSR[0], w[0], ldSR[0], 1'b0);

	// The Other Components
	IMM IMM1 (IRout[11:0], OFout, IMM_AC, conOF, SE12bits, SE4bits, LSB0E);
	INC INC_1 (PC_MEM, toINC, INC_PC);
	INC INC_2 (MEM_AC, 2'd1, INC2_out);
	Tristate TriBuff (toBuff, dataBus, seldataBus);

	// Flags
	assign exp = IRout[3:0];
	
	assign w[4] = w[0] ~^ exp[0];
	assign w[5] = w[1] ~^ exp[1];
	assign w[6] = w[2] ~^ exp[2];
	assign w[7] = w[3] ~^ exp[3];

	assign obs = IRout[7:4];

	assign w[8]  = w[4] & obs[0];
	assign w[9]  = w[5] & obs[1];
	assign w[10] = w[6] & obs[2];
	assign w[11] = w[7] & obs[3];

	assign enSKP = w[8] | w[9] | w[10] | w[11];

	assign MEM_AC = dataBus;
	
endmodule