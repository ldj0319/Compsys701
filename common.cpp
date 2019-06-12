#include "common.h"

std::ostream & operator << (ostream &out, const Block &c)
{
    out << "Hey this is a Block object";
    return out;
}
std::ostream & operator << (ostream &out, const Block_Encode &c)
{
    out << "Hey this is a Block object";
    return out;
}
std::ostream & operator << (ostream &out, const BlockFinal &c)
{
    out << "Hey this is a BlockFinal object";
    return out;
}
std::ostream & operator << (ostream &out, const FixedIntBlock &c)
{
    out << "Hey this is a FixedIntBlock object";
    return out;
}

FixedInt convertIntToFixed(int i){
    return (FixedInt)(i << SHIFT_BITS);
}

FixedInt roundFixedInt(FixedInt z){
    if (z < 0){
        // If fractional component > 0.5
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

double convertFixedToDouble(FixedInt z){
    if (z < 0){
        return ((double)(z >> SHIFT_BITS) + (double)(-(-z & FRACTIONAL_PART)) / SHIFT_VALUE) + 1;
    }
    else{
        return ((double)(z >> SHIFT_BITS) + ((double)(z & FRACTIONAL_PART)) / SHIFT_VALUE);
    }
}

FixedInt convertDoubleToFixed(double f){
    return (FixedInt)((f*SHIFT_VALUE));
}


int32_t convertFixedToShort(FixedInt i){
	return (i >> SHIFT_BITS);
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
