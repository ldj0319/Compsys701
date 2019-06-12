
#include "../common.h"

SC_MODULE(DCT){
	sc_in<bool> clock;
	sc_in<bool> start;
	sc_fifo_in<Block> imageData;
	
	sc_fifo_out<FixedIntBlock> dctData;		// Y  channel
	sc_out<bool> done;
	
	FixedInt dctConstants[8];
	
	void doDCT();
	FixedInt* linearDCT(FixedInt signalArray[8]);
	void blockDCT(Block signalArray, FixedIntBlock& dctArray);
	
	SC_CTOR(DCT){
		dctConstants[0] = convertDoubleToFixed(0.353);
		dctConstants[1] = convertDoubleToFixed(0.408);
		dctConstants[2] = convertDoubleToFixed(0.500);
		dctConstants[3] = convertDoubleToFixed(0.408);
		dctConstants[4] = convertDoubleToFixed(0.353);
		dctConstants[5] = convertDoubleToFixed(0.408);
		dctConstants[6] = convertDoubleToFixed(0.500);
		dctConstants[7] = convertDoubleToFixed(0.408);
		
		SC_THREAD(doDCT){
			sensitive << clock.pos();
			sensitive << start;
			sensitive << imageData;
		}
	}
};

SC_MODULE(IDCT){
	sc_in<bool> clock;
	sc_in<bool> start;
	sc_fifo_in<FixedIntBlock> dctData;
	
	sc_fifo_out<Block> imageData;		// Y  channel
	sc_out<bool> done;
	
	FixedInt dctConstants[8];
	
	void doInverseDCT();
	FixedInt* inverseLinearDCT(FixedInt DCTArray[8]);
	void inverseBlockDCT(FixedIntBlock dctArray, Block& signalArray);
	
	SC_CTOR(IDCT){
		dctConstants[0] = convertDoubleToFixed(0.353);
		dctConstants[1] = convertDoubleToFixed(0.408);
		dctConstants[2] = convertDoubleToFixed(0.500);
		dctConstants[3] = convertDoubleToFixed(0.408);
		dctConstants[4] = convertDoubleToFixed(0.353);
		dctConstants[5] = convertDoubleToFixed(0.408);
		dctConstants[6] = convertDoubleToFixed(0.500);
		dctConstants[7] = convertDoubleToFixed(0.408);
		
		SC_THREAD(doInverseDCT){
			sensitive << clock.pos();
			sensitive << start;
			sensitive << dctData;
		}
	}
};
