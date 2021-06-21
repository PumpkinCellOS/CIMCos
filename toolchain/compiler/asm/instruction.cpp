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
    else if(m_instruction.members.size() == 2)
    {
        auto arg1 = m_instruction.members[0];
        auto arg2 = m_instruction.members[1];

        auto resolve_io_port = [&]()->int16_t {
            int16_t port;
            std::string resolved_arg1;
            if(arg1->type == Operand::Type::Name)
            {
                resolved_arg1 = data.resolve_operand_with_assignments(arg1)->value;
            }
            else
            {
                resolved_arg1 = arg1->value;
            }

            try
            {
                port = std::stoi(resolved_arg1);
            }
            catch(...)
            {
                BUILDER_ERROR_RC(&m_instruction, "I/O port should be a number, it is '" + resolved_arg1 + "'", -1);
            }
            return port;
        };

        if(mnemonic.find("OUT") == 0)
        {
            int16_t port = resolve_io_port();
            if(port == -1)
                return {};

            auto resolved_arg2 = data.resolve_operand_with_assignments(arg2);
            if(!resolved_arg2)
                return {};

            if(mnemonic.size() <= 3)
                BUILDER_ERROR(&m_instruction, "no size given to OUT");
            std::string size = mnemonic.substr(3);
            if(size == "8")
            {
                opcode = std::make_shared<opcodes::InputOutput>(opcodes::InputOutput::Output, opcodes::InputOutput::_8Bits, port, resolved_arg2);
            }
            else if(size == "16")
            {
                opcode = std::make_shared<opcodes::InputOutput>(opcodes::InputOutput::Output, opcodes::InputOutput::_16Bits, port, resolved_arg2);
            }
            else
                BUILDER_ERROR(&m_instruction, "invalid size given to OUT (" + size + ")");
        }
        else if(mnemonic.find("IN") == 0)
        {
            int16_t port = resolve_io_port();
            if(port == -1)
                return {};

            auto resolved_arg2 = data.resolve_operand_with_assignments(arg2);
            if(!resolved_arg2)
                return {};
                
            if(mnemonic.size() <= 2)
                BUILDER_ERROR(&m_instruction, "no size given to IN");
            std::string size = mnemonic.substr(2);
            if(size == "8")
            {
                opcode = std::make_shared<opcodes::InputOutput>(opcodes::InputOutput::Input, opcodes::InputOutput::_8Bits, port, resolved_arg2);
            }
            else if(size == "16")
            {
                opcode = std::make_shared<opcodes::InputOutput>(opcodes::InputOutput::Input, opcodes::InputOutput::_16Bits, port, resolved_arg2);
            }
            else
                BUILDER_ERROR(&m_instruction, "invalid size given to IN (" + size + ")");
        }
        // TODO: MOV, CMP, ADD, SUB, AND, OR, SHL, SHR, XOR, XCHG
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
