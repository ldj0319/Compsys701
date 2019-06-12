#include "systemc.h"
#include "../common.h"

// This module reads from a memory block that holds an entire image, and outputs 8x8 blocks to three FIFOs
// Used to interface between memory blocks and modules that process 8x8 blocks at a time
SC_MODULE(BlockReader){
	sc_in<bool> clock;
	sc_in<bool> start;
	sc_in< uint8_t > memValue;
	sc_out< sc_uint<MEMORY_ADDRESS_WIDTH> > memAddress;
	sc_fifo_out<Block> channelOneData;		// R or Y  channel
	sc_fifo_out<Block> channelTwoData;		// G or Cb channel
	sc_fifo_out<Block> channelThreeData;	// B or Cr channel
	sc_out<bool> done;
	
	void generateBlocks();
	
	SC_CTOR(BlockReader){
		SC_THREAD(generateBlocks){
			sensitive << clock.pos();
			sensitive << start;
		}
	}
};

// This module writes from a FIFO into a memory block that holds an entire image.
// Used to interface between modules that process 8x8 blocks at a time and memory blocks that store an entire image.
SC_MODULE(BlockWriter){
	sc_in<bool> clock;
	sc_in<bool> start;
	sc_out<bool> dataValid;
	sc_out<bool> chipSelect; // select which output to write to
	sc_out< sc_uint<MEMORY_ADDRESS_WIDTH> > memAddress;
	sc_out< uint8_t > memValue;
	sc_fifo_in<Block> data;		// R or Y  channel
	sc_out<bool> done;
	
	void writeBlocks();
	
	SC_CTOR(BlockWriter){
		SC_THREAD(writeBlocks){
			sensitive << clock.pos();
			sensitive << start;
		}
	}
};

// Memory element. The memory block itself only contains a pointer to a 1D array. 
// You will have to make sure that the array you pass in is the proper size for what you want.
SC_MODULE(Memory){
	sc_in<bool> clock;
	sc_in<bool> enableWrite;
	sc_in< sc_uint<MEMORY_ADDRESS_WIDTH> > addressIn;
	sc_in< uint8_t > valueIn;
	sc_in<bool> chipSelect;
	sc_out< uint8_t > valueOut;
	
	void setMemoryPointer(uint8_t initialValues[], uint32_t arrayLength);
	void manageMemory();
	uint8_t* memory;
	uint32_t memorySize;
	
	SC_HAS_PROCESS(Memory);
	
	Memory(sc_module_name name) : sc_module(name){
		SC_METHOD(manageMemory){
			sensitive << clock.pos();
			sensitive << enableWrite << chipSelect;
			sensitive << addressIn << valueIn;
		}
		
	}
	Memory(sc_module_name name, uint8_t initialValues[], uint32_t arrayLength) : sc_module(name){
		memory = initialValues;
		memorySize = arrayLength;
		
		SC_METHOD(manageMemory){
			sensitive << clock.pos();
			sensitive << enableWrite << chipSelect;
			sensitive << addressIn << valueIn;
		}
		
	}
};
