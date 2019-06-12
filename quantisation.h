// quantisation.h

#include "systemc.h"
#include "common.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

SC_MODULE(quantisation)
{
    sc_in<bool> clock;
	sc_in<bool> start;
	sc_out<bool> done;
    sc_fifo_in<FixedIntBlock> in;
    sc_fifo_out<Block_Encode> out;

	FixedIntBlock data_in;
	Block_Encode calcBlock;
    void   my_quantisation( );
    
    SC_CTOR(quantisation)
    {
		SC_THREAD(my_quantisation);
			sensitive << clock.pos();
			sensitive << start;
			sensitive << in << out;
    }
};