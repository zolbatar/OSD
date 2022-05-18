#pragma once
#include <string>

const std::string DARIC_clock = R"(Clock()

DEF Clock()
	H%=4:M%=20
	COLOURBG &FF,&FF,&FF
	FOR I%=0 TO 100
		CLS
		w%=SCREENWIDTH:h%=SCREENHEIGHT
		S%=INT(MID$(TIME$,23,2))

		si%=h%*0.45:A#=180-S%*6:x%=SIN(RAD(A#))*si%:y%=COS(RAD(A#))*si%
		COLOUR 0,0,0:LINE w%/2,h%/2,w%/2+x%,h%/2+y%

		si%=h%*0.375:A#=180-M%*6:x%=SIN(RAD(A#))*si%:y%=COS(RAD(A#))*si%
		COLOUR 60,60,60:LINE w%/2,h%/2,w%/2+x%,h%/2+y%

		si%=h%*0.2:A#=180-H%*6:x%=SIN(RAD(A#))*si%:y%=COS(RAD(A#))*si%
		COLOUR 120,120,120:LINE w%/2,h%/2,w%/2+x%,h%/2+y%

		YIELD
	NEXT
ENDDEF)";