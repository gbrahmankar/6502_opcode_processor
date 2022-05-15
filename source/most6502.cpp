#include "../include/most6502.h"
// concept : we need full obj declaration during usage eg : bus->Read(...)
#include "../include/bus.h"  // to prevent circular includes

// helpers

void MosT6502::PrintState()
{
    std::cout << "\nMosT6502_State : "
              << "a=" << STREAM_BYTE(a) << " | "
              << "x=" << STREAM_BYTE(x) << " | "
              << "y=" << STREAM_BYTE(y) << " | "
              << "sp=" << STREAM_BYTE(sp) << " | "
              << "pc=" << STREAM_WORD(pc) << '\n';
    PrintStatus();
    std::cout << "\n";
}

void MosT6502::Reset()
{
    uint16_t lowByte  = bus->Read(0xFFFC);
    uint16_t highByte = bus->Read(0xFFFD);
    pc                = (highByte << 8) | lowByte;  // hardwired for the programmer

    a = x = y = 0x00;

    sp = 0xFF;  // bus/ram space : 0x0100 - 0x01FF

    sr = 0x00;
    SetFlag(FLAGS6502::U, true);  // this is set at reset
}

void MosT6502::ExecBranchInstr(const MosT6502::Instruction& instr, MosT6502::FLAGS6502 f,
                               uint8_t expectedValue)
{
    uint16_t jumpDelta = (uint16_t)FetchData(instr).data;
    if (jumpDelta & 0x80) {
        jumpDelta |= 0xff00;
    }
    if (GetFlag(f) == expectedValue) {
        pc += jumpDelta;
    }
}

void MosT6502::CompareRegister(const MosT6502::Instruction& instr, uint8_t targetReg)
{
    auto dd = FetchData(instr);

    uint16_t temp = (uint16_t)targetReg - (uint16_t)dd.data;
    SetFlag(FLAGS6502::C, a >= dd.data);
    SetFlag(FLAGS6502::Z, (temp & 0x00ff) == 0x0000);
    SetFlag(FLAGS6502::N, temp & 0x0080);
}

void MosT6502::ExecIRQ()
{
    if (GetFlag(FLAGS6502::I) == 0) {
        bus->Write(0x0100 + sp, (pc >> 8) & 0x00ff);
        sp -= 1;
        bus->Write(0x0100 + sp, pc & 0x00ff);
        sp -= 1;

        SetFlag(FLAGS6502::B, false);
        SetFlag(FLAGS6502::U, true);
        SetFlag(FLAGS6502::I, true);
        bus->Write(0x0100 + sp, sr);
        sp -= 1;

        uint16_t pc_read_addr = 0xfffe;
        uint16_t lo           = bus->Read(pc_read_addr + 0);
        uint16_t hi           = bus->Read(pc_read_addr + 1);
        pc                    = (hi << 8) | lo;
    }
}

void MosT6502::NmExecIRQ()
{
    bus->Write(0x0100 + sp, (pc >> 8) & 0x00ff);
    sp -= 1;
    bus->Write(0x0100 + sp, pc & 0x00ff);
    sp -= 1;

    SetFlag(FLAGS6502::B, false);
    SetFlag(FLAGS6502::U, true);
    SetFlag(FLAGS6502::I, true);
    bus->Write(0x0100 + sp, sr);
    sp -= 1;

    uint16_t pc_read_addr = 0xfffa;
    uint16_t lo           = bus->Read(pc_read_addr + 0);
    uint16_t hi           = bus->Read(pc_read_addr + 1);
    pc                    = (hi << 8) | lo;
}

