#include "Panel_Results.h"
#include "imgui.h"

static void drawSpinner(bool active)
{
    ImGui::SameLine();
    char frame = active ? "|/-\\"[(int)(ImGui::GetTime()*10)&3] : ' ';
    ImGui::TextDisabled("%c", frame);
}

void DrawResultsPanel(ProtocolGuiState& state)
{
    ImGui::Separator();
    ImGui::Text("Validation");

    if (ImGui::Button("Run Checker", ImVec2(140, 0))) {
        state.isChecking = true;
        state.checkStartTime = ImGui::GetTime();
        runHTTP10Check(state.inputText, state.lastResult);
        state.hasResult = true;
    }
    drawSpinner(state.isChecking);

    if (state.isChecking) {
        if (ImGui::GetTime() - state.checkStartTime >= state.fakeCheckDuration)
            state.isChecking = false;
    }

    if (!state.hasResult) {
        ImGui::Spacing();
        ImGui::TextDisabled("No run yet.");
        return;
    }

    auto& r = state.lastResult;
    bool ok = r.syntaxOk && r.semanticsOk;

    ImGui::Spacing();
    ImGui::TextColored(
        ok ? ImVec4(0.2f, 0.8f, 0.2f, 1.f)
           : ImVec4(0.9f, 0.2f, 0.2f, 1.f),
        ok ? "All checks passed." : "Errors detected."
    );

    //
    // Syntax
    //
    ImGui::Separator();
    ImGui::Text("Syntax:");
    ImGui::SameLine();
    ImGui::TextColored(
        r.syntaxOk ? ImVec4(0.2,0.8,0.2,1) : ImVec4(0.9,0.2,0.2,1),
        r.syntaxOk ? "OK" : "ERROR"
    );

    if (!r.syntaxMessage.empty())
        ImGui::TextWrapped("%s", r.syntaxMessage.c_str());

    //
    // Detailed SLR diagnostic
    //
    if (!r.syntaxOk) {
        auto& d = r.diagnostic;
        ImGui::Spacing();
        ImGui::Separator();

        ImGui::TextColored(ImVec4(0.9f,0.3f,0.3f,1),"Syntax Diagnostic");

        if(!d.message.empty())
            ImGui::TextWrapped("%s", d.message.c_str());

        if(!d.expected.empty()) {
            ImGui::Text("Expected:");
            for(auto& e : d.expected)
                ImGui::BulletText("%s", e.c_str());
        }

        if(!d.got.empty()) {
            ImGui::Text("Got:");
            ImGui::BulletText("%s", d.got.c_str());
        }

        if(!d.interpretation.empty()) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.8f,0.8f,0.2f,1),"Interpretation");
            ImGui::TextWrapped("%s", d.interpretation.c_str());
        }

        if(!d.likelyError.empty()) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.3f,0.9f,0.3f,1),"Likely Fix");
            ImGui::TextWrapped("%s", d.likelyError.c_str());
        }
    }

    //
    // Semantics
    //
    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Semantics:");
    ImGui::SameLine();
    ImGui::TextColored(
        r.semanticsOk ? ImVec4(0.2,0.8,0.2,1) : ImVec4(0.9,0.2,0.2,1),
        r.semanticsOk ? "OK" : "ERROR"
    );

    if (!r.semanticsMessage.empty())
        ImGui::TextWrapped("%s", r.semanticsMessage.c_str());

    //
    // Parse tree path
    //
    if (r.hasParseTree) {
        ImGui::Spacing();
        ImGui::Text("Parse tree:");
        ImGui::TextDisabled("%s", r.parseTreePath.c_str());
    }

    //
    // Log + tokens
    //
    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Detailed Log", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginChild("LogChild", ImVec2(-1,150), true);
        ImGui::TextUnformatted(r.logText.c_str());
        ImGui::EndChild();
    }

    if (ImGui::CollapsingHeader("Tokens")) {
        ImGui::BeginChild("TokChild", ImVec2(-1,150), true);
        for (auto& t : r.tokens)
            ImGui::TextUnformatted(t.c_str());
        ImGui::EndChild();
    }
}
