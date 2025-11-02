/*******************************************************************************/
//	Filename:		DPComponents.sv
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
//	Datapath components of the PUNEH processor,
//  consists of ARU, LGU, Register, INC, IMM, Mux4to1, Mux4to1, Tristate                             
/*******************************************************************************/

module ARU (

	input  logic signed [15:0] in0, in1,
	output logic signed [15:0] out,
	input  logic ADD, MUL,
	output logic Z, N, C, V
);
	always_comb begin
		C = 1'b0;
		out = 16'b0;

		if (ADD)
			{C, out} = in0 + in1;
		else if (MUL)
			out = in0[7:0] * in1[7:0];
	end
	
	assign Z = ~|out;
	assign N = out[15]; 
	assign V = (in0[15] & in1[15] & ~out[15]) || (~in0[15] & ~in1[15] & out[15]);

endmodule

/*******************************************************************************/
module LGU (
	input  logic signed [15:0] in0, in1,
	output logic signed [15:0] out,
	input  logic AND, NOT,
	input  logic [1:0] SHF,
	output logic Z, N
);
	always_comb begin
		out = 16'b0;

		if (AND == 1'b1)
			out = in0 & in1;
		else if (NOT == 1'b1)
			out = ~in0;
		else if (SHF == 2'b00)
			out = in0 >>> in1;
		else if (SHF == 2'b01)
			out = in0 >> in1;
		else if (SHF == 2'b10)
			out = in0 << in1;
	end

	assign Z = ~|out;
	assign N = out[15];

endmodule

/*******************************************************************************/
module Register #(parameter N) (
	input  logic clk, rst,
	input  logic [N-1:0] in,
	output logic [N-1:0] out,
	input  logic ld, clr
);
	always_ff @(posedge clk, posedge rst) begin
		if (rst)
			out <= 'b0;
		else if (clr)
			out <= 'b0;
		else if (ld)
			out <= in;
	end
  
endmodule

/*******************************************************************************/
module INC (
	input  logic [15:0] in,
	input  logic [1:0]  inc_val,
	output logic [15:0] out
);
	assign out = in + inc_val;

endmodule

/*******************************************************************************/
module IMM (
	input logic [11:0] in0,
	input logic [3:0]  in1,
	output logic [15:0] out,
	input logic conOF, SE12bits, SE4bits, LSB0E
);
	always_comb begin
		out = 16'b0;

		if (conOF)
			out = {in1, in0};
		else if(LSB0E)
			out = {in1,12'd0};
		else if (SE12bits)
			out = {{12{in0[3]}}, in0[3:0]};
		else if (SE4bits)
			out = {{4{in0[11]}}, in0};
	end
	
endmodule

/*******************************************************************************/
module Mux4to1 #(parameter N) (
	input  logic [N-1:0] in0, in1, in2, in3,
	input  logic sel0, sel1, sel2, sel3,
	output logic [N-1:0] out
);

	always_comb begin
		out = 16'b0;
		
		if (sel0)
			out = in0;
		else if (sel1)
			out = in1;
		else if (sel2)
			out = in2;
		else if (sel3)
			out = in3;
	end
	
endmodule

/*******************************************************************************/
module Mux2to1 #(parameter N) (
	input logic [N-1:0] in0, in1,
	input logic sel0, sel1,
	output logic [N-1:0] out
);

	always_comb begin
		out = 16'b0;
		
		if (sel0)
			out = in0;
		else if (sel1)
			out = in1;
	end
	
endmodule

/*******************************************************************************/
module Tristate (
	input  logic [15:0] in,
	output logic [15:0] out,
	input  logic oe
);
	assign out = oe ? in : 16'bz;

endmodule