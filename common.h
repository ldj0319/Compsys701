#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <cmath>
#include <iostream> 
#include "systemc.h"

#define BLOCK_HEIGHT 8
#define BLOCK_WIDTH 8
#define BLOCK_SIZE (BLOCK_HEIGHT * BLOCK_WIDTH)
#define BLOCKFINAL_SIZE (BLOCK_SIZE * 8)

#define IMAGE_HEIGHT 144
#define IMAGE_WIDTH 176
#define IMAGE_DEPTH 3
#define CHANNEL_SIZE (IMAGE_HEIGHT*IMAGE_WIDTH)
#define IMAGE_SIZE (IMAGE_HEIGHT*IMAGE_WIDTH*IMAGE_DEPTH)


#define MEMORY_SIZE (IMAGE_HEIGHT * IMAGE_WIDTH * IMAGE_DEPTH)
#define MEMORY_ADDRESS_WIDTH 17 // ceil(log2(MEMORY_SIZE)) but must be a constant expression, so change this if necessary

/* Custom types */

class Block{
	private:
		uint8_t block[BLOCK_SIZE]; 
	public:
		uint8_t &operator[](int index){
			if (index >= BLOCK_SIZE){
				cout << "Block, Array index out of bounds, exiting";
				sc_assert(false);
				// exit(0);
			}
			else{
				return block[index];
			}
		}
		Block():block(){};
		friend std::ostream & operator << (ostream &out, const Block &c);
};

class Block_Encode{
	private:
		int8_t block[BLOCK_SIZE]; 
	public:
		int8_t &operator[](int index){
			if (index >= BLOCK_SIZE){
				cout << "Block_Encode, Array index out of bounds, exiting";
				sc_assert(false);
				// exit(0);
			}
			else{
				return block[index];
			}
		}
		Block_Encode():block(){};
		friend std::ostream & operator << (ostream &out, const Block_Encode &c);
};

class BlockFinal{
	private:
		uint8_t block[BLOCKFINAL_SIZE]; 
	public:
		uint8_t &operator[](int index){
			if (index >= BLOCKFINAL_SIZE){
				cout << "BlockFinal, Array index out of bounds, exiting";
				sc_assert(false);
				// exit(0);
			}
			else{
				return block[index];
			}
		}
		BlockFinal():block(){};
		friend std::ostream & operator << (ostream &out, const BlockFinal &c);
};

typedef int32_t FixedInt;

class FixedIntBlock{
	private:
		FixedInt block[BLOCK_SIZE]; 
	public:
		FixedInt &operator[](int index){
			if (index >= BLOCK_SIZE){
				cout << "FixedIntBlock, Array index out of bounds, exiting";
				sc_assert(false);
				// exit(0);
			}
			else{
				return block[index];
			}
		}
		FixedIntBlock():block(){};
		friend std::ostream & operator << (ostream &out, const Block &c);
};

std::ostream & operator << (ostream &out, const Block &c);
std::ostream & operator << (ostream &out, const Block_Encode &c);
std::ostream & operator << (ostream &out, const BlockFinal &c);
std::ostream & operator << (ostream &out, const FixedIntBlock &c);

/* FixedInt defines */
#define SHIFT_BITS (5)
#define MAX_VAL (SHRT_MAX)

#define SHIFT_VALUE (1 << SHIFT_BITS)
#define FRACTIONAL_PART (SHIFT_VALUE - 1)
#define WHOLE_PART ((MAX_VAL) - (FRACTIONAL_PART))

#define FI_MULT(x,y) ((FixedInt)((((long)(x))*y) >> SHIFT_BITS))
#define FI_DIV(x,y) ((FixedInt)((((long)(x)) << SHIFT_BITS)/y))

/* FixedInt conversion functions */

FixedInt convertDoubleToFixed(double f);
double convertFixedToDouble(FixedInt z);
int32_t convertFixedToShort(FixedInt i);
uint8_t convertFixedToUChar(FixedInt i);
FixedInt convertIntToFixed(int i);
FixedInt convertUCharToFixed(uint8_t i);
FixedInt roundFixedInt(FixedInt z);


#endif /* COMMON_H */
