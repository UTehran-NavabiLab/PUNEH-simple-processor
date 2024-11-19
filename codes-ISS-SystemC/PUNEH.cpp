#include "PUNEH.h"
#include "Utilities.h"

void PUNEH::initial()
{
	inst_cntr = 0;

	flag.initial();
	PC.initial();
	IR.initial();
	AR.initial();
	AC.initial();
	OF.initial();

	wait();
}

void PUNEH::reseting()
{
	PC.reset();
	IR.reset();
	AR.reset();
	AC.reset();
	OF.reset();
	flag.reset();
}

void PUNEH::main_port_reseting()
{
	if (rst->read() == sc_logic_1)
	{
		main_port->reset_started();
		cout << "resetting start at " << sc_time_stamp() << endl;
	}
	else
	{
		main_port->reset_stopped();
		cout << "normal operation at " << sc_time_stamp() << endl;
	}
}

void PUNEH::operating()
{
	while (1)
	{
		if (rst->read() == sc_logic_1)
		{
			reseting();
		}
		else if (clk->read() == sc_logic_1)
		{
			fetch();
			wait(go);
			
			if (rst->read() == sc_logic_1)
				continue;

			instruction_decode();

			execute();
			printing();
		}
		wait(go);
	}
}

void PUNEH::fetch()
{
	do_read(IR, mem(PC));
	
	pinf.inst_cntr = inst_cntr;
	pinf.time = sc_time_stamp();
	pinf.PC_val = PC.get_value();
	pinf.IR_val = IR.get_value();

	PC.put_value(PC.get_value() + 1);
}

void PUNEH::instruction_decode()
{
	sc_lv<16> IR_val;
	IR_val = IR.get_value();

	inst_info.opcode = IR_val.range(15, 12).to_uint();
	inst_info.sub_opcode = IR_val.range(11, 8).to_uint();
	inst_info.inst = IR_val.range(7, 0).to_uint();
	inst_info.immediate = IR_val.range(11, 0).to_int();
	inst_info.flag_en = IR_val.range(7, 4).to_uint();
	inst_info.flag_value = IR_val.range(3, 0).to_uint();

	inst_info.type = full_address;
	switch (inst_info.opcode)
	{
	case 15:
		inst_info.mapping = immediate;
		inst_info.type = no_address;
		break;
	case LDm:
	case ANm:
	case ADm:
		inst_info.mapping = immediate;
		break;
	case LDn:
	case STn:
	case ADn:
	case JMn:
		inst_info.mapping = indirect;
		break;
	default:
		inst_info.mapping = addressed;
		if (inst_info.opcode > 15)
			cout << "invalid operation at decode stage" << endl;
		break;
	}
}

void PUNEH::indirect_operand()
{
	sc_lv<16> address;
	address.range(15,12) = OF.get_value(15,12);
	address.range(0, 11) = IR.get_value(0, 11);
	do_read(AR, mem(address.to_int()));
}

void PUNEH::do_read(PUNEH_register &dest, int address)
{
	main_port->trylock(PUNEH_priority, read, address, one);
	main_port->get(dest.put_value());
	dest.value_check();
	main_port->unlock();
}

void PUNEH::do_read(PUNEH_register &dest, PUNEH_register address)
{
	do_read(dest, address.get_value());
}

int PUNEH::do_read(int address)
{
	PUNEH_register dummy;
	do_read(dummy, mem(address));
	return dummy.get_value();
}

void PUNEH::do_write(int address, PUNEH_register source)
{
	main_port->trylock(PUNEH_priority, write, address, one);
	main_port->put(source.get_value());
	main_port->unlock();
}

void PUNEH::do_write(PUNEH_register address, PUNEH_register source)
{
	do_write(address.get_value(), source);
}

