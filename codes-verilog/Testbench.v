/*******************************************************************************/
//	Filename:		Testbench.v
//	Project:		PUNEH : Processor that is Useful and New for Education of Hardware
//  Version:		1.0
//	Created by: 	Helia Hosseini, Negin Safari, Vesal bakhtazad, Neusha Javidnia
//	Approved by:	Nooshin Nosrati
//	Date:			6 June 2021
//  Copyright (C) 2021 University of Teheran
//  This source file may be used and distributed without
//  restriction provided that this copyright statement is not
//  removed from the file and that any derivative work contains
//  the original copyright notice and the associated disclaimer.

//******************************************************************************/
//	File content description:
//	Testbench of the PUNEH processor                            
/*******************************************************************************/

`timescale 1ns/1ns

module Testbench();

	reg  clk, rst;
	wire writeMEM, readMEM;
	wire [15:0] dataBus, addrBus;
	wire readyMem; 
	integer reportFile, i;
	
	TopLevel PUNEH (clk, rst, writeMEM, readMEM, dataBus, addrBus);
	Memory MEMORY  (clk, rst, readMEM, writeMEM, addrBus, dataBus, readyMem);

	initial begin
		reportFile = $fopen("Memory.rep", "w");
		rst = 1;
		clk = 0;
		#20 
		rst = 0;
		#30000 
		for(i = 0; i < 150; i = i+1)
			$fdisplay (reportFile, "%b", MEMORY.memData[i]);
		$stop;
	end
	
	always #40 clk = ~clk; 	
	
endmodule
