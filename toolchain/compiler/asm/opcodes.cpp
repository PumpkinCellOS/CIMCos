#include "opcodes.h"

#include "runtimedata.h"

#include <arpa/inet.h>

namespace assembler
{

namespace opcodes
{

bool Jump::generate(std::string& payload, RuntimeData& data, size_t offset) const
{
    if(m_destination->type == Operand::Type::Name && m_destination->value == "ax")
    {
        payload += "\x70";
    }
    else
    {
        payload += static_cast<unsigned char>(m_condition);
        uint16_t address = data.resolve_operand(m_destination, offset);
        std::cout << "jmp addr = " << address << std::endl;
        if(m_condition == Condition::None)
        {
            address = htons(address);
            std::cout << "jmp addr after offset = " << address << std::endl;
            payload += std::string((char*)&address, 2);
        }
        else
        {
            address -= (int8_t)offset;
            std::cout << "offset = " << offset << ", jmp addr after offset = " << address << std::endl;
            if(address != (address& 0xFF))
                GENERATOR_ERROR("jump offset must be not >255, it is " + std::to_string(address));
            payload += std::string((char*)&address, 1);
        }
    }
    return true;
}
    
bool InputOutput::generate(std::string& payload, RuntimeData& data, size_t offset) const
{
    if(m_port != (m_port & 0xF))
        GENERATOR_ERROR("I/O port address must be not >15, it is " + std::to_string(m_port));

    auto resolve_io_immediate_operand = [&]()->std::string
    {
        std::string payload;
        auto resolved_value = data.resolve_operand_with_assignments(m_value);
        if(resolved_value->can_resolve_as_number())
        {
            int value;
            try
            {
                value = resolved_value->resolve_as_number();
            }
            catch(...)
            {
                GENERATOR_ERROR("invalid number for I/O immediate operand");
            }
            if(value < 0 || value > 0xFFFF)
                GENERATOR_ERROR("I/O immediate operand value must be in range 0 - 65535");
            payload += value & 0xFF00 >> 8;
            payload += value & 0xFF;
        }
        return payload;
    };

    if(m_kind == Output)
    {
        if(m_size == _16Bits)
        {
            if(m_value->type == Operand::Type::Name && m_value->value == "ax")
            {
                payload += 0x00 | m_port & 0xF;
            }
            else
            {
                payload += 0x40 | m_port & 0xF;
                std::string immediate_payload = resolve_io_immediate_operand();
                if(immediate_payload.empty())
                    return false;
                payload += immediate_payload;
            }
        }
        else
        {
            if(m_value->type == Operand::Type::Name && m_value->value == "al")
            {
                payload += 0x10 | m_port & 0xF;
            }
            else
            {
                payload += 0x50 | m_port & 0xF;
                std::string immediate_payload = resolve_io_immediate_operand();
                if(immediate_payload.empty())
                    return false;
                payload += immediate_payload;
            }
        }
    }
    else
    {
        if(m_size == _16Bits)
        {
            if(m_value->type == Operand::Type::Name && m_value->value == "ax")
            {
                payload += 0x20 | m_port & 0xF;
            }
            else
            {
                GENERATOR_ERROR("invalid register for IN");
            }
        }
        else
        {
            if(m_value->type == Operand::Type::Name && m_value->value == "al")
            {
                payload += 0x30 | m_port & 0xF;
            }
            else
            {
                GENERATOR_ERROR("invalid register for IN");
            }
        }
    }
    return true;
}

}

}
