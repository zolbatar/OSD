#pragma once
#include <string>

const std::string DARIC_tester = R"(CONST const%=100,ccf#=12.5

REM STATIC s%
TYPE VertexTest
	x#,y#,z#,rgb%
ENDTYPE
GVT! = VertexTest(10.0,234.56)
GVT!rgb%=&FFFFFF

DATALABEL "Start"
DATA 100, "Fred", 15.5
DATALABEL "Duff"
DATA 150
DATALABEL "Label"
DATA 200
G%=5

Start()

DEF Start()
	CLS
    PRINT "Starting tests!"
    Boolean()
	Chrono()
    Comparison()
    Conversion()
	Data()
	Functions()
    GlobalsLocals()
	Looping()
    Maths()
    Numbers()
    PrintInput()
    Strings()
	Swap()
	Types()
	Variables()
    PRINT "::Misc tests::"
	TestReturnInOtherwise()
	NestedCalling()
    Containers()
    PRINT "All tests passed!"
ENDDEF

DEF Boolean()
    PRINT "::Boolean::"
    PRINT "Or                 = ";:IF ((&F0 OR &F0) = &F0) AND ((&F0 OR &0F) = &FF) Pass() ELSE Fail()
    PRINT "Eor                = ";:IF ((1 EOR 1) = 0) AND ((1 EOR 0) = 1) AND ((0 EOR 0) = 0) Pass() ELSE Fail()
    PRINT "And                = ";:IF ((&F0 AND &F0) = &F0) AND ((&F0 AND &0F) = &0) Pass() ELSE Fail()
    PRINT "Not                = ";:IF NOT 1 = -2 Pass() ELSE Fail()
    PRINT "True               = ";:IF TRUE Pass() ELSE Fail()
    PRINT "False              = ";:IF NOT FALSE Pass() ELSE Fail()
    PRINT
ENDDEF

DEF Chrono()
	PRINT "::Chrono::"
	PRINT "Time               = ";:IF TIME > 0 Pass() ELSE Fail()
PRINT TIME$
	PRINT "Time$              = ";:IF LEN(TIME$) = 24 Pass() ELSE Fail()
	PRINT
ENDDEF

DEF Comparison()
    PRINT "::Comparison::"
    PRINT "Int =              = ";:IF 5 = 5 Pass() ELSE Fail()
    PRINT "Int <>             = ";:IF 5 <> 6 Pass() ELSE Fail()
    PRINT "Int <              = ";:IF 5 < 10 Pass() ELSE Fail()
    PRINT "Int <= (<)         = ";:IF 4 <= 5 Pass() ELSE Fail()
    PRINT "Int <= (=)         = ";:IF 5 <= 5 Pass() ELSE Fail()
    PRINT "Int >              = ";:IF 10 > 5 Pass() ELSE Fail()
    PRINT "Int >= (>)         = ";:IF 10 >= 5 Pass() ELSE Fail()
    PRINT "Int >= (>=)        = ";:IF 5 >= 5 Pass() ELSE Fail()
    PRINT "Float =            = ";:IF 5.0 = 5.0 Pass() ELSE Fail()
    PRINT "Float <>           = ";:IF 5.0 <> 6.0 Pass() ELSE Fail()
    PRINT "Float <            = ";:IF 5.0 < 10.0 Pass() ELSE Fail()
    PRINT "Float <= (<)       = ";:IF 4.0 <= 5.0 Pass() ELSE Fail()
    PRINT "Float <= (=)       = ";:IF 5.0 <= 5.0 Pass() ELSE Fail()
    PRINT "Float >            = ";:IF 10.0 > 5.0 Pass() ELSE Fail()
    PRINT "Float >= (>)       = ";:IF 10.0 >= 5.0 Pass() ELSE Fail()
    PRINT "Float >= (>=)      = ";:IF 5.0 >= 5.0 Pass() ELSE Fail()
    PRINT "String =           = ";:IF "Fred" = "Fred" Pass() ELSE Fail()
    PRINT "String <>          = ";:IF "Fred" <> "fred" Pass() ELSE Fail()
    PRINT "String <           = ";:IF "Fred" < "fred" Pass() ELSE Fail()
    PRINT "String <= (<)      = ";:IF "Fred" <= "fred" Pass() ELSE Fail()
    PRINT "String <= (=)      = ";:IF "Fred" <= "Fred" Pass() ELSE Fail()
    PRINT "String >           = ";:IF "fred" > "Fred" Pass() ELSE Fail()
    PRINT "String >= (>)      = ";:IF "fred" >= "Fred" Pass() ELSE Fail()
    PRINT "String >= (>=)     = ";:IF "Fred" >= "Fred" Pass() ELSE Fail()
    PRINT
