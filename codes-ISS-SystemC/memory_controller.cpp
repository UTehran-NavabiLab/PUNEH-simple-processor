#include "mem_controller.h"
#include "Utilities.h"

#define n 10
#define a0 47
#define a1 48
#define t 49
#define cntr 50
#define pointer 51

void memory_controller::resetting()
{
	// do nothing
	while (1)
	{
		wait(100+60*n, SC_NS);

		cout << "results:" << endl;

		for (int i = 0; i < n; i++)
			cout << "\t" << i<< ") " << mem[64 + i].to_int() << endl;
	}
}

void memory_controller::responding()
{
	while (1)
	{
		if (rst->read() == sc_logic_1)
		{
			//cout << "slave1 reseted at " << sc_time_stamp() << endl;
			wait(rst->default_event());
		}
		else
		{
			main_port->wait_for_call();
			main_port->get_info(info);

			if (rst->read() == sc_logic_1) continue;

			if (info.op == read)
				main_port->put(mem[info.address.to_int()]);
			else
				main_port->get(mem[info.address.to_int()]);
		}
	}
}

void memory_controller::initial()
{
	mem[0] = FA(LDm, -n+2);	// number of terms : n
	mem[1] = FA(STa, cntr);
	mem[2] = FA(LDm, 64);	// results will store in mem[64] to mem[64+abs(n)]
	mem[3] = FA(STa, pointer);
	mem[4] = FA(LDm, 0);
	mem[5] = FA(STa, a0);
	mem[6] = FA(STn, pointer);
	mem[7] = FA(INa, pointer);
	mem[8] = FA(LDm, 1);
	mem[9] = FA(STa, a1);
	mem[10] = FA(STn, pointer);
	mem[11] = FA(INa, pointer);
	// loop:
	mem[12] = FA(LDa, a0);
	mem[13] = FA(ADa, a1);
	mem[14] = FA(STa, t);
	mem[15] = FA(STn, pointer);
	mem[16] = NA(PAC);
	mem[17] = FA(LDa, a1);
	mem[18] = FA(STa, a0);
	mem[19] = FA(LDa, t);
	mem[20] = FA(STa, a1);
	mem[21] = FA(INa, pointer);
	mem[22] = FA(INa, cntr);
	mem[23] = NA(SKP, 0X8, 0X8); // sKP if z == 1
	mem[24] = FA(JMa, 12);
	mem[25] = NA(EXT);



	/*
	mem[0] = FA(LDm, 58);
	mem[1] = FA(MLa, 103);
	mem[2] = FA(ADm, -134);
	mem[3] = FA(ADa, 102);
	mem[4] = FA(ADn, 110);
	mem[5] = FA(JSR, 50);
	mem[6] = FA(JMa, 6);

	//cout << "mem[0] = " << int2hex(mem[0].to_int(), 4) << endl;
	//cout << "mem[1] = " << int2hex(mem[1].to_int(), 4) << endl;
	//cout << "mem[2] = " << int2hex(mem[2].to_int(), 4) << endl;
	//cout << "mem[3] = " << int2hex(mem[3].to_int(), 4) << endl;
	//cout << "mem[4] = " << int2hex(mem[4].to_int(), 4) << endl;
	//cout << "mem[5] = " << int2hex(mem[5].to_int(), 4) << endl;

	mem[50] = 0XDEAD;
	mem[51] = FA(STa, 99);
	mem[52] = FA(LDa, 105);
	mem[53] = FA(ADn, 115);
	mem[54] = NA(SET, 0XF, 0X5);
	mem[55] = FA(LDa, 99);
	mem[56] = FA(JMn, 50);


	mem[100] = -12;
	mem[101] = 58;
	mem[102] = 41;
	mem[103] = 39;
	mem[104] = 83;
	mem[105] = 0;
	mem[106] = 19;
	mem[107] = 74;
	mem[108] = 90;
	mem[109] = 28;
	mem[110] = 100;
	mem[111] = 101;
	mem[112] = 102;
	mem[113] = 103;
	mem[114] = 104;
	mem[115] = 105;
	mem[116] = 106;
	mem[117] = 107;
	mem[118] = 108;
	mem[119] = 109;
	*/




}

