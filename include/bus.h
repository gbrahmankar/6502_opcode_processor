#pragma once
#include <cstdint>
#include <array>
#include <iomanip>

#include "most6502.h"
#include "most_common.h"

class Bus
{
public :
  void Initialize();
  void StartCpu() { 
    std::cout << "\nPowering up the legendary MOS-Technology-6502's basic emulator ...\n";
    mp.Reset(); 
  };
	void StartOpcodeProcessing() {
		mp.ExecuteInstruction();		
		mp.ExecuteInstruction();		
	};

	void Write(uint16_t addr, uint8_t data);
	uint8_t Read(uint16_t addr);
  void PrintRamState();
  void PrintCpuState() {
    mp.PrintState();
  }

private : 
	MosT6502 mp;	
	std::array<uint8_t, 64 * 1024> ram;
};
