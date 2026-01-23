cmake_minimum_required(VERSION 3.29.6)
project(Machine_Berekenbaarheid_Groeps_Opdracht)

set(CMAKE_CXX_STANDARD 20)

# -------------------------------
# Main executable
# -------------------------------
add_executable(Machine-Berekenbaarheid-Groeps-Opdracht
        main.cpp
        grammers/CFG.cpp
        grammers/PDA.cpp
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
        GUI/ImageLoader.cpp
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

configure_file(protocols/HTTP10/http10_pda.json
        protocols/HTTP10/http10_pda.json COPYONLY)

# ----------------------------------------------------
# GLFW integration (platform-specific)
# ----------------------------------------------------
if(APPLE)
    # macOS: Use Homebrew GLFW
    find_package(glfw3 REQUIRED)
    target_link_libraries(Machine-Berekenbaarheid-Groeps-Opdracht PRIVATE
            imgui
            glfw
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )
    target_include_directories(imgui PUBLIC /opt/homebrew/include)
elseif(WIN32)
    set(GLFW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/utils/GLFW/include)
    set(GLFW_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/utils/GLFW/lib)

    # Zorg dat imgui_impl_glfw.cpp GLFW headers vindt
    target_include_directories(imgui PUBLIC ${GLFW_INCLUDE_DIR})

    target_link_directories(Machine-Berekenbaarheid-Groeps-Opdracht PRIVATE ${GLFW_LIBRARY_DIR})

    target_link_libraries(Machine-Berekenbaarheid-Groeps-Opdracht PRIVATE
            imgui
            ${GLFW_LIBRARY_DIR}/libglfw3dll.a
            opengl32
            gdi32
            user32
            shell32
    )

    # Copy glfw3.dll next to exe
    add_custom_command(TARGET Machine-Berekenbaarheid-Groeps-Opdracht POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${GLFW_LIBRARY_DIR}/glfw3.dll
            $<TARGET_FILE_DIR:Machine-Berekenbaarheid-Groeps-Opdracht>
    )
else()
    # Linux: Use system GLFW
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(Machine-Berekenbaarheid-Groeps-Opdracht PRIVATE
            imgui
            glfw
            OpenGL::GL
    )
endif()