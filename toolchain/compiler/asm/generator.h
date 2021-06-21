#pragma once

#include "../compiler.h"
#include "runtimedata.h"

namespace assembler
{

class Generator
{
public:
    Generator(RuntimeData& data, const compiler::Options& options)
    : m_data(data), m_options(options) {}

    bool generate(convert::OutputFile&);

private:
    RuntimeData& m_data;
    const compiler::Options& m_options;
};

}
