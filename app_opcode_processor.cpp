#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "include/opcode_processor.hpp"

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "usage : ./opcode_processor <start_addr_in_hex> 6502_hex_mc (ex: "
                     ">./opcode_processor 0xffa0 6502_hex_mc)\n";
        return 1;
    }

    std::string startProcAddrString(argv[1]);
    std::stringstream startProcAddrStream(startProcAddrString);
    uint16_t startProcAddr;
    startProcAddrStream >> std::hex >> startProcAddr;

    OpcodeProcessor ocp;
    ocp.Init();
    ocp.ProcessFile(std::string(argv[2]), startProcAddr);
    ocp.Shutdown();
    return 0;
}
