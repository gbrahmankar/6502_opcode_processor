#pragma once

#include <iostream>

#include <vector>
#include <string>
#include <map>

#include "../include/most_common.h"

class Bus;

class MosT6502
{
public :

  // addr modes 
  enum AddrMode {
    IMMEDIATE,
    IMPLIED,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    RELATIVE, 
    ABSOLUTE, 
    ABSOLUTE_X, 
    ABSOLUTE_Y, 
    INDIRECT, 
    INDIRECT_X,
    INDIRECT_Y
  };

  // instruction exec
  enum InstrName {
		LDA,
    BREAK
  };

  struct Instruction {
    std::string nameStr;
    AddrMode addrMode;
    InstrName instrName; 
    uint8_t cycles = 0;
  };
  
  MosT6502();

  void ConnectBus(Bus* bp) { bus = bp; std::cout << "MosTech6502 connected to the bus !\n"; }

  void PrintState();
  void Reset();  
  uint8_t FetchData(Instruction instr);
  void ExecuteInstruction();    

public : // private:
  
  // registers 
	uint8_t  a      = 0x00;		
	uint8_t  x      = 0x00;		
	uint8_t  y      = 0x00;		
	uint8_t  sp     = 0x00;		
	uint16_t pc     = 0x0000;	
  
  // status register member + utils 
  enum FLAGS6502
  {
    C = (1 << 0), 
    Z = (1 << 1), 
    I = (1 << 2), 
    D = (1 << 3), 
    B = (1 << 4), 
    U = (1 << 5), 
    V = (1 << 6), 
    N = (1 << 7), 
  };
	uint8_t  sr     = 0x00;		
  bool     GetFlag(FLAGS6502 f)         { return ((sr & f > 0) ? 1 : 0); } 
  void     SetFlag(FLAGS6502 f, bool v) { ((v) ? sr|=f : sr&=~f);        }
  void     PrintStatus() {
    std::cout << "carry   =" << ((sr & FLAGS6502::C) ? "1" : "0") << '\n';
    std::cout << "zero    =" << ((sr & FLAGS6502::Z) ? "1" : "0") << '\n';
    std::cout << "maski   =" << ((sr & FLAGS6502::I) ? "1" : "0") << '\n';
    std::cout << "decim   =" << ((sr & FLAGS6502::D) ? "1" : "0") << '\n';
    std::cout << "break   =" << ((sr & FLAGS6502::B) ? "1" : "0") << '\n';
    std::cout << "unused  =" << ((sr & FLAGS6502::U) ? "1" : "0") << '\n';
    std::cout << "overflow=" << ((sr & FLAGS6502::V) ? "1" : "0") << '\n';
    std::cout << "negative=" << ((sr & FLAGS6502::N) ? "1" : "0") << '\n';
  }
 
  std::string GetAddrModeName(AddrMode addrMode) {
    switch(addrMode) {
      case (AddrMode::IMMEDIATE) : {
        return "immediate";
      }
      case (AddrMode::IMPLIED) : {
        return "implied";
      }
      case (AddrMode::ZERO_PAGE) : {
        return "zero_page";
      }
      case (AddrMode::ZERO_PAGE_X) : {
        return "zero_page_x";
      }
      case (AddrMode::ZERO_PAGE_Y) : {
        return "zero_page_y";
      }
      case (AddrMode::RELATIVE) : {
        return "relative";
      }
      case (AddrMode::ABSOLUTE) : {
        return "absolute";
      }
      case (AddrMode::ABSOLUTE_X) : {
        return "absolute_x";
      }
      case (AddrMode::ABSOLUTE_Y) : {
        return "absolute_y";
      }
      case (AddrMode::INDIRECT) : {
        return "indirect";
      }
      case (AddrMode::INDIRECT_X) : {
        return "indirect_x";
      }
      case (AddrMode::INDIRECT_Y) : {
        return "indirect_y";
      }
    }
    return "xxx";
  }

  std::map<uint8_t, Instruction> m_instrSet;

  Bus*     bus; 
};

