#include "instruction.h"

namespace assembler
{

bool InstructionOperation::execute(RuntimeData& data) const
{
    std::string mnemonic = m_instruction.mnemonic;

    // Change mnemonic to uppercase
    std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(),
    [](char c){ return std::toupper(c); });

    std::shared_ptr<Opcode> opcode;

    auto is_reserved_name = [&](std::string name) {
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

    auto resolve_operand = [&](std::shared_ptr<Operand> operand)->std::shared_ptr<Operand> {
        if(operand->type == Operand::Type::Name && !is_reserved_name(operand->value))
        {
            auto resolved_operand = data.resolve_assignment(operand->value);
            if(!resolved_operand)
                BUILDER_ERROR(&m_instruction, "invalid name operand: " + operand->value);
            return resolved_operand;
        }
        else
        {
            return operand;
        }
    };

    // 0 arguments
    if(m_instruction.members.size() == 0)
    {
        if(mnemonic == "II1")
            opcode = std::make_shared<opcodes::Invalid>();
        else if(mnemonic == "_RMSTFR")
            opcode = std::make_shared<opcodes::NoArgument>(0x89);
        else if(mnemonic == "PUSHA")
            opcode = std::make_shared<opcodes::NoArgument>(0x8c);
        else if(mnemonic == "POPA")
            opcode = std::make_shared<opcodes::NoArgument>(0x8e);
        else if(mnemonic == "RET")
            opcode = std::make_shared<opcodes::NoArgument>(0xba);
        else if(mnemonic == "IRET")
            opcode = std::make_shared<opcodes::NoArgument>(0xbb);
        else if(mnemonic == "HLT")
            opcode = std::make_shared<opcodes::NoArgument>(0xbc);
        else if(mnemonic == "_MKSTFR")
            opcode = std::make_shared<opcodes::NoArgument>(0xbd);
        else if(mnemonic == "CPUID")
            opcode = std::make_shared<opcodes::NoArgument>(0xbe);
    }
    else if(m_instruction.members.size() == 1)
    {
        auto arg1 = m_instruction.members[0];
        if(mnemonic[0] == 'J')
        {
            std::string condition = mnemonic.substr(1);
            if(condition == "MP")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::None);
            else if(condition == "Z" || condition == "E")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::Equal);
            else if(condition == "G")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::Greater);
            else if(condition == "L")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::Less);
            else if(condition == "NE" || condition == "NZ")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::NotEqual);
            else if(condition == "GE")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::GreaterOrEqual);
            else if(condition == "LE")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::LessOrEqual);
            else
                BUILDER_ERROR(&m_instruction, "invalid jump condition");
        }
        else if(mnemonic == "_FENBL")
            opcode = std::make_shared<opcodes::FlagEnableOrDisable>(arg1, true);
        else if(mnemonic == "_FDSBL")
            opcode = std::make_shared<opcodes::FlagEnableOrDisable>(arg1, false);
        else if(mnemonic == "DATA")
            opcode = std::make_shared<opcodes::Data>(arg1);
        else if(mnemonic == "SKIP")
            opcode = std::make_shared<opcodes::Skip>(arg1);
        // TODO: PUSH, POP, NEG, NOT, CALL, LIVT, INT, INC, DEC, PGE, TRC
    }

    if(!opcode)
    {
        opcode = std::make_shared<opcodes::Invalid>();
        // TODO: Turn it into error when all opcodes are implemented
        BUILDER_WARNING(&m_instruction, "invalid opcode");
    }

    data.current_section()->instructions.push_back(opcode);
    return true;
}

}
