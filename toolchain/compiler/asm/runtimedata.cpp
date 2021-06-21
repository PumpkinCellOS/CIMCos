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
    // TODO: Allow specifying what to do with name identifiers (labels OR assignments?)
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

bool RuntimeData::add_assignment(std::string name, std::shared_ptr<Operand> value)
{
    auto it = m_assignments.find(name);
    if(it != m_assignments.end())
        return false;
    m_assignments.insert(std::make_pair(name, value));
    return true;
}

std::shared_ptr<Operand> RuntimeData::resolve_assignment(std::string name) const
{
    auto it = m_assignments.find(name);
    if(it == m_assignments.end())
        return nullptr;
    return it->second;
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

    std::cout << "Assignments:" << std::endl;
    for(auto& it: m_assignments)
    {
        std::cout << "  " << it.first << " := " << it.second->display() << std::endl;
    }
}

bool RuntimeData::is_reserved_name(std::string name) const
{
    const char* reserved_names[] = {
        "al", "bl", "cl", "dl", // 8-bit registers
        "ax", "bx", "cx", "dx", // 16-bit registers
        "IF", "GF", "OF", "ZF", "NF", "PF", "EF", // Flags
    };

    for(auto& reserved_name: reserved_names)
    {
        if(name == reserved_name)
            return true;
    }
    return false;
};

std::shared_ptr<Operand> RuntimeData::resolve_operand_with_assignments(std::shared_ptr<Operand> operand) const
{
    if(operand->type == Operand::Type::Name && !is_reserved_name(operand->value))
    {
        auto resolved_operand = resolve_assignment(operand->value);
        if(!resolved_operand)
            GENERATOR_ERROR("invalid name operand: " + operand->value);
        return resolved_operand;
    }
    else
    {
        return operand;
    }
};

}