MosT6502::DataDetails MosT6502::FetchData(Instruction instr)
{
    DataDetails dd;

    switch (instr.addrMode) {
        case AddrMode::IMPLIED: {
            dd = {a, 0x0000};
            break;
        }
        case AddrMode::IMMEDIATE: {
            dd = {bus->Read(pc), pc};
            pc += 1;
            break;
        }
        case AddrMode::ZERO_PAGE: {
            uint16_t zpOffset = bus->Read(pc);
            pc += 1;
            dd = {bus->Read(zpOffset), zpOffset};
            break;
        }
        case AddrMode::ZERO_PAGE_X: {
            uint16_t zpOffset = bus->Read(pc);
            pc += 1;
            zpOffset += zpOffset + (uint16_t)x;
            dd = {bus->Read((zpOffset) & (0x00FF)), (uint16_t)((zpOffset) & (0x00FF))};
            break;
        }
        case AddrMode::ZERO_PAGE_Y: {
            uint16_t zpOffset = bus->Read(pc);
            pc += 1;
            zpOffset += zpOffset + (uint16_t)y;
            dd = {bus->Read((zpOffset) & (0x00FF)), (uint16_t)((zpOffset) & (0x00FF))};
            break;
        }
        case AddrMode::ABSOLUTE: {
            uint16_t lo = bus->Read(pc);
            pc += 1;
            uint16_t hi = bus->Read(pc);
            pc += 1;
            dd = {bus->Read((hi << 8) | lo), (uint16_t)((hi << 8) | lo)};
            break;
        }
        case AddrMode::ABSOLUTE_X: {
            uint16_t lo = bus->Read(pc);
            pc += 1;
            uint16_t hi = bus->Read(pc);
            pc += 1;
            dd = {bus->Read(((hi << 8) | lo) + x), (uint16_t)(((hi << 8) | lo) + x)};
            break;
        }
        case AddrMode::ABSOLUTE_Y: {
            uint16_t lo = bus->Read(pc);
            pc += 1;
            uint16_t hi = bus->Read(pc);
            pc += 1;
            dd = {bus->Read(((hi << 8) | lo) + y), (uint16_t)(((hi << 8) | lo) + y)};
            break;
        }
        case AddrMode::INDIRECT: {
            uint16_t lo = bus->Read(pc);
            pc += 1;
            uint16_t hi = bus->Read(pc);
            pc += 1;
            uint16_t ptr = (hi << 8) | lo;

            dd = {bus->Read((bus->Read(ptr + 1) << 8) | bus->Read(ptr + 0)),
                  (uint16_t)((bus->Read(ptr + 1) << 8) | bus->Read(ptr + 0))};
            break;
        }
        case AddrMode::INDIRECT_X: {
            uint16_t list_base_addr = bus->Read(pc);
            pc += 1;
            uint16_t list_addr = list_base_addr + x;

            uint16_t lo = bus->Read(list_addr);
            uint16_t hi = bus->Read(list_addr + 1);

            dd = {bus->Read((hi << 8) | lo), (uint16_t)((hi << 8) | lo)};
            break;
        }
        case AddrMode::INDIRECT_Y: {
            uint16_t list_base_addr = bus->Read(pc);
            pc += 1;
            uint16_t list_addr = list_base_addr + y;

            uint16_t lo = bus->Read(list_addr);
            uint16_t hi = bus->Read(list_addr + 1);

            dd = {bus->Read((hi << 8) | lo), (uint16_t)((hi << 8) | lo)};
            break;
        }
        case AddrMode::RELATIVE: {
            uint8_t jumpDelta = bus->Read(pc);
            pc += 1;

            dd = {jumpDelta, pc};
            break;
        }
        default: {
            std::cout << "Addr_mode=" << GetAddrModeName(instr.addrMode)
                      << " not implemented yet for instr=" << instr.nameStr;
            abort();
        }
    }
    return dd;
}

