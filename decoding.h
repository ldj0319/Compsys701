// decoding.h

#include "systemc.h"
#include "iostream"
#include "climits" // for CHAR_BIT
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include "common.h"

SC_MODULE(decoding)
{
    sc_in<bool> clock;
	sc_out<bool> start;
	sc_out<bool> done;
	sc_in<sc_int <8> > valueNum;
	sc_fifo_in<Block_Encode> inputValue;
	sc_fifo_in<BlockFinal> inputFinalCode;
	sc_fifo_in<Block> inputTable0;
	sc_fifo_in<Block> inputTable1;
	sc_fifo_in<Block> inputTable2;
	sc_fifo_in<Block> inputTable3;
	sc_fifo_in<Block> inputTable4;
	sc_fifo_in<Block> inputTable5;
	sc_fifo_in<Block> inputTable6;
	sc_fifo_in<Block> inputTable7;
	sc_fifo_in<Block> inputTable8;
	
	sc_fifo_out<Block_Encode> outputFinal;
	
	BlockFinal data_in;
	Block_Encode data_V;
	Block data_C0;
	Block data_C1;
	Block data_C2;
	Block data_C3;
	Block data_C4;
	Block data_C5;
	Block data_C6;
	Block data_C7;
	Block data_C8;
	Block_Encode data_out;
	
	
	void main();
    
    SC_CTOR(decoding)
    {
		SC_THREAD(main);
			sensitive << clock.pos( );
			sensitive << start;
    }
};