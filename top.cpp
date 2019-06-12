#include <stdint.h>
#include "systemc.h"
#include "common.h"
#include "DCT/DCT.hpp"
#include "DCT/ycbcr.hpp"
#include "Memory/memory.hpp"
#include "quantisation.h"
#include "encoding.h"
#include "decoding.h"
#include "dequantise.h"
#define NUM_FRAMES 8
#define CLOCK_PERIOD_NS 20

SC_MODULE(Consumer){
	sc_fifo_in<FixedIntBlock> input;
	sc_in<bool> clock;
	
	void consume();
	
	SC_CTOR(Consumer){
		SC_THREAD(consume){
			sensitive << clock.pos();
		}
	}
};
void Consumer::consume(){
	while (true){
		wait();
		input.read();
	}
}

SC_MODULE(Producer){
	sc_fifo_out<Block> output;
	sc_in<bool> clock;
	
	void produce();
	
	SC_CTOR(Producer){
		SC_THREAD(produce){
			sensitive << clock.pos();
		}
	}
};
void Producer::produce(){
	Block block;
	while(true){
		wait();
		for (uint8_t i = 0; i < BLOCK_SIZE; i++){
			block[i] = block[i] + 1;
		}
		output.write(block);
	}
}

SC_MODULE(NotGate){
	sc_in< bool > in;
	sc_out< bool > out;
	
	void generateNot(){ out.write(~in.read()); }
	
	SC_CTOR(NotGate){
		SC_METHOD(generateNot){
			sensitive << in;
		}
	}
};

SC_MODULE(AndGate){
	sc_in< bool > a, b;
	sc_out< bool > out;
	
	void generateAnd(){ out.write(a.read() and b.read()); }
	
	SC_CTOR(AndGate){
		SC_METHOD(generateAnd){
			sensitive << a << b;
		}
	}
};

SC_MODULE(OrGate){
	sc_in< bool > a, b;
	sc_out< bool > out;
	
	void generateOr(){ out.write(a.read() or b.read()); }
	
	SC_CTOR(OrGate){
		SC_METHOD(generateOr){
			sensitive << a << b;
		}
	}
};

SC_MODULE(Mux2to1){
	sc_in< uint8_t > a, b;
	sc_in< bool > sel;
	sc_out< uint8_t > out;
	
	
	void generateMux(){ if (sel){out.write(b.read());} else{out.write(a.read());}}
	
	SC_CTOR(Mux2to1){
		SC_METHOD(generateMux){
			sensitive << a << b;
			sensitive << sel;
		}
	}
};

