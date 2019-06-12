
#include "../common.h"

SC_MODULE(YCbCr){
	sc_in<bool> clock;
	sc_in<bool> start;
	sc_fifo_in<Block> rData;	// R channel
	sc_fifo_in<Block> gData;	// G channel
	sc_fifo_in<Block> bData;	// B channel
	
	sc_fifo_out<Block> yData;	// Y  channel
	sc_fifo_out<Block> alsoYData; // second Y output for motion estimation
	sc_fifo_out<Block> cbData;	// Cb channel
	sc_fifo_out<Block> crData;	// Cr channel
	sc_out<bool> done;
	
	void convert();
	
	SC_CTOR(YCbCr){
		SC_THREAD(convert){
			sensitive << clock.pos();
			sensitive << start;
			sensitive << rData << gData << bData;
		}
	}
};

SC_MODULE(YCbCrToRGB){
	sc_in<bool> clock;
	sc_in<bool> start;
	sc_fifo_out<Block> rData;	// R channel
	sc_fifo_out<Block> gData;	// G channel
	sc_fifo_out<Block> bData;	// B channel
	
	sc_fifo_in<Block> yData;	// Y  channel
	sc_fifo_in<Block> cbData;	// Cb channel
	sc_fifo_in<Block> crData;	// Cr channel
	sc_out<bool> done;
	
	void convertToRGB();
	
	SC_CTOR(YCbCrToRGB){
		SC_THREAD(convertToRGB){
			sensitive << clock.pos();
			sensitive << start;
			sensitive << yData << cbData << crData;
		}
	}
};
