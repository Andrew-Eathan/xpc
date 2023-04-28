#include <map>
#include <vector>
#include <stack>
#include <iostream>
#include <functional>

using namespace std;
class Processor;

typedef function<void(Processor*, uint8_t)> interrupt_function;
typedef map<uint8_t, interrupt_function> interrupt_map;

enum Signal {
	DEADEND = 0,
	FAULT,
	ERROR,
	BADOP,
	OVERFLOWED,
	UNHANDLED_INTERRUPT
};

enum MOVFLAGS {
	RegisterToRegister	= 0b00,
	RegisterToMemory	= 0b10,
	MemoryToMemory		= 0b11,
	MemoryToRegister	= 0b01
};

const string SignalLookup[] = {
	"DEADEND",
	"FAULT",
	"ERROR",
	"BADOP",
	"OVERFLOWED",
	"UNHANDLED_INTERRUPT"
};

enum Instruction {
	NOP = 0,

	LOAD1B, LOAD2B, LOAD3B, LOAD4B,
	LOAD5B, LOAD6B, LOAD7B, LOAD8B,

	MOV,

	ADD, SUB,
	MUL, DIV,

	NOT, AND, OR, XOR,

	// cpuid will be implemented later
	JMP, RET, END, CPUID, INT
};

const string InstructionLookup[] = {
	"NOP",

	"LOAD1B", "LOAD2B", "LOAD3B", "LOAD4B",
	"LOAD5B", "LOAD6B", "LOAD7B", "LOAD8B",

	"MOV",

	"ADD", "SUB",
	"MUL", "DIV",

	"NOT", "AND", "OR", "XOR",

	// cpuid will be implemented later
	"JMP", "RET", "END", "CPUID", "INT"
};

class Processor {
private:
	vector<uint8_t> m_memory;
	uint64_t m_pc;

	vector<uint64_t> m_stack;

	interrupt_map m_interrupts;

	uint8_t get_byte();
	void emit_signal(Signal);
	void load_register(uint8_t, uint8_t);

public:
	int64_t m_registers[16];

	bool m_status;
	void reset();
	Processor();
	void process();
	void dump_registry();
	void dump_stack();

	void push_byte(uint8_t);
	void push_bytes(uint64_t bytes, uint8_t bytecount);
	void add_int_handler(uint8_t interrupt, interrupt_function fn);
};


