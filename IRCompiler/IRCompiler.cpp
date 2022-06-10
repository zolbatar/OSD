#include "IRCompiler.h"

#ifndef CLION
#include <circle/new.h>
#include <circle/string.h>
#else

#include <string>

#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 /**< pi */
#endif

void IRCompiler::CompileToken(Token* token)
{
	switch (token->type) {
		case TokenType::END:
			AddIR(IROpcodes::End);
			break;
		case TokenType::YIELD:
			AddIR(IROpcodes::Yield);
			break;
		case TokenType::STATEMENT_START:
			//AddIRWithIndex(IROpcodes::StackCheck, token->line_number);
			if (!type_stack.empty())
				Error(token, "Expected empty type stack");
			break;

			// Variables
		case TokenType::DATA:
		case TokenType::DATALABEL:
		case TokenType::READ:
		case TokenType::RESTORE:
			CompileTokenData(token);
			break;

			// Variables
		case TokenType::LOCAL:
			CompileTokenLocalVariable(token);
			break;
		case TokenType::LOCAL_NOINIT:
			CompileTokenLocalNoInitVariable(token);
			break;
		case TokenType::GLOBAL:
			CompileTokenGlobalVariable(token);
			break;
		case TokenType::GLOBAL_NOINIT:
			CompileTokenGlobalNoInitVariable(token);
			break;

			// Looping
		case TokenType::FOR:
			CompileTokenFor(token);
			break;
		case TokenType::NEXT:
			CompileTokenNext(token);
			break;

			// Chrono
		case TokenType::TIME:
			AddIR(IROpcodes::StackReturnIntArgument);
			AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_TIME, "TIME");
			type_stack.push(ValueType::Integer);
			break;
		case TokenType::TIMES:
			AddIR(IROpcodes::StackReturnStringArgument);
			AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_TIMES, "TIME$");
			type_stack.push(ValueType::String);
			break;

			// Maths
		case TokenType::ADD:
		case TokenType::SUBTRACT:
		case TokenType::MULTIPLY:
		case TokenType::DIVIDE:
		case TokenType::SHIFT_LEFT:
		case TokenType::SHIFT_RIGHT:
		case TokenType::HAT:
		case TokenType::MOD:
		case TokenType::DIV:
		case TokenType::ABS:
		case TokenType::SQR:
		case TokenType::LN:
		case TokenType::LOG:
		case TokenType::EXP:
		case TokenType::SIN:
		case TokenType::ASN:
		case TokenType::COS:
		case TokenType::ACS:
		case TokenType::TAN:
		case TokenType::ATN:
		case TokenType::RAD:
		case TokenType::DEG:
		case TokenType::SGN:
			CompileTokenMaths(token);
			break;
		case TokenType::PI:
			AddIRFloatLiteral(M_PI);
			type_stack.push(ValueType::Float);
			break;

		case TokenType::_TRUE:
			AddIRIntegerLiteral(1);
			type_stack.push(ValueType::Integer);
			break;
		case TokenType::_FALSE:
			AddIRIntegerLiteral(0);
			type_stack.push(ValueType::Integer);
			break;
		case TokenType::AND:
		case TokenType::OR:
		case TokenType::EOR:
		case TokenType::NOT:
			CompileTokenBoolean(token);
			break;

		case TokenType::INT:
		case TokenType::FLOAT:
		case TokenType::STRS:
			CompileTokenConversion(token);
			break;

		case TokenType::ASC:
		case TokenType::CHRS:
		case TokenType::INSTR:
		case TokenType::LEFTS:
		case TokenType::MIDS:
		case TokenType::RIGHTS:
		case TokenType::LEN:
		case TokenType::STRINGS:
			CompileTokenString(token);
			break;

		case TokenType::EQUAL:
		case TokenType::NOTEQUAL:
		case TokenType::LESS:
		case TokenType::LESSEQUAL:
		case TokenType::GREATER:
		case TokenType::GREATEREQUAL:
			CompileTokenComparison(token);
			break;

			// Statements
		case TokenType::SPC:
		case TokenType::TAB:
		case TokenType::PRINTTABBED:
		case TokenType::PRINTTABBEDOFF:
		case TokenType::PRINTNL:
		case TokenType::PRINT:
			CompileTokenPrint(token);
			break;

			// Conditional statements
		case TokenType::CASE:
			CompileTokenCase(token);
			break;
		case TokenType::IF:
			CompileTokenIf(token);
			break;
		case TokenType::REPEAT:
			CompileTokenRepeat(token);
			break;
		case TokenType::WHILE:
			CompileTokenWhile(token);
			break;

			// Literals
		case TokenType::LITERAL_INTEGER:
		case TokenType::LITERAL_FLOAT:
		case TokenType::LITERAL_STRING:
			CompileTokenLiteral(token);
			break;

			// Assignment & swap
		case TokenType::ASSIGNMENT:
			CompileTokenAssignment(token);
			break;
		case TokenType::SWAP:
			CompileTokenSwap(token);
			break;

			// Variables and constants
		case TokenType::CONST:
			CompileTokenConst(token);
			break;
		case TokenType::CONSTANT:
			CompileTokenConstant(token);
			break;
		case TokenType::VARIABLE:
			CompileTokenVariable(token);
			break;

			// Sub-expressions
		case TokenType::SUB_EXPRESSION:
			for (auto& ctoken: token->stack)
				CompileToken(ctoken);
			break;

		case TokenType::DEF:
			CompileTokenDef(token);
			break;
		case TokenType::RETURN:
			CompileTokenReturn(token);
			break;
		case TokenType::PROC_CALL:
			CompileTokenProcCall(token, false);
			break;
		case TokenType::PROC_CALL_EXPR:
			CompileTokenProcCall(token, true);
			break;

			// Unknown?
		default: {
			// Generic function?
			// Save return type
			for (auto& type: token->return_types) {
				if (type.IsInteger()) {
					AddIR(IROpcodes::StackReturnIntArgument);
					type_stack.push(ValueType::Integer);
				}
				else if (type.IsFloat()) {
					AddIR(IROpcodes::StackReturnFloatArgument);
					type_stack.push(ValueType::Float);
				}
				else if (type.IsString()) {
					AddIR(IROpcodes::StackReturnStringArgument);
					type_stack.push(ValueType::String);
				}
			}

			// Check each expression in turn for type
			int i = 0;
			for (auto type = token->required_types.rbegin(); type!=token->required_types.rend(); ++type) {
				for (auto& ctoken: token->expressions[i])
					CompileToken(ctoken);

				auto t = PopType(token);
				if (type->IsInteger()) {
					EnsureStackIsInteger(token, t);
				}
				else if (type->IsFloat()) {
					EnsureStackIsFloat(token, t);
				}
				else if (type->IsString()) {
					if (t!=ValueType::String)
						TypeError(token);
				}
				//type_stack.push(t);
				i++;
			}

			// Set arguments
			for (auto& type: token->required_types) {
				if (type.IsInteger()) {
					AddIR(IROpcodes::ArgumentInteger);
				}
				else if (type.IsFloat()) {
					AddIR(IROpcodes::ArgumentFloat);
				}
				else if (type.IsString()) {
					AddIR(IROpcodes::ArgumentString);
				}
			}
			AddIRWithAddress(IROpcodes::CallFunc, token->func, token->name);
			break;
		}
	}
}

