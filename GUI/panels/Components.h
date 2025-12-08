#pragma once
#include "imgui.h"
#include "imgui_stdlib.h"

//
// SECTION HEADER
//
inline void SectionHeader(const char* text)
{
    ImGui::Spacing();
    ImGui::SeparatorText(text);        // cleaner ImGui 1.90+ header
    ImGui::Spacing();
}

//
// INPUT HELPERS
//
inline bool InputTextField(const char* label, std::string& value)
{
    return ImGui::InputText(label, &value);
}

inline bool CheckboxField(const char* label, bool& value)
{
    return ImGui::Checkbox(label, &value);
}

inline bool ComboField(const char* label, int* index,
                       const char* const items[], int count)
{
    return ImGui::Combo(label, index, items, count);
}

//
// CARD WIDGET (Resizable Panel)
//
inline bool CardBegin(const char* id,
                      ImVec2 size = ImVec2(-1, 0),   // Default: auto-height, full width
                      bool border = true)
{
    ImGui::PushID(id);

    // Rounded panel look
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, border ? 1.0f : 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 8));

    bool open = ImGui::BeginChild(
        id,
        size,
        border
    );

    return open;
}

inline void CardEnd()
{
    ImGui::EndChild();
    ImGui::PopStyleVar(3);  // WindowPadding, BorderSize, Rounding
    ImGui::PopID();
    ImGui::Spacing();
}
