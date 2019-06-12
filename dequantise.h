// dequantise.h

#include "systemc.h"
#include "common.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

SC_MODULE(dequantise)
{
    sc_in<bool> clock;
	sc_in<bool> start;
	sc_out<bool> done;
    sc_fifo_in<Block_Encode> in;
    sc_fifo_out<FixedIntBlock> out;

	Block_Encode data_in;
	FixedIntBlock calcBlock;
    void	my_dequant( );
    
    SC_CTOR(dequantise)
    {
		SC_THREAD(my_dequant);
			sensitive << clock.pos();
			sensitive << start;
			sensitive << in << out;
    }
};