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
            if(address & 0xFF != address)
                GENERATOR_ERROR("jump offset must be not >255, it is " + std::to_string(address));
            payload += std::string((char*)&address, 1);
        }
    }
    return true;
}

}

}
