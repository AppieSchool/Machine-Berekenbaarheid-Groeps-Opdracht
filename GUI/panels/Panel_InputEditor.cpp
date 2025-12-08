#include "Panel_InputEditor.h"
#include "imgui.h"
#include "imgui_stdlib.h"

void DrawInputEditor(ProtocolGuiState& state)
{
    ImGui::Separator();
    ImGui::Text("Protocol message:");

    ImGui::InputTextMultiline("##input", &state.inputText, ImVec2(-1.0f, 180.0f));
    ImGui::Spacing();
}
