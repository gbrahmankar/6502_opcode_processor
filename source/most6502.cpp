#include"../include/most6502.h"
// concept : we need full obj declaration during usage eg : bus->Read(...)
#include"../include/bus.h" // to prevent circular includes

MosT6502::MosT6502() {
  m_instrSet = {

    { 0x00, { "break", AddrMode::IMMEDIATE, InstrName::BRK, 7 } },
		
		// load-store
		{ 0xa9, { "load_a_imm", AddrMode::IMMEDIATE, InstrName::LDA, 2} },
		
		// add-sub
		{ 0x69, { "add_with_carry_imm", AddrMode::IMMEDIATE, InstrName::ADC, 2} },
		{ 0xe9, { "sub_with_burrow_in_imm", AddrMode::IMMEDIATE, InstrName::SBC, 2} },

		// logical
		{ 0x29, { "bitwise_and", AddrMode::IMMEDIATE, InstrName::AND, 2} },

		// shifts
		{ 0x0a, { "arithmetic_shift_left_1_bit", AddrMode::IMPLIED, InstrName::ASL, 2} },
		
		// branch relative
		{ 0x90, { "branch_on_carry_clear", AddrMode::RELATIVE, InstrName::BCC, 2} },
		{ 0xb0, { "branch_on_carry_set", AddrMode::RELATIVE, InstrName::BCS, 2} },
		{ 0xf0, { "branch_on_result_zero", AddrMode::RELATIVE, InstrName::BEQ, 2} },
		{ 0x30, { "branch_on_result_minus", AddrMode::RELATIVE, InstrName::BMI, 2} },
		{ 0xd0, { "branch_on_result_not_zero", AddrMode::RELATIVE, InstrName::BNE, 2} },
		{ 0x10, { "branch_on_result_plus", AddrMode::RELATIVE, InstrName::BPL, 2} },
		{ 0x50, { "branch_on_overflow_clear", AddrMode::RELATIVE, InstrName::BVC, 2} },
		{ 0x70, { "branch_on_overflow_set", AddrMode::RELATIVE, InstrName::BVS, 2} }

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

MosT6502::DataDetails MosT6502::FetchData(Instruction instr) {
  DataDetails dd; 

  switch(instr.addrMode) {
		case AddrMode::IMPLIED : {
			dd = {a, 0x0000};
			break;	
		} 
    case AddrMode::IMMEDIATE : {
			dd = {bus->Read(pc), pc};
      pc += 1;
      break;
    }
    case AddrMode::ZERO_PAGE : {
      uint16_t zpOffset = bus->Read(pc);
      pc += 1;
			dd = {bus->Read(zpOffset), zpOffset};	
      break;
    }
    case AddrMode::ZERO_PAGE_X : {
      uint16_t zpOffset = bus->Read(pc);
      pc += 1;
      zpOffset += zpOffset + (uint16_t)x;
			dd = {bus->Read((zpOffset) & (0x00FF)), (uint16_t)((zpOffset) & (0x00FF))};
      break;
    }  
    case AddrMode::ZERO_PAGE_Y : {
      uint16_t zpOffset = bus->Read(pc);
      pc += 1;
      zpOffset += zpOffset + (uint16_t)y;
			dd = {bus->Read((zpOffset) & (0x00FF)), (uint16_t)((zpOffset) & (0x00FF))};
      break;    
    }
    case AddrMode::ABSOLUTE : {
      uint16_t lo = bus->Read(pc);
      pc += 1;
      uint16_t hi = bus->Read(pc);
      pc += 1;
			dd = {bus->Read((hi<<8) | lo), (uint16_t)((hi<<8) | lo)};
      break;
    }
    case AddrMode::ABSOLUTE_X : {
      uint16_t lo = bus->Read(pc);
      pc += 1;
      uint16_t hi = bus->Read(pc);
      pc += 1;
			dd = {bus->Read(((hi<<8) | lo) + x), (uint16_t)(((hi<<8) | lo) + x)};
      break;
    }
    case AddrMode::ABSOLUTE_Y : {
      uint16_t lo = bus->Read(pc);
      pc += 1;
      uint16_t hi = bus->Read(pc);
      pc += 1;
			dd = {bus->Read(((hi<<8) | lo) + y), (uint16_t)(((hi<<8) | lo) + y)};
      break;
    }
    case AddrMode::INDIRECT : {
      uint16_t lo = bus->Read(pc);
      pc += 1;
      uint16_t hi = bus->Read(pc);
      pc += 1;
      uint16_t ptr = (hi << 8) | lo;

			dd = {bus->Read((bus->Read(ptr + 1) << 8) | bus->Read(ptr + 0)), (uint16_t)((bus->Read(ptr + 1) << 8) | bus->Read(ptr + 0))};
      break; 
    }
    /*case AddrMode::INDIRECT_X : {
      return 0;  
    }
    case AddrMode::INDIRECT_Y : {
      return 0; 
    }*/
    case AddrMode::RELATIVE : {
			uint8_t jumpDelta = bus->Read(pc);
			pc += 1;
	
			dd = {jumpDelta, pc};			
    }
    default : {
      std::cout << "Addr_mode=" << GetAddrModeName(instr.addrMode) 
                << " not implemented yet for instr=" << instr.nameStr;
      abort();
    }
  }
  return dd;
}

void MosT6502::ExecuteInstruction() {
  uint8_t opcode = bus->Read(pc);
  pc += 1; // as soon as a read from pc happens; pc++; from WD spec;

	if(opcode == TERMINATE_OPCODE) {
    std::cout << "\nProgram completed !\n";
		exit(0);				
	}
  
  if(m_instrSet.find(opcode) == m_instrSet.end()) {
    std::cout << "Illegal instr in the code. opcode=" << (opcode);
    abort();
  }
  auto instr = m_instrSet[opcode];
	std::cout << "instr_name=" << instr.nameStr << '\n';

  switch(instr.instrName) {
    case InstrName::BRK : {
    }
    case InstrName::LDA : {
			a = FetchData(instr).data;			
			SetFlag(FLAGS6502::Z, a == 0x00);
			SetFlag(FLAGS6502::N, a &  0x80);
			break;
		}
		case InstrName::ADC : {
			uint16_t byteData = (uint16_t)FetchData(instr).data;
			uint16_t result = (uint16_t)a + byteData + (uint16_t)GetFlag(FLAGS6502::C);	
			
			SetFlag(FLAGS6502::C, result > 255);
			SetFlag(FLAGS6502::Z, (result & 0xff) == 0);
			SetFlag(FLAGS6502::V, (~((uint16_t)a ^ byteData) & ((uint16_t)a ^ (uint16_t)result)) & 0x0080);			
			SetFlag(FLAGS6502::N, result & 0x80);
			
			a = result & 0xff;

			break;			
		}
		case InstrName::SBC : { // TODO ! 
			break;
		}
		case InstrName::AND : {
			a = a & FetchData(instr).data;

			SetFlag(FLAGS6502::Z, a == 0x00);
			SetFlag(FLAGS6502::N, a &  0x80);
			break;
		}
		case InstrName::ASL : {
			auto dd = FetchData(instr); 
			
			uint16_t dataByte = (uint16_t)dd.data << 1;  
			
			SetFlag(FLAGS6502::C, (dataByte & 0xFF00) > 0);
			SetFlag(FLAGS6502::Z, (dataByte & 0x00FF) == 0x00);
			SetFlag(FLAGS6502::N, dataByte & 0x80);
			if (instr.addrMode == AddrMode::IMPLIED) {
				a = dataByte & 0x00FF;
			}
			else {
				bus->Write(dd.addr, dataByte & 0x00FF);
			}
			break;
		}	
		case InstrName::BCC : {
			ExecBranchInstr(instr, FLAGS6502::C, 0);			
			break;
		}
		case InstrName::BCS : {
			ExecBranchInstr(instr, FLAGS6502::C, 1);			
			break;
		}
		case InstrName::BEQ : {
			ExecBranchInstr(instr, FLAGS6502::Z, 1);			
			break;
		}
		case InstrName::BMI : {
			ExecBranchInstr(instr, FLAGS6502::N, 1);			
			break;
		}
		case InstrName::BNE : {
			ExecBranchInstr(instr, FLAGS6502::Z, 0);			
			break;
		}		
		case InstrName::BPL : {
			ExecBranchInstr(instr, FLAGS6502::N, 0);			
			break;
		}
		case InstrName::BVC : {
			ExecBranchInstr(instr, FLAGS6502::V, 0);			
			break;
		}
		case InstrName::BVS : {
			ExecBranchInstr(instr, FLAGS6502::V, 1);			
			break;
		}
    default : {
      std::cout << "Instruction opcode=" << opcode << " not implemented yet";
      abort();
    }
  }   

	PrintState();		
}

// helpers
void MosT6502::ExecBranchInstr(MosT6502::Instruction instr, MosT6502::FLAGS6502 f, uint8_t expectedValue) {
	uint16_t jumpDelta = (uint16_t)FetchData(instr).data;		
	if(jumpDelta & 0x80) {
		jumpDelta |= 0xff00;
	}
	if(GetFlag(f) == expectedValue) {
		pc += jumpDelta;
	}
}
