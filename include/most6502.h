#pragma once

#include <iostream>

#include <vector>
#include <string>
#include <map>

#include "../include/most_common.h"

class Bus;

class MosT6502
{

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
    BRK,
		ADC,
		SBC, 
		AND,
		ASL,
		BCC,
		BCS,
		BEQ,
		BIT,
		BMI,
		BNE,
		BPL,
		BVC,
		BVS,
		CLC,
		CLD,
		CLI,
		CLV,
		CMP,
		CPX,
		CPY,
		DEC,
		DEX,
		DEY
  };

  struct Instruction {
    std::string nameStr;
    AddrMode addrMode;
    InstrName instrName; 
    uint8_t cycles = 0;
  };
	
	struct DataDetails { // data and its mem_loc
		uint8_t data;
		uint16_t addr; 	
	};
  
  MosT6502() {
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
			{ 0x70, { "branch_on_overflow_set", AddrMode::RELATIVE, InstrName::BVS, 2} },

			// clearing flags
			{ 0x18, { "clear_carry_flag", AddrMode::IMPLIED, InstrName::CLC, 2} },
			{ 0xD8, { "clear_decimal_flag", AddrMode::IMPLIED, InstrName::CLD, 2} },
			{ 0x58, { "disable_interrupts", AddrMode::IMPLIED, InstrName::CLI, 2} },
			{ 0xB8, { "clear_overflow_flag", AddrMode::IMPLIED, InstrName::CLV, 2} },
			
			// comparison 
			{ 0xc9, { "cmp_immediate", AddrMode::IMMEDIATE, InstrName::CMP, 2} },
			{ 0xe0, { "cmp_x_immediate", AddrMode::IMMEDIATE, InstrName::CPX, 2} },
			{ 0xc0, { "cmp_y_immediate", AddrMode::IMMEDIATE, InstrName::CPY, 2} },

			// decrement 
			{ 0xc6, { "dec_mem_1", AddrMode::ZERO_PAGE, InstrName::DEC, 5} },
			{ 0xca, { "dec_x_1", AddrMode::IMMEDIATE, InstrName::DEX, 2} },
			{ 0x88, { "dec_y_1", AddrMode::IMMEDIATE, InstrName::DEY, 2} },
			
			// lesser used 
			{ 0x24, { "test_bit_in_mem_with_acc_zp",  AddrMode::ZERO_PAGE, InstrName::BIT, 3} },
			{ 0x2c, { "test_bit_in_mem_with_acc_abs", AddrMode::ABSOLUTE,  InstrName::BIT, 4} }

		};
	}

  void ConnectBus(Bus* bp) { bus = bp; std::cout << "MosTech6502 connected to the bus !\n"; }

  void PrintState();
  void Reset();  
  DataDetails FetchData(Instruction instr);
  void ExecuteInstruction();    

	// helpers
	void ExecBranchInstr(const Instruction& instr, FLAGS6502 f, uint8_t expectedValue);
	void CompareRegister(const Instruction& instr, uint8_t targetReg); 

public : // private:
  
  // registers 
	uint8_t  a      = 0x00;		
	uint8_t  x      = 0x00;		
	uint8_t  y      = 0x00;		
	uint8_t  sp     = 0x00;		
	uint16_t pc     = 0x0000;	
  
  // status register member + utils  
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

