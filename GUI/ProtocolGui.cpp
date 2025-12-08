#pragma once
#include "ProtocolGui.h"

#include "panels/Panel_ProtocolSelector.h"
#include "panels/Panel_Generator.h"
#include "panels/Panel_InputEditor.h"
#include "panels/Panel_Results.h"

void DrawProtocolGui(ProtocolGuiState& state)
{
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |          // <-- not resizable
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    // Fullscreen root
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::Begin("RootWindow", nullptr, flags);

    // Entire UI scrolls inside this child
    ImGui::BeginChild(
        "ScrollableContent",
        ImVec2(0, 0),
        false,
        ImGuiWindowFlags_AlwaysVerticalScrollbar
    );

    DrawProtocolSelector(state);
    DrawMessageGenerator(state);
    DrawInputEditor(state);
    DrawResultsPanel(state);

    ImGui::EndChild();
    ImGui::End();
}

ProtocolGuiState::ProtocolGuiState() {
    genOptions.headers = {
        {"Host", "example.com", true, false},
        {"User-Agent", "TestClient/1.0", true, false},
        {"Accept", "text/html", false, false},
        {"Connection", "keep-alive", false, false}
    };
}
