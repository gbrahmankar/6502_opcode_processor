#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "bus.h"

class OpcodeProcessor {
   public:
    void Init() { bus.Initialize(); }

    bool ProcessFile(const std::string& fileAbs, uint16_t startProcAddr)
    {
        std::ifstream sourceFile;
        sourceFile.open(fileAbs);

        bus.Write(0xfffc, startProcAddr & 0xff);
        bus.Write(0xfffd, (startProcAddr >> 8) & 0xff);

        unsigned int sourceByte;
        uint16_t byteCnt = 0;

        while (sourceFile >> std::hex >> sourceByte) {
            bus.Write(startProcAddr + byteCnt, sourceByte);
            byteCnt += 1;
        }

        sourceFile.close();

        std::cout << "Program start_addr=" << STREAM_BYTE(startProcAddr)
                  << " end_addr=" << STREAM_BYTE(startProcAddr + byteCnt - 1 - 1) << '\n';

        bus.StartCpu();

        while (true) {
            bus.GetMicroprocessor().ExecuteInstruction();
        }

        return true;
    }

    void Shutdown() { bus.Unplug(); }

   private:
    Bus bus;
};
