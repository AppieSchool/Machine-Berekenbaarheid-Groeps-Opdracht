#include "Panel_ProtocolSelector.h"
#include "imgui.h"

void DrawProtocolSelector(ProtocolGuiState& state)
{
    ImGui::Text("Select protocol:");
    ImGui::Separator();

    if (ImGui::Button("HTTP/1.0", ImVec2(140, 40)))
        state.selectedProtocol = ProtocolKind::HTTP10;

    ImGui::SameLine();
    ImGui::TextDisabled("(more protocols soon)");

    ImGui::Spacing();
}