bool PUNEH::execute()
{
	sc_lv<16> address;
	int imm;
	sc_lv<4> old, out;

	if (inst_info.type == full_address)
	{
		pinf.add = int2hex(inst_info.immediate, 3);

		if (inst_info.mapping == indirect)
		{
			indirect_operand();
			wait(go);
			if (rst->read() == sc_logic_1)
				return false;
		}

		imm = inst_info.immediate;
		address.range(15, 12) = OF.get_value(15, 12);
		address.range(11, 0) = imm;
		

		switch (inst_info.opcode)
		{
		case LDm:
			pinf.nom = "LDm";
			AC.put_value(imm);
			update_ZN_flag();
			break;
		case LDa:
			pinf.nom = "LDa";
			do_read(AC, mem(address.to_int()));
			update_ZN_flag();
			break;
		case LDn:
			pinf.nom = "LDn";
			do_read(AC, mem(AR));
			update_ZN_flag();
			break;
		case STa:
			pinf.nom = "STa";
			do_write(mem(address.to_int()), AC);
			break;
		case STn:
			pinf.nom = "STn";
			do_write(mem(AR), AC);
			break;
		case INa:
			pinf.nom = "INa";
			do_read(AC, mem(address.to_int()));
			AC.put_value(AC.get_value() + 1);
			alu_info.op1 = AC.get_value();
			update_ZN_flag();
			update_CV_flag(inc);
			wait(go);
			do_write(mem(address.to_int()), AC);
			break;
		case ANm :
			pinf.nom = "ANm";
			AC.put_value(AC.get_value() & imm);
			update_ZN_flag();
			break;
		case ANa:
			pinf.nom = "ANa";
			imm = do_read(mem(address.to_int()));
			AC.put_value(AC.get_value() & imm);
			update_ZN_flag();
			break;
		case ADm:
			pinf.nom = "ADm";
			alu_info.op1 = AC.get_value();
			alu_info.op2 = imm;
			AC.put_value(AC.get_value() + imm);
			update_ZN_flag();
			update_CV_flag(two_op);
			break;
		case ADa:
			pinf.nom = "ADa";
			imm = do_read(mem(address.to_int()));
			alu_info.op1 = AC.get_value();
			alu_info.op2 = imm;
			AC.put_value(AC.get_value() + imm);
			update_ZN_flag();
			update_CV_flag(two_op);
			break;
		case ADn:
			pinf.nom = "ADn";
			imm = do_read(mem(AR.get_value()));
			alu_info.op1 = AC.get_value();
			alu_info.op2 = imm;
			AC.put_value(AC.get_value() + imm);
			update_ZN_flag();
			update_CV_flag(two_op);
			break;
		case MLa:
			pinf.nom = "MLa";
			imm = do_read(mem(address.to_int()));
			AC.put_value(AC.get_value(7,0) * (imm%256));
			update_ZN_flag();
			break;
		case JMa:
			pinf.nom = "JMa";
			PC.put_value(address);
			break;
		case JMn:
			pinf.nom = "JMn";
			PC.put_value(AR.get_value());
			break;
		case JSR:
			pinf.nom = "JSR";
			do_write(mem(address.to_int()), PC);
			PC.put_value(address.to_int() + 1);
			break;
		default:
			pinf.nom = "NON";
			cout << "invalid operation for full-address operations" << endl;
			break;
		}
	}
	else
	{
		switch (inst_info.sub_opcode)
		{
		case LOm: 
			pinf.nom = "LOm";
			imm = IR.get_value(3,0);
			OF.put_value(imm * 4096);
			pinf.add = inst_info.inst;
			break;
		case ALU:
			ALU_op();
			pinf.add = "   ";
			break;
		case SKP:
			pinf.nom = "SKP";
			if (check_flags())
				PC.put_value(PC.get_value() + 1);
			pinf.add = int2hex(inst_info.flag_en.to_uint(), 1) + "," + int2hex(inst_info.flag_value.to_uint(), 1);
			break;
		case SET:
			pinf.nom = "SET";
			flag.get_value(old);
			out = (inst_info.flag_en & inst_info.flag_value) | ((~inst_info.flag_en) & old);
			flag.put_value(out);
			pinf.add = int2hex(inst_info.flag_en.to_uint(), 1) + "," + int2hex(inst_info.flag_value.to_uint(), 1);
			break;
		case PAC:
			pinf.nom = "PAC";
			pinf.add = "   ";
			cout << endl << "\t\t\tAC Value: " << AC.get_value() << endl << endl;
			break;
		case EXT: 
			pinf.nom = "EXT";
			pinf.add = "   ";
			cout << endl << "\t\t\tEND OF EXECUTION @ " << sc_time_stamp() << endl << endl;
			wait();
			break;
		default:
			pinf.nom = "NON";
			cout << "invalid operation for no-address operations" << endl;
			break;
		}
	}
	return true;
}

bool PUNEH::check_flags()
{
	bool ok(true);
	sc_lv<4> cfval;

	flag.get_value(cfval);

	for (int i = 0; i < 4; i++)
	{
		if (inst_info.flag_en[i] == '0')
			continue;
		if (inst_info.flag_value[i] != cfval[i])
			ok = false;
	}
	return ok;
}

