#include "emulator.h"
#include <algorithm>
#include <iostream>
#include <cstring>

static char ASCIITABLE[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ +-*/<=>()[]{}#$_?|^&!~,.:\n";

static int rl(uint8_t n, uint8_t d) 
{
    return (n << d)|((n >> (6 - d)) & 0x3f);
}

static uint8_t mask(uint8_t v)
{
    return v & 0b00111111;
}

static int8_t to_signed(uint8_t a)
{
    if (a < 32)
        return a;
    return -((a ^ 0b00111111) + 1);
}

Instruction::Instruction(uint8_t *arr)
{
    OPC = arr[0] >> 2;
    A = ((arr[0] & 0b00000011) << 4) | ((arr[1] & 0b11110000) >> 4);
    B = ((arr[1] & 0b00001111) << 2) | ((arr[2] & 0b11000000) >> 6);
    C = arr[2] & 0b00111111;
}

unsigned Instruction::condition() const
{
    int c = (OPC - 1) / 21;
    if (c == 0) return 2;
    if (c == 1) return 1;
    if (c == 2) return 0;
    return -1;
}

unsigned Instruction::opcode() const
{
    return (OPC - 1) % 21;
}

Emulator::Emulator(uint8_t *_ROM, size_t _N) : gui(nullptr), ROM(nullptr), debug(false), pc(0), flag(0)
{
    SetROM(_ROM, _N);
}

void Emulator::PrintMemoryDump(MemoryDump& mmdmp)
{
    std::cout << "===================================================== MEMORY DUMP =====================================================" << std::endl;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((unsigned)mmdmp.mem[i*8+j] != 0)
                std::cout << YELLOW;
            std::cout << i*8+j;
            if (i*8+j < 10)
                std::cout << " ";
            std::cout << ": " << (unsigned)mmdmp.mem[i*8+j] << "\t\t" << CLR;
        }
        std::cout << std::endl;
    }
    std::cout << "Flag: " << mmdmp.flag << "\t\t";
    std::cout << "PC: " << mmdmp.pc << "\t\t";
    std::cout << "Inc: " << (int)mmdmp.serialinc << "\t\t";
    std::cout << "CLK: " << mmdmp.clk << "\t\t";
    std::cout << "GPUX: " << (int)mmdmp.GPUX << "\t\t";
    std::cout << "GPUY: " << (int)mmdmp.GPUY << "\t\t";
    std::cout << "addr: " << mmdmp.addr << std::endl;
    std::cout << "=================================================== END MEMORY DUMP ===================================================" << std::endl;
}

const std::list<std::pair<size_t, size_t>> &Emulator::GetStack() const
{
    return stack;
}

void Emulator::SetROM(uint8_t *_ROM, size_t _N)
{
    if (_ROM != nullptr) {
        if (_N != N) {
            delete[] ROM;
            N = _N;
            ROM = new uint8_t[N];
        }
        std::copy(_ROM, _ROM+_N, ROM);
    }
    else {
        delete[] ROM;
        ROM = nullptr;
        N = 0;
    }
}

void Emulator::WriteSerial(uint8_t word)
{
    std::cout << ASCIITABLE[word];
}

void Emulator::SendMessage(const std::string& str)
{
    for (char c : str) {
        size_t pos = std::string(ASCIITABLE).find(c);
        if (pos == std::string::npos)
            std::cout << RED << "[!] Invalid character: '" << c << "'" << CLR << std::endl;
        else
            SendSerial((uint8_t)pos);
    }
}

void Emulator::SendSerial(uint8_t value) 
{
    if (serialinc < 63)
        serial[serialinc++] = value;
    else
        std::cout << YELLOW << "[!] Serial buffer is full" << CLR << std::endl; 
}

uint8_t Emulator::SerialIncoming()
{
    std::cout << YELLOW << "[!] Attempt to get incoming" << CLR << std::endl;
    return serialinc;
}

uint8_t Emulator::ReadSerial()
{
    std::cout << YELLOW << "[!] Attempt to read from serial" << CLR << std::endl;
    if (serialinc > 0) {
        serialinc--;
        uint8_t result = serial[0];
        std::memmove(serial, serial+1, serialinc);
        return result;
    }
    return -1;
}

