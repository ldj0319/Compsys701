// decoding.cpp

#include "decoding.h"

void decoding::main()
{	
	
	while(true){
		wait();
		done = false;
		if (start){
			//std::cout << "Decoding begin" << std::endl;
			data_in = inputFinalCode.read();
			int arraySize=0;
			for(int k=0; k<512; k++){
				if(data_in[k] == 7){
					arraySize = k;
					k=512;
				}
			}
			std::cout << "ArraySize: " << arraySize << std::endl;

			data_V = inputValue.read();
			data_C0 = inputTable0.read();
			data_C1 = inputTable1.read();
			data_C2 = inputTable2.read();
			data_C3 = inputTable3.read();
			data_C4 = inputTable4.read();
			data_C5 = inputTable5.read();
			data_C6 = inputTable6.read();
			data_C7 = inputTable7.read();
			data_C8 = inputTable8.read();
			// This line was missing:
			
			//However when included, the code tries to access the 65th element of data_out.
			//Just reading the fifo prevents everything from filling up, and produces some kind of output at least.
			// inputTable8.read();
			
			int inputCount = valueNum.read();
			int* inputV = new int[inputCount];
			int* inputC0 = new int[inputCount];
			int* inputC1 = new int[inputCount];
			int* inputC2 = new int[inputCount];
			int* inputC3 = new int[inputCount];
			int* inputC4 = new int[inputCount];
			int* inputC5 = new int[inputCount];
			int* inputC6 = new int[inputCount];
			int* inputC7 = new int[inputCount];
			int* inputC8 = new int[inputCount];
			
			for(int k=0; k<inputCount; k++){
				inputV[k] = data_V[k];
				inputC0[k] = data_C0[k];
				inputC1[k] = data_C1[k];
				inputC2[k] = data_C2[k];
				inputC3[k] = data_C3[k];
				inputC4[k] = data_C4[k];
				inputC5[k] = data_C5[k];
				inputC6[k] = data_C6[k];
				inputC7[k] = data_C7[k];
				inputC8[k] = data_C8[k];
			}
			
			int arr[64];
			int temp[64];
			
			int tempCount;
			int i =0;
			int outCount =0;
			while(i<arraySize){
				int count = inputCount;
				tempCount = 0;
				for(int j=0; j<count; j++){
					if(data_in[i] == inputC0[j]){
						temp[tempCount] = j;
						tempCount++;
												
					}
				}
				i++;
				if(tempCount == 1){
					data_out[outCount] = inputV[temp[0]];
					// std::cout << "i: " << i << " outCount: " << outCount << " inputV[temp[0]]: " << inputV[temp[0]] << std::endl;
					outCount++;
				}
				else{
					count = tempCount;
					tempCount = 0;
					for(int j=0; j<count; j++){
						if(data_in[i] == inputC1[temp[j]]){
							temp[tempCount] = temp[j];
							tempCount++;
						}
					}
					i++;
					if(tempCount == 1){
						data_out[outCount] = inputV[temp[0]];
						outCount++;
					}
					else{
						count = tempCount;
						tempCount = 0;
						for(int j=0; j<count; j++){
							if(data_in[i] == inputC2[temp[j]]){
								temp[tempCount] = temp[j];
								tempCount++;
							}
						}
						i++;
						if(tempCount == 1){
							data_out[outCount] = inputV[temp[0]];
							outCount++;
						}
						else{
							count = tempCount;
							tempCount = 0;
							for(int j=0; j<count; j++){
								if(data_in[i] == inputC3[temp[j]]){
									temp[tempCount] = temp[j];
									tempCount++;
								}
							}
							i++;
							if(tempCount == 1){
								data_out[outCount] = inputV[temp[0]];
								outCount++;
							}
							else{
								count = tempCount;
								tempCount = 0;
								for(int j=0; j<count; j++){
									if(data_in[i] == inputC4[temp[j]]){
										temp[tempCount] = temp[j];
										tempCount++;
									}
								}
								i++;
								if(tempCount == 1){
									data_out[outCount] = inputV[temp[0]];
									outCount++;
								}
								else{
									count = tempCount;
									tempCount = 0;
									for(int j=0; j<count; j++){
										if(data_in[i] == inputC5[temp[j]]){
											temp[tempCount] = temp[j];
											tempCount++;
										}
									}
									i++;
									if(tempCount == 1){
										data_out[outCount] = inputV[temp[0]];
										outCount++;
									}
									else{
										count = tempCount;
										tempCount = 0;
										for(int j=0; j<count; j++){
											if(data_in[i] == inputC6[temp[j]]){
												temp[tempCount] = temp[j];
												tempCount++;
											}
										}
										i++;
										if(tempCount == 1){
											data_out[outCount] = inputV[temp[0]];
											outCount++;
										}
										else{
											count = tempCount;
											tempCount = 0;
											for(int j=0; j<count; j++){
												if(data_in[i] == inputC7[temp[j]]){
													temp[tempCount] = temp[j];
													tempCount++;
												}
											}
											i++;
											if(tempCount == 1){
												data_out[outCount] = inputV[temp[0]];
												outCount++;
											}
											else{
												count = tempCount;
												tempCount = 0;
												for(int j=0; j<count; j++){
													if(data_in[i] == inputC8[temp[j]]){
														temp[tempCount] = temp[j];
														tempCount++;
													}
												}
												i++;
												if(tempCount == 1){
													data_out[outCount] = inputV[temp[0]];
													outCount++;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			
			delete[] inputV;
			delete[] inputC0;
			delete[] inputC1;
			delete[] inputC2;
			delete[] inputC3;
			delete[] inputC4;
			delete[] inputC5;
			delete[] inputC6;
			delete[] inputC7;
			delete[] inputC8;
			
			outputFinal.write(data_out);
			done = true;	
			//std::cout << "Decoding end" << std::endl;
		}	
	} 
	
}
