#include <map>
#include <vector>
#include <stack>
#include <iostream>

using namespace std;
class Processor;

typedef map<uint8_t, function<void (Processor, uint8_t)>> interrupt_map;

enum Signal {
    DEADEND = 0,
    FAULT,
    ERROR,
    BADOP,
    OVERFLOWED
};

enum Instruction {
    NOP = 0,

    LOAD1B, LOAD2B, LOAD3B, LOAD4B,
    LOAD5B, LOAD6B, LOAD7B, LOAD8B,

    ADD, SUB,
    MUL, DIV,

    NOT, AND, OR, XOR,

    // cpuid will be implemented later
    JMP, RET, END, CPUID, INT
};

class Processor {
private:
    vector<uint8_t> m_memory;
    uint64_t m_pc;

    int64_t m_registers[16];
    vector<uint64_t> m_stack;

    interrupt_map m_interrupts;

    uint8_t get_byte();
    void emit_signal(Signal);
    void load_register(uint8_t, uint8_t);

public:
    bool m_status;
    void push_byte(uint8_t);
    void reset();
    Processor();
    void process();
    void dump_registry();
    void dump_stack();
};


