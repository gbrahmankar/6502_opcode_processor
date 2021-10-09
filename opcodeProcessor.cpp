#include<iostream>
#include<fstream>
#include<string>

#include"include/bus.h"

int main(int argc, char *argv[]) {

  Bus bus;
  bus.Initialize();

  if(argc < 2) {
    return 1;
  }

  std::ifstream sourceFile;
  sourceFile.open(std::string(argv[1]));
  
  uint16_t startAddr = 0x8000;
  bus.Write(0xFFFC, 0x00);
  bus.Write(0xFFFD, 0x80);

  unsigned int sourceByte; 
  uint16_t byteCnt = 0; 

  while(sourceFile >> std::hex >> sourceByte) {
    bus.Write(startAddr + byteCnt,sourceByte);
    byteCnt += 1;
  }

  bus.PrintRamState();
  sourceFile.close();

  bus.StartCpu();
  bus.PrintCpuState();
	
	bus.StartOpcodeProcessing();

  return 0;
}
