#pragma once
#include <string>

const std::string DARIC_clock = R"(Clock()

DEF Clock()
	H%=4:M%=20
	REPEAT
		CLS
		w%=SCREENWIDTH:h%=SCREENHEIGHT
		S%=INT(MID$(TIME$,23,2))

		si%=h%*0.45:A#=180-S%*6:x%=SIN(RAD(A#))*si%:y%=COS(RAD(A#))*si%
		COLOUR 255,255,0:LINE w%/2,h%/2,w%/2+x%,h%/2+y%

		si%=h%*0.375:A#=180-M%*6:x%=SIN(RAD(A#))*si%:y%=COS(RAD(A#))*si%
		COLOUR 0,255,0:LINE w%/2,h%/2,w%/2+x%,h%/2+y%

		si%=h%*0.2:A#=180-H%*6:x%=SIN(RAD(A#))*si%:y%=COS(RAD(A#))*si%
		COLOUR 0,255,255:LINE w%/2,h%/2,w%/2+x%,h%/2+y%

		YIELD
	UNTIL FALSE
ENDDEF)";