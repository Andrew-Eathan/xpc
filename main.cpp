#include <iostream>
#include "processor.h"

int main() {
    Processor p = Processor();
    p.push_byte(Instruction::LOAD1B);
    p.push_byte(0);
    p.push_byte(4); 
    p.push_byte(Instruction::LOAD2B);
    p.push_byte(1);
    p.push_byte(500 & 0xFF);
    p.push_byte((500 >> 8) & 0xFF); 
    p.push_byte(Instruction::JMP);
    p.push_byte(17);
    for (int i = 0; i < 7; i++) p.push_byte(0);

    p.push_byte(Instruction::END);

    p.push_byte(Instruction::RET);
    
    cout << ((500 >> 8) & 0xFF) << " " << (500 & 0xFF) << endl;

    while (p.m_status) {
        p.process();
        p.dump_stack(); 
    }

    return 0;
}
