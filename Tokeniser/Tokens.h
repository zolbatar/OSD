#pragma once
#include <string>

enum class TokenType {
	EQUAL = '=',
	LESS = '<',
	GREATER = '>',
	ADD = '+',
	SUBTRACT = '-',
	MULTIPLY = '*',
	DIVIDE = '/',
	COMMA = ',',
	LPARENS = '(',
	RPARENS = ')',
	SEMICOLON = ';',
	TILDE = '~',
	BANG = '!',
	COLON = ':',
	HAT = '^',
	TICK = '\'',

	NONE = 256,
	NEWLINE,
	SUB_EXPRESSION,
	LINE_NUMBER,
	PROC_CALL,
	PROC_CALL_EXPR,
	ASSIGNMENT,
	IDENTIFIER,
	IDENTIFIER_INTEGER,
	IDENTIFIER_FLOAT,
	IDENTIFIER_STRING,
	IDENTIFIER_TYPE,
	FIELD,
	LITERAL_INTEGER,
	LITERAL_FLOAT,
	LITERAL_STRING,
	VARIABLE,
	CONSTANT,
	STATEMENT_START,

	UNARYPLUS,
	UNARYMINUS,

	SHIFT_LEFT,
	SHIFT_RIGHT,

	NOTEQUAL,
	LESSEQUAL,

	GREATEREQUAL,
	ADD_EQUAL,
	SUBTRACT_EQUAL,
	MULTIPLY_EQUAL,
	DIVIDE_INTEGER,
	SHIFT_LEFT_EQUAL,
	SHIFT_RIGHT_EQUAL,

	// CORE
	CASE,
	CONST,
	DATA,
	DATALABEL,
	DEF,
	DIM,
	ELSE,
	END,
	ENDCASE,
	ENDIF,
	ENDDEF,
	ENDTYPE,
	ENDWHILE,
	_FALSE,
	FOR,
	GLOBAL,
	GLOBAL_NOINIT,
//	GOSUB,
//	GOTO,
	INKEY,
	INKEYS,
	INPUT,
	IF,
	LET,
	LOCAL,
	LOCAL_NOINIT,
	NEXT,
	OF,
	OTHERWISE,
	PRINT,
	PRINTNL,
	PRINTTABBED,
	PRINTTABBEDOFF,
	READ,
	REM,
	REPEAT,
	RESTORE,
	RETURN,
	SPC,
	STEP,
	SWAP,
	TAB,
	THEN,
	TO,
	TIME,
	TIMES,
	_TRUE,
	TYPE,
	UNTIL,
	WHEN,
	WHILE,
	YIELD,

	// IO
/*	BGET,
	BPUT,
	CLOSE,
	EOFH,
	OPENIN,
	OPENOUT,
	OPENUP,
	GETSH,*/

	// 2D
	CIRCLE,
	CIRCLEFILLED,
	CLIPON,
	CLIPOFF,
	CLS,
	COLOUR,
	COLOURBG,
//	CREATEFONT,
	FLIP,
//	GRAPHICS,
	LINE,
//	LOADTYPEFACE,
	PLOT,
	RECTANGLE,
	RECTANGLEFILLED,
	SCREENWIDTH,
	SCREENHEIGHT,
	SHADOW,
//	TEXT,
//	TEXTRIGHT,
//	TEXTCENTRE,
	TRIANGLE,
	TRIANGLEFILLED,

	// 3D
/*	VERTEX3D,
	TRIANGLE3D,
	SHAPE3D,
	OBJECT3D,
	SOLID,
	WIREFRAME,
	EDGEDWIREFRAME,
	TRANSLATE3D,
	ROTATE3D,
	SCALE3D,
	RENDER3D,*/

	// String
	ASC,
	CHRS,
	INSTR,
	LEFTS,
	MIDS,
	RIGHTS,
	LEN,
	STRINGS,
	STRS,

	// Boolean
	AND,
	EOR,
	OR,
	NOT,

	// Maths/types
	FLOAT,
	INT,
	ACS,
	DIV,
	MOD,
	SQR,
	LN,
	LOG,
	EXP,
	ATN,
	TAN,
	COS,
	SIN,
	ASN,
	ABS,
	DEG,
	RAD,
	RND,
	RNDF,
	SGN,
	VAL,
	PI
};

