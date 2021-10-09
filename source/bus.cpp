#include"../include/bus.h"

void Bus::Initialize() {
  for(int i=0; i < ram.size(); i++) {
    ram[i] = 0x00;
  }
  mp.ConnectBus(this);  
}

void Bus::Write(uint16_t addr, uint8_t data) {
  if(addr >= 0x0000 and addr <= 0xFFFF) {
    ram[addr] = data;
  }
}

uint8_t Bus::Read(uint16_t addr) {
  if(addr >= 0x0000 and addr <= 0xFFFF) {
    return ram[addr];
  } else {
    return 0x00;
  }
}	
	
void Bus::PrintRamState() {
  uint16_t lineNumber = 0x0000;
  std::cout << STREAM_WORD(lineNumber) << " : ";
  for(int i=0; i<ram.size(); i++) {
    if((i%16 == 0) and (i != 0)) {
      std::cout << '\n';
      lineNumber += 16;
      std::cout << STREAM_WORD(lineNumber) << " : ";
    } 
    std::cout << STREAM_BYTE(ram[i]) << " ";
  }  
  std::cout << '\n';
}

