// quantisation.cpp

#include "quantisation.h"
#include <stdio.h>
#include <string.h>

void quantisation::my_quantisation()
{
	
	while(true){
		wait();
		done = false;
		if (start){
			data_in = in.read();
			int32_t temp;
			std::cout << "DCT output: ";
			for(int i=0; i<64; i++){
				temp = convertFixedToShort(data_in[i]);
				if(strcmp(name(),"top/my_quantCr") == 0){
					std::cout << " " << temp;
				}
				
				calcBlock[i] = temp / 16;
			}
			std::cout << std::endl;
			out.write(calcBlock);
			wait(SC_ZERO_TIME);
			done = true;
		}
	}
	
}

