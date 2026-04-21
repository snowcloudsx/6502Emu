#pragma once
#include <cstdint>

using Byte = uint8_t;
using Word = uint16_t;
using u32  = uint32_t;

struct Mem
{
    static const u32 SIZE = 65536;
    Byte data[SIZE];

    void Init();
    Byte Read(Word addr) const;
    void Write(Word addr, Byte val);
};

struct CPU;

// function pointer types
using AddrMode = Word (CPU::*)(u32&);
using Op       = void (CPU::*)(Word, u32&);

struct Instruction
{
    const char* name;
    Op op;
    AddrMode mode;
    Byte cycles;
};

struct CPU
{
    // -------------------------
    // registers
    // -------------------------
    Word PC;
    Byte SP;

    Byte A;
    Byte X;
    Byte Y;
    Byte P;

    Mem* mem;

    // -------------------------
    // flags
    // -------------------------
    enum FLAGS
    {
        C = 1 << 0,
        Z = 1 << 1,
        I = 1 << 2,
        D = 1 << 3,
        B = 1 << 4,
        U = 1 << 5,
        V = 1 << 6,
        N = 1 << 7
    };

    // ----------------------
    // core
    // -------------------
    void Reset();
    void Step(u32& cycles);

    // ----------------------
    // flag helpers
    // ------------------------
    void SetFlag(Byte f, bool v);
    bool GetFlag(Byte f);
    void SetZN(Byte v);

    // -----------------------
    // stack
    // --------------------
    void PushByte(Byte v, u32& c);
    Byte PopByte(u32& c);
    void PushWord(Word v, u32& c);
    Word PopWord(u32& c);

    // -----------------------
    // addressing modes
    // -------------------------
    Word IMM(u32&);
    Word ZP(u32&);
    Word ZPX(u32&);
    Word ZPY(u32&);
    Word ABS(u32&);
    Word ABSX(u32&);
    Word ABSY(u32&);
    Word IND(u32&);
    Word INDX(u32&);
    Word INDY(u32&);

    // ----------------------
    // instructions
    // ---------------------
    void LDA(Word, u32&);
    void LDX(Word, u32&);
    void LDY(Word, u32&);

    void STA(Word, u32&);
    void STX(Word, u32&);
    void STY(Word, u32&);

    void TAX(Word, u32&);
    void TAY(Word, u32&);
    void TXA(Word, u32&);
    void TYA(Word, u32&);

    void INX(Word, u32&);
    void INY(Word, u32&);
    void DEX(Word, u32&);
    void DEY(Word, u32&);

    void ADC(Word, u32&);
    void SBC(Word, u32&);

    void AND(Word, u32&);
    void ORA(Word, u32&);
    void EOR(Word, u32&);

    void CMP(Word, u32&);
    void CPX(Word, u32&);
    void CPY(Word, u32&);

    void JMP(Word, u32&);
    void JSR(Word, u32&);
    void RTS(Word, u32&);

    void BEQ(Word, u32&);
    void BNE(Word, u32&);
    void BCS(Word, u32&);
    void BCC(Word, u32&);

    void BRK(Word, u32&);
    void NOP(Word, u32&);

    void IRQ(u32&);
    void NMI(u32&);

    static Instruction table[256];
};