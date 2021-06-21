#include "generator.h"

#include <arpa/inet.h>

namespace assembler
{

struct [[gnu::packed]] Cx16ExecHeader
{
    uint8_t magic = 0x50;
    uint8_t arch = 0x80;
    uint16_t entry = 0x0;
    uint8_t exec_type = 0x80; // object file
    uint8_t padding = 0x00;
    uint16_t text_addr = 0x0;
    uint16_t rodata_addr = 0x0;
    uint16_t data_addr = 0x0;
    uint16_t bss_addr = 0x0;
    uint16_t sym_addr = 0x0;
    uint16_t reloc_addr = 0x0;
};

bool Generator::generate(convert::OutputFile& output)
{
    // Header
    Cx16ExecHeader header;
    size_t current_offset = sizeof(Cx16ExecHeader);
    std::string payload;

    // Generate machine code
    for(auto& section: m_data.sections())
    {
        std::string section_payload;
        uint16_t* header_addr = nullptr;
        size_t section_offset = current_offset;

        if(section.first == "text")
        {
            header_addr = &header.text_addr;
        }
        else if(section.first == "rodata")
        {
            header_addr = &header.rodata_addr;
        }
        else if(section.first == "data")
        {
            header_addr = &header.data_addr;
        }
        else if(section.first == "bss")
        {
            header_addr = &header.bss_addr;
        }
        else
            GENERATOR_ERROR("invalid section: " + section.first);

        *header_addr = htons(section_offset);
        m_data.switch_section(section.first);

        for(auto& opcode: section.second.instructions)
        {
            std::string instruction_payload;
            if(!opcode->generate(instruction_payload, m_data, current_offset - section_offset))
                return false;
            section_payload += instruction_payload;
            section.second.instruction_adresses.push_back(current_offset - section_offset);
            current_offset += instruction_payload.size();
        }

        if(!section.second.replace_all_replace_entries(section_payload))
            return false;
        payload += section_payload;
    }

    // TODO: Write relocation entries and symbols

    // Write everything to file :)
    // TODO: This should be in linker
    if(!m_options.f_raw)
        output.stream.write((char*)&header, sizeof(Cx16ExecHeader));
    output.stream.write(payload.data(), payload.size());
    return true;
}

}
