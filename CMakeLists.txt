cmake_minimum_required(VERSION 3.30)
project(Machine-Berekenbaarheid-Groeps-Opdracht)

set(CMAKE_CXX_STANDARD 20)

add_executable(Machine-Berekenbaarheid-Groeps-Opdracht
        main.cpp
        parsers/SLR.cpp
        grammers/CFG.cpp
        grammers/CFG.cpp)
