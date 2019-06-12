#include "systemc.h"
#include "../common.h"
#include "memory.hpp"


void BlockWriter::writeBlocks(){
	Block block;
	uint32_t addressInBlock;
	uint32_t addressOfBlock;
	chipSelect = false;
	while (true){
		//Wait for clock signal
		wait();
		
		if (start){
			chipSelect = not chipSelect;
			done = false;
			// for each 8x8x3 block of pixels
			for (uint16_t row = 0; row < IMAGE_HEIGHT; row += BLOCK_HEIGHT){
				for (uint16_t col = 0; col < IMAGE_WIDTH; col += BLOCK_WIDTH){
					addressOfBlock = col + row*IMAGE_WIDTH;
					
					// Read the input fifo into a variable
					block = data.read(); 
					
					// Write these values to memory block
					for (uint8_t rowInBlock = 0; rowInBlock < BLOCK_HEIGHT; rowInBlock++){
						for (uint8_t colInBlock = 0; colInBlock < BLOCK_WIDTH; colInBlock++){
							addressInBlock = colInBlock + IMAGE_WIDTH*rowInBlock;
							
							// Tell the memory block where we are writing to
							memAddress.write(addressOfBlock + addressInBlock);
							
							// Pass value to be written
							memValue.write(block[colInBlock + BLOCK_WIDTH*rowInBlock]);
							
							//enable write
							dataValid = true;
							
							// wait
							wait();
							
							//disable write
							dataValid = false;
						}
					}
				}
			}
			done = true;
		}
	}
	
}

void BlockReader::generateBlocks(){
	
	Block block;
	uint32_t addressInBlock;
	uint32_t addressOfBlock;
	
	while (true){
		// Wait for start signal
		wait();
		
		if (start){
			done = false;
			// for each 8x8x3 block of pixels
			for (uint16_t row = 0; row < IMAGE_HEIGHT; row += BLOCK_HEIGHT){
				for (uint16_t col = 0; col < IMAGE_WIDTH; col += BLOCK_WIDTH){
					for (uint8_t channel = 0; channel < IMAGE_DEPTH; channel++){
						addressOfBlock = col + row*IMAGE_WIDTH + channel*(CHANNEL_SIZE);
						
						// Load these values up into an array
						for (uint8_t rowInBlock = 0; rowInBlock < BLOCK_HEIGHT; rowInBlock++){
							for (uint8_t colInBlock = 0; colInBlock < BLOCK_WIDTH; colInBlock++){
								addressInBlock = colInBlock + IMAGE_WIDTH*rowInBlock;
								
								// Ask to read the memory address we want
								memAddress.write(addressOfBlock + addressInBlock);
								
								// wait
								wait();
								
								// Read returned value
								block[colInBlock + BLOCK_WIDTH*rowInBlock] = memValue.read();
							}
						}
						
						//And write that block into the output fifo
						switch (channel){
							case 0: channelOneData.write(block); break;
							case 1: channelTwoData.write(block); break;
							case 2: channelThreeData.write(block); break;
						}
					}
				}
			}
			done = true;
		}
	}
}

void Memory::setMemoryPointer(uint8_t initialValues[], uint32_t arrayLength){
	memory = initialValues;
	memorySize = arrayLength;
}

void Memory::manageMemory(){
	// If the block is enabled
	if (chipSelect){
		sc_assert(addressIn.read() < memorySize);
		
		// If we are writing to memory
		if(enableWrite){
			memory[addressIn.read()] = valueIn.read();
		}
		// If we are reading from memory
		else{
			valueOut.write(memory[addressIn.read()]);
		}
	}
}
