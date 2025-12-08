// main_gui.cpp

#include <GLFW/include/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "gui/ProtocolGui.h"
#include "protocols/HTTP10/HTTP10MessageGenerator.h"
#include "utils/imgui/ImGuiTheme.h"

int main()
{
    // Init GLFW FIRST
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(1200, 800, "Protocol Playground", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // -----------------------------------
    // INIT IMGUI (only once!)
    // -----------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ApplyCustomDarkTheme();        // <<< apply theme AFTER context creation
    // (Remove ImGui::StyleColorsDark(); you don't want default theme)

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // -----------------------------------
    // GUI state
    // -----------------------------------
    ProtocolGuiState state;

    // -----------------------------------
    // MAIN LOOP
    // -----------------------------------
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DrawProtocolGui(state);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // -----------------------------------
    // CLEANUP
    // -----------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
