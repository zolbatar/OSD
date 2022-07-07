#include "IRCompiler.h"

void IRCompiler::CompileTokenConversion(Token *token)
{
    switch (token->type)
    {
    case TokenType::INT: {
        auto type = PopType(token);
        if (type == ValueType::Integer)
        {
            // Fine
        }
        else if (type == ValueType::Float)
        {
            AddIR(token, IROpcodes::StackPopFloatOperand1);
            AddIR(token, IROpcodes::ConvertOperand1FloatToInt1);
            AddIR(token, IROpcodes::StackPushIntOperand1);
        }
        else
        {
            AddIR(token, IROpcodes::StackPopStringOperand1);
            AddIR(token, IROpcodes::ConvertOperand1StringToInt1);
            AddIR(token, IROpcodes::StackPushIntOperand1);
        }
        type_stack.push(ValueType::Integer);
        break;
    }
    case TokenType::FLOAT: {
        auto type = PopType(token);
        if (type == ValueType::Integer)
        {
            AddIR(token, IROpcodes::StackPopIntOperand1);
            AddIR(token, IROpcodes::ConvertOperand1IntToFloat1);
            AddIR(token, IROpcodes::StackPushFloatOperand1);
        }
        else if (type == ValueType::Float)
        {
            // Fine
        }
        else
        {
            AddIR(token, IROpcodes::StackPopStringOperand1);
            AddIR(token, IROpcodes::ConvertOperand1StringToFloat1);
            AddIR(token, IROpcodes::StackPushFloatOperand1);
        }
        type_stack.push(ValueType::Float);
        break;
    }
    case TokenType::STRS: {
        auto type = PopType(token);
        if (type == ValueType::Integer)
        {
            AddIR(token, IROpcodes::StackPopIntOperand1);
            AddIR(token, IROpcodes::ConvertOperand1IntToString1);
            AddIR(token, IROpcodes::StackPushStringOperand1);
        }
        else if (type == ValueType::Float)
        {
            AddIR(token, IROpcodes::StackPopFloatOperand1);
            AddIR(token, IROpcodes::ConvertOperand1FloatToString1);
            AddIR(token, IROpcodes::StackPushStringOperand1);
        }
        else
        {
            // Fine
        }
        type_stack.push(ValueType::String);
        break;
    }
    default:
        break;
    }
}

void IRCompiler::EnsureStackIsInteger(Token *token, ValueType type)
{
    if (type == ValueType::Float)
    {
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIR(token, IROpcodes::ConvertOperand1FloatToInt1);
        AddIR(token, IROpcodes::StackPushIntOperand1);
    }
    else if (type == ValueType::Integer)
    {
        // Fine
    }
    else
    {
        TypeError(token);
    }
}

void IRCompiler::EnsureStackIsFloat(Token *token, ValueType type)
{
    if (type == ValueType::Float)
    {
        // Fine
    }
    else if (type == ValueType::Integer)
    {
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIR(token, IROpcodes::ConvertOperand1IntToFloat1);
        AddIR(token, IROpcodes::StackPushFloatOperand1);
    }
    else
    {
        TypeError(token);
    }
}
