// encoding.h

#include "systemc.h"
#include "iostream"
#include "climits" // for CHAR_BIT
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include "common.h"

SC_MODULE(encoding)
{
    sc_in<bool> clock;
	sc_out<bool> start;
	sc_out<bool> done;
	sc_out<sc_int <8> > valueNum;
	sc_fifo_in<Block_Encode> in;
	sc_fifo_out<Block_Encode> outputV;
	sc_fifo_out<Block> outputC0;
	sc_fifo_out<Block> outputC1;
	sc_fifo_out<Block> outputC2;
	sc_fifo_out<Block> outputC3;
	sc_fifo_out<Block> outputC4;
	sc_fifo_out<Block> outputC5;
	sc_fifo_out<Block> outputC6;
	sc_fifo_out<Block> outputC7;
	sc_fifo_out<Block> outputC8;
	sc_fifo_out<BlockFinal> outputFinalCode;

	Block_Encode data_in;
	
	Block_Encode inputV;
	Block inputC0;
	Block inputC1;
	Block inputC2;
	Block inputC3;
	Block inputC4;
	Block inputC5;
	Block inputC6;
	Block inputC7;
	Block inputC8;
	BlockFinal outputFinal;
	sc_int<16> finalCount;
	sc_int<8> count;
	sc_int<10> test_Arr[64];
	
    typedef struct node
	{
			int ch;
			int freq;
			struct node *left;
			struct node *right;
	}node;

	node * heap[100];
	
	void main();
	void Insert(node * element, int test_size);
	// node * DeleteMin(int test_size);
	void print(int size, node *temp,char *code);
    
    SC_CTOR(encoding)
    {
		SC_THREAD(main);
			sensitive << clock.pos( );
			sensitive << start;
			sensitive << in << outputC3;
    }
};