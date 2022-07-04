#include "IRCompiler.h"

void IRCompiler::CompileTokenData(Token *token)
{
    switch (token->type)
    {
    case TokenType::DATA: {
        for (auto &expr : token->expressions)
        {

            // Process expression
            for (auto &t : expr)
                CompileToken(t);

            // Get type
            auto type = PopType(token);

            switch (type)
            {
            case ValueType::Integer:
                AddIR(token, IROpcodes::StackPopIntOperand1);
                AddIR(token, IROpcodes::DataInteger);
                break;
            case ValueType::Float:
                AddIR(token, IROpcodes::StackPopFloatOperand1);
                AddIR(token, IROpcodes::DataFloat);
                break;
            case ValueType::String:
                AddIR(token, IROpcodes::StackPopStringOperand1);
                AddIR(token, IROpcodes::DataString);
                break;
            }
        }
        break;
    }
    case TokenType::DATALABEL: {
        AddIRStringLiteral(token, token->text);
        AddIR(token, IROpcodes::StackPopStringOperand1);
        AddIR(token, IROpcodes::DataLabel);
        break;
    }
    case TokenType::READ: {
        for (auto &t : token->stack)
        {
            if (t->vtype.IsInteger())
            {
                AddIR(token, IROpcodes::ReadInteger);
                AddIR(token, IROpcodes::StackPushIntOperand1);
                AddIR(token, IROpcodes::StackPopIntOperand1);
                AddIRWithIndex(token, IROpcodes::VariableStoreInteger, t->index);
            }
            else if (t->vtype.IsFloat())
            {
                AddIR(token, IROpcodes::ReadFloat);
                AddIR(token, IROpcodes::StackPushFloatOperand1);
                AddIR(token, IROpcodes::StackPopFloatOperand1);
                AddIRWithIndex(token, IROpcodes::VariableStoreFloat, t->index);
            }
            else if (t->vtype.IsString())
            {
                AddIR(token, IROpcodes::ReadString);
                AddIR(token, IROpcodes::StackPushStringOperand1);
                AddIR(token, IROpcodes::StackPopStringOperand1);
                AddIRWithIndex(token, IROpcodes::VariableStoreString, t->index);
            }
            else
                Error(token, "IRData: Why?");
        }
        break;
    }
    case TokenType::RESTORE: {
        AddIRStringLiteral(token, token->text);
        AddIR(token, IROpcodes::StackPopStringOperand1);
        AddIR(token, IROpcodes::Restore);
        break;
    }
    default:
        assert(0);
    }
}
