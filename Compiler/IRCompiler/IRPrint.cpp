#include "IRCompiler.h"

void IRCompiler::CompileTokenPrint(Token *token)
{
    switch (token->type)
    {
    case TokenType::SPC:
    case TokenType::TAB: {
        for (auto &expr : token->expressions)
        {
            for (auto &ctoken : expr)
                CompileToken(ctoken);
        }
        auto v = PopType(token);
        EnsureStackIsInteger(token, v);
        if (token->type == TokenType::SPC)
        {
            AddIR(token, IROpcodes::ArgumentInteger);
            AddIRWithStringLiteral(token, IROpcodes::CallFunc, "SPC");
        }
        else
        {
            AddIR(token, IROpcodes::ArgumentInteger);
            AddIRWithStringLiteral(token, IROpcodes::CallFunc, "TAB");
        }
        break;
    }
    case TokenType::PRINTTABBED:
        AddIRWithStringLiteral(token, IROpcodes::CallFunc, "Tabbed On");
        break;
    case TokenType::PRINTTABBEDOFF:
        AddIRWithStringLiteral(token, IROpcodes::CallFunc, "Tabbed Off");
        break;
    case TokenType::PRINTNL:
        AddIRWithStringLiteral(token, IROpcodes::CallFunc, "NL");
        break;
    case TokenType::PRINT: {
        // Loop through each expression in turn
        for (auto &expr : token->expressions)
        {
            if (!expr.empty())
            {
                for (auto &ctoken : expr)
                    CompileToken(ctoken);
                while (!type_stack.empty())
                {
                    auto type = type_stack.top();
                    type_stack.pop();
                    //                    CLogger::Get()->Write("PRINT", LogNotice, "Line: %d, Type: %d",
                    //                    token->line_number, (int)type);
                    switch (type)
                    {
                    case ValueType::Integer:
                        AddIR(token, IROpcodes::ArgumentInteger);
                        AddIRWithStringLiteral(token, IROpcodes::CallFunc, "PRINT integer");
                        break;
                    case ValueType::Float:
                        AddIR(token, IROpcodes::ArgumentFloat);
                        AddIRWithStringLiteral(token, IROpcodes::CallFunc, "PRINT real");
                        break;
                    case ValueType::String:
                        AddIR(token, IROpcodes::ArgumentString);
                        AddIRWithStringLiteral(token, IROpcodes::CallFunc, "PRINT string");
                        break;
                    }
                }
            }
        }
        break;
    }
    default:
        Error(token, "IRPrint: Why?");
    }
}
