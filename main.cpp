#include <Windows.h>
#include <cstdint>
#include <iostream>

using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;

struct Mem
{
	static constexpr u32 MAX_MEM = 1024 * 64;
	Byte Data[MAX_MEM]; // Fixed-size array instead of flexible array


	
	void init()
	{
		for (u32 i = 0; i < MAX_MEM; i++)
		{
			Data[i] = 0;
		}
	}
	//read one byte
	Byte operator[](u32 Address) const
	{
		//assert here address is < maxmem
		return Data[Address];
	}

	//write one byte
	Byte& operator[](u32 Address)
	{
		//assert here address is < maxmem
		return Data[Address];
	}

	void WriteWord(Word Value, u32 Address, u32& Cycles)
	{
		Data[Address] = Value & 0xFF;
		Data[Address + 1] = (Value >> 8);
		Cycles -= 2;
	}
};

struct CPU
{



	Word PC;//program counter
	Word SP;//stack pointer


	Byte A, X, Y;//registers

	Byte C : 1;//status flags
	Byte Z : 1;//status flags
	Byte I : 1;//status flags
	Byte D : 1;//status flags
	Byte B : 1;//status flags
	Byte V : 1;//status flags
	Byte N : 1;//status flags

	void Reset(Mem& memory)
	{
		PC = 0xFFFC;
		SP = 0x0100;
		C = Z = I = D = B = V = N = 0;
		A = X = Y = 0;
		memory.init();

	}
	Byte FetchByte(u32& Cycles, Mem& memory)
	{
		Byte Data = memory[PC];
		PC++;
		Cycles--;
		return Data;
	}


	Word FetchWord(u32& Cycles, Mem& memory)
	{
		//6503 is little endian
		Word Data = memory[PC];
		PC++;


		Data |= (memory[PC] << 8);
		PC++;
		Cycles-=2;
		//TODO SETUP Byte Swapping for endianness


		return Data;
	}

	Byte ReadByte(
		u32& Cycles,
		Byte Address,
		Mem& memory)
	{
		Byte Data = memory[Address];
		Cycles--;
		return Data;
	}
	// opcodes
	static constexpr Byte
		INS_LDA_IM = 0xA9,
		INS_LDA_ZP = 0xA5,
		INS_LDA_ZPX = 0xB5,
		INS_JSR = 0x20;
		

	void LDASetStatus()
	{
		Z = (A == 0);
		N = (A & 0b10000000) > 0;
	}
	void Execute(u32 Cycles, Mem& memory)
	{
		while (Cycles > 0)
		{
			
			Byte Ins = FetchByte(Cycles, memory);
			switch (Ins)
			{
			case INS_LDA_IM:
			{
				Byte Value = FetchByte(Cycles, memory);
				A = Value;
				LDASetStatus();


			}break;
			case INS_LDA_ZP:
			{
				Byte ZeroPageAddress = 
					FetchByte(Cycles, memory);
				A = ReadByte(Cycles, ZeroPageAddress, memory);
				LDASetStatus();
			}break;
			case INS_LDA_ZPX:
			{
				Byte ZeroPageAddress =
					FetchByte(Cycles, memory);
				ZeroPageAddress += X;
				Cycles--;
				A = ReadByte(
					Cycles, ZeroPageAddress, memory);
				LDASetStatus();

			}break;
			case INS_JSR:
			{
				Word SubAddr =  FetchWord( Cycles,memory );
				memory.WriteWord(PC - 1, SP, Cycles);
				SP += 2;				//(forgot to do this during the video, probably need a function to push onto the stack!)
				PC = SubAddr;
				Cycles--;
			}break;
			default:
			{
				printf("Instructions not handled %d", Ins);
			}break;
			}
		}
	}
	
};

int main()
{
	Mem mem;
	CPU cpu;
	cpu.Reset( mem );	

	//start of inline prog
	mem[0xFFFC] = CPU::INS_JSR;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x42;
	mem[0x4242] = CPU::INS_LDA_IM;
	mem[0x4243] = 0x84;

	//end of inline prog
	cpu.Execute( 9 , mem );

	std::cout << mem[0x4243] << std::endl;
	return 0;
} 