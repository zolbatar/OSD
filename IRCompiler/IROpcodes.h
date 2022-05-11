#pragma once

enum class IROpcodes {
	End,
	Yield,

	// Optimised opcodes
	NOP,
	StackCheck,
	SwapFloatOperands,
	SwapIntegerOperands,
	IntegerOperand1To2,
	IntegerOperand2To1,
	FloatOperand1To2,
	FloatOperand2To1,
	LoadIntegerOperands,
	LoadFloatOperands,

	// Literals
	LiteralInteger,
	LiteralFloat,
	LiteralString,

	// Variables
	VariableLocalCreateInteger,
	VariableLocalCreateFloat,
	VariableLocalCreateString,
    VariableLocalCreateType,
	VariableGlobalCreateInteger,
	VariableGlobalCreateFloat,
	VariableGlobalCreateString,
    VariableGlobalCreateType,
	VariableLoadInteger,
	VariableLoadFloat,
	VariableLoadString,
	VariableStoreInteger,
	VariableStoreFloat,
	VariableStoreString,
    VariableLoadIntegerWithOffset,
    VariableLoadFloatWithOffset,
    VariableLoadStringWithOffset,
    VariableStoreIntegerWithOffset,
    VariableStoreFloatWithOffset,
    VariableStoreStringWithOffset,

	// Swap
	VariableSwapSourceInteger,
	VariableSwapDestinationInteger,
	VariableSwapSourceFloat,
	VariableSwapDestinationFloat,
	VariableSwapSourceString,
	VariableSwapDestinationString,

	// Data
	DataInteger,
	DataFloat,
	DataString,
	DataLabel,
	Restore,
	ReadInteger,
	ReadFloat,
	ReadString,

	// Arguments
	ArgumentInteger,
	ArgumentFloat,
	ArgumentString,
	StackReturnIntArgument,
	StackReturnFloatArgument,
	StackReturnStringArgument,
	CallFunc,

	// Stack
	StackPopIntOperand1,
	StackPopIntOperand2,
	StackPushIntOperand1,
	StackPushIntOperand2,
	StackPopFloatOperand1,
	StackPopFloatOperand2,
	StackPushFloatOperand1,
	StackPushFloatOperand2,
	StackPopStringOperand1,
	StackPopStringOperand2,
	StackPushStringOperand1,

	// Conversion
	ConvertOperand1IntToFloat1,
	ConvertOperand2IntToFloat2,
	ConvertOperand1FloatToInt1,
	ConvertOperand2FloatToInt2,
	ConvertOperand1StringToInt1,
	ConvertOperand1StringToFloat1,
	ConvertOperand1IntToString1,
	ConvertOperand1FloatToString1,

	// Conditional
	JumpCreate,
	JumpCreateForward,
	JumpDestination,
	Jump,
	JumpForward,
	JumpOnConditionTrue,
	JumpOnConditionFalse,
	JumpOnConditionTrueForward,
	JumpOnConditionFalseForward,

	// Comparison
	CompareEqualInt,
	CompareEqualFloat,
	CompareEqualString,
	CompareNotEqualInt,
	CompareNotEqualFloat,
	CompareNotEqualString,
	CompareLessInt,
	CompareLessFloat,
	CompareLessString,
	CompareLessEqualInt,
	CompareLessEqualFloat,
	CompareLessEqualString,
	CompareGreaterInt,
	CompareGreaterFloat,
	CompareGreaterString,
	CompareGreaterEqualInt,
	CompareGreaterEqualFloat,
	CompareGreaterEqualString,

	// Strings
	StringsAsc,
	StringsChrs,
	StringsInstr,
	StringsLefts,
	StringsMids,
	StringsRights,
	StringsLen,
	StringsStrings,

	// Maths
	MathsAddInt,
	MathsAddString,
	MathsAddFloat,
	MathsSubtractInt,
	MathsMultiplyInt,
	MathsSubtractFloat,
	MathsMultiplyFloat,
	MathsShiftLeft,
	MathsShiftRight,
	MathsDivide,
	MathsPower,
	MathsMod,
	MathsDiv,
	MathsAbsInt,
	MathsAbsFloat,
	MathsSqr,
	MathsLn,
	MathsLog,
	MathsExp,
	MathsSin,
	MathsAsn,
	MathsCos,
	MathsAcs,
	MathsTan,
	MathsAtn,
	MathsRad,
	MathsDeg,
	MathsSgn,

	// Boolean
	BooleanNot,
	BooleanAnd,
	BooleanOr,
	BooleanEor,

	// PROC
	PROCCall,
	PROCStart,
	PROCEnd,
	ReturnNone,
	ReturnInteger,
	ReturnFloat,
	ReturnString,
	ParameterInt,
	ParameterFloat,
	ParameterString,
};