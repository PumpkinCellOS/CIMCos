#pragma once

#include "parser.h"
#include "runtimedata.h"

namespace assembler
{

class Builder
{
public:
    Builder(std::shared_ptr<Block> block)
    : m_block(block) {}

    bool build();
    void display();

    RuntimeData& runtime_data() { return m_data; }

private:
    std::shared_ptr<Block> m_block;
    RuntimeData m_data;
};

}
