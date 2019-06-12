// encoding.cpp

#include "encoding.h"

void encoding::Insert(node * element, int test_size)
{
	int now = test_size;

	heap[now] = element;
	while(heap[now/2] -> freq > element -> freq)
	{
			heap[now] = heap[now/2];
			now /= 2;
	}
	heap[now] = element;
}


void encoding::print(int size, node *temp,char *code)
{
        if(temp->left==NULL && temp->right==NULL)
        {
			char* test = code;
			inputV[count] = temp->ch;
			test_Arr[count] = temp->ch;

			inputC0[count] = 7;
			inputC1[count] = 7;
			inputC2[count] = 7;
			inputC3[count] = 7;
			inputC4[count] = 7;
			inputC5[count] = 7;
			inputC6[count] = 7;
			inputC7[count] = 7;
			inputC8[count] = 7;
			if(strlen(test)>0){
				inputC0[count] = int(test[0] -48);
			}
			if(strlen(test)>1){
				inputC1[count] = int(test[1] -48);
			}
			if(strlen(test)>2){
				inputC2[count] = int(test[2] -48);
			}
			if(strlen(test)>3){
				inputC3[count] = int(test[3] -48);
			}
			if(strlen(test)>4){
				inputC4[count] = int(test[4] -48);
			}
			if(strlen(test)>5){
				inputC5[count] = int(test[5] -48);
			}
			if(strlen(test)>6){
				inputC6[count] = int(test[6] -48);
			}
			if(strlen(test)>7){
				inputC7[count] = int(test[7] -48);
			}
			if(strlen(test)>8){
				inputC8[count] = int(test[8] -48);
			}
			
			std::cout << "Value: " << test_Arr[count] << "           Code: " << test << std::endl;

			count++;
			
            return;
        }
		
        int length = strlen(code);
        char leftcode[64],rightcode[64];
        strcpy(leftcode,code);
        strcpy(rightcode,code);
        leftcode[length] = '0';
        leftcode[length+1] = '\0';
        rightcode[length] = '1';
        rightcode[length+1] = '\0';
        print(size, temp->left,leftcode);
        print(size, temp->right,rightcode);
} 

