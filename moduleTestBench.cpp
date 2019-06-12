#include <stdint.h>
#include <sstream>
#include <string>

#include "systemc.h"
#include "common.h"
#include "DCT/DCT.hpp"
#include "DCT/ycbcr.hpp"
#include "Memory/memory.hpp"
#include "frames.h"
#define CLOCK_PERIOD_NS 20

SC_MODULE(Consumer){
	sc_fifo_in<Block> input;
	sc_in<bool> clock;
	
	void consume();
	
	SC_CTOR(Consumer){
		SC_THREAD(consume){
			sensitive << clock.pos();
		}
	}
};
void Consumer::consume(){
	while (true){
		wait();
		input.read();
	}
}

SC_MODULE(Producer){
	sc_fifo_out<Block> output;
	sc_in<bool> clock;
	
	void produce();
	
	SC_CTOR(Producer){
		SC_THREAD(produce){
			sensitive << clock.pos();
		}
	}
};
void Producer::produce(){
	Block block;
	while(true){
		wait();
		for (uint8_t i = 0; i < BLOCK_SIZE; i++){
			block[i] = block[i] + 1;
		}
		output.write(block);
	}
}

SC_MODULE(top){
	sc_signal<bool> readerDone, writeEnable, CS, startNextFrame;
	sc_signal< sc_uint<8> > frameMemoryValue, inputMemoryValue;
	sc_signal< sc_uint<MEMORY_ADDRESS_WIDTH> > inputAddressLine;
	uint8_t currentFrame;
	sc_fifo<Block> fifoMemR;
	sc_fifo<Block> fifoMemG;
	sc_fifo<Block> fifoMemB;
	
	// Default array for memory blocks. 
	//The actual image array is too large for the stack and must be instantiated on the heap later, potentially after the memory block instantiation
	uint8_t placeholderArray[2];
	uint8_t* inputFrame;
	
	BlockReader inputReader;
	Memory frameMemory;
	
	void updateFrame();
	
	Consumer rConsumer;
	Consumer gConsumer;
	Consumer bConsumer;
	
	sc_clock t_clock;
	
	SC_CTOR(top) :
		t_clock("clock", CLOCK_PERIOD_NS),
		frameMemory("inputMemory", placeholderArray, 2),
		inputReader("inputReader"),
		rConsumer("rConsumer"),
		gConsumer("gConsumer"),
		bConsumer("bConsumer")
	{ 
		SC_THREAD(updateFrame);
		
		CS = true;
		writeEnable = false;
		inputMemoryValue = 0;
		
		frameMemory.clock(t_clock);
		frameMemory.addressIn(inputAddressLine);
		frameMemory.enableWrite(writeEnable);
		frameMemory.valueIn(inputMemoryValue);
		frameMemory.chipSelect(CS);
		frameMemory.valueOut(frameMemoryValue);
		
		inputReader.clock(t_clock);
		inputReader.start(CS);
		inputReader.memValue(frameMemoryValue);
		inputReader.memAddress(inputAddressLine);
		inputReader.channelOneData(fifoMemR);
		inputReader.channelTwoData(fifoMemG);
		inputReader.channelThreeData(fifoMemB);
		inputReader.done(readerDone);
		
		rConsumer.input(fifoMemR);
		rConsumer.clock(t_clock);
		
		gConsumer.input(fifoMemG);
		gConsumer.clock(t_clock);
		
		bConsumer.clock(t_clock);
		bConsumer.input(fifoMemB);
	}

};
void top::updateFrame(){
	char* val = new char[1];
	inputFrame = new uint8_t[IMAGE_SIZE];
	frameMemory.setMemoryPointer(inputFrame, IMAGE_SIZE);
	std::stringstream ss;
	char filename[20];
	for (uint8_t n = 0; n < NUM_FRAMES; n++){
		// Open file for nth frame
		sprintf(filename, "Matlab\\outputFrames\\frame%d.bin", n);
		std::ifstream file (filename, std::ifstream::binary);
		
		if(file){
			file.seekg (0, file.end);
			int length = file.tellg();
			cout << "This file is " << length << " long." << endl;
			file.seekg (0, file.beg);
			// Each value is one byte, so read byte-by-byte
			for (uint32_t i = 0; i < IMAGE_SIZE; i++){
				file.read(val, 1);
				// Note that val is char, and inputFrame is uchar, so watch the types
				inputFrame[i] = static_cast<uint8_t>(val[0]);
			}
		}
		else{
			cout << "Uh oh, file not loaded";
		}
		
		startNextFrame = true;
		currentFrame = n;
		wait(CLOCK_PERIOD_NS, SC_NS);
		startNextFrame = false;
		wait(33333333, SC_NS);
	}
	cout << "Out of frames!" << endl;
	delete[] inputFrame;
}

SC_MODULE_EXPORT(top);
