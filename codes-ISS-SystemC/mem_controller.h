#include <systemc.h>
#include "channel.h"

#ifndef _MY_MEMORY_CONTROLLER_H_
#define _MY_MEMORY_CONTROLLER_H_


class memory_controller : public sc_module
{
public:
	sc_port< sc_signal_in_if< sc_logic >, 1 > clk, rst;
	sc_port< slave_port > main_port;
	// a TLM channel will be constracted here soon

	sc_lv<16> mem[4096];
	transaction_info info;

	SC_CTOR(memory_controller)
	{
		SC_METHOD(initial);
		SC_THREAD(resetting); //sensitive << rst;
		SC_THREAD(responding);
	}

	void resetting();
	void responding();
	void initial();
};





#endif