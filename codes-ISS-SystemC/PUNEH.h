#include <systemc.h>
#include "Utilities.h"
#include "channel.h"


#ifndef _MY_PUNEH_H_
#define _MY_PUNEH_H_


class PUNEH : public sc_module
{
public:
	sc_port< sc_signal_in_if< sc_logic >, 1 > clk, rst;
	sc_port< master_port > main_port;

	PUNEH_register PC, IR, AR, AC, OF;

	Flags flag;

	const int PUNEH_priority = 0;

	PUNEH_inst inst_info;

	ALU_info alu_info;

	int inst_cntr;

	sc_event go;

	printing_info pinf;

	SC_CTOR(PUNEH)
	{
		SC_THREAD(initial);
		SC_THREAD(operating);
		SC_METHOD(main_port_reseting); sensitive << rst;
		SC_METHOD(go_generator); sensitive << rst << clk;
	}
	
	void initial();
	void main_port_reseting();
	void operating();
	void go_generator();

	void fetch();
	void reseting();
	void instruction_decode();
	void indirect_operand();
	bool execute();
	void do_read(PUNEH_register &dest, int address);				//	do_read(destination_reg, mem_address)
	void do_read(PUNEH_register &dest, PUNEH_register address);		//	do_read(destination_reg, address_reg)
	int  do_read(int address);										//	just read the address
	void do_write(int address, PUNEH_register source);				//	do_write(mem_address, source_register)
	void do_write(PUNEH_register address, PUNEH_register source);	//	do_write(address_reg, source_register)
	void ALU_op();

	bool check_flags();

	void update_ZN_flag();
	void update_CV_flag(update_CV_flag_type type);

	void printing();
};







#endif