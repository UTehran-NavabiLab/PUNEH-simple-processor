#include "modules.h"

void CCE1::pstate_updating()
{
	if (rst->read() == '1')
		pstate.write(s0);
	else
		if ((clk->read() == '1') && (clk->event()))
			pstate.write(nstate.read());
}

void CCE1::nstate_updating()
{
	switch (pstate.read())
	{
	case s0: if (start->read() == '1') nstate.write(s1); else nstate.write(s0); break;
	case s1: nstate.write(s2); break;
	case s2: nstate.write(s3); break;
	case s3: nstate.write(s4); break;
	case s4: nstate.write(s5); break;
	case s5: nstate.write(s6); break;
	case s6: if (cntr.read() == "111") nstate.write(s0); else nstate.write(s4); break;
	}
}

void CCE1::calculate_cce1()
{
	int tmp;

	if (rst->read() == '1')
	{
		data_out->write("0000000000");
		X_val.write("0000000000");
		term.write("0000000000");
		cntr.write("000");
	}
	else if ((clk->read() == '1') && (clk->event()))
	{
		switch (pstate.read())
		{
		case s1: 
			data_out->write("0010000000");
			X_val.write("0000000000");
			term.write("0010000000");
			cntr.write("000");
			break;
		case s2:
			X_val.write( ("00",data_in->read()) );
			break;
		case s3:
			tmp = X_val.read().to_uint() * X_val.read().to_uint();
			X_val.write( tmp / 128);
			break;
		case s4:
			tmp = X_val.read().to_uint() * term.read().to_uint();
			term.write(tmp/128);
			break;
		case s5:
			tmp = term.read().to_uint() * coef.read().to_uint();
			term.write(tmp / 128);
			break;
		case s6:
			if ( (cntr.read().to_uint() % 2) == 0)
				tmp = data_out->read().to_uint() - term.read().to_uint();
			else
				tmp = data_out->read().to_uint() + term.read().to_uint();
			data_out->write(tmp);
			cntr.write(cntr.read().to_uint() + 1 );
			break;
		}
	}
}

void CCE1::coefficient()
{
	if (cntr.read() == "000")
		coef.write(128 / 1 / 2);
	else if (cntr.read() == "001")
		coef.write(128 / 3 / 4);
	else if (cntr.read() == "010")
		coef.write(128 / 5 / 6);
	else if (cntr.read() == "011")
		coef.write(128 / 7 / 8);
	else if (cntr.read() == "100")
		coef.write(128 / 9 / 10);
	else if (cntr.read() == "101")
		coef.write(128 / 11 / 12);
	else if (cntr.read() == "110")
		coef.write(128 / 13 / 14);
	else if (cntr.read() == "111")
		coef.write(128 / 15 / 16);
}

void CCE1::assigning()
{
	if (pstate.read() == s0)
		done->write(sc_logic_1);
	else
		done->write(sc_logic_0);
}

void counter::counting()
{
	if (rst->read() == '1')
		cntr->write("00");
	else if ((clk->read() == '1') && (clk->event()))
		if (init->read() == '1')
			cntr->write("11");
		else if (cnt_up->read() == '1')
			cntr->write(cntr->read().to_uint() + 1);
}

void mux::selecting()
{
	if (select->read() == '1')
		out->write(in1->read());
	else
		out->write(in0->read());
}

void reg::registering()
{
	if (rst->read() == '1')
		out->write(0);
	else if ( (clk->read() == '1') && (clk->event()) )
		if (clear->read() == '1')
			out->write(0);
		else if (load->read() == '1')
			out->write(in->read());
}

void sub::subtracting()
{
	int i1, i2;
	i1 = in1->read().to_uint();
	i2 = in2->read().to_uint();
	out->write(i1-i2);
	if (i1 <= i2)
		less_or_equal->write(sc_logic_1);
	else
		less_or_equal->write(sc_logic_0);
}

void CCE4_data_path::assigning()
{
	cce1_data_in.write(xreg_out.read().range(7,0));
	sc_0.write(sc_logic_0);

	if (cntr_out.read()[0] == cntr_out.read()[1])
		sign->write(sc_logic_0);
	else
		sign->write(sc_logic_1);

	cal_sin->write(cntr_out.read()[0]);
}

void CCE4_controller::pstate_updating()
{
	if (rst->read() == '1')
		pstate.write(0);
	else
		if ( (clk->read() == '1') && (clk->event()) )
			pstate.write(nstate.read());
}

void CCE4_controller::nstate_updating()
{
	switch (pstate.read().to_uint())
	{
	case 0:
		if (start_cce4->read() == '1')
			nstate.write(1);
		else
			nstate.write(0);
		break;
	case 1 :
		nstate.write(2);
		break;
	case 2 :
		nstate.write(3);
		break;
	case 3:
		if (less_or_equal->read() == '1')
			nstate.write(4);
		else
			nstate.write(3);
		break;
	case 4:
		if (cal_sin->read() == '1')
			nstate.write(5);
		else
			nstate.write(6);
		break;
	case 5:
		nstate.write(6);
		break;
	case 6:
		nstate.write(7);
		break;
	case 7 :
		if (cce1_done->read() == '1')
			nstate.write(0);
		else
			nstate.write(7);
		break;
	}
}

void CCE4_controller::output_updating()
{

	cntr_up->write(sc_logic_0);
	cntr_init->write(sc_logic_0);
	cce1_start->write(sc_logic_0);
	sub_opr_sel->write(sc_logic_0);
	reg_src_sel->write(sc_logic_0);
	load_reg->write(sc_logic_0);
	CCE4ready->write(sc_logic_0);

	switch (pstate.read().to_uint())
	{
	case 0: 
		CCE4ready->write(sc_logic_1);
		break;
	case 1:
		cntr_init->write(sc_logic_1);
		break;
	case 2:
		load_reg->write(sc_logic_1);
		break;
	case 3:
		cntr_up->write(sc_logic_1);
		reg_src_sel->write(sc_logic_1);
		load_reg->write(sc_logic_1);
		break;
	case 5:
		reg_src_sel->write(sc_logic_1);
		load_reg->write(sc_logic_1);
		sub_opr_sel->write(sc_logic_1);
		break;
	case 6:
		cce1_start->write(sc_logic_1);
	}
}

void CCE4::assigning()
{
	data_out->write(temp_out.read().range(7,0));
}