void PUNEH::update_ZN_flag()
{
	if (AC.get_value() == 0)
		flag.Z = '1';
	else
		flag.Z = '0';

	if (AC.get_value() < 0)
		flag.N = '1';
	else
		flag.N = '0';
}

void PUNEH::update_CV_flag(update_CV_flag_type type)
{
	if (type == two_op)
	{
		if (((alu_info.op1 < 0) && (alu_info.op2 < 0) && (AC.get_value() >= 0)) || ((alu_info.op1 >= 0) && (alu_info.op2 >= 0) && (AC.get_value() < 0)))
			flag.V = '1';
		else
			flag.V = '0';

		if (abs(AC.get_value()) < abs(alu_info.op1 + alu_info.op2))
			flag.C = '1';
		else
			flag.C = '0';
	}
	else if (type == inc)
	{
		if (alu_info.op1 > 0 && AC.get_value() < 0)
		{
			flag.C = '0';
			flag.V = '1';
		}
		else if ((alu_info.op1 < 0 && AC.get_value() >= 0))
		{
			flag.C = '1';
			flag.V = '1';
		}
		else
		{
			flag.C = '0';
			flag.V = '0';
		}
	}
	else if (type == shift)
	{
		if ((alu_info.op1 >= 0 && AC.get_value() < 0) || (alu_info.op1 < 0 && AC.get_value() >= 0))
			flag.V = '1';
		else
			flag.V = '0';

		if (alu_info.op1 < 0)
			flag.C = '1';
		else
			flag.C = '0';
	}
	else
		cout << "error while doing flags" << endl;
}

void PUNEH::ALU_op()
{
	alu_info.op1 = AC.get_value();
	alu_info.op2 = 0;

	switch (inst_info.inst)
	{
	case INO:
		pinf.nom = "INO";
		OF.put_value(OF.get_value() + 4096);
		break;
	case LPO:
		pinf.nom = "LPO";
		OF.put_value(PC.get_value());
		break;
	case LOP:
		pinf.nom = "LOP";
		PC.put_value(OF.get_value());
		break;
	case ACZ:
		pinf.nom = "ACZ";
		AC.put_value(0);
		update_ZN_flag();
		break;
	case ACN:
		pinf.nom = "ACN";
		AC.put_value(-1 - AC.get_value());
		update_ZN_flag();
		break;
	case ACI:
		pinf.nom = "ACI";
		AC.put_value(AC.get_value() + 1);
		update_ZN_flag();
		update_CV_flag(inc);
		break;
	case SLA:
		pinf.nom = "SLA";
		AC.put_value(AC.get_value() * 2);
		update_ZN_flag();
		update_CV_flag(shift);
		break;
	case SLL:
		pinf.nom = "SLL";
		AC.put_value(AC.get_value() * 2);
		update_ZN_flag();
		update_CV_flag(shift);
		break;
	case SRA:
		pinf.nom = "SRA";
		AC.put_value(AC.get_value() / 2);
		update_ZN_flag();
		update_CV_flag(shift);
		break;
	case SRL:
		pinf.nom = "SRL";
		AC.put_value( ((unsigned)AC.get_value()) / 2);
		update_ZN_flag();
		update_CV_flag(shift);
		break;
	default:
		pinf.nom = "NON";
		break;
	}
}

void PUNEH::go_generator()
{
	if (rst->read() == sc_logic_1)
	{
		go.notify();
	}
	else if ((clk->read() == sc_logic_1) && clk->event())
	{
		go.notify();
	}
}

void PUNEH::printing()
{
	sc_lv<4> bin_f;
	

	pinf.AC_val = AC.get_value();
	pinf.OF_val = OF.get_value();
	pinf.flag = flag.get_value();

	pinf.type = inst_info.type;

	bin_f = pinf.flag;

	cout << "\tInst." << pinf.inst_cntr << "\t@ " << sc_time_stamp() <<
		" =>\tPC:" << int2hex(pinf.PC_val, 4) << ",\tIR:" << int2hex(pinf.IR_val, 4) <<
		" =>\t" << pinf.nom << " " << pinf.add <<
		";\tAC: " << pinf.AC_val << ",\tOF: " << pinf.OF_val <<
		",\tZNCV: " << bin_f;
	cout << endl;
	
	inst_cntr++;
}



