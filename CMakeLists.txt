cmake_minimum_required(VERSION 3.29.6)
project(Machine_Berekenbaarheid_Groeps_Opdracht)

set(CMAKE_CXX_STANDARD 20)

# -------------------------------
# Main executable
# -------------------------------
add_executable(Machine-Berekenbaarheid-Groeps-Opdracht
        main.cpp
        grammers/CFG.cpp
        parsers/SLR.cpp
        protocols/HTTP10/HTTP10Protocol.cpp
        protocols/HTTP10/HTTP10Tokenizer.cpp
        grammers/CFG_CYK.cpp
        visualization/DotGenerator.cpp
        visualization/HTTPTreeBuilder.cpp
        protocols/HTTP10/HTTP10_semantics.cpp
        protocols/HTTP10/HTTPrequest.cpp
)


target_include_directories(Machine-Berekenbaarheid-Groeps-Opdracht PRIVATE
        grammers
        protocols/HTTP10
        utils
        parsers
        visualization
)

add_executable(test_http10
        protocols/HTTP10/tests/test_http10_main.cpp
        protocols/HTTP10/tests/HTTP10tests.cpp
        grammers/CFG.cpp
        parsers/SLR.cpp
        protocols/HTTP10/HTTP10Protocol.cpp
        protocols/HTTP10/HTTP10Tokenizer.cpp
)

target_include_directories(test_http10 PRIVATE
        grammers
        protocols/HTTP10
        protocols/HTTP10/tests
        utils
        parsers
)


configure_file(protocols/HTTP10/http10.json
        protocols/HTTP10/http10.json COPYONLY)
