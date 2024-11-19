#include "Utilities.h"

void Flags::get_value(sc_lv<4> &fval)
{//Z,N,C,V
	fval.range(3, 3) = Z;
	fval.range(2, 2) = N;
	fval.range(1, 1) = C;
	fval.range(0, 0) = V;
}

int Flags::get_value()
{
	return 8 * Z.to_uint() + 4 * N.to_uint() + 2 * C.to_uint() + V.to_uint();
}

void Flags::put_value(sc_lv<4> fval)
{
	Z = fval.range(3, 3);
	N = fval.range(2, 2);
	C = fval.range(1, 1);
	V = fval.range(0, 0);
}

void Flags::initial()
{
	Z = (sc_lv<1>)SC_LOGIC_X;
	N = (sc_lv<1>)SC_LOGIC_X;
	C = (sc_lv<1>)SC_LOGIC_X;
	V = (sc_lv<1>)SC_LOGIC_X;
}

void Flags::reset()
{
	Z = (sc_lv<1>)SC_LOGIC_0;
	N = (sc_lv<1>)SC_LOGIC_0;
	C = (sc_lv<1>)SC_LOGIC_0;
	V = (sc_lv<1>)SC_LOGIC_0;
}

void PUNEH_register::reset()
{
	val = 0;
}

void PUNEH_register::put_value(sc_lv<16> data)
{
	val = data.to_int() % max_val;
}

void PUNEH_register::put_value(int data)
{
	val = data % max_val ;
}

int& PUNEH_register::put_value()
{
	return val;
}

void PUNEH_register::get_value(sc_lv<16> &data)
{
	value_check();
	data = val;
}

void PUNEH_register::get_value(int &data)
{
	value_check();
	data = val;
}

int  PUNEH_register::get_value()
{
	value_check();
	return val;
}

int  PUNEH_register::get_value(int a, int b)
{
	sc_lv<16> temp;
	temp = val;
	return temp.range(a, b).to_int();
}

void PUNEH_register::initial()
{
	val = 0XDEAD;
}

void PUNEH_register::value_check()
{
	if (val >= max_val)
	{
		cout << "out-if-range value for register. :D" << endl;
		val = val % max_val;
	}
}

int FA(int opcode, int imm)
{
	if (imm < 0)
		imm = 4096 + imm;
	return (opcode * 4096 + imm);
}

int NA(int sub_opcode)
{
	return (15 * 4096 + sub_opcode * 256);
}

int NA(int sub_opcode, int imm)
{
	if (imm < 0)
		imm = 256 + imm;
	return (15 * 4096 + sub_opcode * 256 + imm);
}

int NA(int sub_opcode, int en, int val)
{
	return (15 * 4096 + sub_opcode * 256 + en * 16 + val);
}

std::string int2hex(int val, int len)
{
	int i;
	int rem;
	char table[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	std::string str;

	if (val < 0) val = 65536 + val; 

	for (i = 0; i < len; i++)
	{
		rem = val % 16;
		val = val / 16;
		str = table[rem] + str;
	}
	return str;
}

