/*******************************************************************************/
//	Filename:		DPComponents.v
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

module ARU (in0, in1, out, ADD, MUL, Z, N, C, V);

	input  ADD, MUL;
	output Z, N, V;
	output reg C;
	input signed [15:0] in0, in1;
	output reg signed [15:0] out;

	always @(ADD, MUL, in0, in1) begin
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
module LGU (in0, in1, out, AND, NOT, SHF, Z, N);

	input AND, NOT;
	output Z, N;
	input [1:0] SHF;
	input signed [15:0] in0, in1;
	output reg signed [15:0] out;

	always @(AND, SHF, NOT, in0, in1) begin
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
module Register #(parameter N) (clk, rst, in, out, ld, clr);

	input clk, rst, ld, clr;
	input [N-1:0] in;
	output reg [N-1:0] out;
  
	always @(posedge clk, posedge rst) begin
		if (rst)
			out = 'b0;
		else if (clr)
			out = 'b0;
		else if (ld)
			out = in;
	end
  
endmodule

/*******************************************************************************/
module INC (in, inc_val, out);

	input  [15:0] in;
	input  [1:0]  inc_val;
	output [15:0] out;

	assign out = in + inc_val;

endmodule

/*******************************************************************************/
module IMM (in0, in1, out, conOF, SE12bits, SE4bits, LSB0E);
	
	input conOF, SE12bits, SE4bits,LSB0E;
	input [11:0] in0;
	input [3:0]  in1;
	output reg [15:0] out;

	always @(conOF, in0, in1, SE12bits, SE4bits, LSB0E) begin
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
module Mux4to1 #(parameter N) (in0, in1, in2, in3, sel0, sel1, sel2, sel3, out);

	input [N-1:0] in0, in1, in2, in3;
	input sel0, sel1, sel2, sel3;
	output reg [N-1:0] out;
  
	always @(in0, in1, in2, in3, sel0, sel1, sel2, sel3) begin
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
module Mux2to1 #(parameter N) (in0, in1, sel0, sel1, out);

	input [N-1:0] in0, in1;
	input sel0, sel1;
	output reg [N-1:0] out;
  
	always @(in0, in1, sel0, sel1) begin
		if (sel0)
			out = in0;
		else if (sel1)
			out = in1;
	end
	
endmodule

/*******************************************************************************/
module Tristate (in, out, oe);

	input oe;
	input  [15:0] in;
	output [15:0] out;

	assign out = oe ? in : 16'bz;

endmodule