uint16_t Emulator::ReadClock() const
{
    auto end = std::chrono::high_resolution_clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-CLK).count() / 10;

    //std::cout << YELLOW << "[!] Read from clock: " << (duration < 4096 ? duration : 4095) << CLR << std::endl;
    if (duration >= 0 && duration <= 4095)
        return (uint16_t) duration;
    else
        return 4095;
}

void Emulator::WriteClock(uint8_t value)
{
    if (value != 0) {
        gui->update();
        CLK = std::chrono::high_resolution_clock::now();
    }
}

void Emulator::MemADDRHigh(uint8_t value)
{ 
    addr &= 0b00000000000000000000111111111111;
    addr |= (value % 0x40) << 12;
}

void Emulator::MemADDRMid(uint8_t value)
{
    addr &= 0b00000000000000111111000000111111;
    addr |= (value % 0x40) << 6;
}

void Emulator::MemADDRLow(uint8_t value)
{
    addr &= 0b00000000000000111111111111000000;
    addr |= value % 0x40;
}

void Emulator::MemWrite(uint8_t value)
{
    if (addr >= 256000)
        std::cout << RED << "[!] Invalid RAM write at " << addr << " (" << pc << ")" << CLR << std::endl;
    else
        RAM[addr++] = value;
}

uint8_t Emulator::MemRead()
{
    if (addr >= 256000)
        std::cout << RED << "[!] Invalid RAM write at " << addr << " (" << pc << ")" << CLR << std::endl;
    else
        return RAM[addr++];
    return 0;
}

void Emulator::SetGPUX(uint8_t value)
{
    GPUX = value % 0x40;
}

void Emulator::SetGPUY(uint8_t value)
{
    GPUY = value % 0x40;
}

void Emulator::GPUDraw(uint8_t value)
{
    Color c;
    c.r = (((value & 0b100000) >> 4) + ((value & 0b010000) >> 4)) * 85;
    c.g = (((value & 0b001000) >> 2) + ((value & 0b000100) >> 2)) * 85;
    c.b = (((value & 0b000010) >> 0) + ((value & 0b000001) >> 0)) * 85;
    gui->drawPoint(GPUX, GPUY, c);
    //if (GPUX == 0 && GPUY == 0)
        //gui->update();
}

uint8_t Emulator::ReadDPAD() const
{
    std::cout << YELLOW << "[!] Attempt to read from DPAD" << CLR << std::endl;
    return 0;
}

