#pragma once
#include <string>

const std::string DARIC_mandelbrot = R"(Mandelbrot()
DEF Mandelbrot()
	ITERS% = 256:XRANGE# = -1.5:YRANGE# = -1.5:COLS% = 16
	REPEAT
    ww%=SCREENWIDTH():hh%=SCREENHEIGHT()
		CLS
		FOR Y%=0 TO hh%
			FOR X%=0 TO ww%
				CR# = XRANGE# + (ww%-X%) * 4 / ww%
				CI# = YRANGE# + Y% * 3 / hh%
				ZM# = 0:ZR# = 0:ZI# = 0:ZR2# = 0:ZI2# = 0:IT% = 0
				REPEAT
					Z1# = ZR2# - ZI2# + CR#
					Z2# = 2 * ZR# * ZI# + CI#
					ZR# = Z1#
					ZI# = Z2#
					ZR2# = ZR# * ZR#
					ZI2# = ZI# * ZI#
					ZM# = ZR2# + ZI2#
					IT% += 1
				UNTIL IT% = ITERS% OR ZM# >= 4
				c%=IT% MOD 16

				COLOUR c%*16,c%*16,c%*16
				PLOT X%,Y%
			NEXT
			YIELD
		NEXT
	UNTIL FALSE
ENDDEF
)";

const std::string DARIC_mandelbrot_single = R"(Mandelbrot()
DEF Mandelbrot()
	ITERS% = 256:XRANGE# = -1.5:YRANGE# = -1.5:COLS% = 16
    ww%=SCREENWIDTH():hh%=SCREENHEIGHT()
	CLS
	FOR Y%=0 TO hh%
		FOR X%=0 TO ww%
			CR# = XRANGE# + (ww%-X%) * 4 / ww%
			CI# = YRANGE# + Y% * 3 / hh%
			ZM# = 0:ZR# = 0:ZI# = 0:ZR2# = 0:ZI2# = 0:IT% = 0
			REPEAT
				Z1# = ZR2# - ZI2# + CR#
				Z2# = 2 * ZR# * ZI# + CI#
				ZR# = Z1#
				ZI# = Z2#
				ZR2# = ZR# * ZR#
				ZI2# = ZI# * ZI#
				ZM# = ZR2# + ZI2#
				IT% += 1
			UNTIL IT% = ITERS% OR ZM# >= 4
			c%=IT% MOD 16

			COLOUR c%*16,c%*16,c%*16
			PLOT X%,Y%
		NEXT
		YIELD
	NEXT
ENDDEF
)";