SC_MODULE(top){
	sc_signal<bool> readerDone, ycbcrDone, dctYDone, dctCbDone, dctCrDone, vlcDone, writerDone;
	sc_signal<bool> t_start, writerChipSelect, notWriterChipSelect, memoryWriterStart;
	sc_signal<bool> alwaysOn, alwaysOff;
	
	sc_signal<bool> startNextFrame, writerDataValid, MotionEstimationMemory1WriteEn, MotionEstimationMemory2WriteEn;
	sc_signal<uint8_t> frameMemoryValue, inputMemoryValue, memoryWriterValueOut, MotionEstimationMemory1ValueOut, MotionEstimationMemory2ValueOut;
	sc_signal<uint8_t> motionEstimationInput1, motionEstimationInput2;
	sc_signal< sc_uint<MEMORY_ADDRESS_WIDTH> > inputAddressLine, memoryWriterAddressOut;
	
	
	// Inverse signals
	sc_signal<bool> inverseRWriterDone, inverseGWriterDone, inverseBWriterDone;
	sc_signal<bool> inverseRWriterCS, inverseGWriterCS, inverseBWriterCS;
	sc_signal<bool> inverseRWriterDataValid, inverseGWriterDataValid, inverseBWriterDataValid;
	sc_signal< sc_uint<MEMORY_ADDRESS_WIDTH> > inverseRWriterAddress, inverseGWriterAddress, inverseBWriterAddress;
	sc_signal<uint8_t> inverseRWriterValueOut, inverseGWriterValueOut, inverseBWriterValueOut;
	sc_signal<uint8_t> inverseMemoryRValueOut, inverseMemoryGValueOut, inverseMemoryBValueOut;
	sc_signal<bool> idctYDone, idctCbDone, idctCrDone, inverseYCbCrDone;
	
	//Default array for memory blocks. 
	//The actual image array is too large for the stack and must be instantiated on the heap later, potentially after the memory block instantiation
	uint8_t placeholderArray[2];
	uint8_t* inputFrame;
	uint8_t* motionEstimationFrame1;
	uint8_t* motionEstimationFrame2;
	uint8_t* inverseMemoryRFrame;
	uint8_t* inverseMemoryGFrame;
	uint8_t* inverseMemoryBFrame;
	
	uint8_t currentFrame;
	
	void updateFrame(); // Reads in a new frame every 33.3 ms
	void writeOutput(); // Debug output. Currently writes contents of ME memory block 1
	
	sc_fifo<Block> fifoR;
	sc_fifo<Block> fifoG;
	sc_fifo<Block> fifoB;
	
	sc_fifo<Block> fifoY;
	sc_fifo<Block> fifoCb;
	sc_fifo<Block> fifoCr;
	
	sc_fifo<FixedIntBlock> fifoYDCT;
	sc_fifo<FixedIntBlock> fifoCbDCT;
	sc_fifo<FixedIntBlock> fifoCrDCT;
	
	sc_clock t_clock;  // clock
	
	Memory frameMemory;
	BlockReader inputReader;
	
	sc_fifo<Block_Encode> quantY, quantCb, quantCr;
	sc_fifo<FixedIntBlock> dequantY, dequantCb, dequantCr;
	
	sc_signal<bool> vlc_YDone, quant_YDone, dequant_YDone, vlc_CbDone, quant_CbDone, dequant_CbDone, vlc_CrDone, quant_CrDone, dequant_CrDone;
	sc_signal<bool> encoding_YDone, decoding_YDone, encoding_CbDone, decoding_CbDone, encoding_CrDone, decoding_CrDone;
	sc_signal<sc_int<8> > valueNumY, valueNumCb, valueNumCr;
	
	sc_fifo<Block_Encode> valueY;
	sc_fifo<Block> codeY0;
	sc_fifo<Block> codeY1;
	sc_fifo<Block> codeY2;
	sc_fifo<Block> codeY3;
	sc_fifo<Block> codeY4;
	sc_fifo<Block> codeY5;
	sc_fifo<Block> codeY6;
	sc_fifo<Block> codeY7;
	sc_fifo<Block> codeY8;
	sc_fifo<BlockFinal> vleCodeY;
	sc_fifo<Block_Encode> outputFinalY;
	
	sc_fifo<Block_Encode> valueCb;
	sc_fifo<Block> codeCb0;
	sc_fifo<Block> codeCb1;
	sc_fifo<Block> codeCb2;
	sc_fifo<Block> codeCb3;
	sc_fifo<Block> codeCb4;
	sc_fifo<Block> codeCb5;
	sc_fifo<Block> codeCb6;
	sc_fifo<Block> codeCb7;
	sc_fifo<Block> codeCb8;
	sc_fifo<BlockFinal> vleCodeCb;
	sc_fifo<Block_Encode> outputFinalCb;
	
	sc_fifo<Block_Encode> valueCr;
	sc_fifo<Block> codeCr0;
	sc_fifo<Block> codeCr1;
	sc_fifo<Block> codeCr2;
	sc_fifo<Block> codeCr3;
	sc_fifo<Block> codeCr4;
	sc_fifo<Block> codeCr5;
	sc_fifo<Block> codeCr6;
	sc_fifo<Block> codeCr7;
	sc_fifo<Block> codeCr8;
	sc_fifo<BlockFinal> vleCodeCr;
	sc_fifo<Block_Encode> outputFinalCr;
	
	YCbCr ycbcr;
	
	DCT dctY;
	DCT dctCb;
	DCT dctCr;
	
	Consumer yDCTConsumer;
	Consumer cbDCTConsumer;
	Consumer crDCTConsumer;
	
	quantisation my_quantY;
	quantisation my_quantCb;
	quantisation my_quantCr;
	
	encoding my_encodingY;
	encoding my_encodingCb;
	encoding my_encodingCr;
	
	decoding my_decodingY;
	decoding my_decodingCb;
	decoding my_decodingCr;
	
	dequantise my_dequantY;
	dequantise my_dequantCb;
	dequantise my_dequantCr;
	
	// Leading into Motion Estimation
	sc_fifo<Block> fifoYMotionEstimation;
	BlockWriter motionEstimationWriter;
	Memory motionEstimationMemory1;
	Memory motionEstimationMemory2;
	
	NotGate notGate;
	AndGate andGate1;
	AndGate andGate2;
	OrGate orGate1;
	Mux2to1 mux1;
	Mux2to1 mux2;
	
	// Inverse to show correctness
	
	sc_fifo<FixedIntBlock> fifoYDCTToInverse;
	sc_fifo<FixedIntBlock> fifoCbDCTToInverse;
	sc_fifo<FixedIntBlock> fifoCrDCTToInverse;
	
	IDCT idctY;
	IDCT idctCb;
	IDCT idctCr;
	
	sc_fifo<Block> fifoYToInverse;
	sc_fifo<Block> fifoCbToInverse;
	sc_fifo<Block> fifoCrToInverse;
	
	YCbCrToRGB inverseYCbCr;
	
	sc_fifo<Block> fifoRToInverse;
	sc_fifo<Block> fifoGToInverse;
	sc_fifo<Block> fifoBToInverse;
	
	BlockWriter inverseRWriter;
	BlockWriter inverseGWriter;
	BlockWriter inverseBWriter;
	
	Memory inverseMemoryR;
	Memory inverseMemoryG;
	Memory inverseMemoryB;
	
	SC_CTOR(top) :
		t_clock("clock", CLOCK_PERIOD_NS),
		frameMemory("inputMemory", placeholderArray, 2),
		inputReader("inputReader"),
		ycbcr("ycbcr"),
		dctY("dctY"),
		dctCb("dctCb"),
		dctCr("dctCr"),
		yDCTConsumer("yDCTConsumer"),
		cbDCTConsumer("cbDCTConsumer"),
		crDCTConsumer("crDCTConsumer"),
		fifoYMotionEstimation("fifoYMotionEstimation"),
		motionEstimationWriter("motionEstimationWriter"),
		motionEstimationMemory1("motionEstimationMemory1", placeholderArray, 2),
		motionEstimationMemory2("motionEstimationMemory2", placeholderArray, 2),
		notGate("notGate"),
		andGate1("andGate1"),
		andGate2("andGate2"),
		mux1("mux1"),
		mux2("mux2"),
		orGate1("orGate1"),
		idctY("idctY"),
		idctCb("idctCb"),
		idctCr("idctCr"),
		inverseYCbCr("inverseYCbCr"),
		inverseRWriter("inverseRWriter"),
		inverseGWriter("inverseGWriter"),
		inverseBWriter("inverseBWriter"),
		inverseMemoryR("inverseMemoryR", placeholderArray, 2),
		inverseMemoryG("inverseMemoryG", placeholderArray, 2),
		inverseMemoryB("inverseMemoryB", placeholderArray, 2),
		my_quantY("my_quantY"),
		my_quantCb("my_quantCb"),
		my_quantCr("my_quantCr"),
		my_encodingY("my_encodingY"),
		my_encodingCb("my_encodingCb"),
		my_encodingCr("my_encodingCr"),
		my_decodingY("my_decodingY"),
		my_decodingCb("my_decodingCb"),
		my_decodingCr("my_decodingCr"),
		my_dequantY("my_dequantY"),
		my_dequantCb("my_dequantCb"),
		my_dequantCr("my_dequantCr")
	{ 
		SC_THREAD(writeOutput);
		SC_THREAD(updateFrame);
		t_start = true;
		alwaysOn = true;
		alwaysOff = false;
		startNextFrame = true;
		inputMemoryValue = 0;
		motionEstimationFrame1 = new uint8_t[IMAGE_WIDTH * IMAGE_HEIGHT];
		motionEstimationFrame2 = new uint8_t[IMAGE_WIDTH * IMAGE_HEIGHT];
		motionEstimationMemory1.setMemoryPointer(motionEstimationFrame1, IMAGE_WIDTH * IMAGE_HEIGHT);
		motionEstimationMemory2.setMemoryPointer(motionEstimationFrame2, IMAGE_WIDTH * IMAGE_HEIGHT);
		
		inverseMemoryRFrame = new uint8_t[IMAGE_WIDTH * IMAGE_HEIGHT];
		inverseMemoryGFrame = new uint8_t[IMAGE_WIDTH * IMAGE_HEIGHT];
		inverseMemoryBFrame = new uint8_t[IMAGE_WIDTH * IMAGE_HEIGHT];
		inverseMemoryR.setMemoryPointer(inverseMemoryRFrame, IMAGE_WIDTH * IMAGE_HEIGHT);
		inverseMemoryG.setMemoryPointer(inverseMemoryGFrame, IMAGE_WIDTH * IMAGE_HEIGHT);
		inverseMemoryB.setMemoryPointer(inverseMemoryBFrame, IMAGE_WIDTH * IMAGE_HEIGHT);
		
		//Temporary RGB Producers
		frameMemory.clock(t_clock);
		frameMemory.addressIn(inputAddressLine);
		frameMemory.enableWrite(alwaysOff);
		frameMemory.valueIn(inputMemoryValue);
		frameMemory.chipSelect(alwaysOn);
		frameMemory.valueOut(frameMemoryValue);
		
		inputReader.clock(t_clock);
		inputReader.start(startNextFrame);
		inputReader.memValue(frameMemoryValue);
		inputReader.memAddress(inputAddressLine);
		inputReader.channelOneData(fifoR);
		inputReader.channelTwoData(fifoG);
		inputReader.channelThreeData(fifoB);
		inputReader.done(readerDone);
		
		//YCbCr inputs
		ycbcr.clock(t_clock);
		ycbcr.rData(fifoR);
		ycbcr.gData(fifoG);
		ycbcr.bData(fifoB);
		ycbcr.start(t_start);
		
		//YCbCr outputs
		ycbcr.yData(fifoY);
		ycbcr.alsoYData(fifoYMotionEstimation);
		ycbcr.cbData(fifoCb);
		ycbcr.crData(fifoCr);
		ycbcr.done(ycbcrDone);
		
		
		//Y-channel DCT inputs
		dctY.start(ycbcrDone);
		dctY.clock(t_clock);
		dctY.imageData(fifoY);
		
		//outputs
		dctY.dctData(fifoYDCT);
		dctY.done(dctYDone);
		
		
		//Cb-channel DCT inputs
		dctCb.start(ycbcrDone);
		dctCb.clock(t_clock);
		dctCb.imageData(fifoCb);
		
		//outputs
		dctCb.dctData(fifoCbDCT);
		dctCb.done(dctCbDone);
		
		
		//Cr-channel DCT inputs
		dctCr.start(ycbcrDone);
		dctCr.clock(t_clock);
		dctCr.imageData(fifoCr);
		
		//outputs
		dctCr.dctData(fifoCrDCT);
		dctCr.done(dctCrDone);
		
		/*
		//Temporary DCT Consumers
		yDCTConsumer.clock(t_clock);
		yDCTConsumer.input(fifoYDCT);
		
		cbDCTConsumer.clock(t_clock);
		cbDCTConsumer.input(fifoCbDCT);
		
		crDCTConsumer.clock(t_clock);
		crDCTConsumer.input(fifoCrDCT);
		*/
		// memoryWriter start signal
		orGate1.a(startNextFrame);
		orGate1.b(alwaysOff /*Replace with MotionEstimation done signal*/);
		orGate1.out(memoryWriterStart);
		
		// Motion Estimation stuff
		motionEstimationWriter.clock(t_clock);
		motionEstimationWriter.start(memoryWriterStart);
		motionEstimationWriter.dataValid(writerDataValid);
		motionEstimationWriter.chipSelect(writerChipSelect);
		motionEstimationWriter.memAddress(memoryWriterAddressOut);
		motionEstimationWriter.memValue(memoryWriterValueOut);
		motionEstimationWriter.data(fifoYMotionEstimation);
		motionEstimationWriter.done(writerDone /*Hook this up to the start signal on the Motion Estimation block*/);
		
		// Memory block 1 WriteEnable = writeEnable AND chipSelect
		// Memory block 2 writeEnable = writeEnable AND NOT chipSelect
		
		andGate1.a(writerChipSelect);
		andGate1.b(writerDataValid);
		andGate1.out(MotionEstimationMemory1WriteEn);
		
		andGate2.a(notWriterChipSelect);
		andGate2.b(writerDataValid);
		andGate2.out(MotionEstimationMemory2WriteEn);
		
		notGate.in(writerChipSelect);
		notGate.out(notWriterChipSelect);
		
		motionEstimationMemory1.clock(t_clock);
		motionEstimationMemory1.enableWrite(MotionEstimationMemory1WriteEn);
		motionEstimationMemory1.addressIn(memoryWriterAddressOut);
		motionEstimationMemory1.valueIn(memoryWriterValueOut);
		motionEstimationMemory1.chipSelect(alwaysOn);
		motionEstimationMemory1.valueOut(MotionEstimationMemory1ValueOut );
		
		motionEstimationMemory2.clock(t_clock);
		motionEstimationMemory2.enableWrite(MotionEstimationMemory2WriteEn);
		motionEstimationMemory2.addressIn(memoryWriterAddressOut);
		motionEstimationMemory2.valueIn(memoryWriterValueOut);
		motionEstimationMemory2.chipSelect(alwaysOn);
		motionEstimationMemory2.valueOut(MotionEstimationMemory2ValueOut);
		
		// Since we are alternating which memory block we are writing to each frame, we need some additional logic
		// to make sure that the ME block always sees input1 as the old frame and input2 as the new frame, for example.
		mux1.a(MotionEstimationMemory1ValueOut);
		mux1.b(MotionEstimationMemory2ValueOut);
		mux1.sel(writerChipSelect);
		mux1.out(motionEstimationInput1 /*Connect to Motion Estimation Block*/);
		
		mux2.a(MotionEstimationMemory2ValueOut);
		mux2.b(MotionEstimationMemory1ValueOut);
		mux2.sel(writerChipSelect);
		mux2.out(motionEstimationInput2 /*Connect to Motion Estimation Block*/);
		
		// FIFOs between DCT and inverse DCT
		/*
		dctY.toInverseData(fifoYDCTToInverse);
		dctCb.toInverseData(fifoCbDCTToInverse);
		dctCr.toInverseData(fifoCrDCTToInverse);
		*/
		
		// Inverse DCT
		idctY.clock(t_clock);
		idctY.start(alwaysOn);
		idctY.dctData(dequantY);
		idctY.imageData(fifoYToInverse);
		idctY.done(idctYDone);
		
		idctCb.clock(t_clock);
		idctCb.start(alwaysOn);
		idctCb.dctData(dequantCb);
		idctCb.imageData(fifoCbToInverse);
		idctCb.done(idctCbDone);
		
		idctCr.clock(t_clock);
		idctCr.start(alwaysOn);
		idctCr.dctData(dequantCr);
		idctCr.imageData(fifoCrToInverse);
		idctCr.done(idctCrDone);
		
		// Inverse YCbCr
		inverseYCbCr.clock(t_clock);
		inverseYCbCr.start(alwaysOn);
		inverseYCbCr.yData(fifoYToInverse);
		inverseYCbCr.cbData(fifoCbToInverse);
		inverseYCbCr.crData(fifoCrToInverse);
		inverseYCbCr.rData(fifoRToInverse);
		inverseYCbCr.gData(fifoGToInverse);
		inverseYCbCr.bData(fifoBToInverse);
		inverseYCbCr.done(inverseYCbCrDone);
		
		// Write 8x8 blocks into frame memory
		inverseRWriter.clock(t_clock);
		inverseRWriter.start(startNextFrame);
		inverseRWriter.dataValid(inverseRWriterDataValid);
		inverseRWriter.chipSelect(inverseRWriterCS);
		inverseRWriter.memAddress(inverseRWriterAddress);
		inverseRWriter.memValue(inverseRWriterValueOut);
		inverseRWriter.data(fifoRToInverse);
		inverseRWriter.done(inverseRWriterDone);
		
		inverseGWriter.clock(t_clock);
		inverseGWriter.start(startNextFrame);
		inverseGWriter.dataValid(inverseGWriterDataValid);
		inverseGWriter.chipSelect(inverseGWriterCS);
		inverseGWriter.memAddress(inverseGWriterAddress);
		inverseGWriter.memValue(inverseGWriterValueOut);
		inverseGWriter.data(fifoGToInverse);
		inverseGWriter.done(inverseGWriterDone);
		
		inverseBWriter.clock(t_clock);
		inverseBWriter.start(startNextFrame);
		inverseBWriter.dataValid(inverseBWriterDataValid);
		inverseBWriter.chipSelect(inverseBWriterCS);
		inverseBWriter.memAddress(inverseBWriterAddress);
		inverseBWriter.memValue(inverseBWriterValueOut);
		inverseBWriter.data(fifoBToInverse);
		inverseBWriter.done(inverseBWriterDone);
		
		// Memory blocks for inverted frames
		inverseMemoryR.clock(t_clock);
		inverseMemoryR.enableWrite(inverseRWriterDataValid);
		inverseMemoryR.addressIn(inverseRWriterAddress);
		inverseMemoryR.valueIn(inverseRWriterValueOut);
		inverseMemoryR.chipSelect(alwaysOn);
		inverseMemoryR.valueOut(inverseMemoryRValueOut);
		
		inverseMemoryG.clock(t_clock);
		inverseMemoryG.enableWrite(inverseGWriterDataValid);
		inverseMemoryG.addressIn(inverseGWriterAddress);
		inverseMemoryG.valueIn(inverseGWriterValueOut);
		inverseMemoryG.chipSelect(alwaysOn);
		inverseMemoryG.valueOut(inverseMemoryGValueOut);
		
		inverseMemoryB.clock(t_clock);
		inverseMemoryB.enableWrite(inverseBWriterDataValid);
		inverseMemoryB.addressIn(inverseBWriterAddress);
		inverseMemoryB.valueIn(inverseBWriterValueOut);
		inverseMemoryB.chipSelect(alwaysOn);
		inverseMemoryB.valueOut(inverseMemoryBValueOut);
		
		//QuantY
		my_quantY.clock(t_clock);
		my_quantY.start(dctYDone);
		my_quantY.in(fifoYDCT);
		
		my_quantY.done(quant_YDone);
		my_quantY.out(quantY);
		
		//QuantCb
		my_quantCb.clock(t_clock);
		my_quantCb.start(dctCbDone);
		my_quantCb.in(fifoCbDCT);
		
		my_quantCb.done(quant_CbDone);
		my_quantCb.out(quantCb);
		
		//QuantCr
		my_quantCr.clock(t_clock);
		my_quantCr.start(dctCrDone);
		my_quantCr.in(fifoCrDCT);
		
		my_quantCr.done(quant_CrDone);
		my_quantCr.out(quantCr);
		
		//Encoding Y
		my_encodingY.clock(t_clock);
		my_encodingY.start(quant_YDone);
		my_encodingY.in(quantY);
		
		my_encodingY.valueNum(valueNumY);
		my_encodingY.done(encoding_YDone);
		my_encodingY.outputV(valueY);
		my_encodingY.outputC0(codeY0);
		my_encodingY.outputC1(codeY1);
		my_encodingY.outputC2(codeY2);
		my_encodingY.outputC3(codeY3);
		my_encodingY.outputC4(codeY4);
		my_encodingY.outputC5(codeY5);
		my_encodingY.outputC6(codeY6);
		my_encodingY.outputC7(codeY7);
		my_encodingY.outputC8(codeY8);
		
		my_encodingY.outputFinalCode(vleCodeY);
		
		//Encoding Cb
		my_encodingCb.clock(t_clock);
		my_encodingCb.start(quant_CbDone);
		my_encodingCb.in(quantCb);
		
		my_encodingCb.valueNum(valueNumCb);
		my_encodingCb.done(encoding_CbDone);
		my_encodingCb.outputV(valueCb);
		my_encodingCb.outputC0(codeCb0);
		my_encodingCb.outputC1(codeCb1);
		my_encodingCb.outputC2(codeCb2);
		my_encodingCb.outputC3(codeCb3);
		my_encodingCb.outputC4(codeCb4);
		my_encodingCb.outputC5(codeCb5);
		my_encodingCb.outputC6(codeCb6);
		my_encodingCb.outputC7(codeCb7);
		my_encodingCb.outputC8(codeCb8);
		
		my_encodingCb.outputFinalCode(vleCodeCb);
		
		//Encoding Cr
		my_encodingCr.clock(t_clock);
		my_encodingCr.start(quant_CrDone);
		my_encodingCr.in(quantCr);
		
		my_encodingCr.valueNum(valueNumCr);
		my_encodingCr.done(encoding_CrDone);
		my_encodingCr.outputV(valueCr);
		my_encodingCr.outputC0(codeCr0);
		my_encodingCr.outputC1(codeCr1);
		my_encodingCr.outputC2(codeCr2);
		my_encodingCr.outputC3(codeCr3);
		my_encodingCr.outputC4(codeCr4);
		my_encodingCr.outputC5(codeCr5);
		my_encodingCr.outputC6(codeCr6);
		my_encodingCr.outputC7(codeCr7);
		my_encodingCr.outputC8(codeCr8);
		
		my_encodingCr.outputFinalCode(vleCodeCr);	
	
		//Decoding Y
		my_decodingY.clock(t_clock);
		my_decodingY.start(encoding_YDone);
	
		my_decodingY.inputValue(valueY);
		my_decodingY.valueNum(valueNumY);
		my_decodingY.inputTable0(codeY0);
		my_decodingY.inputTable1(codeY1);
		my_decodingY.inputTable2(codeY2);
		my_decodingY.inputTable3(codeY3);
		my_decodingY.inputTable4(codeY4);
		my_decodingY.inputTable5(codeY5);
		my_decodingY.inputTable6(codeY6);
		my_decodingY.inputTable7(codeY7);
		my_decodingY.inputTable8(codeY8);
		my_decodingY.inputFinalCode(vleCodeY);
		
		my_decodingY.done(decoding_YDone);
		my_decodingY.outputFinal(outputFinalY);
		
		//Decoding Cb
		my_decodingCb.clock(t_clock);
		my_decodingCb.start(encoding_CbDone);
	
		my_decodingCb.inputValue(valueCb);
		my_decodingCb.valueNum(valueNumCb);
		my_decodingCb.inputTable0(codeCb0);
		my_decodingCb.inputTable1(codeCb1);
		my_decodingCb.inputTable2(codeCb2);
		my_decodingCb.inputTable3(codeCb3);
		my_decodingCb.inputTable4(codeCb4);
		my_decodingCb.inputTable5(codeCb5);
		my_decodingCb.inputTable6(codeCb6);
		my_decodingCb.inputTable7(codeCb7);
		my_decodingCb.inputTable8(codeCb8);
		my_decodingCb.inputFinalCode(vleCodeCb);
		
		my_decodingCb.done(decoding_CbDone);
		my_decodingCb.outputFinal(outputFinalCb);
		
		//Decoding Cr
		my_decodingCr.clock(t_clock);
		my_decodingCr.start(encoding_CrDone);
	
		my_decodingCr.inputValue(valueCr);
		my_decodingCr.valueNum(valueNumCr);
		my_decodingCr.inputTable0(codeCr0);
		my_decodingCr.inputTable1(codeCr1);
		my_decodingCr.inputTable2(codeCr2);
		my_decodingCr.inputTable3(codeCr3);
		my_decodingCr.inputTable4(codeCr4);
		my_decodingCr.inputTable5(codeCr5);
		my_decodingCr.inputTable6(codeCr6);
		my_decodingCr.inputTable7(codeCr7);
		my_decodingCr.inputTable8(codeCr8);
		my_decodingCr.inputFinalCode(vleCodeCr);
		
		my_decodingCr.done(decoding_CrDone);
		my_decodingCr.outputFinal(outputFinalCr);
		
		//Dequant Y
		my_dequantY.clock(t_clock);
		my_dequantY.start(decoding_YDone);
		my_dequantY.in(outputFinalY);
		
		my_dequantY.done(dequant_YDone);
		my_dequantY.out(dequantY);
		
		//Dequant Cb
		my_dequantCb.clock(t_clock);
		my_dequantCb.start(decoding_CbDone);
		my_dequantCb.in(outputFinalCb);
		
		my_dequantCb.done(dequant_CbDone);
		my_dequantCb.out(dequantCb);
		
		//Dequant Cr
		my_dequantCr.clock(t_clock);
		my_dequantCr.start(decoding_CrDone);
		my_dequantCr.in(outputFinalCr);
		
		my_dequantCr.done(dequant_CrDone);
		my_dequantCr.out(dequantCr);
	}

};
// Top-level process that progresses frames every 33.3 ms
void top::updateFrame(){
	char* val = new char[1];
	inputFrame = new uint8_t[IMAGE_SIZE];
	frameMemory.setMemoryPointer(inputFrame, IMAGE_SIZE);
	char filename[50];
	for (uint8_t n = 0; n < NUM_FRAMES; n++){
		// Open file for nth frame
		sprintf(filename, "Matlab\\outputFrames\\frame%d.bin", n);
		std::ifstream file (filename, std::ifstream::binary);
		
		if(file){
			//file.seekg (0, file.end);
			//int length = file.tellg();
			//cout << "This file is " << length << " long." << endl;
			//file.seekg (0, file.beg);
			
			// Each value is one byte, so read byte-by-byte
			for (uint32_t i = 0; i < IMAGE_SIZE; i++){
				file.read(val, 1);
				// Note that val is char (file.read() only supports char), and inputFrame is uchar, so watch the types
				inputFrame[i] = static_cast<uint8_t>(val[0]);
			}
		}
		else{
			cout << "Uh oh, file not loaded";
		}
		
		startNextFrame = true;
		currentFrame = n;
		wait(CLOCK_PERIOD_NS, SC_NS);
		startNextFrame = false;
		wait(33333333, SC_NS);
	}
	cout << "Out of frames!" << endl;
	delete[] inputFrame;
	delete[] motionEstimationFrame1;
	delete[] motionEstimationFrame2;
}

 // Debug
