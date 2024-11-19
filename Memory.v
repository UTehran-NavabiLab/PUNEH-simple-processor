/*******************************************************************************/
//	Filename:		Memory.v
//	Project:		PUNEH : Processor that is Useful and New for Education of Hardware
//  Version:		1.0
//	Created by: 	Hasan Sadeghzadeh
//	Approved by:	Nooshin Nosrati
//	Date:			6 June 2021
//  Copyright (C) 2021 University of Teheran
//  This source file may be used and distributed without
//  restriction provided that this copyright statement is not
//  removed from the file and that any derivative work contains
//  the original copyright notice and the associated disclaimer.

//******************************************************************************/
//	File content description:
//	Memory Unit with Async read, Async rst, Bidirectional dataBus                        
/*******************************************************************************/
 
// Load instructions memory
`define PROGRAM_START_ADDR 0
`define PROGRAM_END_ADDR 25
`define PROGRAM_FILE_LOCATION "fibonacci_program.txt"
`define LOAD_PROGRAM_FROM_FILE $readmemb(`PROGRAM_FILE_LOCATION, memData, `PROGRAM_START_ADDR, `PROGRAM_END_ADDR); 

// Load data memory
`define DATA_START_ADDR 26
`define DATA_END_ADDR 40
`define DATA_FILE_LOCATION "memData.txt"
`define LOAD_DATA_FROM_FILE $readmemb(`DATA_FILE_LOCATION, memData, `DATA_START_ADDR, `DATA_END_ADDR); 

module Memory (clk, rst, readMem, writeMem, addrBus, dataBus, readyMem);
	input clk, rst, readMem, writeMem;
	input [15:0] addrBus;
	inout [15:0] dataBus;
	output reg readyMem;
	
	reg [15:0] memData[0:2000];
	reg [15:0] outReg;
	integer i;
	
	// Memory Initialization
	initial begin
	for(i = 0; i < 65535; i = i+1)
		memData[i] = 16'b0; 
	`LOAD_PROGRAM_FROM_FILE
	`LOAD_DATA_FROM_FILE
	readyMem <= 1'b0;
	end
	
	always @(posedge clk, posedge rst) begin
		if (rst == 1'b1) begin
			for(i = 0; i < 65535; i = i+1)
				memData[i] = 16'b0; 
			`LOAD_PROGRAM_FROM_FILE
			`LOAD_DATA_FROM_FILE
			readyMem <= 1'b0;
		end
		else if (clk == 1'b1) begin
			if (writeMem == 1'b1) begin
				readyMem = 1'b0;
				memData[addrBus] = dataBus;
				readyMem = 1'b1;
			end
			/*
			else if(readMem == 1'b1) begin	// Sync read
				readyMem = 1'b0;
				outReg = memData[addrBus];
				readyMem = 1'b1;
			end
			*/
		end
		
	end
	
	always @(posedge readMem, addrBus) begin  // Async read
		if(readMem == 1'b1) begin
			readyMem = 1'b0;
			outReg = memData[addrBus];
			readyMem = 1'b1;
		end
	end
	
	assign dataBus = (readyMem == 1'b1 && writeMem == 1'b0) ? outReg : 16'bz;
	
endmodule   
