#pragma once

#include "parser.h"
#include "runtimedata.h"

namespace assembler
{

class Builder
{
public:
    Builder(std::shared_ptr<Block> block, const compiler::Options& options)
    : m_block(block), m_options(options) {}

    bool build();
    void display();

    RuntimeData& runtime_data() { return m_data; }

private:
    std::shared_ptr<Block> m_block;
    const compiler::Options& m_options;
    RuntimeData m_data;
};

}
