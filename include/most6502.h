#pragma once

#include <iostream>

#include <map>
#include <string>
#include <vector>

#include "../include/most_common.h"

class Bus;

class MosT6502 {
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

   public:
    // addr modes
    enum AddrMode
    {
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
    enum InstrName
    {
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
        DEY,
        EOR,
        INC,
        INX,
        INY,
        JMP,
        JSR,
        LDA,
        LDX,
        LDY,
        LSR,
        NOP,
        ORA,
        PHA,
        PHP,
        PLA,
        PLP,
        ROL,
        ROR,
        RTI,
        RTS,
        SEC,
        SED,
        SEI,
        STA,
        STX,
        STY,
        TAX,
        TAY,
        TSX,
        TXA,
        TXS,
        TYA
    };

    struct Instruction {
        std::string nameStr;
        AddrMode addrMode;
        InstrName instrName;
        uint8_t cycles = 0;
    };

    struct DataDetails {  // data and its mem_loc
        uint8_t data;
        uint16_t addr;
    };

    MosT6502()
    {
        m_instrSet = {

            {0x00, {"break", AddrMode::IMMEDIATE, InstrName::BRK, 7}},

            // load-store
            {0xa9, {"load_a_imm", AddrMode::IMMEDIATE, InstrName::LDA, 2}},
            {0xa5, {"load_a_zp", AddrMode::ZERO_PAGE, InstrName::LDA, 3}},
            {0xb5, {"load_a_zp_x", AddrMode::ZERO_PAGE_X, InstrName::LDA, 4}},
            {0xad, {"load_a_abs", AddrMode::ABSOLUTE, InstrName::LDA, 4}},
            {0xbd, {"load_a_abs_x", AddrMode::ABSOLUTE_X, InstrName::LDA, 4}},
            {0xb9, {"load_a_abs_y", AddrMode::ABSOLUTE_Y, InstrName::LDA, 4}},
            {0xa1, {"load_a_ind_x", AddrMode::INDIRECT_X, InstrName::LDA, 6}},
            {0xb1, {"load_a_ind_y", AddrMode::INDIRECT_Y, InstrName::LDA, 5}},

            {0xa2, {"load_x_imm", AddrMode::IMMEDIATE, InstrName::LDX, 2}},
            {0xa6, {"load_x_zp", AddrMode::ZERO_PAGE, InstrName::LDX, 3}},
            {0xb6, {"load_x_zp_x", AddrMode::ZERO_PAGE_X, InstrName::LDX, 4}},
            {0xae, {"load_x_abs", AddrMode::ABSOLUTE, InstrName::LDX, 4}},
            {0xbe, {"load_x_abs_x", AddrMode::ABSOLUTE_X, InstrName::LDX, 4}},

            {0xa0, {"load_y_imm", AddrMode::IMMEDIATE, InstrName::LDY, 2}},
            {0xa4, {"load_y_zp", AddrMode::ZERO_PAGE, InstrName::LDY, 3}},
            {0xb4, {"load_y_zp_x", AddrMode::ZERO_PAGE_X, InstrName::LDY, 4}},
            {0xac, {"load_y_abs", AddrMode::ABSOLUTE, InstrName::LDY, 4}},
            {0xbc, {"load_y_abs_x", AddrMode::ABSOLUTE_X, InstrName::LDY, 4}},

            {0x85, {"store_acc_in_memory_zp", AddrMode::ZERO_PAGE, InstrName::STA, 3}},
            {0x95, {"store_acc_in_memory_zp_x", AddrMode::ZERO_PAGE_X, InstrName::STA, 4}},
            {0x8d, {"store_acc_in_memory_abs", AddrMode::ABSOLUTE, InstrName::STA, 4}},
            {0x9d, {"store_acc_in_memory_abs_x", AddrMode::ABSOLUTE_X, InstrName::STA, 5}},
            {0x99, {"store_acc_in_memory_abs_y", AddrMode::ABSOLUTE_Y, InstrName::STA, 5}},
            {0x81, {"store_acc_in_memory_ind_x", AddrMode::INDIRECT_X, InstrName::STA, 6}},
            {0x91, {"store_acc_in_memory_ind_y", AddrMode::INDIRECT_Y, InstrName::STA, 6}},

            {0x86, {"store_index_x_in_memory_zp", AddrMode::ZERO_PAGE, InstrName::STX, 3}},
            {0x96, {"store_index_x_in_memory_zp_y", AddrMode::ZERO_PAGE_Y, InstrName::STX, 4}},
            {0x8e, {"store_index_x_in_memory_abs", AddrMode::ABSOLUTE, InstrName::STX, 4}},

            {0x84, {"store_index_y_in_memory_zp", AddrMode::ZERO_PAGE, InstrName::STY, 3}},
            {0x94, {"store_index_y_in_memory_zp_x", AddrMode::ZERO_PAGE_X, InstrName::STY, 4}},
            {0x8c, {"store_index_y_in_memory_abs", AddrMode::ABSOLUTE, InstrName::STY, 4}},

            {0xaa, {"transfer_a_to_x", AddrMode::IMPLIED, InstrName::TAX, 2}},

            {0xa8, {"transfer_a_to_y", AddrMode::IMPLIED, InstrName::TAY, 2}},

            {0xba, {"transfer_sp_to_x", AddrMode::IMPLIED, InstrName::TSX, 2}},

            {0x8a, {"transfer_x_to_a", AddrMode::IMPLIED, InstrName::TXA, 2}},

            {0x9a, {"transfer_x_to_sp", AddrMode::IMPLIED, InstrName::TXS, 2}},

            {0x98, {"transfer_y_to_a", AddrMode::IMPLIED, InstrName::TYA, 2}},

            // add-sub
            {0x69, {"add_with_carry_imm", AddrMode::IMMEDIATE, InstrName::ADC, 2}},
            {0x65, {"add_with_carry_zp", AddrMode::ZERO_PAGE, InstrName::ADC, 3}},
            {0x75, {"add_with_carry_zp_x", AddrMode::ZERO_PAGE_X, InstrName::ADC, 4}},
            {0x6d, {"add_with_carry_abs", AddrMode::ABSOLUTE, InstrName::ADC, 4}},
            {0x7d, {"add_with_carry_abs_x", AddrMode::ABSOLUTE_X, InstrName::ADC, 4}},
            {0x79, {"add_with_carry_abs_y", AddrMode::ABSOLUTE_Y, InstrName::ADC, 4}},
            {0x61, {"add_with_carry_ind_x", AddrMode::INDIRECT_X, InstrName::ADC, 6}},
            {0x71, {"add_with_carry_ind_y", AddrMode::INDIRECT_Y, InstrName::ADC, 5}},

            {0xe9, {"sub_with_burrow_in_imm", AddrMode::IMMEDIATE, InstrName::SBC, 2}},
            {0xe5, {"sub_with_burrow_in_zp", AddrMode::ZERO_PAGE, InstrName::SBC, 3}},
            {0xf5, {"sub_with_burrow_in_zp_x", AddrMode::ZERO_PAGE_X, InstrName::SBC, 4}},
            {0xed, {"sub_with_burrow_in_abs", AddrMode::ABSOLUTE, InstrName::SBC, 4}},
            {0xfd, {"sub_with_burrow_in_abs_x", AddrMode::ABSOLUTE_X, InstrName::SBC, 4}},
            {0xf9, {"sub_with_burrow_in_abs_y", AddrMode::ABSOLUTE_Y, InstrName::SBC, 4}},
            {0xe1, {"sub_with_burrow_in_ind_x", AddrMode::INDIRECT_X, InstrName::SBC, 6}},
            {0xf1, {"sub_with_burrow_in_ind_y", AddrMode::INDIRECT_Y, InstrName::SBC, 5}},

            // logical
            {0x29, {"and_mem_and_acc_imm", AddrMode::IMMEDIATE, InstrName::AND, 2}},
            {0x25, {"and_mem_and_acc_zp", AddrMode::ZERO_PAGE, InstrName::AND, 3}},
            {0x35, {"and_mem_and_acc_zp_x", AddrMode::ZERO_PAGE_X, InstrName::AND, 4}},
            {0x2d, {"and_mem_and_acc_abs", AddrMode::ABSOLUTE, InstrName::AND, 4}},
            {0x3d, {"and_mem_and_acc_abs_x", AddrMode::ABSOLUTE_X, InstrName::AND, 4}},
            {0x39, {"and_mem_and_acc_abs_y", AddrMode::ABSOLUTE_Y, InstrName::AND, 4}},
            {0x21, {"and_mem_and_acc_ind_x", AddrMode::INDIRECT_X, InstrName::AND, 6}},
            {0x31, {"and_mem_and_acc_ind_y", AddrMode::INDIRECT_Y, InstrName::AND, 5}},

            {0x49, {"exclusive_or_imm", AddrMode::IMMEDIATE, InstrName::EOR, 2}},
            {0x46, {"exclusive_or_zp", AddrMode::ZERO_PAGE, InstrName::EOR, 3}},
            {0x55, {"exclusive_or_zp_x", AddrMode::ZERO_PAGE_X, InstrName::EOR, 4}},
            {0x4d, {"exclusive_or_abs", AddrMode::ABSOLUTE, InstrName::EOR, 4}},
            {0x5d, {"exclusive_or_abs_x", AddrMode::ABSOLUTE_X, InstrName::EOR, 4}},
            {0x59, {"exclusive_or_abs_y", AddrMode::ABSOLUTE_Y, InstrName::EOR, 4}},
            {0x41, {"exclusive_or_ind_x", AddrMode::INDIRECT_X, InstrName::EOR, 6}},
            {0x51, {"exclusive_or_ind_y", AddrMode::INDIRECT_Y, InstrName::EOR, 5}},

            {0x09, {"or_mem_with_acc_imm", AddrMode::IMMEDIATE, InstrName::ORA, 2}},
            {0x05, {"or_mem_with_acc_zp", AddrMode::ZERO_PAGE, InstrName::ORA, 3}},
            {0x15, {"or_mem_with_acc_zp_x", AddrMode::ZERO_PAGE_X, InstrName::ORA, 4}},
            {0x0d, {"or_mem_with_acc_abs", AddrMode::ABSOLUTE, InstrName::ORA, 4}},
            {0x1d, {"or_mem_with_acc_abs_x", AddrMode::ABSOLUTE_X, InstrName::ORA, 4}},
            {0x19, {"or_mem_with_acc_abs_y", AddrMode::ABSOLUTE_Y, InstrName::ORA, 4}},
            {0x01, {"or_mem_with_acc_ind_x", AddrMode::INDIRECT_X, InstrName::ORA, 6}},
            {0x11, {"or_mem_with_acc_ind_y", AddrMode::INDIRECT_Y, InstrName::ORA, 5}},

            // shifts
            {0x0a, {"arithmetic_shift_left_1_bit_acc", AddrMode::IMPLIED, InstrName::ASL, 2}},
            {0x06, {"arithmetic_shift_left_1_bit_zp", AddrMode::ZERO_PAGE, InstrName::ASL, 5}},
            {0x16, {"arithmetic_shift_left_1_bit_zp_x", AddrMode::ZERO_PAGE_X, InstrName::ASL, 6}},
            {0x0e, {"arithmetic_shift_left_1_bit_abs", AddrMode::ABSOLUTE, InstrName::ASL, 6}},
            {0x1e, {"arithmetic_shift_left_1_bit_abs_x", AddrMode::ABSOLUTE_X, InstrName::ASL, 7}},

            {0x4a, {"shift_one_bit_right_acc", AddrMode::IMPLIED, InstrName::LSR, 2}},
            {0x46, {"shift_one_bit_right_zp", AddrMode::ZERO_PAGE, InstrName::LSR, 5}},
            {0x56, {"shift_one_bit_right_zp_x", AddrMode::ZERO_PAGE_X, InstrName::LSR, 6}},
            {0x4e, {"shift_one_bit_right_abs", AddrMode::ABSOLUTE, InstrName::LSR, 6}},
            {0x5e, {"shift_one_bit_right_abs_x", AddrMode::ABSOLUTE_X, InstrName::LSR, 7}},

            {0x2a, {"rotate_one_bit_left_acc", AddrMode::IMPLIED, InstrName::ROL, 2}},
            {0x26, {"rotate_one_bit_left_zp", AddrMode::ZERO_PAGE, InstrName::ROL, 5}},
            {0x36, {"rotate_one_bit_left_zp_x", AddrMode::ZERO_PAGE_X, InstrName::ROL, 6}},
            {0x2e, {"rotate_one_bit_left_abs", AddrMode::ABSOLUTE, InstrName::ROL, 6}},
            {0x3e, {"rotate_one_bit_left_abs_x", AddrMode::ABSOLUTE_X, InstrName::ROL, 7}},

            {0x6a, {"rotate_one_bit_right_acc", AddrMode::IMPLIED, InstrName::ROR, 2}},
            {0x66, {"rotate_one_bit_right_zp", AddrMode::ZERO_PAGE, InstrName::ROR, 5}},
            {0x76, {"rotate_one_bit_right_zp_x", AddrMode::ZERO_PAGE_X, InstrName::ROR, 6}},
            {0x6e, {"rotate_one_bit_right_abs", AddrMode::ABSOLUTE, InstrName::ROR, 6}},
            {0x7e, {"rotate_one_bit_right_abs_x", AddrMode::ABSOLUTE_X, InstrName::ROR, 7}},

            // branch relative
            {0x90, {"branch_on_carry_clear", AddrMode::RELATIVE, InstrName::BCC, 2}},
            {0xb0, {"branch_on_carry_set", AddrMode::RELATIVE, InstrName::BCS, 2}},
            {0xf0, {"branch_on_result_zero", AddrMode::RELATIVE, InstrName::BEQ, 2}},
            {0x30, {"branch_on_result_minus", AddrMode::RELATIVE, InstrName::BMI, 2}},
            {0xd0, {"branch_on_result_not_zero", AddrMode::RELATIVE, InstrName::BNE, 2}},
            {0x10, {"branch_on_result_plus", AddrMode::RELATIVE, InstrName::BPL, 2}},
            {0x50, {"branch_on_overflow_clear", AddrMode::RELATIVE, InstrName::BVC, 2}},
            {0x70, {"branch_on_overflow_set", AddrMode::RELATIVE, InstrName::BVS, 2}},

            {0x4c, {"jump_to_new_loc_abs", AddrMode::ABSOLUTE, InstrName::JMP, 3}},
            {0x6c, {"jump_to_new_loc_ind", AddrMode::INDIRECT, InstrName::JMP, 5}},

            {0x20, {"jump_to_subroutine", AddrMode::ABSOLUTE, InstrName::JSR, 6}},

            {0x40, {"return_from_interrupt", AddrMode::IMPLIED, InstrName::RTI, 6}},
            {0x60, {"return_from_subroutine", AddrMode::IMPLIED, InstrName::RTS, 6}},

            // set/clear flags
            {0x18, {"clear_carry_flag", AddrMode::IMPLIED, InstrName::CLC, 2}},

            {0xD8, {"clear_decimal_flag", AddrMode::IMPLIED, InstrName::CLD, 2}},

            {0x58, {"disable_interrupts", AddrMode::IMPLIED, InstrName::CLI, 2}},

            {0xB8, {"clear_overflow_flag", AddrMode::IMPLIED, InstrName::CLV, 2}},

            {0x38, {"set_carry_flag", AddrMode::IMPLIED, InstrName::SEC, 2}},

            {0xf8, {"set_decimal_flag", AddrMode::IMPLIED, InstrName::SED, 2}},

            {0x78, {"set_interrupt_disable_status_flag", AddrMode::IMPLIED, InstrName::SEI, 2}},

            // comparison
            {0xc9, {"cmp_imm", AddrMode::IMMEDIATE, InstrName::CMP, 2}},
            {0xc5, {"cmp_zp", AddrMode::ZERO_PAGE, InstrName::CMP, 3}},
            {0xd5, {"cmp_zp_x", AddrMode::ZERO_PAGE_X, InstrName::CMP, 4}},
            {0xcd, {"cmp_abs", AddrMode::ABSOLUTE, InstrName::CMP, 4}},
            {0xdd, {"cmp_abs_x", AddrMode::ABSOLUTE_X, InstrName::CMP, 4}},
            {0xd9, {"cmp_abs_y", AddrMode::ABSOLUTE_Y, InstrName::CMP, 4}},
            {0xc1, {"cmp_ind_x", AddrMode::INDIRECT_X, InstrName::CMP, 6}},
            {0xd1, {"cmp_ind_y", AddrMode::INDIRECT_Y, InstrName::CMP, 5}},

            {0xe0, {"cmp_x_imm", AddrMode::IMMEDIATE, InstrName::CPX, 2}},
            {0xe4, {"cmp_x_zp", AddrMode::ZERO_PAGE, InstrName::CPX, 3}},
            {0xec, {"cmp_x_abs", AddrMode::ABSOLUTE, InstrName::CPX, 4}},

            {0xc0, {"cmp_y_imm", AddrMode::IMMEDIATE, InstrName::CPY, 2}},
            {0xc4, {"cmp_y_zp", AddrMode::ZERO_PAGE, InstrName::CPY, 3}},
            {0xcc, {"cmp_y_abs", AddrMode::ABSOLUTE, InstrName::CPY, 4}},

            // decrement/increment
            {0xc6, {"dec_mem_1_zp", AddrMode::ZERO_PAGE, InstrName::DEC, 5}},
            {0xd6, {"dec_mem_1_zp_x", AddrMode::ZERO_PAGE_X, InstrName::DEC, 6}},
            {0xce, {"dec_mem_1_abs", AddrMode::ABSOLUTE, InstrName::DEC, 6}},
            {0xde, {"dec_mem_1_abs_x", AddrMode::ABSOLUTE_X, InstrName::DEC, 7}},

            {0xca, {"dec_x_1", AddrMode::IMPLIED, InstrName::DEX, 2}},

            {0x88, {"dec_y_1", AddrMode::IMPLIED, InstrName::DEY, 2}},

            {0xe6, {"inc_mem_1_zp", AddrMode::ZERO_PAGE, InstrName::INC, 5}},
            {0xf6, {"inc_mem_1_zp_x", AddrMode::ZERO_PAGE_X, InstrName::INC, 6}},
            {0xee, {"inc_mem_1_abs", AddrMode::ABSOLUTE, InstrName::INC, 6}},
            {0xfe, {"inc_mem_1_abs_x", AddrMode::ABSOLUTE_X, InstrName::INC, 7}},

            {0xe8, {"inc_x_1", AddrMode::IMPLIED, InstrName::INX, 2}},

            {0xc8, {"inc_y_1", AddrMode::IMPLIED, InstrName::INY, 2}},

            // stack ops
            {0x48, {"push_acc_on_stack", AddrMode::IMPLIED, InstrName::PHA, 3}},

            {0x08, {"push_proc_status_on_stack", AddrMode::IMPLIED, InstrName::PHP, 3}},

            {0x68, {"pull_acc_from_stack", AddrMode::IMPLIED, InstrName::PLA, 4}},

            {0x28, {"pull_proc_status_from_stack", AddrMode::IMPLIED, InstrName::PLP, 4}},

            // lesser used
            {0x24, {"test_bit_in_mem_with_acc_zp", AddrMode::ZERO_PAGE, InstrName::BIT, 3}},

            {0x2c, {"test_bit_in_mem_with_acc_abs", AddrMode::ABSOLUTE, InstrName::BIT, 4}}

        };
    }

