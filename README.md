# OS/D
OS/D, uses Circle for Pi and Clion (or Xcode?) for desktop dev

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

TODO
----

Structured types

Collections

String allocations (keep string of STORE and LOAD strings (and DATA/CONST), and garbage collect on LOAD?)

INSTALL

Deal with messages from dead applications

Nested REPEAT and WHILEs, IFs