Error Emulator::ExecuteInstruction(Instruction& i)
{
    if (debug) std::cout << DissassembleCurrInst() << std::endl;
    if (i.OPC == 0) return HALT;

    if (i.condition() != 2 && i.condition() != flag) return OK;

    switch (i.opcode()) {
        case 0: { // add rd, ra, rb
            SetRegister(i.A, GetRegister(i.B)+GetRegister(i.C));
            return OK;
        }
        case 1: { // add rd, ra, ib
            SetRegister(i.A, GetRegister(i.B)+i.C);
            return OK;
        }
        case 2: { // sub rd, ra, rb
            SetRegister(i.A, GetRegister(i.B)-GetRegister(i.C));
            return OK;
        }
        case 3: { // cmpCM
            uint8_t a, b;
            if (i.A  < 8) {
                a = GetRegister(i.B);
                b = GetRegister(i.C);
            }
            else if (i.A < 16) {
                a = GetRegister(i.C);
                b = GetRegister(i.B);
            }
            else if (i.A < 24) {
                a = GetRegister(i.B);
                b = i.C;
            }
            else {
                a = i.B;
                b = GetRegister(i.C);
            }            
            switch (i.A % 8) {
                case 0:
                    flag = 1;
                    break;
                case 1:
                    flag = 0;
                    break;
                case 2:
                    flag = (a == b);
                    break;
                case 3:
                    flag = (a != b);
                    break;
                case 4:
                    flag = (to_signed(a) < to_signed(b));
                    break;
                case 5:
                    flag = (to_signed(a) > to_signed(b));
                    break;
                case 6:
                    flag = (a < b);
                    break;
                case 7:
                    flag = (a > b);
                    break;
            }
            return OK;
        }
        case 4: { // or rd, ra, rb
            SetRegister(i.A, mask(GetRegister(i.B)) | mask(GetRegister(i.C)));
            return OK;
        }
        case 5: { // or rd, ra, ib
            SetRegister(i.A, mask(GetRegister(i.B)) | mask(i.C));
            return OK;
        }
        case 6: { // xor rd, ra, rb
            SetRegister(i.A, mask(GetRegister(i.B)) ^ mask(GetRegister(i.C)));
            return OK;
        }
        case 7: { // xor rd, ra, ib
            SetRegister(i.A, mask(GetRegister(i.B)) ^ mask(i.C));
            return OK;
        }
        case 8: { // and rd, ra, rb
            SetRegister(i.A, GetRegister(i.B) & GetRegister(i.C));
            return OK;
        }
        case 9: { // and rd, ra, ib
            SetRegister(i.A, GetRegister(i.B) & i.C);
            return OK;
        }
        case 10: { // Shift/rotate by immediate
            if (i.C < 8) { // shl rd, ra, ib
                SetRegister(i.A, mask(GetRegister(i.B) << (i.C % 8)));
            }
            else if (i.C < 16) { // shr rd, ra, ib
                SetRegister(i.A, mask(GetRegister(i.B) >> (i.C % 8)));
            }
            else if (i.C < 24) { // sar rd, ra, ib
                SetRegister(i.A, mask(((int8_t)GetRegister(i.B)) >> (i.C % 8)));
            }
            else if (i.C < 32) { // rol rd, ra, ib
                SetRegister(i.A, mask(rl(GetRegister(i.B), i.C % 8)));
            }
            else {
                std::cout << RED << "[!] Invalid instruction!" << CLR << std::endl;
                return ERROR;
            }
            return OK;
        }
        case 11: { // shl rd, ra, rb
            SetRegister(i.A, mask(GetRegister(i.B) << GetRegister(i.C)));
            return OK;
        }
        case 12: { // shr rd, ra, rb
            SetRegister(i.A, mask(GetRegister(i.B) >> GetRegister(i.C)));
            return OK;
        }
        case 13: { // ld rd, [ra+ib]
            SetRegister(i.A, GetRegister((GetRegister(i.B) + i.C) % 0x40));
            return OK;
        }
        case 14: { // st [ra+ib], rs
            SetRegister((GetRegister(i.B) + i.C) % 0x40, GetRegister(i.A));
            return OK;
        }
        case 15: { // lbl lab, lc
            return OK;
        }
        case 16: { // jup lab, rc
            int lab = 64 * i.A + i.B;
            size_t prev = pc;
            while (true) {
                if (pc < 0) pc = N + pc;
                Instruction ii(ROM+pc);
                pc -= 3;
                int llab = 64 * ii.A + ii.B;
                if (ii.opcode() == 15 && lab == llab && GetRegister(i.C) == ii.C) {
                    if (ii.condition() == 2 || ii.condition() == flag) {
                        // We found our label
                        if (stack.size() == 3)
                            stack.pop_front();
                        stack.emplace_back(prev, pc);

                        //pc += 3;
                        if (pc > (long long)N-3) pc = pc - N;
                        break;
                    }
                }
            }
        
            return OK;
        }
        case 17: { // jdn lab, rc
            int lab = 64 * i.A + i.B;
            size_t prev = pc;
            while (true) {
                // if (pc > (long long)N - 3)
                //     return ERROR;
                if (pc > (long long)N-3) pc = pc - N;
                Instruction ii(ROM+pc);
                pc += 3;
                int llab = 64 * ii.A + ii.B;
                if (ii.opcode() == 15 && lab == llab && GetRegister(i.C) == ii.C) {
                    if (ii.condition() == 2 || ii.condition() == flag) {
                        // We found our label
                        if (stack.size() == 3)
                            stack.pop_front();
                        stack.emplace_back(prev, pc);
                        
                        pc -= 3;
                        if (pc < 0) pc = N + pc;
                        break;
                    }
                }
            }
            return OK;
        }
        case 18: { // io rd, ix, rs
            switch (i.B) { // Devices
                case 0: { // SERIAL_INCOMING
                    SetRegister(i.A, SerialIncoming());
                    break;
                }
                case 1: { // SERIAL_READ
                    SetRegister(i.A, ReadSerial());
                    break;
                }
                case 2: { // SERIAL_WRITE
                    WriteSerial(GetRegister(i.C));
                    SetRegister(i.A, 0);
                    break;
                }
                case 3: { // CLOCK_LO_CS
                    WriteClock(GetRegister(i.C));
                    SetRegister(i.A, (uint8_t)(ReadClock() & 0b0000000000111111));
                    break;
                }
                case 4: { // CLOCK_HI_CS
                    WriteClock(GetRegister(i.C));
                    SetRegister(i.A, (uint8_t)((ReadClock() & 0b0000111111000000) >> 6));
                    break;
                }
                case 16: { // MEM ADDR HI
                    MemADDRHigh(GetRegister(i.C));
                    break;
                }
                case 17: { // MEM ADDR MID
                    MemADDRMid(GetRegister(i.C));
                    break;
                }
                case 18: { // MEM ADDR LO
                    MemADDRLow(GetRegister(i.C));
                    break;
                }
                case 19: { // MEM READ
                    SetRegister(i.A, MemRead());
                    break;
                }
                case 20: { // MEM WRITE
                    MemWrite(GetRegister(i.C));
                    break;
                }
                case 21: { // GPU X
                    SetGPUX(GetRegister(i.C));
                    break;
                }
                case 22: { // GPU Y
                    SetGPUY(GetRegister(i.C));
                    break;
                }
                case 23: { // GPU DRAW
                    GPUDraw(GetRegister(i.C));
                    break;
                }
                case 34: { // DPAD READ
                    SetRegister(i.A, ReadDPAD());
                    break;
                }
                default:
                    std::cout << RED << "[!] Invalid device (" << (int)i.B << ")" << CLR << std::endl;
                    return ERROR;
            }
            return OK;
        }
        case 19: { // fm/pr rd, ra
            if (i.C < 16) {
                uint16_t o1 = GetRegister(i.A), o2 = GetRegister(i.B);
                uint16_t internal = o1 * o2;
                internal >>= i.C;
                internal &= 0b0000000000111111;
                SetRegister(i.A, (uint8_t)internal);
            }
            else {
                int16_t o1 = to_signed(GetRegister(i.A)), o2 = to_signed(GetRegister(i.B));
                int16_t internal = o1 * o2;
                internal >>= i.C - 16;
                internal &= 0b0000000000111111;
                SetRegister(i.A, (uint8_t)internal);
            }
            return OK;
        }
        case 20:
            return HALT;
        default:
            std::cout << RED << "[!] Invalid instruction!" << CLR << std::endl;
            return ERROR;
    }
    return OK;
}

