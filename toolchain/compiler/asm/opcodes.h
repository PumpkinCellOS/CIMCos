#pragma once

#include <string>

#include "ast.h"

namespace assembler
{

class RuntimeData;

class Opcode
{
public:
    virtual bool generate(std::string& payload, RuntimeData&, size_t) const 
    {
        // Placeholder/NOOP
        payload = 0xd4;
        return true; 
    }
    virtual std::string display() const = 0;
};

namespace opcodes
{

class NoArgument : public Opcode
{
public:
    NoArgument(unsigned char id)
    : m_id(id) {}

    virtual bool generate(std::string& payload, RuntimeData&, size_t) const { payload = m_id; return true; }
    virtual std::string display() const { return "NoArgument " + std::to_string((unsigned)m_id); }

private:
    unsigned char m_id;
};

class Invalid : public NoArgument
{
public:
    Invalid()
    : NoArgument(0xd4) {}

    virtual std::string display() const { return "Invalid"; }
};

class Jump : public Opcode
{
public:
    enum Condition
    {
        None = 0x71,
        Equal,
        Greater,
        Less,
        NotEqual,
        GreaterOrEqual,
        LessOrEqual,
        Invalid
    };

    Jump(std::shared_ptr<Operand> destination, Condition condition)
    : m_destination(destination), m_condition(condition) {}

    virtual bool generate(std::string& payload, RuntimeData&, size_t) const override;
    virtual std::string display() const override { return "Jump" + std::to_string(m_condition) + " -> " + m_destination->display(); }

private:
    std::shared_ptr<Operand> m_destination;
    Condition m_condition;
};

class FlagEnableOrDisable : public Opcode
{
public:
    FlagEnableOrDisable(std::shared_ptr<Operand> flag, bool enable)
    : m_flag(flag), m_enable(enable) {}

    virtual std::string display() const { return std::string("Flag") + (m_enable ? "Enable " : "Disable ") + m_flag->display(); }

private:
    std::shared_ptr<Operand> m_flag;
    bool m_enable;
};

class Data : public Opcode
{
public:
    Data(std::shared_ptr<Operand> value)
    : m_value(value) {}

    virtual std::string display() const { return "Data " + m_value->display(); }

private:
    std::shared_ptr<Operand> m_value;
};

class Skip : public Opcode
{
public:
    Skip(std::shared_ptr<Operand> value)
    : m_value(value) {}

    virtual std::string display() const { return "Skip " + m_value->display(); }

private:
    std::shared_ptr<Operand> m_value;
};

class InputOutput : public Opcode
{
public:
    enum Kind
    {
        Input,
        Output
    };

    enum Size
    {
        _8Bits,
        _16Bits
    };

    InputOutput(Kind kind, Size size, uint8_t port, std::shared_ptr<Operand> value)
    : m_kind(kind), m_size(size), m_port(port), m_value(value) {}

    virtual bool generate(std::string& payload, RuntimeData&, size_t) const override;
    virtual std::string display() const override
    {
        return (m_kind == Input ? "IN" : "OUT") + std::string(m_size == _8Bits ? "8 " : "16 ") + std::to_string(m_port) + " <- " + m_value->display();
    }

private:
    Kind m_kind;
    Size m_size;
    uint8_t m_port;
    std::shared_ptr<Operand> m_value;
};

}

}