void IRCompiler::Compile(std::list<Token*>* tokens)
{
	// Do forward lookups
	for (auto& token: *tokens)
		ForwardLookups(token);

	for (auto& token: *tokens) {
		CompileToken(token);
	}

	// Split global (local init)
	ir_global.splice(ir_global.begin(), ir_global_init);

	// Optimise
	if (optimise) {
		RunOptimiser(&ir_global);
		RunOptimiser(&ir);
	}
}

void IRCompiler::CheckForForwardLookup(Token* token)
{
	if (token->type==TokenType::DEF) {
		proc_lookup.insert(std::make_pair(token->text, proc_index++));
	}
}

void IRCompiler::ForwardLookups(Token* token)
{
	CheckForForwardLookup(token);
	for (auto& expr: token->expressions)
		for (auto& ctoken: expr)
			ForwardLookups(ctoken);
	for (auto& branch: token->branches)
		for (auto& ctoken: branch)
			ForwardLookups(ctoken);
}

void IRCompiler::Error(Token* token, std::string error)
{
	throw DARICException(ExceptionType::COMPILER, filenames->at(token->file_number), token->line_number, token->char_number, error);
}

void IRCompiler::TypeError(Token* token)
{
	throw DARICException(ExceptionType::COMPILER, filenames->at(token->file_number), token->line_number, token->char_number, "Unexpected type or syntax error");
}

