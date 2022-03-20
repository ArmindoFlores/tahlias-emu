#include <iostream>
#include <fstream>
#include <limits>
#include "emulator.h"

std::string readROM(std::string& file)
{
    std::string contents;
    std::ifstream in(file);

    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();

    return contents;
}

void dissassemble(std::string &content)
{
    Emulator emu;
    emu.SetROM((uint8_t *)content.c_str(), content.size());
    std::cout << emu.Dissassemble();
}

void runInteractive(std::string &content)
{
    Emulator emu;
    emu.Init();
    emu.SetROM((uint8_t *)content.c_str(), content.size());

    std::string cmd = "", prev = "";
    while (true) {
        std::cout << "(EMU1.0) ";
        prev = cmd;
        std::getline(std::cin, cmd);

        if (cmd == "") cmd = prev;

        if (cmd == "run" || cmd == "r") {
            std::cout << "Resumed emulation!" << std::endl; 
            Error err = emu.Run(false);
            std::cout << std::endl;
            if (err == HALT)
                std::cout << "Program finished!" << std::endl;
            else if (err == ERROR)
                std::cout << RED << "Program finished with errors!" << CLR << std::endl;
            else
                std::cout << "Reached breakpoint" << std::endl;
        }

        else if (cmd == "step" || cmd == "s")
            emu.Step();

        else if (cmd == "debug" || cmd == "d") {
            emu.SetDebug(!emu.Debug());
            if (emu.Debug())
                std::cout << "Debug mode is now " << GREEN << BOLD << "on" << CLR << std::endl;
            else
                std::cout << "Debug mode is now " << RED << BOLD << "off" << CLR << std::endl;
        }

        else if (cmd == "init" || cmd == "i") {
            emu.Init();
            emu.SendMessage("YES\n");
            std::cout << "Reset emulator!" << std::endl;
        }

        else if (cmd == "memdump" || cmd == "m") {
            MemoryDump m = emu.MemDump();
            emu.PrintMemoryDump(m);
        }

        else if (cmd == "ram") {
            const uint8_t *ram = emu.RAMDump();
            std::ofstream f("RAM.bin", std::fstream::binary);
            f.write((const char *)ram, 256000);
            f.close();
            std::cout << "Wrote RAM contents to " << BOLD << "RAM.bin" << CLR << std::endl;
        }

        else if (cmd.substr(0, 6) == "break ") {
            std::string n = cmd.substr(6);
            long long breakpoint = std::atoll(n.c_str());
            emu.SetBreakpoint(breakpoint);
            std::cout << "Set breakpoint at " << BOLD << breakpoint << CLR << std::endl;
        }

        else if (cmd == "clear" || cmd == "c") {
            emu.DeleteBreakpoints();
            std::cout << "Cleared all breakpoints" << std::endl;
        }

        else if (cmd == "st") {
            auto stack = emu.GetStack();
            if (stack.size() == 0)
                std::cout << YELLOW << "Nothing on the stack" << CLR << std::endl;
            else {
                for (auto p : stack)
                    std::cout << MAGENTA << BOLD << "#" << p.first 
                              << CLR <<" -> " << MAGENTA << BOLD "#" 
                              << p.second << CLR << std::endl;
            }
            
        }

        else if (cmd == "quit" || cmd == "q") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [ROMFILE] [--dump]" << std::endl;
        return 0;
    }

    system(("base64 -d " + std::string(argv[1]) + " > rom.rom").c_str());

    std::string filename("rom.rom");
    std::string content = readROM(filename);

    if (argc == 2 || std::string(argv[2]) != std::string("--dump"))
        runInteractive(content);
    else
        dissassemble(content);
}