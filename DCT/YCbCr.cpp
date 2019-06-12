
#include "../common.h"
#include "ycbcr.hpp"

uint8_t squash(int16_t num){
	if (num < 0){
		return 0;
	}
	else if (num > UCHAR_MAX){
		return UCHAR_MAX;
	}
	else{
		return (uint8_t)num;
	}
}


void YCbCr::convert(){
	// Store pixel values in vars
	int16_t r,g,b;
	Block redBlock, greenBlock, blueBlock;
	Block yBlock, cbBlock, crBlock;
		while (true){
		wait();
		
		if (start){
			done = false;
			//Read from FIFO into variables
			redBlock = rData.read();
			greenBlock = gData.read();
			blueBlock = bData.read();
			
			for ( unsigned int row = 0; row < BLOCK_HEIGHT; row++ ) {
				for( unsigned int col = 0; col < BLOCK_WIDTH; col++ ) {
					// Read values from memory address into vars for easy access
					r = redBlock[col + BLOCK_WIDTH*row];
					g = greenBlock[col + BLOCK_WIDTH*row];
					b = blueBlock[col + BLOCK_WIDTH*row];
					
					//Convert RGB values into YCbCr using shifts, if we reimplement using RTL
					// Y = (red * 33/128) + (g / 2) + (b * 25/256) + 16			= 0.257R + 0.5G + 0.0976B + 16
					// Cb = -(red * 19/128) - (g * 75/256) + (b * 7/16) + 128	= -0.148R -0.293G + 0.4375B + 128
					// Cr = (red  * 7/16) + (g * 47/128) - (b * 9/128) + 128		= 0.4375R + 0.367G - 0.07B + 128
					
											
				  //imageOut[row][col].Y = 0.257*red + 0.5*green + 0.0976*blue + 16;
					yBlock[col + BLOCK_WIDTH*row] = squash( ((r>>2) + (r>>7))
													+ (g>>1)
													+ ((b>>4) + (b>>5) + (b>>8))
													);
												
				  //imageOut[row][col].Cb = -0.148*red  -0.293*green + 0.4375*blue + 128;
					cbBlock[col + BLOCK_WIDTH*row] =	squash(- ((r>>3) + (r>>6) + (r>>7)) 
														- ((g>>2) + (g>>5) + (g>>7) + (g>>8))
														+ ((b>>2) + (b>>3) + (b>>4))
														);
					
				  //imageOut[row][col].Cr = 0.4375*red + 0.367*green - 0.07*blue + 128;
					crBlock[col + BLOCK_WIDTH*row] =	squash(((r>>2) + (r>>3) + (r>>4))
														- ((g>>2) + (g>>4) + (g>>5) + (g>>6) + (g>>7))
														- ((b>>4) + (b>>7))
														);
				}
			}
			//Cycle-accurate timing
			wait(16);
			
			yData.write(yBlock);
			alsoYData.write(yBlock);
			cbData.write(cbBlock);
			crData.write(crBlock);
			done = true;
		}
	}
}

void YCbCrToRGB::convertToRGB(){
	// Store pixel values in vars
	int16_t y,cb,cr;
	Block yBlock, cbBlock, crBlock;
	Block redBlock, greenBlock, blueBlock;
		while (true){
		wait();
		
		if (start){
			done = false;
			//Read from FIFO into variables
			yBlock = yData.read();
			cbBlock = cbData.read();
			crBlock = crData.read();
			
			for ( unsigned int row = 0; row < BLOCK_HEIGHT; row++ ) {
				for( unsigned int col = 0; col < BLOCK_WIDTH; col++ ) {
					// Read values from memory address into vars for easy access
					y = yBlock[col + BLOCK_WIDTH*row];
					cb = cbBlock[col + BLOCK_WIDTH*row];
					cr = crBlock[col + BLOCK_WIDTH*row];
					
											
				  //R = 1.16*y + 1.596*cr - 1.4;
					redBlock[col + BLOCK_WIDTH*row] = squash((y + (y >> 3) + (y >> 4))
														+ (cr + (cr >> 1) + (cr >> 4) + (cr >> 5))
														-1);
												
				  //G = 1.16*y - 0.396*cb - 0.812*cr + 6.2;
					greenBlock[col + BLOCK_WIDTH*row] =	  squash((y + (y >> 3) + (y >> 4))
														- ((cb >> 2) + (cb >> 3) + (cb >> 6))
														- ((cr >> 1) + (cr >> 2) + (cr >> 4))
														+6);
					
				  //B = 1.16*y + 2.017*cb - 17.3;
					blueBlock[col + BLOCK_WIDTH*row] =	squash(y + (y >> 3) + (y >> 4)
														+ (cb << 1)
														-17);
				}
			}
			//Cycle-accurate timing
			wait(16);
			
			rData.write(redBlock);
			gData.write(greenBlock);
			bData.write(blueBlock);
			done = true;
		}
	}
}

