#include "IRCompiler.h"
#include <cstring>

#ifndef CLION
#include <circle/timer.h>
extern CTimer *timer;
#endif

void IRCompiler::IRPrinter(std::list<std::string>* irl)
{
	irl->push_back("** GLOBAL **");
	IRPrinterSection(irl, &ir_global);
	irl->push_back("");
	irl->push_back("** PROCs **");
	IRPrinterSection(irl, &ir);
}

bool replace(std::string& str, const std::string& from, const std::string& to);

void IRCompiler::IRPrintValueBased(char* l, IROpcodes o, IROpcodes in, IROpcodes fl, IROpcodes st, std::string fmt, ...)
{
	if (o==in)
		replace(fmt, "@", "integer");
	else if (o==fl)
		replace(fmt, "@", "float");
	else
		replace(fmt, "@", "string");

	va_list var;
	va_start(var, fmt);
	sprintf(l, fmt.c_str(), va_arg(var, int64_t), va_arg(var, int64_t));
	va_end(var);
}

void IRCompiler::IRPrinterSection(std::list<std::string>* irl, std::list<IRInstruction>* ir)
{
	char l[256];
	for (auto& instruction: *ir) {
		switch (instruction.type) {
			case IROpcodes::End:
				sprintf(l, "[General ] End");
				break;
			case IROpcodes::Yield:
				sprintf(l, "[General ] Yield");
				break;

				// Optimisation
			case IROpcodes::NOP:
				sprintf(l, "");
				// Show nothing
				break;
			case IROpcodes::StackCheck:
				sprintf(l, "[Stack   ] Stack check");
				break;
			case IROpcodes::SwapIntegerOperands:
				sprintf(l, "[Operand ] Swap integer operands");
				break;
			case IROpcodes::SwapFloatOperands:
				sprintf(l, "[Operand ] Swap float operands");
				break;
			case IROpcodes::IntegerOperand1To2:
				sprintf(l, "[Operand ] Integer operand 1 to 2");
				break;
			case IROpcodes::IntegerOperand2To1:
				sprintf(l, "[Operand ] Integer operand 2 to 1");
				break;
			case IROpcodes::FloatOperand1To2:
				sprintf(l, "[Operand ] Float operand 1 to 2");
				break;
			case IROpcodes::FloatOperand2To1:
				sprintf(l, "[Operand ] Float operand 2 to 1");
				break;
			case IROpcodes::LoadIntegerOperands:
				sprintf(l, "[Variable] Load operands from integer variables");
				break;
			case IROpcodes::LoadFloatOperands:
				sprintf(l, "[Variable] Load operands from float variables");
				break;

				// Variables
			case IROpcodes::VariableLocalCreateInteger:
			case IROpcodes::VariableLocalCreateFloat:
			case IROpcodes::VariableLocalCreateString:
				IRPrintValueBased(l, instruction.type, IROpcodes::VariableLocalCreateInteger,
						IROpcodes::VariableLocalCreateFloat,
						IROpcodes::VariableLocalCreateString,
						"[Variable] Local create @ (%lld)", instruction.index);
				break;
			case IROpcodes::VariableLocalCreateType:
				sprintf(l, "[Variable] Local create structured type of %lld bytes (%lld)", instruction.iv,
						instruction.index);
			case IROpcodes::VariableGlobalCreateInteger:
			case IROpcodes::VariableGlobalCreateFloat:
			case IROpcodes::VariableGlobalCreateString:
				IRPrintValueBased(l, instruction.type, IROpcodes::VariableGlobalCreateInteger,
						IROpcodes::VariableGlobalCreateFloat,
						IROpcodes::VariableGlobalCreateString,
						"[Variable] Global create @ (%lld)", instruction.index);
				break;
			case IROpcodes::VariableGlobalCreateType:
				sprintf(l, "[Variable] Global create structured type of %lld bytes (%lld)", instruction.iv,
						instruction.index);
				break;
			case IROpcodes::VariableLoadInteger:
			case IROpcodes::VariableLoadFloat:
			case IROpcodes::VariableLoadString:
				IRPrintValueBased(l, instruction.type, IROpcodes::VariableLoadInteger, IROpcodes::VariableLoadFloat,
						IROpcodes::VariableLoadString,
						"[Variable] Load @ (%lld)", instruction.index);
				break;
			case IROpcodes::VariableStoreInteger:
			case IROpcodes::VariableStoreFloat:
			case IROpcodes::VariableStoreString:
				IRPrintValueBased(l, instruction.type, IROpcodes::VariableStoreInteger, IROpcodes::VariableStoreFloat,
						IROpcodes::VariableStoreString,
						"[Variable] Store @ (%lld)", instruction.index);
				break;
			case IROpcodes::VariableLoadIntegerWithOffset:
			case IROpcodes::VariableLoadFloatWithOffset:
			case IROpcodes::VariableLoadStringWithOffset:
				IRPrintValueBased(l, instruction.type, IROpcodes::VariableLoadIntegerWithOffset, IROpcodes::VariableLoadFloatWithOffset,
						IROpcodes::VariableLoadStringWithOffset,
						"[Variable] Load offset @ (%lld/%lld)", instruction.index, instruction.iv);
				break;
			case IROpcodes::VariableStoreIntegerWithOffset:
			case IROpcodes::VariableStoreFloatWithOffset:
			case IROpcodes::VariableStoreStringWithOffset:
				IRPrintValueBased(l, instruction.type, IROpcodes::VariableStoreIntegerWithOffset, IROpcodes::VariableStoreFloatWithOffset,
						IROpcodes::VariableStoreStringWithOffset,
						"[Variable] Store offset @ (%lld/%lld)", instruction.index, instruction.iv);
				break;

				// Swap
			case IROpcodes::VariableSwapSourceInteger:
			case IROpcodes::VariableSwapSourceFloat:
			case IROpcodes::VariableSwapSourceString:
				IRPrintValueBased(l, instruction.type, IROpcodes::VariableSwapSourceInteger,
						IROpcodes::VariableSwapSourceFloat,
						IROpcodes::VariableSwapSourceString,
						"[Variable] Swap source @ (%lld)", instruction.index);
				break;
			case IROpcodes::VariableSwapDestinationInteger:
			case IROpcodes::VariableSwapDestinationFloat:
			case IROpcodes::VariableSwapDestinationString:
				IRPrintValueBased(l, instruction.type, IROpcodes::VariableSwapDestinationInteger,
						IROpcodes::VariableSwapDestinationFloat,
						IROpcodes::VariableSwapDestinationString,
						"[Variable] Swap destination @ (%lld)", instruction.index);
				break;

				// Arguments
			case IROpcodes::StackReturnIntArgument:
			case IROpcodes::StackReturnFloatArgument:
			case IROpcodes::StackReturnStringArgument:
				IRPrintValueBased(l, instruction.type, IROpcodes::StackReturnIntArgument,
						IROpcodes::StackReturnFloatArgument,
						IROpcodes::StackReturnFloatArgument,
						"[Argument] Return type is @");
				break;
			case IROpcodes::ArgumentInteger:
			case IROpcodes::ArgumentFloat:
			case IROpcodes::ArgumentString:
				IRPrintValueBased(l, instruction.type, IROpcodes::ArgumentInteger, IROpcodes::ArgumentFloat,
						IROpcodes::ArgumentString,
						"[Argument] Parameter is @");
				break;

				// Comparison
			case IROpcodes::CompareEqualInt:
			case IROpcodes::CompareEqualFloat:
			case IROpcodes::CompareEqualString:
				IRPrintValueBased(l, instruction.type, IROpcodes::CompareEqualInt, IROpcodes::CompareEqualFloat,
						IROpcodes::CompareEqualString,
						"[Variable] Compare = @");
				break;
			case IROpcodes::CompareNotEqualInt:
			case IROpcodes::CompareNotEqualFloat:
			case IROpcodes::CompareNotEqualString:
				IRPrintValueBased(l, instruction.type, IROpcodes::CompareNotEqualInt, IROpcodes::CompareNotEqualFloat,
						IROpcodes::CompareNotEqualString,
						"[Variable] Compare <> @");
				break;
			case IROpcodes::CompareLessInt:
			case IROpcodes::CompareLessFloat:
			case IROpcodes::CompareLessString:
				IRPrintValueBased(l, instruction.type, IROpcodes::CompareLessInt, IROpcodes::CompareLessFloat,
						IROpcodes::CompareLessString,
						"[Variable] Compare < @");
				break;
			case IROpcodes::CompareLessEqualInt:
			case IROpcodes::CompareLessEqualFloat:
			case IROpcodes::CompareLessEqualString:
				IRPrintValueBased(l, instruction.type, IROpcodes::CompareLessEqualInt, IROpcodes::CompareLessEqualFloat,
						IROpcodes::CompareLessEqualString,
						"[Variable] Compare <= @");
				break;
			case IROpcodes::CompareGreaterInt:
			case IROpcodes::CompareGreaterFloat:
			case IROpcodes::CompareGreaterString:
				IRPrintValueBased(l, instruction.type, IROpcodes::CompareGreaterInt, IROpcodes::CompareGreaterFloat,
						IROpcodes::CompareGreaterString,
						"[Variable] Compare > @");
				break;
			case IROpcodes::CompareGreaterEqualInt:
			case IROpcodes::CompareGreaterEqualFloat:
			case IROpcodes::CompareGreaterEqualString:
				sprintf(l, "[Compare ] Compare >= string");
				IRPrintValueBased(l, instruction.type, IROpcodes::CompareGreaterEqualInt,
						IROpcodes::CompareGreaterEqualFloat,
						IROpcodes::CompareGreaterEqualString,
						"[Variable] Compare >= @");
				break;

				// Literal
			case IROpcodes::LiteralInteger:
				sprintf(l, "[Stack   ] Literal integer %lld", instruction.iv);
				break;
			case IROpcodes::LiteralFloat:
				sprintf(l, "[Stack   ] Literal float %f", instruction.rv);
				break;
			case IROpcodes::LiteralString:
				sprintf(l, "[Stack   ] Literal string '%s'", instruction.sv.c_str());
				break;

				// Stack
			case IROpcodes::StackPopIntOperand1:
				sprintf(l, "[Stack   ] Pop integer to Operand1");
				break;
			case IROpcodes::StackPopIntOperand2:
				sprintf(l, "[Stack   ] Pop integer to Operand2");
				break;
			case IROpcodes::StackPushIntOperand1:
				sprintf(l, "[Stack   ] Push integer Operand1");
				break;
			case IROpcodes::StackPushIntOperand2:
				sprintf(l, "[Stack   ] Push integer Operand2");
				break;
			case IROpcodes::StackPopFloatOperand1:
				sprintf(l, "[Stack   ] Pop float to Operand1");
				break;
			case IROpcodes::StackPopFloatOperand2:
				sprintf(l, "[Stack   ] Pop float to Operand2");
				break;
			case IROpcodes::StackPushFloatOperand1:
				sprintf(l, "[Stack   ] Push float Operand1");
				break;
			case IROpcodes::StackPushFloatOperand2:
				sprintf(l, "[Stack   ] Push float Operand2");
				break;
			case IROpcodes::StackPopStringOperand1:
				sprintf(l, "[Stack   ] Pop string Operand1");
				break;
			case IROpcodes::StackPopStringOperand2:
				sprintf(l, "[Stack   ] Pop string Operand2");
				break;
			case IROpcodes::StackPushStringOperand1:
				sprintf(l, "[Stack   ] Push string Operand1");
				break;

				// Conversion
			case IROpcodes::ConvertOperand1IntToFloat1:
				sprintf(l, "[Convert ] Operand1, integer to float");
				break;
			case IROpcodes::ConvertOperand2IntToFloat2:
				sprintf(l, "[Convert ] Operand2, integer to float");
				break;
			case IROpcodes::ConvertOperand1FloatToInt1:
				sprintf(l, "[Convert ] Operand1, float to integer");
				break;
			case IROpcodes::ConvertOperand2FloatToInt2:
				sprintf(l, "[Convert ] Operand2, float to integer");
				break;
			case IROpcodes::ConvertOperand1IntToString1:
				sprintf(l, "[Convert ] Operand1, int to string");
				break;
			case IROpcodes::ConvertOperand1FloatToString1:
				sprintf(l, "[Convert ] Operand1, float to string");
				break;
			case IROpcodes::ConvertOperand1StringToInt1:
				sprintf(l, "[Convert ] Operand1, string to integer");
				break;
			case IROpcodes::ConvertOperand1StringToFloat1:
				sprintf(l, "[Convert ] Operand1, string to integer");
				break;

				// Maths
			case IROpcodes::StringsStrings:
				sprintf(l, "[Strings ] STRING$");
				break;
			case IROpcodes::StringsRights:
				sprintf(l, "[Strings ] RIGHT$");
				break;
			case IROpcodes::StringsMids:
				sprintf(l, "[Strings ] MID$");
				break;
			case IROpcodes::StringsLefts:
				sprintf(l, "[Strings ] LEFT$");
				break;
			case IROpcodes::StringsInstr:
				sprintf(l, "[Strings ] INSTR");
				break;
			case IROpcodes::StringsChrs:
				sprintf(l, "[Strings ] CHR$");
				break;
			case IROpcodes::StringsAsc:
				sprintf(l, "[Strings ] ASC");
				break;
			case IROpcodes::StringsLen:
				sprintf(l, "[Strings ] LEN");
				break;

				// Maths
			case IROpcodes::MathsAddString:
				sprintf(l, "[Maths   ] +, string");
				break;
			case IROpcodes::MathsAddInt:
				sprintf(l, "[Maths   ] +, integer");
				break;
			case IROpcodes::MathsSubtractInt:
				sprintf(l, "[Maths   ] -, integer");
				break;
			case IROpcodes::MathsMultiplyInt:
				sprintf(l, "[Maths   ] *, integer");
				break;
			case IROpcodes::MathsAddFloat:
				sprintf(l, "[Maths   ] +, float");
				break;
			case IROpcodes::MathsSubtractFloat:
				sprintf(l, "[Maths   ] -, float");
				break;
			case IROpcodes::MathsMultiplyFloat:
				sprintf(l, "[Maths   ] *, float");
				break;
			case IROpcodes::MathsDivide:
				sprintf(l, "[Maths   ] /, float");
				break;
			case IROpcodes::MathsShiftLeft:
				sprintf(l, "[Maths   ] <<, float");
				break;
			case IROpcodes::MathsShiftRight:
				sprintf(l, "[Maths   ] >>, float");
				break;
			case IROpcodes::MathsPower:
				sprintf(l, "[Maths   ] ^, float");
				break;
			case IROpcodes::MathsMod:
				sprintf(l, "[Maths   ] MOD, float");
				break;
			case IROpcodes::MathsDiv:
				sprintf(l, "[Maths   ] DIV, float");
				break;
			case IROpcodes::MathsAbsInt:
				sprintf(l, "[Maths   ] ABS, int");
				break;
			case IROpcodes::MathsAbsFloat:
				sprintf(l, "[Maths   ] ABS, float");
				break;
			case IROpcodes::MathsSqr:
				sprintf(l, "[Maths   ] SQR, float");
				break;
			case IROpcodes::MathsLn:
				sprintf(l, "[Maths   ] LN, float");
				break;
			case IROpcodes::MathsLog:
				sprintf(l, "[Maths   ] LOG, float");
				break;
			case IROpcodes::MathsExp:
				sprintf(l, "[Maths   ] LOG, float");
				break;
			case IROpcodes::MathsSin:
				sprintf(l, "[Maths   ] SIN, float");
				break;
			case IROpcodes::MathsAsn:
				sprintf(l, "[Maths   ] ASN, float");
				break;
			case IROpcodes::MathsCos:
				sprintf(l, "[Maths   ] COS, float");
				break;
			case IROpcodes::MathsAcs:
				sprintf(l, "[Maths   ] ACS, float");
				break;
			case IROpcodes::MathsTan:
				sprintf(l, "[Maths   ] TAN, float");
				break;
			case IROpcodes::MathsAtn:
				sprintf(l, "[Maths   ] ATN, float");
				break;
			case IROpcodes::MathsRad:
				sprintf(l, "[Maths   ] RAD, float");
				break;
			case IROpcodes::MathsDeg:
				sprintf(l, "[Maths   ] DEG, float");
				break;
			case IROpcodes::MathsSgn:
				sprintf(l, "[Maths   ] SGN, Float, returns int");
				break;

				// Boolean
			case IROpcodes::BooleanNot:
				sprintf(l, "[Boolean ] NOT, integer");
				break;
			case IROpcodes::BooleanAnd:
				sprintf(l, "[Boolean ] AND, integer");
				break;
			case IROpcodes::BooleanOr:
				sprintf(l, "[Boolean ] OR, integer");
				break;
			case IROpcodes::BooleanEor:
				sprintf(l, "[Boolean ] EOR, integer");
				break;

				// Jumps
			case IROpcodes::Jump:
				sprintf(l, "[Branch  ] Jump: (%lld)", instruction.index);
				break;
			case IROpcodes::JumpForward:
				sprintf(l, "[Branch  ] Jump (forward): (%lld)", instruction.index);
				break;
			case IROpcodes::JumpCreate:
				sprintf(l, "[Branch  ] Jump create: (%lld)", instruction.index);
				break;
			case IROpcodes::JumpCreateForward:
				sprintf(l, "[Branch  ] Jump create (forward): (%lld)", instruction.index);
				break;
			case IROpcodes::JumpDestination:
				sprintf(l, "[Branch  ] Jump destination: (%lld)", instruction.index);
				break;
			case IROpcodes::JumpOnConditionTrue:
				sprintf(l, "[Branch  ] Jump on true: (%lld)", instruction.index);
				break;
			case IROpcodes::JumpOnConditionFalse:
				sprintf(l, "[Branch  ] Jump on false: (%lld)", instruction.index);
				break;
			case IROpcodes::JumpOnConditionTrueForward:
				sprintf(l, "[Branch  ] Jump on true (forward): (%lld)", instruction.index);
				break;
			case IROpcodes::JumpOnConditionFalseForward:
				sprintf(l, "[Branch  ] Jump on false (forward): (%lld)", instruction.index);
				break;

				// PROCs
			case IROpcodes::PROCCall:
				sprintf(l, "[Proc    ] PROC call: (%lld/%s)", instruction.index, instruction.sv.c_str());
				break;
			case IROpcodes::PROCStart:
				sprintf(l, "[Proc    ] PROC start: (%lld/%s)", instruction.index, instruction.sv.c_str());
				break;
			case IROpcodes::PROCEnd:
				sprintf(l, "[Proc    ] PROC end: (%lld)", instruction.index);
				break;
			case IROpcodes::ReturnNone:
				sprintf(l, "[Proc    ] Return none, final:%lld", instruction.index);
				break;
			case IROpcodes::ReturnInteger:
				sprintf(l, "[Proc    ] Return integer, final:%lld", instruction.index);
				break;
			case IROpcodes::ReturnFloat:
				sprintf(l, "[Proc    ] Return float, final:%lld", instruction.index);
				break;
			case IROpcodes::ReturnString:
				sprintf(l, "[Proc    ] Return string, final:%lld", instruction.index);
				break;
			case IROpcodes::ParameterInt:
				sprintf(l, "[Proc    ] Parameter integer, %lld", instruction.index);
				break;
			case IROpcodes::ParameterFloat:
				sprintf(l, "[Proc    ] Parameter float, %lld", instruction.index);
				break;
			case IROpcodes::ParameterString:
				sprintf(l, "[Proc    ] Parameter string, %lld", instruction.index);
				break;
			case IROpcodes::CallFunc:
				// Can we look it up?
				sprintf(l, "[Func    ] Call function: (%p)", instruction.func);
				break;

				// DATA
			case IROpcodes::DataInteger:
				sprintf(l, "[Data    ] DATA integer");
				break;
			case IROpcodes::DataFloat:
				sprintf(l, "[Data    ] DATA float");
				break;
			case IROpcodes::DataString:
				sprintf(l, "[Data    ] DATA string");
				break;
			case IROpcodes::ReadInteger:
				sprintf(l, "[Data    ] READ integer");
				break;
			case IROpcodes::ReadFloat:
				sprintf(l, "[Data    ] READ float");
				break;
			case IROpcodes::ReadString:
				sprintf(l, "[Data    ] READ string");
				break;
			case IROpcodes::DataLabel:
				sprintf(l, "[Data    ] DATA label");
				break;
			case IROpcodes::Restore:
				sprintf(l, "[Stack   ] RESTORE");
				break;

			default:
				assert(false);
		}
		if (strlen(l)>0)
			irl->push_back(std::string(l));
	}
}