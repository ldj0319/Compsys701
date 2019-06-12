#include <iostream>
#include <stdint.h>
#include <limits.h>
#include <bitset>
using namespace std;

#define SHIFT_BITS (5)
#define MAX_VAL (INT_MAX)

#define SHIFT_VALUE (1 << SHIFT_BITS)
#define FRACTIONAL_PART (SHIFT_VALUE - 1)
#define WHOLE_PART ((MAX_VAL) - (FRACTIONAL_PART))

#define FI_MULT(x,y) ((FixedInt)((((long)(x))*y) >> SHIFT_BITS))
#define FI_DIV(x,y) ((FixedInt)((((long)(x)) << SHIFT_BITS)/y))

#define BLOCK_HEIGHT 8
#define BLOCK_WIDTH 8
#define BLOCK_SIZE (BLOCK_HEIGHT * BLOCK_WIDTH)

#define IMAGE_HEIGHT 144
#define IMAGE_WIDTH 176
#define IMAGE_DEPTH 3
#define CHANNEL_SIZE (IMAGE_HEIGHT*IMAGE_WIDTH)
#define IMAGE_SIZE (IMAGE_HEIGHT*IMAGE_WIDTH*IMAGE_DEPTH)

typedef uint8_t Block[64];
typedef int32_t FixedInt;
typedef FixedInt FixedIntBlock[64];

FixedInt dctConstants[8];
double dctConstantsD[8];
FixedInt convertIntToFixed(int i){
    return (FixedInt)(i << SHIFT_BITS);
}

double convertFixedToDouble(FixedInt z){
    if (z < 0){
        cout << -(-z & FRACTIONAL_PART) << endl;
        return ((double)(z >> SHIFT_BITS) + (double)(-(-z & FRACTIONAL_PART)) / SHIFT_VALUE) + 1;
    }
    else{
        return ((double)(z >> SHIFT_BITS) + ((double)(z & FRACTIONAL_PART)) / SHIFT_VALUE);
    }
}

FixedInt roundFixedInt(FixedInt z){
    if (z < 0){
        // If fractional component < -0.5
    	if (-(-z & FRACTIONAL_PART) <= -(FRACTIONAL_PART / 2)) {
    		// Set fractional bits to zero and sub 1
    		return -(-z & WHOLE_PART) - convertIntToFixed(1);
    	}
    	else{
    		// Set fractional bits to zero
    		return -(-z & WHOLE_PART);
    	}
    }
    else{
    	// If fractional component > 0.5
    	if ((z & FRACTIONAL_PART) >= (FRACTIONAL_PART / 2)) {
    		// Set fractional bits to zero and add 1
    		return (z & WHOLE_PART) + convertIntToFixed(1);
    	}
    	else{
    		// Set fractional bits to zero
    		return (z & WHOLE_PART);
    	}
    }
}



FixedInt convertDoubleToFixed(double f){
    return (FixedInt)((f*SHIFT_VALUE));
}


int32_t convertFixedToShort(FixedInt i){
	return (roundFixedInt(i) >> SHIFT_BITS);
}

uint8_t convertFixedToUChar(FixedInt i){
	if ((roundFixedInt(i) >> SHIFT_BITS) >= UCHAR_MAX){
		return UCHAR_MAX;
	}
	else if ((roundFixedInt(i) >> SHIFT_BITS) < 0){
		return 0;
	}
	else{
		return (roundFixedInt(i) >> SHIFT_BITS);
	}
}

FixedInt convertUCharToFixed(uint8_t i){
	return (((uint16_t)i) << SHIFT_BITS);
}

