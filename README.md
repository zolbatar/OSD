# OS/D
OS/D, uses Circle for Pi

Uses C++ and C.

# Uses the following open source libraries:

Circle (for bare metal Pi):
GPL3
https://github.com/rsta2/circle

GNU Lightning (for native compile): 
LGPL
https://www.gnu.org/software/lightning/

Capstone (for disassembly)
BSD license
https://www.capstone-engine.org

lvgl (for UI)
MIT license
https://lvgl.io/

stb-truetype
MIT

JSON for Modern C++
MIT
https://github.com/nlohmann/json

# TODO

Collections
INSTALL
Deal with messages from dead applications

# Acorn file system

.	Separates directory specifications, eg $.fred
:	Introduces a drive or disc specification, eg :0, :welcome. It also marks the end of a filing system name, eg adfs:
*	Acts as a 'wildcard' to match zero or more characters, eg prog*
#	Acts as a 'wildcard' to match any single character, eg $.ch##
$	is the name of the root directory of the disc
&	is the user root directory (URD)
@	is the currently selected directory (CSD)
^	is the 'parent' directory
%	is the currently selected library directory (CSL)
\	is the previously selected directory (PSD - available on FileCore-based filing systems, and any others that choose to do so)
