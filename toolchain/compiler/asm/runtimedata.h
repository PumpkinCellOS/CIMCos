#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "opcodes.h"

namespace assembler
{

class RuntimeData
{
public:
    struct Section
    {
        std::string name;
        std::vector<std::shared_ptr<Opcode>> instructions;
        std::vector<size_t> instruction_adresses;
        std::map<std::string, size_t> labels;
        std::vector<std::pair<size_t, std::string>> replace_entries;

        void add_replace_entry(size_t offset, std::string symbol) { replace_entries.push_back(std::make_pair(offset, symbol)); }
        bool replace_all_replace_entries(std::string& payload);
    };

    struct Symbol
    {
        std::string name;
        Section* section = nullptr;

        enum class Type
        {
            Variable,
            Function,
            Invalid
        } type = Type::Invalid;
    };

    void switch_section(std::string name);
    bool add_symbol(std::string name);
    bool set_symbol_type(std::string name, Symbol::Type type);
    const Section* current_section() const { return m_current_section; }
    Section* current_section() { return m_current_section; }
    std::map<std::string, Section>& sections() { return m_sections; }
    void add_relocation_entry(size_t value) { m_relocation_entries.push_back(value); }
    uint16_t resolve_operand(std::shared_ptr<Operand> operand, size_t offset);
    bool add_assignment(std::string name, std::shared_ptr<Operand> value);
    std::shared_ptr<Operand> resolve_assignment(std::string name) const;

    void display() const;

private:
    std::map<std::string, Section> m_sections;
    std::map<std::string, Symbol> m_symbols;
    std::map<std::string, std::shared_ptr<Operand>> m_assignments;
    std::vector<size_t> m_relocation_entries;
    Section* m_current_section = nullptr;
};

}
