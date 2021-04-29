#include "runtimedata.h"

#include <iostream>

namespace assembler
{

void RuntimeData::switch_section(std::string name)
{
    auto it = m_sections.find(name);
    if(it == m_sections.end())
    {
        auto it2 = m_sections.insert(std::make_pair(name, Section{name}));
        m_current_section = &it2.first->second;
        return;
    }
    m_current_section = &it->second;
}

bool RuntimeData::add_symbol(std::string name)
{
    auto it = m_symbols.find(name);
    if(it != m_symbols.end())
        return false;
    m_symbols.insert(std::make_pair(name, Symbol{name, m_current_section}));
    return true;
}

bool RuntimeData::set_symbol_type(std::string name, Symbol::Type type)
{
    auto it = m_symbols.find(name);
    if(it == m_symbols.end())
        return false;
    it->second.type = type;
    return true;
}

uint16_t RuntimeData::resolve_operand(std::shared_ptr<Operand> operand, size_t offset)
{
    // Absolute adress
    try
    {
        if(operand->type == Operand::Type::DecNumber)
            return std::stoi(operand->value, nullptr, 10);
        else if(operand->type == Operand::Type::HexNumber)
            return std::stoi(operand->value, nullptr, 16);
    }
    catch(...)
    {
        // ICE?
        GENERATOR_ERROR("internal compiler error: invalid number in resolve_operand");
    }

    // Label
    if(operand->type == Operand::Type::Name)
    {
        // Offsets are needed because jmps are relative to beginning of instruction.
        // See emulator/Executor.cpp:45.
        m_current_section->add_replace_entry(offset + 1, operand->value);
        return offset; // It should be replaced in replace_all_replace_entries!
    }
    else
    {
        GENERATOR_ERROR("invalid operand type");
    }

    return offset;
}

bool RuntimeData::Section::replace_all_replace_entries(std::string& payload)
{
    // Display for now
    std::cout << "Replace entries for ." << name << ":" << std::endl;
    for(auto& entry: replace_entries)
    {
        std::cout << "  " << entry.second << " at 0x" << std::hex << entry.first << std::dec << std::endl;
        size_t label_index = labels[entry.second];
        int8_t new_value = instruction_adresses[label_index] - entry.first; // Ignore instruction opcode
        std::cout << "    label index: 0x" << std::hex << label_index << ", new value: 0x" << (int)new_value << std::dec << std::endl;

        // Offsets are needed because jmps are relative to beginning of instruction.
        // See emulator/Executor.cpp:45.
        payload.at(entry.first) = new_value + 1;
    }

    return true;
}

void RuntimeData::display() const
{
    std::cout << "Sections:" << std::endl;
    for(auto& it: m_sections)
    {
        std::cout << "  ." << it.first << std::endl;
        size_t offset = 0;
        std::cout << "    Dump:" << std::endl;
        for(auto& op: it.second.instructions)
        {
            std::cout << "      " << op->display() << std::endl;
            offset++;
        }
        std::cout << "    Labels:" << std::endl;
        for(auto& label: it.second.labels)
        {
            std::cout << "      " << label.first << ": 0x" << std::hex << label.second << std::dec << std::endl;
            offset++;
        }
    }

    std::cout << "Symbols:" << std::endl;
    for(auto& it: m_symbols)
    {
        auto section = it.second.section;
        std::cout << "  " << it.first   << " type " << (int)it.second.type
                  << " section ." << (section ? section->name : "<None>") << std::endl;
    }
}

}