//Do inverse DCT on eight values
FixedInt* inverseLinearDCT(FixedInt DCTArray[8]) {
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

//Do a 1D DCT on eight values
//Algorithm from https://arxiv.org/pdf/1402.6034.pdf
FixedInt* linearDCT(FixedInt signalArray[8]) {
	FixedInt* dctArray = new FixedInt[8];
	//cout << "First number in LinearDCT: " << convertFixedToShort(signalArray[0]) << endl;
	
	int32_t v0 = signalArray[0] + signalArray[7];
	int32_t v1 = signalArray[1] + signalArray[6];
	int32_t v2 = signalArray[2] + signalArray[5];
	int32_t v3 = signalArray[3] + signalArray[4];
	int32_t v4 = -signalArray[4] + signalArray[3];
	int32_t v5 = -signalArray[5] + signalArray[2];
	int32_t v6 = -signalArray[6] + signalArray[1];
	int32_t v7 = -signalArray[7] + signalArray[0];
	
	int32_t w0 = v0 + v3;
	int32_t w1 = v1 + v2;
	int32_t w2 = -v2 + v1;
	int32_t w3 = -v3 + v0;
	int32_t w4 = -v4 + v5 -v6;
	int32_t w5 = -v5 -v4 + v7;
	int32_t w6 = -v6 + v4 + v7;
	int32_t w7 = v7 + v6 + v5;
	
	int32_t x0 = w0 + w1;
	int32_t x1 = w0 - w1;
	int32_t x2 = -w2;
	
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

void blockDCT(Block signalArray, FixedIntBlock& dctArray){
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

void inverseBlockDCT(FixedIntBlock dctArray, FixedIntBlock& signalArray){
	FixedInt tempDCTArray[8];
	
	FixedInt* fixedSignalArray;
	FixedInt* intermediateArray;
	FixedInt tempOutArray[8];
	
	// DCT along columns
	for (unsigned int col = 0; col < 8; col++){
		
		for (unsigned int i = 0; i < 8; i++){
			tempDCTArray[i] = dctArray[i*BLOCK_WIDTH+col];
		}
		
		intermediateArray = inverseLinearDCT(tempDCTArray);
		
		for (unsigned int i = 0; i < 8; i++){
			dctArray[i*BLOCK_WIDTH + col] = intermediateArray[i];
		}
		delete [] intermediateArray;
	}
	
	// DCT along rows
	for (unsigned int row = 0; row < 8; row++){
		for (unsigned int i = 0; i < 8; i++){
			tempOutArray[i] = dctArray[row*BLOCK_WIDTH + i];
		}
		
		fixedSignalArray = inverseLinearDCT(tempOutArray);
		
		for (unsigned int i = 0; i < 8; i++){
			signalArray[row*BLOCK_WIDTH + i] = fixedSignalArray[i];
		}
		delete [] fixedSignalArray;
	}
}


int main() {
    dctConstants[0] = convertDoubleToFixed(0.353);
	dctConstants[1] = convertDoubleToFixed(0.408);
	dctConstants[2] = convertDoubleToFixed(0.500);
	dctConstants[3] = convertDoubleToFixed(0.408);
	dctConstants[4] = convertDoubleToFixed(0.353);
	dctConstants[5] = convertDoubleToFixed(0.408);
	dctConstants[6] = convertDoubleToFixed(0.500);
	dctConstants[7] = convertDoubleToFixed(0.408);
	
	dctConstantsD[0] = (0.3536);
	dctConstantsD[1] = (0.4082);
	dctConstantsD[2] = (0.5000);
	dctConstantsD[3] = (0.4082);
	dctConstantsD[4] = (0.3536);
	dctConstantsD[5] = (0.4082);
	dctConstantsD[6] = (0.5000);
	dctConstantsD[7] = (0.4082);
	
	FixedInt test[8];
	test[0] = convertDoubleToFixed(1);
	test[1] = convertDoubleToFixed(1);
	test[2] = convertDoubleToFixed(1);
	test[3] = convertDoubleToFixed(1);
	test[4] = convertDoubleToFixed(1);
	test[5] = convertDoubleToFixed(1);
	test[6] = convertDoubleToFixed(1);
	test[7] = convertDoubleToFixed(1);
	
    FixedInt* testOut = inverseLinearDCT(test);
    for (uint16_t i = 0; i < 8; i++){
        printf("%9.4f ", convertFixedToDouble(testOut[i]));
    }
    cout << endl;
	
	Block signal;
	FixedIntBlock output;
	for (uint8_t i = 0; i < 64; i++){
	    signal[i] = i;
	}
	FixedIntBlock dct;
	
    cout << "Original:";
    for (uint16_t i = 0; i < 64; i++){
        if (i % 8 == 0){
            cout << endl;
        }
        printf("%02d ", signal[i]);
    }
    
    blockDCT(signal, dct);
    cout << endl << endl << "DCT:";
    for (uint16_t i = 0; i < 64; i++){
        if (i % 8 == 0){
            cout << endl;
        }
        printf("%9.4f ", convertFixedToDouble(dct[i]));
    }
    inverseBlockDCT(dct, output);
     cout << endl << endl << "Output:";
    for (uint16_t i = 0; i < 64; i++){
        if (i % 8 == 0){
            cout << endl;
        }
        printf("%02d ", (convertFixedToUChar(output[i])));
    }
}
