#include "IRCompiler.h"

void IRCompiler::CompileTokenString(Token *token)
{
    switch (token->type)
    {
    case TokenType::ASC:
        CheckParamType(token, ValueType::String);
        AddIR(token, IROpcodes::StringsAsc);
        SetReturnType(token, ValueType::Integer);
        break;
    case TokenType::CHRS:
        CheckParamType(token, ValueType::Integer);
        AddIR(token, IROpcodes::StringsChrs);
        SetReturnType(token, ValueType::String);
        break;
    case TokenType::INSTR:
        CheckParamType(token, ValueType::Integer);
        CheckParamType(token, ValueType::String);
        CheckParamType(token, ValueType::String);
        AddIR(token, IROpcodes::StringsInstr);
        SetReturnType(token, ValueType::Integer);
        break;
    case TokenType::LEFTS:
        CheckParamType(token, ValueType::Integer);
        CheckParamType(token, ValueType::String);
        AddIR(token, IROpcodes::StringsLefts);
        SetReturnType(token, ValueType::String);
        break;
    case TokenType::MIDS:
        CheckParamType(token, ValueType::Integer);
        CheckParamType(token, ValueType::Integer);
        CheckParamType(token, ValueType::String);
        AddIR(token, IROpcodes::StringsMids);
        SetReturnType(token, ValueType::String);
        break;
    case TokenType::RIGHTS:
        CheckParamType(token, ValueType::Integer);
        CheckParamType(token, ValueType::String);
        AddIR(token, IROpcodes::StringsRights);
        SetReturnType(token, ValueType::String);
        break;
    case TokenType::LEN:
        CheckParamType(token, ValueType::String);
        AddIR(token, IROpcodes::StringsLen);
        SetReturnType(token, ValueType::Integer);
        break;
    case TokenType::STRINGS:
        CheckParamType(token, ValueType::Integer);
        CheckParamType(token, ValueType::String);
        AddIR(token, IROpcodes::StringsStrings);
        SetReturnType(token, ValueType::String);
        break;
    default:
        Error(token, "Why?");
    }
}
