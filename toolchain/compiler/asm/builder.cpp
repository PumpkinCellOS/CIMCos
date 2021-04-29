#include "builder.h"

namespace assembler
{

bool Builder::build()
{
    std::cout << "Building semantic tree..." << std::endl;

    bool success = true;
    for(auto& op: m_block->members)
    {
        success &= op->execute(m_data);
    }

    return success;
}

void Builder::display()
{
    std::cout << "Builder dump:" << std::endl;
    m_data.display();
}

}
