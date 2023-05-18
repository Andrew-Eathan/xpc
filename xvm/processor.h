#include <map>
#include <vector>
#include <stack>
#include <iostream>
#include <functional>

using namespace std;
class Processor;

typedef function<void(Processor*, uint8_t)> interrupt_function;
typedef map<uint8_t, interrupt_function> interrupt_map;

enum Signal : uint8_t {
	DEADEND = 0,
	FAULT,
	ERROR,
	BADOP,
	OVERFLOWED,
	UNHANDLED_INTERRUPT
};

enum MemoryTypes {
	Register = 0b00,
	Memory = 0b01,
	Constant = 0b10
};

enum MOVFLAGS {
	RegisterToRegister	= 0,
	RegisterToMemory = Memory << 2,
	MemoryToMemory		= Memory | (Memory << 2),
	MemoryToRegister	= Memory | (Register << 2)
};

const string SignalLookup[] = {
	"DEADEND",
	"FAULT",
	"ERROR",
	"BADOP",
	"OVERFLOWED",
	"UNHANDLED_INTERRUPT"
};

enum Instruction : uint8_t {
	NOP = 0,

	LOAD1B, LOAD2B, LOAD3B, LOAD4B,
	LOAD5B, LOAD6B, LOAD7B, LOAD8B,

	MOV,

	ADD, SUB,
	MUL, DIV,

	JE, JNE, JG, JL, JGE, JLE, CMP,

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

	"JE", "JNE", "JG", "JL", "JGE", "JLE", "CMP",

	"NOT", "AND", "OR", "XOR",

	// cpuid will be implemented later
	"JMP", "RET", "END", "CPUID", "INT"
};

class Processor {
private:
	vector<uint8_t> m_memory;
	uint64_t m_pc;

	vector<uint64_t> m_stack;
	int64_t m_last_comparison_1;
	int64_t m_last_comparison_2;
	interrupt_map m_interrupts;

	uint8_t get_byte();
	uint64_t get_bytes(uint8_t bytes);
	uint64_t read_bytes_mem(uint64_t byteIndex, uint8_t bytes);
	void write_bytes_mem(uint64_t address, uint64_t byteIndex, uint8_t bytes);
	void emit_signal(Signal);
	void load_register(uint8_t, uint8_t);

public:
	int64_t m_registers[16];

	bool m_status;
	bool m_verbose;
	void reset();
	Processor();
	void process();
	void dump_registry();
	void dump_stack();

	void push_byte(uint8_t);
	void push_bytes(uint64_t bytes, uint8_t bytecount);
	void add_int_handler(uint8_t interrupt, interrupt_function fn);
	uint64_t get_memory_pointer();
};