void top::writeOutput(){
	std::ofstream file;
	wait(1, SC_NS);
	file.open("Rin.bin", std::ofstream::binary);
	cout << "Starting init file write..." << endl;
	for (uint32_t i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++){
		file.put(static_cast<int8_t>(inputFrame[i]));
	}
	file.close();
	file.open("Gin.bin", std::ofstream::binary);
	cout << "Starting init file write..." << endl;
	for (uint32_t i = IMAGE_WIDTH * IMAGE_HEIGHT; i < 2 * IMAGE_WIDTH * IMAGE_HEIGHT; i++){
		file.put(static_cast<int8_t>(inputFrame[i]));
	}
	file.close();
	file.open("Bin.bin", std::ofstream::binary);
	cout << "Starting init file write..." << endl;
	for (uint32_t i = 2 * IMAGE_WIDTH * IMAGE_HEIGHT; i < 3 * IMAGE_WIDTH * IMAGE_HEIGHT; i++){
		file.put(static_cast<int8_t>(inputFrame[i]));
	}
	file.close();
	
	wait(2, SC_MS);
	while (true){
		// Content of Motion Estimation Memory 1
		file.open("debug1.bin", std::ofstream::binary);
		cout << "Starting file write..." << endl;
		for (uint32_t i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++){
			file.put(static_cast<int8_t>(motionEstimationFrame1[i]));
		}
		file.close();
		
		// Image channels for encoded + decoded images
		file.open("thereAndBackR.bin", std::ofstream::binary);
		for (uint32_t i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++){
			file.put(static_cast<int8_t>(inverseMemoryRFrame[i]));
		}
		file.close();
		
		file.open("thereAndBackG.bin", std::ofstream::binary);
		for (uint32_t i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++){
			file.put(static_cast<int8_t>(inverseMemoryGFrame[i]));
		}
		file.close();
		
		file.open("thereAndBackB.bin", std::ofstream::binary);
		for (uint32_t i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++){
			file.put(static_cast<int8_t>(inverseMemoryBFrame[i]));
		}
		file.close();
		wait(33333333, SC_NS);
		
		// Content of Motion Estimation Memory 2
		file.open("debug2.bin", std::ofstream::binary);
		cout << "Starting file write..." << endl;
		for (uint32_t i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++){
			file.put(static_cast<int8_t>(motionEstimationFrame2[i]));
		}
		file.close();
		wait(33333333, SC_NS);
	}
}

SC_MODULE_EXPORT(top);
