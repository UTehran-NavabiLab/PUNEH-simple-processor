#include "modules_TB.h"

void CCE1_TB::clocking() {
	int i;
	clk.write(sc_logic_0);
	for(i=0;i<=400;i++) {
		clk.write(sc_logic_0);
		wait(5,SC_NS);
		clk.write(sc_logic_1);
		wait(5,SC_NS);
    }
	wait();
}

void CCE1_TB::displaying() {
	cout << "At: " << sc_time_stamp() << " data_in is: " << data_in.read() << "   data out is: "<<data_out.read()<<'\n';
}

void CCE1_TB::starting()
{
	start.write(sc_logic_0);
	wait(50, SC_NS);
	while (true)
	{
		data_in.write("11001001");
		start.write(sc_logic_1);
		wait();
		wait();
		start.write(sc_logic_0);
		while (true)
		{
			wait();
			if (done.read() == '1')
				break;
		}
		wait(20, SC_NS);
	}
}

void CCE1_TB::reseting()
{
	rst.write(SC_LOGIC_1);
	wait(50, SC_NS);
	rst.write(SC_LOGIC_0);
}

void CCE4_TB::clocking() {
	int i;
	clk.write(sc_logic_0);
	for (i = 0; i <= 10000; i++) {
		clk.write(sc_logic_0);
		wait(5, SC_NS);
		clk.write(sc_logic_1);
		wait(5, SC_NS);
	}
	wait();
}

void CCE4_TB::displaying() {
	cout << "At: " << sc_time_stamp() << " data_in is: " << data_in.read() << "   data out is: " << data_out.read() << "   sign is:" << sign.read() << '\n';
}

void CCE4_TB::starting()
{
	int i = 0;
	CCE4start.write(sc_logic_0);
	wait(100, SC_NS);
	for (i = 0 ; i<=800; i=i+20)
	{
		data_in.write(i%1024);
		CCE4start.write(sc_logic_1);
		wait();
		wait();
		CCE4start.write(sc_logic_0);
		while (true)
		{
			wait();
			wait();
			if (CCE4ready.read() == '1')
				break;
		}
		
		wait(100, SC_NS);
	}
}

void CCE4_TB::reseting()
{
	rst.write(SC_LOGIC_1);
	wait(50, SC_NS);
	rst.write(SC_LOGIC_0);
}