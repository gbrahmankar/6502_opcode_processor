all : opcode_processor

opcode_processor : bus.o most6502.o opcodeProcessor.o
	g++ bus.o most6502.o opcodeProcessor.o -o opcode_processor

bus.o : source/bus.cpp
	g++ -c source/bus.cpp

most6502.o : source/most6502.cpp
	g++ -c source/most6502.cpp

opcodeProcessor.o : opcodeProcessor.cpp
	g++ -c opcodeProcessor.cpp

clean : 
	sudo rm -f opcode_processor *o