    void ConnectBus(Bus* bp)
    {
        bus = bp;
        std::cout << "MosTech6502 connected to the bus !\n";
    }

    void PrintState();
    void Reset();
    DataDetails FetchData(Instruction instr);
    void ExecuteInstruction();

    // helpers
    void ExecBranchInstr(const Instruction& instr, FLAGS6502 f, uint8_t expectedValue);
    void CompareRegister(const Instruction& instr, uint8_t targetReg);
    void ExecIRQ();
    void NmExecIRQ();

   public:  // private:
            // registers
    uint8_t a   = 0x00;
    uint8_t x   = 0x00;
    uint8_t y   = 0x00;
    uint8_t sp  = 0x00;
    uint16_t pc = 0x0000;

    // status register member + utils
    uint8_t sr = 0x00;
    bool GetFlag(FLAGS6502 f) { return (((sr & f) > 0) ? 1 : 0); }
    void SetFlag(FLAGS6502 f, bool v) { ((v) ? sr |= f : sr &= ~f); }
    void PrintStatus()
    {
        std::cout << "carry   =" << ((sr & FLAGS6502::C) ? "1" : "0") << '\n';
        std::cout << "zero    =" << ((sr & FLAGS6502::Z) ? "1" : "0") << '\n';
        std::cout << "maski   =" << ((sr & FLAGS6502::I) ? "1" : "0") << '\n';
        std::cout << "decim   =" << ((sr & FLAGS6502::D) ? "1" : "0") << '\n';
        std::cout << "break   =" << ((sr & FLAGS6502::B) ? "1" : "0") << '\n';
        std::cout << "unused  =" << ((sr & FLAGS6502::U) ? "1" : "0") << '\n';
        std::cout << "overflow=" << ((sr & FLAGS6502::V) ? "1" : "0") << '\n';
        std::cout << "negative=" << ((sr & FLAGS6502::N) ? "1" : "0") << '\n';
    }

    std::string GetAddrModeName(AddrMode addrMode)
    {
        switch (addrMode) {
            case (AddrMode::IMMEDIATE): {
                return "immediate";
            }
            case (AddrMode::IMPLIED): {
                return "implied";
            }
            case (AddrMode::ZERO_PAGE): {
                return "zero_page";
            }
            case (AddrMode::ZERO_PAGE_X): {
                return "zero_page_x";
            }
            case (AddrMode::ZERO_PAGE_Y): {
                return "zero_page_y";
            }
            case (AddrMode::RELATIVE): {
                return "relative";
            }
            case (AddrMode::ABSOLUTE): {
                return "absolute";
            }
            case (AddrMode::ABSOLUTE_X): {
                return "absolute_x";
            }
            case (AddrMode::ABSOLUTE_Y): {
                return "absolute_y";
            }
            case (AddrMode::INDIRECT): {
                return "indirect";
            }
            case (AddrMode::INDIRECT_X): {
                return "indirect_x";
            }
            case (AddrMode::INDIRECT_Y): {
                return "indirect_y";
            }
        }
        return "xxx";
    }

    std::map<uint8_t, Instruction> m_instrSet;

    Bus* bus;
};
