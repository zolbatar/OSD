#include "IRCompiler.h"

void IRCompiler::CompileTokenAssignment(Token *token) {
    // Expression value
    for (auto &t: token->expressions[0])
        CompileToken(t);

    // Type
    auto type = PopType(token);

    // Build IR
    if (token->vtype.IsInteger()) {
		EnsureStackIsInteger(token, type);
        AddIR(IROpcodes::StackPopIntOperand1);
        AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index);
    } else if (token->vtype.IsFloat()) {
    	EnsureStackIsFloat(token, type);
        AddIR(IROpcodes::StackPopFloatOperand1);
        AddIRWithIndex(IROpcodes::VariableStoreFloat, token->index);
    } else if (token->vtype.IsString()) {
        if (type != ValueType::String)
            TypeError(token);
        AddIR(IROpcodes::StackPopStringOperand1);
        AddIRWithIndex(IROpcodes::VariableStoreString, token->index);
    } else if (token->vtype.IsType()) {
    	if (token->stack.front()->vtype.IsInteger()) {
    		EnsureStackIsInteger(token, type);
    		AddIR(IROpcodes::StackPopIntOperand1);
    		AddIRWithIndexAndInteger(IROpcodes::VariableStoreIntegerWithOffset, token->index, token->stack.front()->index);
    	} else if (token->stack.front()->vtype.IsFloat()) {
    		EnsureStackIsFloat(token, type);
    		AddIR(IROpcodes::StackPopFloatOperand1);
    		AddIRWithIndexAndInteger(IROpcodes::VariableStoreFloatWithOffset, token->index, token->stack.front()->index);
    	} else if (token->stack.front()->vtype.IsString()) {
    		if (type != ValueType::String)
    			TypeError(token);
    		AddIR(IROpcodes::StackPopStringOperand1);
    		AddIRWithIndexAndInteger(IROpcodes::VariableStoreStringWithOffset, token->index, token->stack.front()->index);
    	}
    }
}

void IRCompiler::CompileTokenSwap(Token *token) {
    if (token->vtype.IsInteger()) {
        AddIRWithIndex(IROpcodes::VariableSwapSourceInteger, token->index);
        AddIRWithIndex(IROpcodes::VariableSwapDestinationInteger, token->index2);
    } else if (token->vtype.IsFloat()) {
        AddIRWithIndex(IROpcodes::VariableSwapSourceFloat, token->index);
        AddIRWithIndex(IROpcodes::VariableSwapDestinationFloat, token->index2);
    } else if (token->vtype.IsString()) {
        AddIRWithIndex(IROpcodes::VariableSwapSourceString, token->index);
        AddIRWithIndex(IROpcodes::VariableSwapDestinationString, token->index2);
    } else
        Error(token, "Why?");
}

