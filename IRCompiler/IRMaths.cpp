#include "IRCompiler.h"

void IRCompiler::CompileTokenMaths(Token *token)
{
    switch (token->type)
    {
    case TokenType::UNARYMINUS: {
        auto type = PopType(token);
        if (type == ValueType::Integer)
        {
            AddIR(token, IROpcodes::StackPopIntOperand1);
            AddIR(token, IROpcodes::MathsUnaryMinusInt);
            AddIR(token, IROpcodes::StackPushIntOperand1);
        }
        else if (type == ValueType::Float)
        {
            AddIR(token, IROpcodes::StackPopFloatOperand1);
            AddIR(token, IROpcodes::MathsUnaryMinusFloat);
            AddIR(token, IROpcodes::StackPushFloatOperand1);
        }
        else
        {
            TypeError(token);
        }
        type_stack.push(type);
        break;
    }
    case TokenType::ADD: {

        auto type1 = PopType(token);
        auto type2 = PopType(token);

        // String add?
        if (type1 == ValueType::String || type2 == ValueType::String)
        {
            if (type1 != type2)
                TypeError(token);
            AddIR(token, IROpcodes::StackPopStringOperand1);
            AddIR(token, IROpcodes::StackPopStringOperand2);
            AddIR(token, IROpcodes::MathsAddString);
            AddIR(token, IROpcodes::StackPushStringOperand1);
            type_stack.push(ValueType::String);
        }
        else
        {
            type_stack.push(type2);
            type_stack.push(type1);
            GenericMaths(token, TypePromotion(token, false), IROpcodes::MathsAddInt, IROpcodes::MathsAddFloat);
        }
        break;
    }
    case TokenType::SUBTRACT:
        // We need to figure out if this is unary minus
        if (type_stack.size() == 1)
        {
            auto type = PopType(token);
            if (type == ValueType::Integer)
            {
                AddIR(token, IROpcodes::StackPopIntOperand1);
                AddIR(token, IROpcodes::MathsUnaryMinusInt);
                AddIR(token, IROpcodes::StackPushIntOperand1);
            }
            else if (type == ValueType::Float)
            {
                AddIR(token, IROpcodes::StackPopFloatOperand1);
                AddIR(token, IROpcodes::MathsUnaryMinusFloat);
                AddIR(token, IROpcodes::StackPushFloatOperand1);
            }
            else
            {
                TypeError(token);
            }
            type_stack.push(type);
        }
        else
        {
            GenericMaths(token, TypePromotion(token, false), IROpcodes::MathsSubtractInt,
                         IROpcodes::MathsSubtractFloat);
        }
        break;
    case TokenType::MULTIPLY:
        GenericMaths(token, TypePromotion(token, false), IROpcodes::MathsMultiplyInt, IROpcodes::MathsMultiplyFloat);
        break;
    case TokenType::SHIFT_LEFT:
        DemoteMaths(token, IROpcodes::MathsShiftLeft);
        break;
    case TokenType::SHIFT_RIGHT:
        DemoteMaths(token, IROpcodes::MathsShiftRight);
        break;
    case TokenType::DIVIDE:
        PromoteMaths(token, IROpcodes::MathsDivide);
        break;
    case TokenType::HAT:
        PromoteMaths(token, IROpcodes::MathsPower);
        break;
    case TokenType::MOD:
        DemoteMaths(token, IROpcodes::MathsMod);
        break;
    case TokenType::DIV:
        DemoteMaths(token, IROpcodes::MathsDiv);
        break;
    case TokenType::SQR:
        PromoteMathsSingle(token, IROpcodes::MathsSqr);
        break;
    case TokenType::LN:
        PromoteMathsSingle(token, IROpcodes::MathsLn);
        break;
    case TokenType::LOG:
        PromoteMathsSingle(token, IROpcodes::MathsLog);
        break;
    case TokenType::EXP:
        PromoteMathsSingle(token, IROpcodes::MathsExp);
        break;
    case TokenType::SIN:
        PromoteMathsSingle(token, IROpcodes::MathsSin);
        break;
    case TokenType::ASN:
        PromoteMathsSingle(token, IROpcodes::MathsAsn);
        break;
    case TokenType::COS:
        PromoteMathsSingle(token, IROpcodes::MathsCos);
        break;
    case TokenType::ACS:
        PromoteMathsSingle(token, IROpcodes::MathsAcs);
        break;
    case TokenType::TAN:
        PromoteMathsSingle(token, IROpcodes::MathsTan);
        break;
    case TokenType::ATN:
        PromoteMathsSingle(token, IROpcodes::MathsAtn);
        break;
    case TokenType::RAD:
        PromoteMathsSingle(token, IROpcodes::MathsRad);
        break;
    case TokenType::DEG:
        PromoteMathsSingle(token, IROpcodes::MathsDeg);
        break;
    case TokenType::SGN: {
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
            TypeError(token);
        }
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIR(token, IROpcodes::MathsSgn);
        AddIR(token, IROpcodes::StackPushIntOperand1);
        type_stack.push(ValueType::Integer);
        break;
    }

    case TokenType::ABS: {
        auto type1 = PopType(token);
        if (type1 == ValueType::Integer)
        {
            AddIR(token, IROpcodes::StackPopIntOperand1);
            AddIR(token, IROpcodes::MathsAbsInt);
            AddIR(token, IROpcodes::StackPushIntOperand1);
            type_stack.push(ValueType::Integer);
        }
        else if (type1 == ValueType::Float)
        {
            AddIR(token, IROpcodes::StackPopFloatOperand1);
            AddIR(token, IROpcodes::MathsAbsFloat);
            AddIR(token, IROpcodes::StackPushFloatOperand1);
            type_stack.push(ValueType::Float);
        }
        else
        {
            TypeError(token);
        }
        break;
    }
    default:
        Error(token, "IRMaths: Why?");
    }
}

