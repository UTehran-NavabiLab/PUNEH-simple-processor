#include <systemc.h>
#include "channel.h"
#include "mem_controller.h"
#include "PUNEH.h"

#ifndef _MY_PUNEH_SYSTEM_
#define _MY_PUNEH_SYSTEM_

class my_system : public sc_module
{
public:
	
	sc_signal<sc_logic> clk, rst;

	PUNEH *PUNEH_MP;
	main_channel *CH;
	memory_controller *mem_ctrl;
	
	SC_CTOR(my_system)
	{
		CH = new main_channel;

		PUNEH_MP = new PUNEH("PUNEH_processor");
		PUNEH_MP->clk(clk);
		PUNEH_MP->rst(rst);
		PUNEH_MP->main_port(*CH);

		mem_ctrl = new memory_controller("mem_controller_of_PUNEH");
		mem_ctrl->clk(clk);
		mem_ctrl->rst(rst);
		mem_ctrl->main_port(*CH);

		SC_THREAD(clocking);
		SC_THREAD(reseting);
	}

	void clocking();
	void reseting();
	
};

#endif