CC = g++
CPPSTD = 14

all : opcode_processor

opcode_processor : bus.o mos_t_6502.o app_opcode_processor.o
	${CC} bus.o mos_t_6502.o app_opcode_processor.o -o opcode_processor

bus.o : source/bus.cpp
	${CC} --std=c++${CPPSTD} -c source/bus.cpp

mos_t_6502.o : source/mos_t_6502.cpp
	${CC} --std=c++${CPPSTD} -c source/mos_t_6502.cpp

app_opcode_processor.o : app_opcode_processor.cpp
	${CC} --std=c++${CPPSTD} -c app_opcode_processor.cpp

clean : 
	sudo rm -f opcode_processor *o

cstyle :
	find -f . | awk -f .filter_hpp | xargs clang-format -i -style=file
