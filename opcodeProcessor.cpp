#include<iostream>
#include<fstream>
#include <sstream>
#include<string>

#include"include/bus.h"

int main(int argc, char *argv[]) {

  Bus bus;
  bus.Initialize();

  if(argc < 2) {
    return 1;
  }

  std::ifstream sourceFile;
  sourceFile.open(std::string(argv[2]));
  
  std::string startAddrString(argv[1]);
  std::stringstream startAddrStream(startAddrString);     
  uint16_t startAddr;
  startAddrStream >> std::hex >> startAddr;
  bus.Write(0xfffc, startAddr & 0xff);
  bus.Write(0xfffd, (startAddr >> 8) & 0xff);

  unsigned int sourceByte; 
  uint16_t byteCnt = 0; 

  while(sourceFile >> std::hex >> sourceByte) {
    bus.Write(startAddr + byteCnt,sourceByte);
    byteCnt += 1;
  }

  std::cout << "Program start_addr=" << STREAM_BYTE(startAddr) << " end_addr=" << STREAM_BYTE(startAddr + byteCnt - 1 - 1) <<
  '\n';

  sourceFile.close();

  bus.StartCpu();
	
  bus.StartOpcodeProcessing();

  return 0;
}
