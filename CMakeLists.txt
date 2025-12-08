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
        GUI/HTTP10Checker.cpp
        GUI/ProtocolGui.cpp
        protocols/HTTP10/HTTP10MessageGenerator.cpp
        GUI/panels/Panel_Generator.cpp
        GUI/panels/Panel_InputEditor.cpp
        GUI/panels/Panel_ProtocolSelector.cpp
        GUI/panels/Panel_Results.cpp
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
# -------------------------
# ImGui library
# -------------------------
set(IMGUI_DIR utils/imgui)

add_library(imgui
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_demo.cpp  # optional
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${IMGUI_DIR}/misc/cpp
)

configure_file(protocols/HTTP10/http10.json
        protocols/HTTP10/http10.json COPYONLY)

# ----------------------------------------------------
# GLFW integration
# ----------------------------------------------------
# Adjust if your directory names differ
set(GLFW_INCLUDE_DIR utils/GLFW/include)
set(GLFW_LIBRARY_DIR utils/GLFW/lib)

target_include_directories(Machine-Berekenbaarheid-Groeps-Opdracht PRIVATE ${GLFW_INCLUDE_DIR})
target_link_directories(Machine-Berekenbaarheid-Groeps-Opdracht PRIVATE ${GLFW_LIBRARY_DIR})

# Link against dll import lib: libglfw3dll.a  --> glfw3dll
target_link_libraries(Machine-Berekenbaarheid-Groeps-Opdracht PRIVATE
        imgui
        glfw3dll
        opengl32
        gdi32
        user32
        shell32
)

# If you need GLFW also in the test executable, add:
# target_include_directories(test_http10 PRIVATE ${GLFW_INCLUDE_DIR})
# target_link_directories(test_http10 PRIVATE ${GLFW_LIBRARY_DIR})
# target_link_libraries(test_http10 PRIVATE glfw3dll opengl32 gdi32 user32 shell32)
