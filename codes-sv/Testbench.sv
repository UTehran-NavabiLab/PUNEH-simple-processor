/*******************************************************************************/
//	Filename:		Testbench.sv
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

	logic clk, rst;
	logic writeMEM, readMEM;
	logic [15:0] addrBus;
	wire  [15:0] dataBus;
	logic readyMem; 

	integer reportFile, i;
	
	TopLevel PUNEH (.clk(clk), .rst(rst), .dataBus(dataBus), .writeMEM(writeMEM),
	                .readMEM(readMEM), .addrBus(addrBus));
	Memory MEMORY  (.clk(clk), .rst(rst), .readMEM(readMEM), .writeMEM(writeMEM),
	                .addrBus(addrBus), .dataBus(dataBus), .readyMem(readyMem));

	initial begin
		reportFile = $fopen("Memory.rep", "w");
		rst = 1'b1;
		clk = 1'b0;
		#17 
		rst = 1'b0;
		#30000 
		for(i = 0; i < 150; i = i+1)
			$fdisplay (reportFile, "%b", MEMORY.memData[i]);
		$stop;
	end
	
	always #40 clk = ~clk; 	
	
endmodule
