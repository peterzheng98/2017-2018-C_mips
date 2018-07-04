cmake_minimum_required(VERSION 3.9)
project(MIPS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O2 -g")

add_executable(MIPS main.cpp Base.h utilities.h mips_main.h stdc++.h parser.h executedInstruction.h)