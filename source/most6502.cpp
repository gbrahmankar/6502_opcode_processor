#include"../include/most6502.h"
// concept : we need full obj declaration during usage eg : bus->Read(...)
#include"../include/bus.h" // to prevent circular includes

MosT6502::MosT6502() {
  m_instrSet = {
    { 0x00, { "break", AddrMode::IMMEDIATE, InstrName::BREAK, 7 } },
		{ 0xa9, { "load_a_imm", AddrMode::IMMEDIATE, InstrName::LDA, 2} }
  };
}

void MosT6502::PrintState() {
  std::cout << "\nMosT6502_State : " 
            << "a=" << STREAM_BYTE(a) << " | "  
            << "x=" << STREAM_BYTE(x) << " | "  
            << "y=" << STREAM_BYTE(y) << " | "  
            << "sp=" << STREAM_BYTE(sp) << " | "  
            << "pc=" << STREAM_WORD(pc) << '\n';  
  PrintStatus();
  std::cout << "\n";
}

void MosT6502::Reset() {
  uint16_t lowByte  = bus->Read(0xFFFC);
  uint16_t highByte = bus->Read(0xFFFD);
  pc = (highByte<<8) | lowByte; // hardwired for the programmer 

  a = x = y = 0x00;

  sp = 0xFF; // bus/ram space : 0x0100 - 0x01FF

  sr = 0x00;
  SetFlag(FLAGS6502::U, true); // this is set at reset
}

uint8_t MosT6502::FetchData(Instruction instr) {
  uint8_t byteData; 
  switch(instr.addrMode) {
    case AddrMode::IMMEDIATE : {
      byteData = bus->Read(pc);            
      pc += 1;
      break;
    }
    case AddrMode::ZERO_PAGE : {
      uint16_t zpOffset = bus->Read(pc);
      pc += 1;
      byteData = bus->Read(zpOffset);
      break;
    }
    case AddrMode::ZERO_PAGE_X : {
      uint16_t zpOffset = bus->Read(pc);
      pc += 1;
      zpOffset += zpOffset + (uint16_t)x;
      byteData = bus->Read((zpOffset) & (0x00FF));
      break;
    }  
    case AddrMode::ZERO_PAGE_Y : {
      uint16_t zpOffset = bus->Read(pc);
      pc += 1;
      zpOffset += zpOffset + (uint16_t)y;
      byteData = bus->Read((zpOffset) & (0x00FF));
      break;    
    }
    case AddrMode::ABSOLUTE : {
      uint16_t lo = bus->Read(pc);
      pc += 1;
      uint16_t hi = bus->Read(pc);
      pc += 1;
      byteData = bus->Read((hi<<8) | lo);
      break;
    }
    case AddrMode::ABSOLUTE_X : {
      uint16_t lo = bus->Read(pc);
      pc += 1;
      uint16_t hi = bus->Read(pc);
      pc += 1;
      byteData = bus->Read(((hi<<8) | lo) + x); // add clock cycle on page change
      break;
    }
    case AddrMode::ABSOLUTE_Y : {
      uint16_t lo = bus->Read(pc);
      pc += 1;
      uint16_t hi = bus->Read(pc);
      pc += 1;
      byteData = bus->Read(((hi<<8) | lo) + y); // add clock cycle on page change
      break;
    }
    case AddrMode::INDIRECT : {
      uint16_t lo = bus->Read(pc);
      pc += 1;
      uint16_t hi = bus->Read(pc);
      pc += 1;
      uint16_t ptr = (hi << 8) | lo;

      byteData = bus->Read((bus->Read(ptr + 1) << 8) | bus->Read(ptr + 0)); 
      break; 
    }
    /*case AddrMode::INDIRECT_X : {
      return 0;  
    }
    case AddrMode::INDIRECT_Y : {
      return 0; 
    }
    case AddrMode::RELATIVE : {
      return 0;
    }*/
    default : {
      std::cout << "Addr_mode=" << GetAddrModeName(instr.addrMode) 
                << " not implemented yet for instr=" << instr.nameStr;
      abort();
    }
  }
  return byteData;
}

void MosT6502::ExecuteInstruction() {
  uint8_t opcode = bus->Read(pc);
  pc += 1; // as soon as a read from pc happens; pc++; from WD spec;
  
  if(m_instrSet.find(opcode) == m_instrSet.end()) {
    std::cout << "Illegal instr in the code. opcode=" << (opcode);
    abort();
  }
  auto instr = m_instrSet[opcode];

  switch(instr.instrName) {
    case InstrName::BREAK : {
    }
    case InstrName::LDA : {
			a = FetchData(instr);			
			std::cout << "instr_name=" << instr.nameStr << " a=" << STREAM_BYTE(a) << " pc=" << STREAM_WORD(pc) << '\n';
			break;
		}
    default : {
      std::cout << "Instruction opcode=" << opcode << " not implemented yet";
      abort();
    }
  }   
}
