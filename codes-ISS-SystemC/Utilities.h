#include <systemc.h>

#ifndef _MY_UTILITIES_H_
#define _MY_UTILITIES_H_

#define max_val 65536

/************************************************************************/
/*								PROCESSOR								*/
/************************************************************************/
enum InstType {full_address, no_address};
enum MapType {immediate, addressed, indirect};

struct Flags
{
	sc_lv<1> Z, N, C, V;
	void get_value(sc_lv<4> &fval);
	int get_value();
	void put_value(sc_lv<4> fval);
	void reset();
	void initial();
};

struct PUNEH_inst
{
	InstType type;
	MapType	 mapping;
	unsigned char opcode;
	unsigned char sub_opcode;
	char inst;
	int immediate;
	sc_lv<4> flag_en;
	sc_lv<4> flag_value;
};

struct PUNEH_info
{
	sc_lv<16> PC, IR, AR, ACC, offset;
	PUNEH_inst ins;
	Flags flag;
};

struct PUNEH_register
{
	int val;
	void reset();
	void put_value(sc_lv<16> data);
	void put_value(int data);
	int& put_value();
	void get_value(sc_lv<16> &data);
	void get_value(int &data);
	int get_value();
	int get_value(int a, int b); // (min , max) or (max, min)
	void initial();
	void value_check();
};

struct ALU_info
{
	int op1,op2;
};

#define mem(x) x

#define LDm 0
#define LDa 1 
#define LDn 2
#define STa 3
#define STn 4
#define INa 5
#define ANm 6
#define ANa 7
#define ADm 8
#define ADa 9 
#define ADn 10
#define MLa 11
#define JMa 12
#define JMn 13
#define JSR 14

#define LOm 0
#define ALU 1
#define SKP 2
#define SET 3
#define PAC 4	//print AC
#define EXT 5	// EXIT

#define INO 0 
#define LPO 1
#define LOP 2
#define ACZ 3
#define ACN 4
#define ACI 5
#define SLA 6
#define SRA 7
#define SLL 8
#define SRL 9
/************************************************************************/
/************************************************************************/
/*								  channel								*/
/************************************************************************/
#define access_cnt 10

enum operation {read, write};
enum burst {one, two, four, eight};

struct transaction_info
{
	operation op;
	sc_lv<16> address;
	burst burst_cnt;
};
/************************************************************************/
/************************************************************************/
/*								  assembler								*/
/************************************************************************/
enum update_CV_flag_type{two_op, inc, shift};

int FA(int opcode, int imm);
int NA(int sub_opcode);
int NA(int sub_opcode, int imm);
int NA(int sub_opcode, int en, int val);
std::string int2hex(int val, int len);

struct printing_info
{
	int inst_cntr;
	sc_time time;
	int PC_val;
	int IR_val;
	int AC_val;
	int OF_val;
	InstType type;
	std::string nom;
	std::string add;
	int flag;
};
/************************************************************************/






#endif