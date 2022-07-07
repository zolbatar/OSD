#include "IRCompiler.h"

void IRCompiler::CompileTokenComparison(Token *token)
{
    auto type = TypePromotion(token, false);
    switch (token->type)
    {
    case TokenType::EQUAL:
        GenericComparison(token, type, IROpcodes::CompareEqualInt, IROpcodes::CompareEqualFloat,
                          IROpcodes::CompareEqualString);
        break;
    case TokenType::NOTEQUAL:
        GenericComparison(token, type, IROpcodes::CompareNotEqualInt, IROpcodes::CompareNotEqualFloat,
                          IROpcodes::CompareNotEqualString);
        break;
    case TokenType::LESS:
        GenericComparison(token, type, IROpcodes::CompareLessInt, IROpcodes::CompareLessFloat,
                          IROpcodes::CompareLessString);
        break;
    case TokenType::LESSEQUAL:
        GenericComparison(token, type, IROpcodes::CompareLessEqualInt, IROpcodes::CompareLessEqualFloat,
                          IROpcodes::CompareLessEqualString);
        break;
    case TokenType::GREATER:
        GenericComparison(token, type, IROpcodes::CompareGreaterInt, IROpcodes::CompareGreaterFloat,
                          IROpcodes::CompareGreaterString);
        break;
    case TokenType::GREATEREQUAL:
        GenericComparison(token, type, IROpcodes::CompareGreaterEqualInt, IROpcodes::CompareGreaterEqualFloat,
                          IROpcodes::CompareGreaterEqualString);
        break;
    default:
        Error(token, "Why?");
    }
}

void IRCompiler::GenericComparison(Token *token, ValueType type, IROpcodes i, IROpcodes r, IROpcodes s)
{
    switch (type)
    {
    case ValueType::Integer:
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIR(token, IROpcodes::StackPopIntOperand2);
        AddIR(token, i);
        AddIR(token, IROpcodes::StackPushIntOperand1);
        break;
    case ValueType::Float:
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIR(token, IROpcodes::StackPopFloatOperand2);
        AddIR(token, r);
        AddIR(token, IROpcodes::StackPushIntOperand1);
        break;
    case ValueType::String:
        AddIR(token, IROpcodes::StackPopStringOperand1);
        AddIR(token, IROpcodes::StackPopStringOperand2);
        AddIR(token, s);
        AddIR(token, IROpcodes::StackPushIntOperand1);
        break;
    }
    type_stack.push(ValueType::Integer);
}
