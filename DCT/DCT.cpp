#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <iostream>

#include "../common.h"
#include "DCT.hpp"

void DCT::doDCT(){
	FixedIntBlock output;
	Block input;
	
	while(true){
		wait();
		done = false;
		
		if (start){
			input = imageData.read();
			
			blockDCT(input, output);
			
			//Cycle-accurate timing
			wait(64);
			
			dctData.write(output);
			
			done = true;
		}
	}
}

//Do a 1D DCT on eight values
//Algorithm from https://arxiv.org/pdf/1402.6034.pdf
FixedInt* DCT::linearDCT(FixedInt signalArray[8]) {
	FixedInt* dctArray = new FixedInt[8];
	//cout << "First number in LinearDCT: " << convertFixedToShort(signalArray[0]) << endl;
	
	FixedInt v0 = signalArray[0] + signalArray[7];
	FixedInt v1 = signalArray[1] + signalArray[6];
	FixedInt v2 = signalArray[2] + signalArray[5];
	FixedInt v3 = signalArray[3] + signalArray[4];
	FixedInt v4 = -signalArray[4] + signalArray[3];
	FixedInt v5 = -signalArray[5] + signalArray[2];
	FixedInt v6 = -signalArray[6] + signalArray[1];
	FixedInt v7 = -signalArray[7] + signalArray[0];
	
	FixedInt w0 = v0 + v3;
	FixedInt w1 = v1 + v2;
	FixedInt w2 = -v2 + v1;
	FixedInt w3 = -v3 + v0;
	FixedInt w4 = -v4 + v5 -v6;
	FixedInt w5 = -v5 -v4 + v7;
	FixedInt w6 = -v6 + v4 + v7;
	FixedInt w7 = v7 + v6 + v5;
	
	FixedInt x0 = w0 + w1;
	FixedInt x1 = w0 - w1;
	FixedInt x2 = -w2;
	
	dctArray[0] = FI_MULT(x0,dctConstants[0]);
	dctArray[4] = FI_MULT(x1,dctConstants[1]);
	dctArray[6] = FI_MULT(x2,dctConstants[2]);
	dctArray[2] = FI_MULT(w3,dctConstants[3]);
	dctArray[7] = FI_MULT(w4,dctConstants[4]);
	dctArray[3] = FI_MULT(w5,dctConstants[5]);
	dctArray[5] = FI_MULT(w6,dctConstants[6]);
	dctArray[1] = FI_MULT(w7,dctConstants[7]);
	
	//cout << "First number out from LinearDCT: " << convertFixedToShort(dctArray[0]) << endl;
	return dctArray;
}

// Do a 2D DCT on an 8x8 block
void DCT::blockDCT(Block signalArray, FixedIntBlock& dctArray){
	FixedInt fixedSignalArray[8];
	
	FixedInt tempDCTArray[8];
	FixedInt* tempOutArray;
	// DCT along rows
	for (unsigned int row = 0; row < 8; row++){
		for (unsigned int i = 0; i < 8; i++){
			fixedSignalArray[i] = convertUCharToFixed(signalArray[row*BLOCK_WIDTH + i]);
		}
		
		tempOutArray = linearDCT(fixedSignalArray);
		
		for (unsigned int i = 0; i < 8; i++){
			dctArray[row*BLOCK_WIDTH + i] = tempOutArray[i];
		}
		delete [] tempOutArray;
	}
	
	// DCT along columns
	for (unsigned int col = 0; col < 8; col++){
		
		for (unsigned int i = 0; i < 8; i++){
			tempDCTArray[i] = dctArray[i*BLOCK_WIDTH+col];
		}
		
		tempOutArray = linearDCT(tempDCTArray);
		
		for (unsigned int i = 0; i < 8; i++){
			dctArray[i*BLOCK_WIDTH + col] = tempOutArray[i];
		}
		delete [] tempOutArray;
	}
}

void IDCT::doInverseDCT(){
	Block output;
	FixedIntBlock input;
	
	while(true){
		wait();
		
		if (start){
			done = false;
			//cout << "hey, DCT is starting" << endl;
			input = dctData.read();
			
			inverseBlockDCT(input, output);
			
			//Cycle-accurate timing
			wait(64);
			
			imageData.write(output);
			done = true;
		}
	}
}

//Do inverse DCT on eight values
FixedInt* IDCT::inverseLinearDCT(FixedInt DCTArray[8]) {
	FixedInt* signalArray = new FixedInt[8];
	
	FixedInt x0 = FI_DIV(DCTArray[0], (dctConstants[0]));
	FixedInt x1 = FI_DIV(DCTArray[4], (dctConstants[4]));
	FixedInt x2 = FI_DIV(DCTArray[6], (dctConstants[6]));
	FixedInt x3 = FI_DIV(DCTArray[2], (dctConstants[2]));
	FixedInt x4 = FI_DIV(DCTArray[7], (dctConstants[7]));
	FixedInt x5 = FI_DIV(DCTArray[3], (dctConstants[3]));
	FixedInt x6 = FI_DIV(DCTArray[5], (dctConstants[5]));
	FixedInt x7 = FI_DIV(DCTArray[1], (dctConstants[1]));

	FixedInt w0 = (x0+x1) / 2;
	FixedInt w1 = (x0-x1) / 2;
	FixedInt w2 = -x2;
	
	FixedInt v0 = (w0 + x3) / 2;
	FixedInt v1 = (w1 + w2) / 2;
	FixedInt v2 = (w1 - w2) / 2;
	FixedInt v3 = (w0 - x3) / 2;
	FixedInt v4 = (-x4 - x5 + x6) / 3;
	FixedInt v5 = (x4 - x5 + x7) / 3;
	FixedInt v6 = (-x4 -x6 + x7) / 3;
	FixedInt v7 = (x5 + x6 + x7) / 3;
	
	signalArray[0] = (v0 + v7) / 2;
	signalArray[1] = (v1 + v6) / 2;
	signalArray[2] = (v2 + v5) / 2;
	signalArray[3] = (v3 + v4) / 2;
	signalArray[4] = (v3 - v4) / 2;
	signalArray[5] = (v2 - v5) / 2;
	signalArray[6] = (v1 - v6) / 2;
	signalArray[7] = (v0 - v7) / 2;
	
	return signalArray;
}

void IDCT::inverseBlockDCT(FixedIntBlock dctArray, Block& signalArray){
	FixedInt tempDCTArray[8];
	
	FixedInt* fixedSignalArray;
	FixedInt* intermediateArray;
	FixedInt tempOutArray[8];
	FixedIntBlock tempBlock;
	
	// DCT along columns
	for (unsigned int col = 0; col < 8; col++){
		
		for (unsigned int i = 0; i < 8; i++){
			tempDCTArray[i] = dctArray[i*BLOCK_WIDTH+col];
		}
		
		intermediateArray = inverseLinearDCT(tempDCTArray);
		
		for (unsigned int i = 0; i < 8; i++){
			tempBlock[i*BLOCK_WIDTH + col] = intermediateArray[i];
		}
		delete [] intermediateArray;
	}
	
	// DCT along rows
	for (unsigned int row = 0; row < 8; row++){
		for (unsigned int i = 0; i < 8; i++){
			tempOutArray[i] = tempBlock[row*BLOCK_WIDTH + i];
		}
		
		fixedSignalArray = inverseLinearDCT(tempOutArray);
		
		for (unsigned int i = 0; i < 8; i++){
			signalArray[row*BLOCK_WIDTH + i] = convertFixedToUChar(fixedSignalArray[i]);
		}
		delete [] fixedSignalArray;
	}
}