ENDDEF

DEF Conversion()
    PRINT "::Conversion::"
    PRINT "10 + 15.5          = ";:IF 10 + 15.5 = 25.5 Pass() ELSE Fail()
    PRINT
ENDDEF

DEF Data()
	PRINT "::Data::"
	READ d1%, d2$, d3#
	PRINT "Data (Int)         = ";:IF d1% = 100 Pass() ELSE Fail()
	PRINT "Data (Float)       = ";:IF d3# = 15.5 Pass() ELSE Fail()
	PRINT "Data (String)      = ";:IF d2$ = "Fred" Pass() ELSE Fail()
	RESTORE "Label"
	READ d1%
	PRINT "Data+Restore       = ";:IF d1% = 200 Pass() ELSE Fail()
	PRINT
ENDDEF

DEF Functions()
	PRINT "::Functions::"
	ReturnValues()
 	SendParameter(15+10,123.1,"Test")
	PRINT
ENDDEF

DEF SendParameter(A%,B#,C$)
	PRINT "Send parameter%    = ";:IF A% = 25 Pass() ELSE Fail()
	PRINT "Send parameter#    = ";:IF B# = 123.1 Pass() ELSE Fail()
	PRINT "Send parameter$    = ";:IF C$ = "Test" Pass() ELSE Fail()
ENDDEF

DEF GlobalsLocals()
	PRINT "::Locals & Globals::"
	LocalVars()
	PRINT "Preserving Global  = ";:IF G% = 5 Pass() ELSE Fail()
	PRINT
ENDDEF

DEF LocalVars()
 	LOCAL G% = 10
ENDDEF

DEF Looping()
	PRINT "::Looping::"
	PRINT "One line IF        = ";:IF 1 = 1 Pass() ELSE Fail()
	PRINT "Multiline IF       = ";
	IF 1 = 1 THEN
		Pass()
	ELSE
		Fail()
	ENDIF
	I% = 0
	REPEAT
		I% = I% + 1
	UNTIL I% = 10
	PRINT "Repeat             = ";:IF I% = 10 Pass() ELSE Fail()
	I% = 0
	WHILE I% < 10
		I% = I% + 1
	ENDWHILE
	PRINT "While              = ";:IF I% = 10 Pass() ELSE Fail()
	I% = 0
	FOR J% = 0 TO 9
		I% = I% + 1
	NEXT
	PRINT "FOR (Int)          = ";:IF I% = 10 Pass() ELSE Fail()
	I% = 0
	FOR J% = 0 TO 9 STEP 2
		I% = I% + 1
	NEXT
	PRINT "FOR + Step (Int)   = ";:IF I% = 5 Pass() ELSE Fail()
	I% = 0
	FOR J% = 9 TO 0 STEP -1
		I% = I% + 1
	NEXT
	PRINT "FOR - (Int)        = ";:IF I% = 10 Pass() ELSE Fail()
	I% = 0
	FOR J# = 0.0 TO 9.0
		I% = I% + 1
	NEXT
	PRINT "FOR (Float)        = ";:IF I% = 10 Pass() ELSE Fail()
	I% = 0
	FOR J# = 0.0 TO 9.0 STEP 2.0
		I% = I% + 1
	NEXT
	PRINT "FOR + Step (Float) = ";:IF I% = 5 Pass() ELSE Fail()
	PRINT "Case               = ";
	I%=1
	CASE I% OF
		WHEN 0, 1, 2: Pass()
		WHEN 3: Fail()
		OTHERWISE Fail()
	ENDCASE
	PRINT "Case (not first)   = ";
	I%=2
	CASE I% OF
		WHEN 0,1: Fail()
		WHEN 2: Pass()
		OTHERWISE Fail()
	ENDCASE
	PRINT "Case (Otherwise)   = ";
	I%=4
	CASE I% OF
		WHEN 0, 1, 2: Fail()
		WHEN 3: Fail()
		OTHERWISE Pass()
	ENDCASE
	PRINT "Case (String)      = ";
	A$="B"
	CASE A$ OF
 		WHEN "A":Fail()
	 	WHEN "B":Pass()
		OTHERWISE Fail()
	ENDCASE
ENDDEF

DEF Maths()
    PRINT "::Maths::"
    PRINT "- (Unary)          = ";:IF 2 - -2 = 4 Pass() ELSE Fail()
    PRINT "- (Int)            = ";:IF 10 - 8 = 2 Pass() ELSE Fail()
    PRINT "+ (Int)            = ";:IF 10 + 8 = 18 Pass() ELSE Fail()
    PRINT "* (Int)            = ";:IF 10 * 8 = 80 Pass() ELSE Fail()
    PRINT "^ (Int)            = ";:IF 2 ^ 3 = 8 Pass() ELSE Fail()
    PRINT "Mod (Int)          = ";:IF 11 MOD 5 = 1 Pass() ELSE Fail()
    PRINT "Div (Int)          = ";:IF 11 DIV 5 = 2 Pass() ELSE Fail()
    PRINT "Abs (Int)          = ";:IF ABS(-10) = 10 Pass() ELSE Fail()
    PRINT "- (Float)          = ";:IF 10.5 - 8.25 = 2.25 Pass() ELSE Fail()
    PRINT "+ (Float)          = ";:IF 10.4 + 8.1 = 18.5 Pass() ELSE Fail()
    PRINT "* (Float)          = ";:IF 2.5 * 4.5 = 11.25 Pass() ELSE Fail()
    PRINT "/ (Float)          = ";:IF 8.4 / 4.2 = 2.0 Pass() ELSE Fail()
    PRINT "<< (Shift left)    = ";:IF 60 << 2 = 240 Pass() ELSE Fail()
    PRINT ">> (Shift right)   = ";:IF 200 >> 2 = 50 Pass() ELSE Fail()
    PRINT "Mod (Float)        = ";:IF 11.0 MOD 5.0 = 1 Pass() ELSE Fail()
    PRINT "Div (Float)        = ";:IF 11.0 DIV 5.0 = 2 Pass() ELSE Fail()
    PRINT "Abs (Float)        = ";:IF ABS(-10.5) = 10.5 Pass() ELSE Fail()
    PRINT "Sqr                = ";:IF SQR(16) = 4 Pass() ELSE Fail()
    PRINT "Ln                 = ";:IF INT(LN(16) * 100.0) = 277 Pass() ELSE Fail()
    PRINT "Log                = ";:IF INT(LOG(16) * 100.0) = 120 Pass() ELSE Fail()
    PRINT "Exp                = ";:IF INT(EXP(1) * 100.0) = 271 Pass() ELSE Fail()
    PRINT "Atn                = ";:IF INT(ATN(3) * 100.0) = 124 Pass() ELSE Fail()
    PRINT "Tan                = ";:IF INT(TAN(3) * 100.0) = -14 Pass() ELSE Fail()
    PRINT "Sin                = ";:IF INT(SIN(3) * 100.0) = 14 Pass() ELSE Fail()
    PRINT "Cos                = ";:IF INT(COS(3) * 100.0) = -98 Pass() ELSE Fail()
    PRINT "Acs                = ";:IF INT(ACS(0.5) * 100.0) = 104 Pass() ELSE Fail()
    PRINT "Asn                = ";:IF INT(ASN(0.5) * 100.0) = 52 Pass() ELSE Fail()
    PRINT "Rad                = ";:IF INT(RAD(90) * 100.0) = 157 Pass() ELSE Fail()
    PRINT "Deg                = ";:IF INT(DEG(1.5)) = 85 Pass() ELSE Fail()
    PRINT "Pi                 = ";:IF INT(PI*100) = 314 Pass() ELSE Fail()
    PRINT "Sgn (-)            = ";:IF SGN(-10) = -1 Pass() ELSE Fail()
    PRINT "Sgn                = ";:IF SGN(0) = 0 Pass() ELSE Fail()
    PRINT "Sgn (+)            = ";:IF SGN(10) = 1 Pass() ELSE Fail()
    PRINT "Precedence         = ";:IF 2+3*4 = 14 Pass() ELSE Fail()
    PRINT "Parentheses        = ";:IF (2+3)*4 = 20 Pass() ELSE Fail()
    PRINT
ENDDEF

DEF NestedCalling()
    Nested1()
    RETURN
ENDDEF

DEF Nested1()
    Nested2()
    RETURN
ENDDEF

DEF Nested2()
    PRINT "Nested DEFs        = ";:Pass()
	PRINT
    RETURN
ENDDEF

DEF Numbers()
    PRINT "::Numbers::"
    PRINT "Hex                = ";:IF &40 = 64 Pass() ELSE Fail()
    PRINT "Binary             = ";:IF %100 = 4 Pass() ELSE Fail()
    PRINT
ENDDEF

DEF PrintInput()
    PRINT "::PRINT & INPUT::"
    PRINT "Multi-PRINT ,      = ";:PRINT 10,123.45,"Test"
    PRINT "Multi-PRINT ;      = ";:PRINT 10,123.45;"Test"
    PRINT "Multi-PRINT end ;  = ";:PRINT 10;123.45;"Test";:PRINT
    PRINT "PRINT SPC          = ";:PRINT "Test"SPC4;"End"
    PRINT "PRINT SPC()        = ";:PRINT "Test"SPC(4);"End"
    PRINT "PRINT TAB          = ";:PRINT "Test"TAB40;"End"
    PRINT "PRINT TAB()        = ";:PRINT "Test"TAB(60);"End"
    PRINT
ENDDEF

DEF ReturnValues()
    PRINT "Returning int      = ";:IF ReturnInteger%()=100 Pass() ELSE Fail()
    PRINT "Returning float    = ";:IF ReturnFloat#()=146.45 Pass() ELSE Fail()
    PRINT "Returning string   = ";:IF ReturnString$()="ff" Pass() ELSE Fail()
ENDDEF

DEF ReturnInteger%()
	RETURN 100
ENDDEF

DEF ReturnFloat#()
	RETURN 146.45
ENDDEF

DEF ReturnString$()
	RETURN "ff"
ENDDEF

DEF Strings()
    PRINT "::Strings::"
    PRINT FLOAT("10.5")
    PRINT "Asc                = ";:IF ASC("Fred") = 70 AND ASC("") = -1 Pass() ELSE Fail()
    PRINT "Chr$               = ";:IF CHR$(65) = "A" Pass() ELSE Fail()
    PRINT "Instr              = ";:IF INSTR("Test this this string", "this", 1) = 6 Pass() ELSE Fail()
    PRINT "Instr (with start) = ";:IF INSTR("Test this this string", "this", 8) = 11 Pass() ELSE Fail()
    PRINT "Left$              = ";:IF LEFT$("A test string", 6) = "A test" Pass() ELSE Fail()
    PRINT "Mid$               = ";:IF MID$("A test string", 3, 4) = "test" Pass() ELSE Fail()
    PRINT "Right$             = ";:IF RIGHT$("A test string", 6) = "string" Pass() ELSE Fail()
    PRINT "Len                = ";:IF LEN("A test string") = 13 Pass() ELSE Fail()
    PRINT "String to float    = ";:IF FLOAT("10.5") = 10.5 Pass() ELSE Fail()
    PRINT "Float to string    = ";:IF LEFT$(STR$(10.5),4) = "10.5" Pass() ELSE Fail()
    PRINT "String$            = ";:IF STRING$("A", 4) = "AAAA" Pass() ELSE Fail()
    PRINT
ENDDEF

DEF Swap()
	PRINT "::Swap::"
	swpa%=1:swpb%=2:swpf1#=123:swpf2#=456:swps1$="11":swps2$="22"
	SWAP swpa%,swpb%
	SWAP swpf1#,swpf2#
	SWAP swps1$,swps2$
	PRINT "Swap integer       = ";:IF swpa%=2 AND swpb%=1 Pass() ELSE Fail()
	PRINT "Swap float         = ";:IF swpf1#=456 AND swpf2#=123 Pass() ELSE Fail()
	PRINT "Swap string        = ";:IF swps1$="22" AND swps2$="11" Pass() ELSE Fail()
	PRINT
ENDDEF

DEF TestReturnInOtherwise()
    B$="5"
	PRINT "Return inside DEF  = ";:
    CASE B$ OF
        WHEN "1","2","3","4":Fail()
        OTHERWISE:Pass():	RETURN
    ENDCASE
	Fail()
ENDDEF

DEF Types()
	PRINT "::Types::"
	A! = VertexTest()
	A!y# = 100.5
	A!rgb% = &FF2080
	PRINT "Load field (float) = ";: IF A!y# = 100.5 Pass() ELSE Fail()
	PRINT "Load field (int)   = ";: IF A!rgb% = &FF2080 Pass() ELSE Fail()
	PRINT "Load field (Glob1) = ";: IF GVT!x# = 10.0 AND GVT!y# = 234.56 Pass() ELSE Fail()
	PRINT "Load field (Glob2) = ";: IF GVT!rgb% = &FFFFFF Pass() ELSE Fail()
	PRINT
ENDDEF

DEF Variables()
	A%=0:A#=0:A$="":B%=5+5:B#=123.45:B$="Test":C%=0:C#=0
    PRINT "::Variables::"
	C%=100
	LET C#=99.9
    PRINT "Local integer (0)  = ";:IF A%=0 Pass() ELSE Fail()
    PRINT "Local float (0)    = ";:IF A#=0.0 Pass() ELSE Fail()
    PRINT "Local string (0 )  = ";:IF A$="" Pass() ELSE Fail()
	PRINT B%,B#
    PRINT "Local integer      = ";:IF B%=10 Pass() ELSE Fail()
    PRINT "Local float        = ";:IF B#=123.45 Pass() ELSE Fail()
    PRINT "Local string       = ";:IF B$="Test" Pass() ELSE Fail()
    PRINT "Constants       	  = ";:IF const%=100 AND ccf#=12.5 Pass() ELSE Fail()
    PRINT "Implicit LET       = ";:IF C%=100 Pass() ELSE Fail()
    PRINT "Explicit LET       = ";:IF C#=99.9 Pass() ELSE Fail()
	A%=10:A%+=10
	PRINT "Var integer +=     = ";:IF A%=20 Pass() ELSE Fail()
	A#=10.5:A#+=10.5
	PRINT "Var float +=       = ";:IF A#=21.0 Pass() ELSE Fail()
	A%=10:A%-=5
	PRINT "Var integer -=     = ";:IF A%=5 Pass() ELSE Fail()
	A%=10:A%*=2
	PRINT "Var integer *=     = ";:IF A%=20 Pass() ELSE Fail()
	A%=10:A%<<=2
	PRINT "Var integer <<=    = ";:IF A%=40 Pass() ELSE Fail()
	A%=10:A%>>=2
	PRINT "Var integer >>=    = ";:IF A%=2 Pass() ELSE Fail()
	PRINT
ENDDEF

DEF Pass()
    PRINT "PASS!"
ENDDEF

DEF Fail()
    PRINT "!! FAILED TEST !!":END
ENDDEF

DEF Containers()
    PRINT "::Containers::"
    PRINT
ENDDEF
)";