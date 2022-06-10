#include "IRCompiler.h"

void IRCompiler::CompileTokenDef(Token* token)
{
	// Create local instruction lists, to append later
	current_def.clear();
	current_def_init.clear();

	is_global = false;
	auto index = proc_lookup.find(token->text)->second;
	Init_AddIRWithStringLiteralWithIntegerAndIndex(IROpcodes::PROCStart, token->text, token->required_types.size(), index);

	// Parameters? I.e. local variables? Vars first
	int i = 0;
	for (auto& type: token->required_types) {
		if (type.IsInteger())
			Init_AddIRWithIndex(IROpcodes::VariableLocalCreateInteger, i);
		else if (type.IsFloat())
			Init_AddIRWithIndex(IROpcodes::VariableLocalCreateFloat, i);
		else if (type.IsString())
			Init_AddIRWithIndex(IROpcodes::VariableLocalCreateString, i);
		i++;
	}

	// Save actual values now
	i = token->required_types.size()-1;
	for (auto type = token->required_types.rbegin(); type!=token->required_types.rend(); ++type) {
		if (type->IsInteger())
			Init_AddIRWithIndex(IROpcodes::ParameterInt, i);
		else if (type->IsFloat())
			Init_AddIRWithIndex(IROpcodes::ParameterFloat, i);
		else if (type->IsString())
			Init_AddIRWithIndex(IROpcodes::ParameterString, i);
		i--;
	}

	// Do actual code
	for (auto& branch: token->branches)
		for (auto& ctoken: branch)
			CompileToken(ctoken);

	AddIRWithIndex(IROpcodes::PROCEnd, index);
	is_global = true;

	// Add local instruction lists
	ir.splice(ir.end(), current_def_init);
	ir.splice(ir.end(), current_def);
}

void IRCompiler::CompileTokenReturn(Token* token)
{
	auto type = token->return_types.front();
	if (type.IsNone()) {
		AddIRWithIndex(IROpcodes::ReturnNone, token->index);
	}
	else if (type.IsInteger()) {
		for (auto& t: token->expressions[0])
			CompileToken(t);
		auto type = PopType(token);
		EnsureStackIsInteger(token, type);
		AddIR(IROpcodes::StackPopIntOperand1);
		AddIRWithIndex(IROpcodes::ReturnInteger, token->index);
	}
	else if (type.IsFloat()) {
		for (auto& t: token->expressions[0])
			CompileToken(t);
		auto type = PopType(token);
		EnsureStackIsFloat(token, type);
		AddIR(IROpcodes::StackPopFloatOperand1);
		AddIRWithIndex(IROpcodes::ReturnFloat, token->index);
	}
	else if (type.IsString()) {
		for (auto& t: token->expressions[0])
			CompileToken(t);
		auto type = PopType(token);
		if (type!=ValueType::String)
			TypeError(token);
		AddIR(IROpcodes::StackPopStringOperand1);
		AddIRWithIndex(IROpcodes::ReturnString, token->index);
	}
}

void IRCompiler::CompileTokenProcCall(Token* token, bool expression)
{
	auto f = proc_lookup.find(token->text);
	if (f==proc_lookup.end()) {
		throw DARICException(ExceptionType::COMPILER, filenames->at(token->file_number), token->line_number, token->char_number,
				"Procedure '"+token->text+"' not found");
	}

	// Return type
	if (expression) {
		auto top = token->return_types.front();
		if (top.IsInteger()) {
			AddIR(IROpcodes::StackReturnIntArgument);
			type_stack.push(ValueType::Integer);
		}
		else if (top.IsFloat()) {
			AddIR(IROpcodes::StackReturnFloatArgument);
			type_stack.push(ValueType::Float);
		}
		else if (top.IsString()) {
			AddIR(IROpcodes::StackReturnStringArgument);
			type_stack.push(ValueType::String);
		}
	}

	// Parameters?
	if (token->required_types.size()>0) {

		// Check each expression in turn for type
		int i = 0;
		for (auto type = token->required_types.begin(); type!=token->required_types.end(); ++type) {
			for (auto& ctoken: token->expressions[i])
				CompileToken(ctoken);

			if (type->IsInteger()) {
				CheckParamTypeOnly(token, ValueType::Integer);
			}
			else if (type->IsFloat()) {
				CheckParamTypeOnly(token, ValueType::Float);
			}
			else if (type->IsString()) {
				CheckParamTypeOnly(token, ValueType::String);
			}
			i++;
		}

		for (auto type = token->required_types.begin(); type!=token->required_types.end(); ++type) {
			if (type->IsInteger()) {
				AddIR(IROpcodes::ArgumentInteger);
			}
			else if (type->IsFloat()) {
				AddIR(IROpcodes::ArgumentFloat);
			}
			else if (type->IsString()) {
				AddIR(IROpcodes::ArgumentString);
			}
		}
	}

	AddIRWithStringLiteralWithInteger(IROpcodes::PROCCall, token->text, f->second);
}
