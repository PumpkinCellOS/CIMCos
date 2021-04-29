#pragma once

namespace cpp_compiler
{

void print_indent(size_t depth)
{
    for(size_t s = 0; s < depth; s++)
        std::cout << "    ";
}

class Node
{
public:
    virtual void display(size_t depth) = 0;
};

// <external-declaration> ::= <function-definition>
//                          | <declaration>
class ExternalDeclaration
{
};

// <function-definition> ::= {<declaration-specifier>}* <declarator> {<declaration>}* <compound-statement>
class FunctionDefinition
{
    std::shared_ptr<>
};

// <declaration-specifier> ::= <storage-class-specifier>
//                           | <type-specifier>
//                           | <type-qualifier>
class DeclarationSpecifier
{
};

// <translation-unit> ::= {<external-declaration>}*
class TranslationUnit
{
public:
    std::map<std::string, std::shared_ptr<ExternalDeclaration>> declarations;

    virtual void display(size_t depth) override
    {
        for(auto& pr: declarations)
        {
            print_indent(depth);
            std::cout << pr.first;
            pr.second->display(depth + 1);
        }
    }
};

}
