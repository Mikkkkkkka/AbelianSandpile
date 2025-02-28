cmake_minimum_required(VERSION 3.30)
project(AbelianSandpile)

set(CMAKE_CXX_STANDARD 20)

add_executable(AbelianSandpile src/main.cpp
        src/Printer.cpp
        src/Printer.h
        src/Model.cpp
        src/Model.h
        lib/ArgParser.cpp
        lib/ArgParser.h)