void Emulator::SetRegister(uint8_t reg, uint8_t value)
{
    if (reg > 0 && reg < 64)
        memory[reg-1] = value % 0x40;
    else if (reg != 0)
        std::cerr << RED << "[!] Invalid write to address " << (unsigned)reg << " (PC: " << pc << ")" << CLR << std::endl; 
}

uint8_t Emulator::GetRegister(uint8_t reg) const
{
    if (reg > 0 && reg < 64)
        return memory[reg-1];
    else if (reg == 0)
        return 0;
    else
        std::cerr << RED << "[!] Invalid read from address " << (unsigned)reg << " (PC: " << pc << ")" << CLR << std::endl; 
    return 0;
}

void Emulator::Init()
{
    std::memset(memory, 0, 63);
    std::memset(serial, 0, 64);
    std::memset(RAM, 0, 256000);
    pc = 0;
    flag = 0;
    serialinc = 0;
    addr = 0;
    GPUX = 0;
    GPUY = 0;
    CLK = std::chrono::high_resolution_clock::now();
    delete gui;
    gui = new GUI(512, 512, "Emulator");
}

Error Emulator::Run(bool restart)
{
    // Zero memory, program counter, and flags
    if (restart)
        Init();

    // Main loop
    while (true) {        
        Error step = Step();
        if (step != OK)
            return step;        
    }
    
    return OK;
}

