#pragma once

#include <array>
#include <cstdint>
#include <iomanip>

#include "mos_t_6502.h"
#include "mos_t_common.h"

class Bus {
   public:
    void Initialize();

    void StartCpu()
    {
        std::cout << "\nPowering up the legendary MOS-Technology-6502's basic emulator ...\n";
        mp.Reset();
    };

    MosT6502 GetMicroprocessor() { return mp; };

    void Write(uint16_t addr, uint8_t data);
    uint8_t Read(uint16_t addr);
    void PrintRamState();
    void PrintCpuState() { mp.PrintState(); }

    void Unplug(){};

   private:
    MosT6502 mp;
    std::array<uint8_t, 64 * 1024> ram;
};
