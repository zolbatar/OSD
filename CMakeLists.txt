cmake_minimum_required(VERSION 3.20)
project(osd)

set(CMAKE_CXX_STANDARD 17)

add_executable(osd
        clion_main.cpp
        Compiler/Tokeniser/Tokeniser.cpp Compiler/Tokeniser/TokeniserNumber.cpp Compiler/Tokeniser/TokeniserPrinter.cpp
        Compiler/Tokeniser/TokeniserString.cpp Compiler/Tokeniser/TokeniserSymbol.cpp Compiler/Tokeniser/TokeniserText.cpp)
