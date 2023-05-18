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

uint64_t Processor::get_bytes(uint8_t bytes) {
	uint64_t ret = 0;

	for (int i = 0; i < bytes; i++) {
		ret |= (uint64_t)get_byte() << (i * 8);
	}

	return ret;
}

uint64_t Processor::read_bytes_mem(uint64_t byteIndex, uint8_t bytes) {
	uint64_t ret = 0;

	for (int i = 0; i < bytes; i++) {
		ret |= (uint64_t) m_memory[byteIndex + i] << (i * 8);
	}

	return ret;
}

void Processor::write_bytes_mem(uint64_t address, uint64_t data, uint8_t bytes) {
	for (int i = 0; i < bytes; i++) {
		m_memory[address + i] = data >> (i * 8) & 0xFF;
	}
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
		m_registers[reg] |= (uint64_t) val << (i * 8);
	}
}

void Processor::process() {
	if (m_pc >= m_memory.size()) {
		emit_signal(Signal::DEADEND);
		m_status = false;
		return;
	}

	Instruction inst = (Instruction) get_byte();

	if (m_verbose)
		cout << "inst: " << InstructionLookup[inst] << endl;

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

		case Instruction::MOV:
		{
			// bits 12345678:
			// 1234 - how many bytes to read from the source, 8 max which is a uint64_t
			// 56 - the source data type (register, memory, constant)
			// 78 - the destination data type (register, memory)
			// the last 4 bits aren't used for constant -> register and register -> register moves

			uint8_t flags = get_byte();
			uint8_t source = flags >> 4 & 0b11;
			uint8_t destination = flags >> 6 & 0b11;
			uint8_t bytecount = flags & 0b1111;

			uint64_t sourceData = 0;

			switch (source) {
				case MemoryTypes::Register:
					sourceData = m_registers[get_byte()];
				break;
				case MemoryTypes::Memory:
				{
					uint64_t address = get_bytes(bytecount);

					sourceData = read_bytes_mem(address, bytecount);
				} break;
				case MemoryTypes::Constant:
				{
					sourceData = get_bytes(bytecount);
				} break;
			}

			switch (destination) {
				case MemoryTypes::Register:
				{
					m_registers[get_byte()] = sourceData;
				} break;
				case MemoryTypes::Memory:
				{
					uint64_t address = get_bytes(bytecount);
					write_bytes_mem(address, sourceData, bytecount);
				} break;
			}
		} break;

		// math
		case Instruction::ADD:
		{
			auto reg1 = get_byte();
			auto reg2 = get_byte();
			auto store = get_byte();

			m_registers[store] = m_registers[reg1] + m_registers[reg2];
		} break;
		case Instruction::SUB:
		{
			auto reg1 = get_byte();
			auto reg2 = get_byte();
			auto store = get_byte();

			m_registers[store] = m_registers[reg1] - m_registers[reg2];
		} break;
		case Instruction::MUL:
		{
			auto reg1 = get_byte();
			auto reg2 = get_byte();
			auto store = get_byte();

			m_registers[store] = m_registers[reg1] * m_registers[reg2];
		} break;
		case Instruction::DIV:
		{
			auto reg1 = get_byte();
			auto reg2 = get_byte();
			auto store = get_byte();

			m_registers[store] = m_registers[reg1] / m_registers[reg2];
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
				address |= (uint64_t) get_byte() << (i * 8);
			}
			
			if (m_verbose)
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
			auto idx = get_byte();
			if (m_interrupts[idx]) {
				m_interrupts[idx](this, idx);
			}
			else {
				emit_signal(Signal::UNHANDLED_INTERRUPT);
			}
		} break;

		case Instruction::CMP:
		{
			auto reg1 = get_byte();
			auto reg2 = get_byte();

			m_last_comparison_1 = reg1;
			m_last_comparison_2 = reg2;
		} break;
	}
}

void Processor::push_bytes(uint64_t bytes, uint8_t bytecount) {
	for (int i = 0; i < bytecount; i++) {
		uint8_t byte = bytes >> (i * 8) & 0xFF;
		push_byte(byte);
	}
}

void Processor::emit_signal(Signal signal) {
	if (m_verbose)
		cout << m_pc << " signaled " << SignalLookup[signal] << std::endl;
}

void Processor::add_int_handler(uint8_t interrupt, interrupt_function fn) {
	m_interrupts[interrupt] = fn;
}

uint64_t Processor::get_memory_pointer()
{
	return m_memory.size() - 1;
}

void Processor::dump_registry() {
	cout << "-- registry dump --" << endl;

	for (int i = 0; i < 16; i++) {
		if (m_registers[i] == 0) continue;
		cout << "    " << i << ": " << m_registers[i] << endl;
	}

	cout << "-------" << endl;
}

void Processor::dump_stack() {
	cout << "-- stack dump --" << endl;

	for (int i = 0; i < m_stack.size(); i++) {
		auto val = m_stack[i];
		if (val == 0) continue;

		cout << i << ": " << val;

		if (i == 0) {
			cout << " <-- top";
		}

		cout << endl;
	}

	cout << "-------" << endl;
}
