/*******************************************************************************/
//	Filename:		TopLevel.v
//	Project:		PUNEH : Processor that is Useful and New for Education of Hardware
//  Version:		1.0
//	Created by: 	Omid Mohammadi
//	Approved by:	Nooshin Nosrati
//	Date:			6 June 2021
//  Copyright (C) 2021 University of Teheran
//  This source file may be used and distributed without
//  restriction provided that this copyright statement is not
//  removed from the file and that any derivative work contains
//  the original copyright notice and the associated disclaimer.

//******************************************************************************/
//	File content description:
//	TopLevel of the PUNEH processor, 
//	cosists of the datapath and controller units                                
/*******************************************************************************/

module TopLevel (clk, rst, writeMEM, readMEM, dataBus, addrBus);

	input clk, rst;
	inout  [15:0] dataBus;
	output writeMEM, readMEM;
	output [15:0] addrBus;

	wire [15:0] inst;
	wire [3:0]  ldSR;
	wire [1:0]  SHF;
	wire enSKP, ldIR, ldOF, ldPC, ldIN, ldAC, zeroAC, seldataBus, selPC_OF,
	     selIMM_OF, selMEM_IN, selIMM_LGU, selMEM_LGU, sel1_ARU, selMO_ARU, 
	     selINC_PC, selMEM_PC, selIMM_PC, selIN_ADR, selIR_ADR, selPC_ADR,
	     selAC_MEM, selIMM_AC, selMEM_AC, selARU_AC, selLGU_AC, conOF,
	     SE12bits, SE4bits, AND, NOT, ADD, MUL,selSET_SR, selINC_IN,
	     INC1, INC2, selARU_SR, selLGU_SR,selIN_MEM, selPC_MEM, LSB0E;

	Datapath DP (clk, rst, dataBus, AND, NOT, SHF, ADD, MUL, sel1_ARU,
				selIMM_LGU, selMEM_LGU, ldAC, zeroAC, selIMM_AC, selMEM_AC,
				selARU_AC, selLGU_AC, ldPC, selINC_PC, selMEM_PC, selIMM_PC,
				ldIN, selINC_IN, selMEM_IN, conOF, SE12bits, SE4bits, ldIR,
				ldOF, selPC_OF, selIMM_OF, selIN_ADR, selPC_ADR, selAC_MEM,
				selIR_ADR, seldataBus, INC1, INC2, ldSR, selSET_SR, selLGU_SR,
				selARU_SR, enSKP, addrBus, inst, selMO_ARU,selIN_MEM, selPC_MEM, LSB0E);

	Controller CU (clk, rst, inst, enSKP, ldSR, ldIR, ldOF, ldPC, 
				ldIN, ldAC, zeroAC, seldataBus, selPC_OF, selIMM_OF, 
				selMEM_IN, selIMM_LGU, selMEM_LGU, sel1_ARU, selMO_ARU, 
				selINC_PC, selMEM_PC, selIMM_PC, selIN_ADR, selIR_ADR, 
				selPC_ADR, selAC_MEM, selIMM_AC, selMEM_AC, selARU_AC,
				selLGU_AC, conOF, SE12bits, SE4bits, AND, NOT, SHF, ADD,
				MUL, readMEM, writeMEM, selSET_SR, selINC_IN,
				INC1, INC2, selARU_SR, selLGU_SR,selIN_MEM, selPC_MEM, LSB0E);

endmodule