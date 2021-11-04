CC = g++
CPPSTD = 14

all : opcode_processor

opcode_processor : bus.o most6502.o opcodeProcessor.o
	${CC} bus.o most6502.o opcodeProcessor.o -o opcode_processor

bus.o : source/bus.cpp
	${CC} --std=c++${CPPSTD} -c source/bus.cpp

most6502.o : source/most6502.cpp
	${CC} --std=c++${CPPSTD} -c source/most6502.cpp

opcodeProcessor.o : opcodeProcessor.cpp
	${CC} --std=c++${CPPSTD} -c opcodeProcessor.cpp

clean : 
	sudo rm -f opcode_processor *o
