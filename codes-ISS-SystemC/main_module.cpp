//#include "temp.h"
#include "system.h"

int sc_main(int argc, char **argv)
{

	
	my_system UUT("my_system");
	
	
	//sc_trace_file* VCDFile;
	//VCDFile = sc_create_vcd_trace_file("test");
	//sc_trace(VCDFile, UUT.clk, "clk");
	//sc_trace(VCDFile, UUT.rst, "rst");
	//sc_trace(VCDFile, UUT.intr, "intr");
	//sc_trace(VCDFile, UUT.out, "out");
	

	sc_start(750,SC_NS);
	return 0;
}