void MosT6502::ExecuteInstruction()
{
    uint8_t opcode = bus->Read(pc);
    pc += 1;  // as soon as a read from pc happens; pc++; from WD spec;

    if (opcode == TERMINATE_OPCODE) {
        std::cout << "\nProgram completed !\n";
        bus->PrintRamState();
        exit(0);
    }

    if (m_instrSet.find(opcode) == m_instrSet.end()) {
        std::cout << "Illegal instr in the code. opcode=" << (opcode);
        abort();
    }
    auto instr = m_instrSet[opcode];
    std::cout << "instr_name=" << instr.nameStr << '\n';

    switch (instr.instrName) {
        case InstrName::BRK: {
            break;
        }
        case InstrName::ADC: {
            uint16_t byteData = (uint16_t)FetchData(instr).data;
            uint16_t result   = (uint16_t)a + byteData + (uint16_t)GetFlag(FLAGS6502::C);

            SetFlag(FLAGS6502::C, result > 255);
            SetFlag(FLAGS6502::Z, (result & 0xff) == 0);
            SetFlag(FLAGS6502::V,
                    (~((uint16_t)a ^ byteData) & ((uint16_t)a ^ (uint16_t)result)) & 0x0080);
            SetFlag(FLAGS6502::N, result & 0x80);

            a = result & 0xff;

            break;
        }
        case InstrName::SBC: {  // TODO !
            break;
        }
        case InstrName::AND: {
            a = a & FetchData(instr).data;

            SetFlag(FLAGS6502::Z, a == 0x00);
            SetFlag(FLAGS6502::N, a & 0x80);
            break;
        }
        case InstrName::ASL: {
            auto dd = FetchData(instr);

            uint16_t dataByte = (uint16_t)dd.data << 1;

            SetFlag(FLAGS6502::C, (dataByte & 0xFF00) > 0);
            SetFlag(FLAGS6502::Z, (dataByte & 0x00FF) == 0x00);
            SetFlag(FLAGS6502::N, dataByte & 0x80);
            if (instr.addrMode == AddrMode::IMPLIED) {
                a = dataByte & 0x00FF;
            } else {
                bus->Write(dd.addr, dataByte & 0x00FF);
            }
            break;
        }
        case InstrName::BCC: {
            ExecBranchInstr(instr, FLAGS6502::C, 0);
            break;
        }
        case InstrName::BCS: {
            ExecBranchInstr(instr, FLAGS6502::C, 1);
            break;
        }
        case InstrName::BEQ: {
            ExecBranchInstr(instr, FLAGS6502::Z, 1);
            break;
        }
        case InstrName::BIT: {
            auto dd = FetchData(instr);

            SetFlag(FLAGS6502::Z, (a & dd.data) == 0x00);
            SetFlag(FLAGS6502::N, dd.data & (1 << 7));
            SetFlag(FLAGS6502::V, dd.data & (1 << 6));

            break;
        }
        case InstrName::BMI: {
            ExecBranchInstr(instr, FLAGS6502::N, 1);
            break;
        }
        case InstrName::BNE: {
            ExecBranchInstr(instr, FLAGS6502::Z, 0);
            break;
        }
        case InstrName::BPL: {
            ExecBranchInstr(instr, FLAGS6502::N, 0);
            break;
        }
        case InstrName::BVC: {
            ExecBranchInstr(instr, FLAGS6502::V, 0);
            break;
        }
        case InstrName::BVS: {
            ExecBranchInstr(instr, FLAGS6502::V, 1);
            break;
        }
        case InstrName::CLC: {
            SetFlag(FLAGS6502::C, false);
            break;
        }
        case InstrName::CLD: {
            SetFlag(FLAGS6502::D, false);
            break;
        }
        case InstrName::CLI: {
            SetFlag(FLAGS6502::I, false);
            break;
        }
        case InstrName::CLV: {
            SetFlag(FLAGS6502::V, false);
            break;
        }
        case InstrName::CMP: {
            CompareRegister(instr, a);
            break;
        }
        case InstrName::CPX: {
            CompareRegister(instr, x);
            break;
        }
        case InstrName::CPY: {
            CompareRegister(instr, y);
            break;
        }
        case InstrName::DEC: {
            auto dd = FetchData(instr);

            uint16_t temp = (uint16_t)dd.data - 1;
            bus->Write(dd.addr, temp & 0x00ff);
            SetFlag(FLAGS6502::Z, (temp & 0x00ff) == 0x0000);
            SetFlag(FLAGS6502::N, temp & 0x0080);
            break;
        }
        case InstrName::DEX: {
            uint16_t temp = (uint16_t)x - 1;
            SetFlag(FLAGS6502::Z, (temp & 0x00ff) == 0x0000);
            SetFlag(FLAGS6502::N, temp & 0x0080);
            x = temp;
            break;
        }
        case InstrName::DEY: {
            uint16_t temp = (uint16_t)y - 1;
            SetFlag(FLAGS6502::Z, (temp & 0x00ff) == 0x0000);
            SetFlag(FLAGS6502::N, temp & 0x0080);
            y = temp;
            break;
        }
        case InstrName::EOR: {
            auto dd = FetchData(instr);

            a = a ^ dd.data;
            SetFlag(FLAGS6502::Z, a == 0x00);
            SetFlag(FLAGS6502::N, a & 0x80);
            break;
        }
        case InstrName::INC: {
            auto dd = FetchData(instr);

            uint16_t temp = (uint16_t)dd.data + 1;
            bus->Write(dd.addr, temp & 0x00ff);
            SetFlag(FLAGS6502::Z, (temp & 0x00ff) == 0x0000);
            SetFlag(FLAGS6502::N, temp & 0x0080);
            break;
        }
        case InstrName::INX: {
            uint16_t temp = (uint16_t)x + 1;
            SetFlag(FLAGS6502::Z, (temp & 0x00ff) == 0x0000);
            SetFlag(FLAGS6502::N, temp & 0x0080);
            x = temp;
            break;
        }
        case InstrName::INY: {
            uint16_t temp = (uint16_t)y + 1;
            SetFlag(FLAGS6502::Z, (temp & 0x00ff) == 0x0000);
            SetFlag(FLAGS6502::N, temp & 0x0080);
            y = temp;
            break;
        }
        case InstrName::JMP: {
            pc = FetchData(instr).addr;
            break;
        }
        case InstrName::JSR: {
            uint16_t jumpAddr = FetchData(instr).addr;

            pc -= 1;

            bus->Write(0x0100 + sp, (pc >> 8) & 0x00ff);
            sp -= 1;
            bus->Write(0x0100 + sp, pc & 0x00ff);
            sp -= 1;

            pc = jumpAddr;
            break;
        }
        case InstrName::LDA: {
            a = FetchData(instr).data;
            SetFlag(FLAGS6502::Z, a == 0x00);
            SetFlag(FLAGS6502::N, a & 0x80);
            break;
        }
        case InstrName::LDX: {
            x = FetchData(instr).data;
            SetFlag(FLAGS6502::Z, x == 0x00);
            SetFlag(FLAGS6502::N, x & 0x80);
            break;
        }
        case InstrName::LDY: {
            y = FetchData(instr).data;
            SetFlag(FLAGS6502::Z, y == 0x00);
            SetFlag(FLAGS6502::N, y & 0x80);
            break;
        }
        case InstrName::LSR: {
            auto dd = FetchData(instr);
            SetFlag(FLAGS6502::C, dd.data & 0x01);
            uint8_t fData = dd.data >> 1;
            SetFlag(FLAGS6502::Z, (fData & 0xff) == 0x00);
            SetFlag(FLAGS6502::N, fData & 0x80);
            if (instr.addrMode == AddrMode::IMPLIED) {
                a = fData;
            } else {
                bus->Write(dd.addr, fData);
            }
            break;
        }
        case InstrName::NOP: {
            break;
        }
        case InstrName::ORA: {
            auto dd = FetchData(instr);

            a = a | dd.data;
            SetFlag(FLAGS6502::Z, a == 0x00);
            SetFlag(FLAGS6502::N, a & 0x80);
            break;
        }
        case InstrName::PHA: {
            bus->Write(0x0100 + sp, a);
            sp -= 1;
            break;
        }
        case InstrName::PHP: {
            bus->Write(0x0100 + sp, sr | FLAGS6502::B | FLAGS6502::U);
            SetFlag(FLAGS6502::B, false);
            SetFlag(FLAGS6502::U, false);
            sp -= 1;
            break;
        }
        case InstrName::PLA: {
            sp += 1;
            a = bus->Read(0x0100 + sp);
            SetFlag(FLAGS6502::Z, a == 0x00);
            SetFlag(FLAGS6502::N, a & 0x80);
            break;
        }
        case InstrName::PLP: {
            sp += 1;
            sr = bus->Read(0x0100 + sp);
            SetFlag(FLAGS6502::U, 1);
            break;
        }
        case InstrName::ROL: {
            auto dd = FetchData(instr);

            uint16_t rolData = (uint16_t)(dd.data << 1) | GetFlag(FLAGS6502::C);
            SetFlag(FLAGS6502::C, rolData & 0xff00);
            SetFlag(FLAGS6502::Z, (rolData & 0x00ff) == 0x0000);
            SetFlag(FLAGS6502::N, rolData & 0x0080);
            if (instr.addrMode == AddrMode::IMPLIED) {
                a = rolData & 0x00ff;
            } else {
                bus->Write(dd.addr, rolData & 0x00ff);
            }
            break;
        }
        case InstrName::ROR: {
            auto dd = FetchData(instr);

            uint16_t rorData = (uint16_t)(GetFlag(FLAGS6502::C) << 7) | (dd.data >> 1);
            SetFlag(FLAGS6502::C, rorData & 0x01);
            SetFlag(FLAGS6502::Z, (rorData & 0x00ff) == 0x00);
            SetFlag(FLAGS6502::N, rorData & 0x0080);
            if (instr.addrMode == AddrMode::IMPLIED) {
                a = rorData & 0x00ff;
            } else {
                bus->Write(dd.addr, rorData & 0x00ff);
            }
            break;
        }
        case InstrName::RTI: {
            sp += 1;
            sr = bus->Read(0x0100 + sp);
            sr &= ~FLAGS6502::B;
            sr &= ~FLAGS6502::U;

            sp += 1;
            pc = (uint16_t)bus->Read(0x0100 + sp);
            sp += 1;
            pc |= (uint16_t)bus->Read(0x0100 + sp) << 8;
            break;
        }
        case InstrName::RTS: {
            sp += 1;
            pc = (uint16_t)bus->Read(0x0100 + sp);
            sp += 1;
            pc |= (uint16_t)bus->Read(0x0100 + sp) << 8;

            pc += 1;  // Important : pc is inc here as we stacked the 2nd arg byte addr when we
                      // execd JSR
            // JSR aa bb : in this eg, pc was pointing to bb when we stacked the return addr
            // incrementing pc will point us to the next instr when we return ...
            break;
        }
        case InstrName::SEC: {
            SetFlag(FLAGS6502::C, true);
            break;
        }
        case InstrName::SED: {
            SetFlag(FLAGS6502::D, true);
            break;
        }
        case InstrName::SEI: {
            SetFlag(FLAGS6502::I, true);
            break;
        }
        case InstrName::STA: {
            auto dd = FetchData(instr);
            bus->Write(dd.addr, a);
            break;
        }
        case InstrName::STX: {
            auto dd = FetchData(instr);
            bus->Write(dd.addr, x);
            break;
        }
        case InstrName::STY: {
            auto dd = FetchData(instr);
            bus->Write(dd.addr, y);
            break;
        }
        case InstrName::TAX: {
            x = a;
            SetFlag(FLAGS6502::Z, x == 0x00);
            SetFlag(FLAGS6502::N, x & 0x80);
            break;
        }
        case InstrName::TAY: {
            y = a;
            SetFlag(FLAGS6502::Z, y == 0x00);
            SetFlag(FLAGS6502::N, y & 0x80);
            break;
        }
        case InstrName::TSX: {
            x = sp;
            SetFlag(FLAGS6502::Z, x == 0x00);
            SetFlag(FLAGS6502::N, x & 0x80);
            break;
        }
        case InstrName::TXA: {
            a = x;
            SetFlag(FLAGS6502::Z, a == 0x00);
            SetFlag(FLAGS6502::N, a & 0x80);
            break;
        }
        case InstrName::TXS: {
            sp = x;
            break;
        }
        case InstrName::TYA: {
            a = y;
            SetFlag(FLAGS6502::Z, a == 0x00);
            SetFlag(FLAGS6502::N, a & 0x80);
            break;
        }
        default: {
            std::cout << "Instruction opcode=" << opcode << " not implemented yet";
            abort();
        }
    }

    PrintState();
}