void encoding::main()
{	
	while(true){
		wait();
		done = false;
		if (start){
			const int arraySize = 64;
			int arr[64];
			data_in = in.read();
			std::cout << "Quant Output: ";
			for(int i=0; i<arraySize; i++){
				arr[i] = data_in[i];
				std::cout << arr[i] << " ";
			}
			std::cout << std::endl;
			const int UniqueSymbols = 1 << CHAR_BIT;
			
			int values[] = {};
			int frequent[] = {};
			int test_size = 0;
			
			int frequencies[UniqueSymbols] = {0};
			const int *ptr = &arr[0];
			const int *newPtr = &arr[0];
			int num = 0;
			
			for(int i = 0; i < arraySize; i++) {  
				frequencies[*ptr] = 0;
				*ptr++;
			}
			
			for(int i = 0; i < arraySize; i++) {  
				if(frequencies[*newPtr] == 0){
					values[num] = *newPtr;
					num++;
				}
				++frequencies[*newPtr++];
			}
			heap[0] = (node *)malloc(sizeof(node));
			heap[0]->freq = 0;
			const int n = num;
			int ch;
			int freq;
			int test0 = test_size;
			for(int i=0;i<n;i++)
			{
				test_size++;
				ch = values[i];
				// inputV[i] = ch;
				
				freq = frequencies[values[i]];
				// inputC3[i] = freq;
				node * temp = (node *) malloc(sizeof(node));
				temp -> ch = ch;
				temp -> freq = freq;
				temp -> left = temp -> right = NULL;
				Insert(temp, test_size);
			}
			int test1 = test_size;
			for(int i=0;i<n-1 ;i++)
			{
				
				node * minElement,*lastElement;
				int child,now;
				minElement = heap[1];
				lastElement = heap[test_size--];
				for(now = 1; now*2 <= test_size ;now = child)
				{
					 child = now*2;
						 if(child != test_size && heap[child+1]->freq < heap[child] -> freq )
						{
								child++;
						}
						if(lastElement -> freq > heap[child] -> freq)
						{
								heap[now] = heap[child];
						}
						else
						{
								break;
						}
				}
				heap[now] = lastElement;
				node * left = minElement;
				
				minElement = heap[1];
				lastElement = heap[test_size--];
				for(now = 1; now*2 <= test_size ;now = child)
				{
					 child = now*2;
						 if(child != test_size && heap[child+1]->freq < heap[child] -> freq )
						{
								child++;
						}
						if(lastElement -> freq > heap[child] -> freq)
						{
								heap[now] = heap[child];
						}
						else
						{
								break;
						}
				}
				heap[now] = lastElement;
				node * right = minElement;
				/*
				node * left = DeleteMin(test_size);
				node * right = DeleteMin(test_size);
				*/
				node * temp = (node *) malloc(sizeof(node));
				temp -> ch = 0;
				temp -> left = left;
				temp -> right = right;
				temp -> freq = left-> freq + right -> freq;
				test_size++;
				Insert(temp, test_size);
			}
			int test2 = test_size;
			node * minElement,*lastElement;
			int child,now;
			minElement = heap[1];
			lastElement = heap[test_size--];
			for(now = 1; now*2 <= test_size ;now = child)
			{
				 child = now*2;
					 if(child != test_size && heap[child+1]->freq < heap[child] -> freq )
					{
							child++;
					}
					if(lastElement -> freq > heap[child] -> freq)
					{
							heap[now] = heap[child];
					}
					else
					{
							break;
					}
			}
			heap[now] = lastElement;
			node *tree = minElement;
			char code[200];
			code[0] = '\0';	
			// testBlock = outputV.read();
			int test3 = test_size;
			count = 0;
			print(arraySize, tree, code);
			if(n==1){
				inputV[0] = ch;
				test_Arr[0] = ch;
				inputC0[0] = 0;
				count = 1;
			}
			finalCount = 0;	
			std::cout << "VLE ";			
			for(int i=0; i<arraySize; i++){
				for(int j=0; j<count; j++){
					if(arr[i] == test_Arr[j]){
						if(inputC0[j] == 0 or inputC0[j] ==1){
							outputFinal[finalCount] = inputC0[j];
							finalCount++;
						}
						if(inputC1[j] == 0 or inputC1[j] ==1){
							outputFinal[finalCount] = inputC1[j];

							finalCount++;
						}
						if(inputC2[j] == 0 or inputC2[j] ==1){
							outputFinal[finalCount] = inputC2[j];

							finalCount++;
						}
						if(inputC3[j] == 0 or inputC3[j] ==1){
							outputFinal[finalCount] = inputC3[j];

							finalCount++;
						}
						if(inputC4[j] == 0 or inputC4[j] ==1){
							outputFinal[finalCount] = inputC4[j];

							finalCount++;
						}
						if(inputC5[j] == 0 or inputC5[j] ==1){
							outputFinal[finalCount] = inputC5[j];

							finalCount++;
						}
						if(inputC6[j] == 0 or inputC6[j] ==1){
							outputFinal[finalCount] = inputC6[j];

							finalCount++;
						}
						if(inputC7[j] == 0 or inputC7[j] ==1){
							outputFinal[finalCount] = inputC7[j];

							finalCount++;
						}
						if(inputC8[j] == 0 or inputC8[j] ==1){
							outputFinal[finalCount] = inputC8[j];

							finalCount++;
						}
						std::cout << " ";
					}
				}
			}
			outputFinal[finalCount] = 7;
			std::cout << std::endl;
			valueNum.write(num);
			outputV.write(inputV);
			outputC0.write(inputC0);
			outputC1.write(inputC1);
			outputC2.write(inputC2);
			outputC3.write(inputC3);
			outputC4.write(inputC4);
			outputC5.write(inputC5);
			outputC6.write(inputC6);
			outputC7.write(inputC7);
			outputC8.write(inputC8);
			outputFinalCode.write(outputFinal);

			done = true;	
		}	
	}
}
