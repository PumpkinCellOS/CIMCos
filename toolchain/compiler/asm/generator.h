#pragma once

#include "../compiler.h"
#include "runtimedata.h"

namespace assembler
{

class Generator
{
public:
    Generator(RuntimeData& data)
    : m_data(data) {}

    bool generate(convert::OutputFile&);

private:
    RuntimeData& m_data;
};

}
