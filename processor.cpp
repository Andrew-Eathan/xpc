#include "processor.h"
    
using namespace std;

Processor::Processor() {
    m_memory = vector<uint8_t>();
    m_stack = vector<uint64_t>();
    m_interrupts = interrupt_map();

    reset();
}

void Processor::push_byte(uint8_t b) {
    m_memory.push_back(b);
}

uint8_t Processor::get_byte() {
    return m_memory[m_pc++];
}

void Processor::reset() {
    m_status = true;
    m_pc = 0;

    for (int i = 0; i < 16; i++)
        m_registers[i] = 0;
}

void Processor::load_register(uint8_t reg, uint8_t count) {
    m_registers[reg] = 0;

    for (int i = 0; i < count; i++) {
        auto val = get_byte();
        m_registers[reg] |= val << (i * 8);
    }
}

void Processor::process() {
    if (m_pc >= m_memory.size()) {
        emit_signal(Signal::DEADEND);
        m_status = false;
        return;
    }

    Instruction inst = (Instruction) get_byte();

    cout << "inst: " << inst << endl;

    switch (inst) {
        case Instruction::NOP: break;

        // load to registers
        case Instruction::LOAD1B: 
        {
            auto reg = get_byte();
            auto data = get_byte();

            m_registers[reg] = data;
        } break;
        case Instruction::LOAD2B:
        {
            load_register(get_byte(), 2);
        } break;
        case Instruction::LOAD3B:
        {
            load_register(get_byte(), 3);
        } break;
        case Instruction::LOAD4B:
        {
            load_register(get_byte(), 4);
        } break;
        case Instruction::LOAD5B:
        {
            load_register(get_byte(), 5);
        } break;
        case Instruction::LOAD6B:
        {
            load_register(get_byte(), 6);
        } break;
        case Instruction::LOAD7B:
        {
            load_register(get_byte(), 7);
        } break;
        case Instruction::LOAD8B:
        {
            load_register(get_byte(), 8);
        } break;

        // math
        case Instruction::ADD:
        {
            auto reg1 = get_byte();
            auto reg2 = get_byte();
            auto store = get_byte();

            m_registers[store] = reg1 + reg2;
        } break;
        case Instruction::SUB:
        {
            auto reg1 = get_byte();
            auto reg2 = get_byte();
            auto store = get_byte();

            m_registers[store] = reg1 - reg2;
        } break;
        case Instruction::MUL:
        {
            auto reg1 = get_byte();
            auto reg2 = get_byte();
            auto store = get_byte();

            m_registers[store] = reg1 * reg2;
        } break;
        case Instruction::DIV:
        {
            auto reg1 = get_byte();
            auto reg2 = get_byte();
            auto store = get_byte();

            m_registers[store] = reg1 / reg2;
        } break;

        // binary operations
        case Instruction::NOT:
        {
            auto reg = get_byte();

            m_registers[reg] = ~m_registers[reg];
        } break;
        case Instruction::AND:
        {
            auto reg1 = get_byte();
            auto reg2 = get_byte();

            m_registers[reg1] &= m_registers[reg2];
        } break;
        case Instruction::OR:
        {
            auto reg1 = get_byte();
            auto reg2 = get_byte();

            m_registers[reg1] |= m_registers[reg2];
        } break;
        case Instruction::XOR:
        {
            auto reg1 = get_byte();
            auto reg2 = get_byte();

            m_registers[reg1] ^= m_registers[reg2];
        } break;

        // stack related
        case Instruction::JMP:
        {
            uint64_t address = 0;

            for (int i = 0; i < 8; i++) {
                address |= get_byte() << (i * 8);
            }
            
            cout << "jump " << m_pc << " " << address << endl;
            m_stack.push_back(m_pc);
            m_pc = address;
        } break;

        case Instruction::RET:
        {
            m_pc = m_stack[m_stack.size() - 1];
            m_stack.pop_back();
        } break;

        // program related
        case Instruction::END:
        {
            m_status = false;
            
        } break;

        case Instruction::INT:
        {
            m_inthandlers
        }
    }
}

void Processor::emit_signal(Signal signal) {
    cout << m_pc << " signaled " << signal << std::endl;
}

void Processor::dump_registry() {
    cout << "-- registry dump --" << endl;

    for (int i = 0; i < 16; i++) {
        cout << "    " << i << ": " << m_registers[i] << endl;
    }

    cout << "-------" << endl;
}

void Processor::dump_stack() {
    cout << "-- stack dump --" << endl;

    for (int i = 0; i < m_stack.size(); i++) {
        auto val = m_stack[i];
        cout << i << ": " << val;

        if (i == 0) {
            cout << " <-- top";
        }

        cout << endl;
    }

    cout << "-------" << endl;
}