void IRCompiler::Init_AddIR(IROpcodes type)
{
	if (is_global)
		ir_global_init.emplace_back(type, 0, 0, 0.0, "", nullptr);
	else
		current_def_init.emplace_back(type, 0, 0, 0.0, "", nullptr);
}

void IRCompiler::AddIR(IROpcodes type)
{
	if (is_global)
		ir_global.emplace_back(type, 0, 0, 0.0, "", nullptr);
	else
		current_def.emplace_back(type, 0, 0, 0.0, "", nullptr);
}

void IRCompiler::Init_AddIRWithIndex(IROpcodes type, int64_t index)
{
	if (is_global)
		ir_global_init.emplace_back(type, index, 0, 0.0, "", nullptr);
	else
		current_def_init.emplace_back(type, index, 0, 0.0, "", nullptr);
}

void IRCompiler::AddIRWithIndexAndInteger(IROpcodes type, int64_t index, int64_t iv)
{
	if (is_global)
		ir_global.emplace_back(type, index, iv, 0.0, "", nullptr);
	else
		current_def.emplace_back(type, index, iv, 0.0, "", nullptr);
}

void IRCompiler::Init_AddIRWithIndexAndInteger(IROpcodes type, int64_t index, int64_t iv)
{
	if (is_global)
		ir_global_init.emplace_back(type, index, iv, 0.0, "", nullptr);
	else
		current_def_init.emplace_back(type, index, iv, 0.0, "", nullptr);
}

void IRCompiler::AddIRWithIndex(IROpcodes type, int64_t index)
{
	if (is_global)
		ir_global.emplace_back(type, index, 0, 0.0, "", nullptr);
	else
		current_def.emplace_back(type, index, 0, 0.0, "", nullptr);
}

void IRCompiler::AddIRWithIntegerLiteral(IROpcodes type, int64_t v)
{
	if (is_global)
		ir_global.emplace_back(type, 0, v, 0.0, "", nullptr);
	else
		current_def.emplace_back(type, 0, v, 0.0, "", nullptr);
}

void IRCompiler::AddIRWithFloatLiteral(IROpcodes type, double v)
{
	if (is_global)
		ir_global.emplace_back(type, 0, 0, v, "", nullptr);
	else
		current_def.emplace_back(type, 0, 0, v, "", nullptr);
}

void IRCompiler::AddIRWithStringLiteral(IROpcodes type, std::string v)
{
	if (is_global)
		ir_global.emplace_back(type, 0, 0, 0.0, v, nullptr);
	else
		current_def.emplace_back(type, 0, 0, 0.0, v, nullptr);
}

void IRCompiler::AddIRWithStringLiteralWithInteger(IROpcodes type, std::string v, int64_t iv)
{
	if (is_global)
		ir_global.emplace_back(type, iv, 0, 0.0, v, nullptr);
	else
		current_def.emplace_back(type, iv, 0, 0.0, v, nullptr);
}

void IRCompiler::Init_AddIRWithStringLiteralWithIntegerAndIndex(IROpcodes type, std::string v, int64_t iv, int64_t index)
{
	if (is_global)
		ir_global_init.emplace_back(type, index, iv, 0.0, v, nullptr);
	else
		current_def_init.emplace_back(type, index, iv, 0.0, v, nullptr);
}

void IRCompiler::AddIRWithStringLiteralWithIntegerAndIndex(IROpcodes type, std::string v, int64_t iv, int64_t index)
{
	if (is_global)
		ir_global.emplace_back(type, index, iv, 0.0, v, nullptr);
	else
		current_def.emplace_back(type, index, iv, 0.0, v, nullptr);
}

void IRCompiler::AddIRWithAddress(IROpcodes type, void* func, std::string name)
{
	if (is_global)
		ir_global.emplace_back(type, 0, 0, 0.0, name, func);
	else
		current_def.emplace_back(type, 0, 0, 0.0, name, func);
}