void IRCompiler::DemoteMaths(Token *token, IROpcodes r)
{
    auto type = TypeDemotion(token);
    AddIR(token, IROpcodes::StackPopIntOperand1);
    AddIR(token, IROpcodes::StackPopIntOperand2);
    AddIR(token, r);
    AddIR(token, IROpcodes::StackPushIntOperand1);
    type_stack.push(ValueType::Integer);
}

void IRCompiler::DemoteMathsSingle(Token *token, IROpcodes r)
{
    auto type = PopType(token);
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
    AddIR(token, IROpcodes::StackPopIntOperand1);
    AddIR(token, r);
    AddIR(token, IROpcodes::StackPushIntOperand1);
    type_stack.push(ValueType::Integer);
}

void IRCompiler::PromoteMathsSingle(Token *token, IROpcodes r)
{
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
        TypeError(token);
    }
    AddIR(token, IROpcodes::StackPopFloatOperand1);
    AddIR(token, r);
    AddIR(token, IROpcodes::StackPushFloatOperand1);
    type_stack.push(ValueType::Float);
}

void IRCompiler::PromoteMaths(Token *token, IROpcodes r)
{
    auto type = TypePromotion(token, true);
    AddIR(token, IROpcodes::StackPopFloatOperand1);
    AddIR(token, IROpcodes::StackPopFloatOperand2);
    AddIR(token, r);
    AddIR(token, IROpcodes::StackPushFloatOperand1);
    type_stack.push(type);
}

void IRCompiler::GenericMaths(Token *token, ValueType type, IROpcodes i, IROpcodes r)
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
        AddIR(token, IROpcodes::StackPushFloatOperand1);
        break;
    default:
        TypeError(token);
    }
    type_stack.push(type);
}

ValueType IRCompiler::TypePromotion(Token *token, bool promote)
{
    auto type1 = PopType(token);
    auto type2 = PopType(token);

    // Types same?
    if (type1 == type2)
    {
        if (!promote)
            return type1;

        if (type1 == ValueType::Integer)
        {
            AddIR(token, IROpcodes::StackPopIntOperand1);
            AddIR(token, IROpcodes::StackPopIntOperand2);
            AddIR(token, IROpcodes::ConvertOperand1IntToFloat1);
            AddIR(token, IROpcodes::ConvertOperand2IntToFloat2);
            AddIR(token, IROpcodes::StackPushFloatOperand2);
            AddIR(token, IROpcodes::StackPushFloatOperand1);
        }
        return ValueType::Float;
    }

    // One an int and one a real?
    if (type1 == ValueType::Integer && type2 == ValueType::Float)
    {
        // Top item needs converting to real
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIR(token, IROpcodes::ConvertOperand1IntToFloat1);
        AddIR(token, IROpcodes::StackPushFloatOperand1);
        return type2;
    }
    if (type1 == ValueType::Float && type2 == ValueType::Integer)
    {
        // 2nd item needs converting to real
        AddIR(token, IROpcodes::StackPopFloatOperand2);
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIR(token, IROpcodes::ConvertOperand1IntToFloat1);
        AddIR(token, IROpcodes::StackPushFloatOperand1);
        AddIR(token, IROpcodes::StackPushFloatOperand2);
        return type1;
    }
    if (type1 == ValueType::String && type2 != ValueType::String)
        Error(token, "Invalid type combination.");
    if (type2 == ValueType::String && type1 != ValueType::String)
        Error(token, "Invalid type combination.");
    Error(token, "Unknown type combination.");
    return type1;
}

ValueType IRCompiler::TypeDemotion(Token *token)
{
    auto type1 = PopType(token);
    auto type2 = PopType(token);

    // Types same?
    if (type1 == type2)
    {
        if (type1 == ValueType::Float)
        {
            AddIR(token, IROpcodes::StackPopFloatOperand1);
            AddIR(token, IROpcodes::StackPopFloatOperand2);
            AddIR(token, IROpcodes::ConvertOperand1FloatToInt1);
            AddIR(token, IROpcodes::ConvertOperand2FloatToInt2);
            AddIR(token, IROpcodes::StackPushIntOperand2);
            AddIR(token, IROpcodes::StackPushIntOperand1);
            return ValueType::Integer;
        }
        else
        {
            return type1;
        }
    }

    // One an int and one a real?
    if (type1 == ValueType::Integer && type2 == ValueType::Float)
    {
        // 2nd item needs converting to real
        AddIR(token, IROpcodes::StackPopIntOperand2);
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIR(token, IROpcodes::ConvertOperand1FloatToInt1);
        AddIR(token, IROpcodes::StackPushIntOperand1);
        AddIR(token, IROpcodes::StackPushIntOperand2);
        return type2;
    }
    if (type1 == ValueType::Float && type2 == ValueType::Integer)
    {
        // Top item needs converting to real
        AddIR(token, IROpcodes::StackPopIntOperand2);
        AddIR(token, IROpcodes::ConvertOperand1FloatToInt1);
        AddIR(token, IROpcodes::StackPushIntOperand1);
        return type1;
    }
    if (type1 == ValueType::String && type2 != ValueType::String)
        Error(token, "Invalid type combination.");
    if (type2 == ValueType::String && type1 != ValueType::String)
        Error(token, "Invalid type combination.");
    Error(token, "Unknown type combination.");
    return type1;
}