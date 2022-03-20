#ifndef EMULATOR_H
#define EMULATOR_H

#define RED "\033[31m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"
#define MAGENTA "\033[35m"
#define BOLD "\033[1m"
#define CLR "\033[0m"

#include <cstdint>
#include <cstddef>
#include <set>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <chrono>

#include "gui.h"

struct Instruction {
    Instruction() = default;
    Instruction(uint8_t* arr);
    ~Instruction() = default;

    unsigned condition() const;
    unsigned opcode() const;

    unsigned OPC, A, B, C;
};

enum Error {
    HALT,
    OK,
    ERROR,
    BREAK
};

struct MemoryDump {
    std::vector<uint8_t> mem;
    long long pc;
    bool flag;
    uint8_t serialinc, GPUX, GPUY;
    uint16_t clk;
    uint32_t addr;
};

class Emulator {
public:
    Emulator(uint8_t *ROM = nullptr, size_t N = 0);
    ~Emulator();

    void SetROM(uint8_t *ROM, size_t N = 0);
    void SetRegister(uint8_t reg, uint8_t value);
    uint8_t GetRegister(uint8_t) const;

    void Init();
    Error Run(bool restart=true);
    Error Step();
    std::string DissassembleCurrInst() const;
    std::string Dissassemble();
    Instruction GetCurrInst() const;

    void SetBreakpoint(size_t addr);
    void DeleteBreakpoint(size_t addr);
    void DeleteBreakpoints();
    const std::set<size_t> &GetBreakpoints() const;
    MemoryDump MemDump() const;
    const uint8_t *RAMDump() const;
    void SetDebug(bool value=true);
    bool Debug();

    void SendMessage(const std::string&);
    void SendSerial(uint8_t);
    uint8_t SerialIncoming();
    uint8_t ReadSerial();

    uint16_t ReadClock() const;
    void WriteClock(uint8_t value);

    void MemADDRHigh(uint8_t value);
    void MemADDRMid(uint8_t value);
    void MemADDRLow(uint8_t value);
    void MemWrite(uint8_t value);
    uint8_t MemRead();
    void SetGPUX(uint8_t value);
    void SetGPUY(uint8_t value);
    void GPUDraw(uint8_t value);
    uint8_t ReadDPAD() const;

    const std::list<std::pair<size_t, size_t>> &GetStack() const;

    static void PrintMemoryDump(MemoryDump&);

private:
    GUI *gui; 
    uint8_t *ROM;
    bool debug;
    size_t N;
    long long pc;
    uint8_t memory[63], serial[64], serialinc, flag;
    uint8_t RAM[256000], GPUX, GPUY;
    uint32_t addr;

    std::list<std::pair<size_t, size_t>> stack;
    std::chrono::time_point<std::chrono::system_clock> CLK;
    
    std::set<size_t> breakpoints;

    Error ExecuteInstruction(Instruction&);
    void WriteSerial(uint8_t word);
};

#endif