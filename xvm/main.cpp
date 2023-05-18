#include <iostream>
#include "processor.h"

int main() {
    Processor p = Processor();

    p.push_byte(Instruction::LOAD1B);
    p.push_byte(14);
    p.push_byte(1);

    p.push_byte(Instruction::LOAD1B);
    p.push_byte(15);
    p.push_byte(0);

    p.push_byte(Instruction::INT); uint64_t ins = p.get_memory_pointer();
    p.push_byte(0x13);
    
    p.push_byte(Instruction::ADD);
    p.push_byte(15);
    p.push_byte(14);
    p.push_byte(15);

    p.push_byte(Instruction::JMP);
    p.push_bytes(ins, 8);

    interrupt_function printfn = [](Processor* proc, uint8_t interrupt_idx) {
        cout << (char)proc->m_registers[15];
    };

    p.add_int_handler(0x13, printfn);

    p.m_verbose = false;

    /*
    p.push_byte(Instruction::LOAD1B);
    p.push_byte(0);
    p.push_byte(4); 

    p.push_byte(Instruction::LOAD2B);
    p.push_byte(1);
    p.push_bytes(500, 2);

    p.push_byte(Instruction::JMP);
    p.push_byte(17);

    for (int i = 0; i < 7; i++) p.push_byte(0);

    p.push_byte(Instruction::END);

    p.push_byte(Instruction::INT);
    p.push_byte(0x13);
    p.push_byte(Instruction::RET);
    
    interrupt_function printfn = [](Processor* proc, uint8_t interrupt_idx) {
        cout << proc->m_registers[15];
    };

    p.add_int_handler(0x13, printfn);*/

    while (p.m_status) {
        p.process();
        //p.dump_stack(); 
        //p.dump_registry();
    }

    return 0;
}