Error Emulator::Step()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            delete gui;
            gui = nullptr;
            return HALT;
        }     
    }

    if (pc > (long long)N - 3) {
        std::cout << RED << "[!] !" << CLR << std::endl;
        return ERROR;
    }
    Instruction i(ROM+pc);

    Error result = ExecuteInstruction(i);
    if (result == ERROR) return ERROR;
    else if (result == HALT) return HALT;

    pc += 3;
    if (breakpoints.find(pc) != breakpoints.end()) { return BREAK; }
    return OK;
}

std::string Emulator::Dissassemble() 
{
    long long prevpc = pc;
    std::stringstream ss;

    while (pc <= (long long)N - 3) {
        ss << DissassembleCurrInst() << std::endl;
        pc += 3;
    }
    pc = prevpc;

    return ss.str();
}

std::string Emulator::DissassembleCurrInst() const
{
    Instruction i(ROM+pc);
    std::stringstream ss;

    ss << "(" << pc << ")\t\t";

    if (i.OPC == 0) {
        ss << BLUE << "   HALT" << CLR;
        return ss.str();
    }

    if (i.condition() == 2) ss << "   ";
    else if (i.condition() == 1) ss << " + ";
    else if (i.condition() == 0) ss << " - ";

    switch (i.opcode()) {
        case 0: { // add rd, ra, rb
            ss << "ADD R" << (int)i.A << ", R" << (int)i.B << ", R" << (int)i.C;
            break;
        }
        case 1: { // add rd, ra, ib
            ss << "ADD R" << (int)i.A << ", R" << (int)i.B << ", #" << (int)i.C;
            break;
        }
        case 2: { // sub rd, ra, rb
            ss << "SUB R" << (int)i.A << ", R" << (int)i.B << ", R" << (int)i.C;
            break;
        }
        case 3: { // cmpCM
            ss << "CMP";

            std::string sa = "R", sb = "R";
            if (16 <= i.A && i.A < 32)
                sb = "#";
            else if (i.A >= 32)
                sa = "#";
                      
            switch (i.A % 8) {
                case 0:
                    ss << "TR ";
                    break;
                case 1:
                    ss << "FA ";
                    break;
                case 2:
                    ss << "EQ ";
                    break;
                case 3:
                    ss << "NE ";
                    break;
                case 4:
                    ss << "SL ";
                    break;
                case 5:
                    ss << "SG ";
                    break;
                case 6:
                    ss << "UL ";
                    break;
                case 7:
                    ss << "UG ";
                    break;
            }
            if (i.A >= 8 && i.A < 16)
                ss << sa << (int)i.C << ", " << sb << (int)i.B;
            else
                ss << sa << (int)i.B << ", " << sb << (int)i.C;
            break;
        }
        case 4: { // or rd, ra, rb
            ss << "OR R" << (int)i.A << ", R" << (int)i.B << ", R" << (int)i.C;
            break;
        }
        case 5: { // or rd, ra, ib
            ss << "OR R" << (int)i.A << ", R" << (int)i.B << ", #" << (int)i.C;
            break;
        }
        case 6: { // xor rd, ra, rb
            ss << "XOR R" << (int)i.A << ", R" << (int)i.B << ", R" << (int)i.C;
            break;
        }
        case 7: { // xor rd, ra, ib
            ss << "XOR R" << (int)i.A << ", R" << (int)i.B << ", #" << (int)i.C;
            break;
        }
        case 8: { // and rd, ra, rb
            ss << "AND R" << (int)i.A << ", R" << (int)i.B << ", R" << (int)i.C;
            break;
        }
        case 9: { // and rd, ra, ib
            ss << "AND R" << (int)i.A << ", R" << (int)i.B << ", #" << (int)i.C;
            break;
        }
        case 10: { // Shift/rotate by immediate
            if (i.C < 8) // shl rd, ra, ib
                ss << "SHL R" << (int)i.A << ", R" << (int)i.B << ", #" << ((int)i.C % 8);

            else if (i.C < 16) // shr rd, ra, ib
                ss << "SHR R" << (int)i.A << ", R" << (int)i.B << ", #" << ((int)i.C % 8);

            else if (i.C < 24) // sar rd, ra, ib
                ss << "SAR R" << (int)i.A << ", R" << (int)i.B << ", #" << ((int)i.C % 8);

            else if (i.C < 32) // rol rd, ra, ib
                ss << "ROL R" << (int)i.A << ", R" << (int)i.B << ", R" << ((int)i.C % 8);
            break;
        }
        case 11: { // shl rd, ra, rb
            ss << "SHL R" << (int)i.A << ", R" << (int)i.B << ", R" << (int)i.C;
            break;
        }
        case 12: { // shr rd, ra, rb
            ss << "SHR R" << (int)i.A << ", R" << (int)i.B << ", R" << (int)i.C;
            break;
        }
        case 13: { // ld rd, [ra+ib]
            ss << "LD R" << (int)i.A << ", [R" << (int)i.B << "+#" << (int)i.C << "]";
            break;
        }
        case 14: { // st [ra+ib], rs
            ss << "ST [R" << (int)i.B << "+#" << (int)i.C << "], R" << (int)i.A;
            break;
        }
        case 15: { // lbl lab, lc
            ss << MAGENTA << "LBL #" << (64 * i.A + i.B) << ", #" << (int)i.C << CLR;
            break;
        }
        case 16: { // jup lab, rc
            int lab = 64 * i.A + i.B;
            ss << MAGENTA << "JUP #" << lab << ", R" << (int)i.C << CLR;
            break;
        }
        case 17: { // jdn lab, rc
            int lab = 64 * i.A + i.B;
            ss << MAGENTA << "JDN #" << lab << ", R" << (int)i.C << CLR;
            break;
        }
        case 18: { // io rd, ix, rs
            ss << "IO R" << (int)i.A << ", &" << (int)i.B << ", R" << (int)i.C;
            break;
        }
        case 19: { // fmu/pr rd, ra
            if (i.C < 16)
                ss << "FMU/" << i.C << " R" << (int)i.A << ", R" << (int)i.B;
            else
                ss << "FMS/" << i.C-16 << " R" << (int)i.A << ", R" << (int)i.B;
            break;
        }
        case 20:
            ss << BLUE << "HALT" << CLR;
            break;
        default:
            ss << RED << "[!] Invalid instruction: " << i.opcode() << " (PC: " << pc << ")" << CLR;
            break;
    }

    return ss.str();
}

