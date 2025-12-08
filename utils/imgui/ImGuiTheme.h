#pragma once
#include "imgui.h"

inline void ApplyCustomDarkTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Primary branding colors (purple–pink gradient vibes)
    const ImVec4 ACCENT1 = ImVec4(0.70f, 0.40f, 1.00f, 1.0f);
    const ImVec4 ACCENT2 = ImVec4(1.00f, 0.40f, 0.80f, 1.0f);

    // Backgrounds
    colors[ImGuiCol_WindowBg]         = ImVec4(0.08f, 0.08f, 0.10f, 1.0f);
    colors[ImGuiCol_ChildBg]          = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    colors[ImGuiCol_PopupBg]          = ImVec4(0.12f, 0.12f, 0.14f, 1.0f);

    // Borders
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 0.0f;

    // Rounding
    style.FrameRounding   = 8.0f;
    style.WindowRounding  = 10.0f;
    style.ChildRounding   = 10.0f;
    style.PopupRounding   = 10.0f;

    // Text
    colors[ImGuiCol_Text]            = ImVec4(0.95f, 0.95f, 0.96f, 1.0f);
    colors[ImGuiCol_TextDisabled]    = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);

    // Buttons
    colors[ImGuiCol_Button]          = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    colors[ImGuiCol_ButtonHovered]   = ACCENT1;
    colors[ImGuiCol_ButtonActive]    = ACCENT2;

    // Frames
    colors[ImGuiCol_FrameBg]         = ImVec4(0.15f, 0.15f, 0.20f, 1.0f);
    colors[ImGuiCol_FrameBgHovered]  = ACCENT1;
    colors[ImGuiCol_FrameBgActive]   = ACCENT2;

    // Checkmark
    colors[ImGuiCol_CheckMark]       = ACCENT2;

    // Slider
    colors[ImGuiCol_SliderGrab]      = ACCENT1;
    colors[ImGuiCol_SliderGrabActive]= ACCENT2;

    // Headers (collapsing tree titles)
    colors[ImGuiCol_Header]          = ImVec4(0.17f, 0.15f, 0.22f, 1.0f);
    colors[ImGuiCol_HeaderActive]    = ACCENT2;
    colors[ImGuiCol_HeaderHovered]   = ACCENT1;

    // Tabs
    colors[ImGuiCol_Tab]             = ImVec4(0.12f, 0.12f, 0.16f, 1.0f);
    colors[ImGuiCol_TabHovered]      = ACCENT1;
    colors[ImGuiCol_TabActive]       = ACCENT2;
}
