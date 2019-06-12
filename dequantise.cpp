// dequantise.cpp

#include "dequantise.h"


void dequantise::my_dequant()
{
	
	while(true){
		wait();
		done = false;
		if (start){
			data_in = in.read();
			std::cout << "Dequant output:";
			FixedInt temp;
			for(int i=0; i<64; i++){
				temp = convertIntToFixed(data_in[i]);
				calcBlock[i] = temp * 16;
				std::cout << " " << calcBlock[i];
			}
			out.write(calcBlock);
			std::cout << std::endl <<std::endl;
			wait(SC_ZERO_TIME);
			done = true;
		}
	}
	
}