Instruction Emulator::GetCurrInst() const
{
    std::cout << (unsigned)(ROM[pc]) << std::endl;
    return Instruction(ROM+pc);
}

void Emulator::SetBreakpoint(size_t addr)
{
    breakpoints.emplace(addr);
}

void Emulator::DeleteBreakpoint(size_t addr)
{
    breakpoints.erase(addr);
}

void Emulator::DeleteBreakpoints()
{
    breakpoints.clear();
}

const std::set<size_t> &Emulator::GetBreakpoints() const
{
    return breakpoints;
}

MemoryDump Emulator::MemDump() const
{
    MemoryDump mmdmp;
    mmdmp.mem.resize(64);
    for (int i = 0; i < 64; i++)
        mmdmp.mem[i] = GetRegister(i);
    mmdmp.flag = flag;
    mmdmp.pc = pc;
    mmdmp.clk = ReadClock();
    mmdmp.serialinc = serialinc;
    mmdmp.addr = addr;
    mmdmp.GPUX = GPUX;
    mmdmp.GPUY = GPUY;
    return mmdmp;
}

const uint8_t *Emulator::RAMDump() const
{
    return RAM;
}

void Emulator::SetDebug(bool value) 
{
    debug = value;
}

bool Emulator::Debug()
{
    return debug;
}

Emulator::~Emulator()
{
    delete gui;
    delete[] ROM;
}