void IRCompiler::AddIRIntegerLiteral(int64_t v)
{
	if (is_global) {
		ir_global.emplace_back(IROpcodes::LiteralInteger, 0, v, 0.0, "", nullptr);
		ir_global.emplace_back(IROpcodes::StackPushIntOperand1, 0, v, 0.0, "", nullptr);
	}
	else {
		current_def.emplace_back(IROpcodes::LiteralInteger, 0, v, 0.0, "", nullptr);
		current_def.emplace_back(IROpcodes::StackPushIntOperand1, 0, v, 0.0, "", nullptr);
	}
}

void IRCompiler::AddIRFloatLiteral(double v)
{
	if (is_global) {
		ir_global.emplace_back(IROpcodes::LiteralFloat, 0, 0, v, "", nullptr);
		ir_global.emplace_back(IROpcodes::StackPushFloatOperand1, 0, v, 0.0, "", nullptr);
	}
	else {
		current_def.emplace_back(IROpcodes::LiteralFloat, 0, 0, v, "", nullptr);
		current_def.emplace_back(IROpcodes::StackPushFloatOperand1, 0, v, 0.0, "", nullptr);
	}
}

void IRCompiler::AddIRStringLiteral(std::string v)
{
	if (is_global) {
		ir_global.emplace_back(IROpcodes::LiteralString, 0, 0, 0.0, v, nullptr);
		ir_global.emplace_back(IROpcodes::StackPushStringOperand1, 0, 0, 0.0, "", nullptr);
	}
	else {
		current_def.emplace_back(IROpcodes::LiteralString, 0, 0, 0.0, v, nullptr);
		current_def.emplace_back(IROpcodes::StackPushStringOperand1, 0, 0, 0.0, "", nullptr);
	}
}

void IRCompiler::CheckParamType(Token* token, ValueType wanted_type)
{
	if (type_stack.empty())
		Error(token, "Stack empty, expected value.");
	auto type = PopType(token);
	if (type!=wanted_type) {
		// Conversion?
		if (wanted_type==ValueType::Integer && type==ValueType::Float) {
			AddIR(IROpcodes::StackPopFloatOperand1);
			AddIR(IROpcodes::ConvertOperand1FloatToInt1);
			AddIR(IROpcodes::StackPushIntOperand1);
		}
		else if (wanted_type==ValueType::Float && type==ValueType::Integer) {
			AddIR(IROpcodes::StackPopIntOperand1);
			AddIR(IROpcodes::ConvertOperand1IntToFloat1);
			AddIR(IROpcodes::StackPushFloatOperand1);
		}
		else {
			TypeError(token);
		}
	}
	switch (wanted_type) {
		case ValueType::Integer:
			AddIR(IROpcodes::ArgumentInteger);
			break;
		case ValueType::Float:
			AddIR(IROpcodes::ArgumentFloat);
			break;
		case ValueType::String:
			AddIR(IROpcodes::ArgumentString);
			break;
	}
}

void IRCompiler::CheckParamTypeOnly(Token* token, ValueType wanted_type)
{
	if (type_stack.empty())
		Error(token, "Stack empty, expected value.");
	auto type = PopType(token);
	if (type!=wanted_type) {
		// Conversion?
		if (wanted_type==ValueType::Integer && type==ValueType::Float) {
			AddIR(IROpcodes::StackPopFloatOperand1);
			AddIR(IROpcodes::ConvertOperand1FloatToInt1);
			AddIR(IROpcodes::StackPushIntOperand1);
		}
		else if (wanted_type==ValueType::Float && type==ValueType::Integer) {
			AddIR(IROpcodes::StackPopIntOperand1);
			AddIR(IROpcodes::ConvertOperand1IntToFloat1);
			AddIR(IROpcodes::StackPushFloatOperand1);
		}
		else {
			TypeError(token);
		}
	}
}

void IRCompiler::SetReturnType(Token* token, ValueType type)
{
	switch (type) {
		case ValueType::Integer:
			AddIR(IROpcodes::StackPushIntOperand1);
			type_stack.push(ValueType::Integer);
			break;
		case ValueType::Float:
			AddIR(IROpcodes::StackPushFloatOperand1);
			type_stack.push(ValueType::Float);
			break;
		case ValueType::String:
			AddIR(IROpcodes::StackPushStringOperand1);
			type_stack.push(ValueType::String);
			break;
	}
}