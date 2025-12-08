#include "Panel_Generator.h"
#include "components.h"
#include "../../protocols/HTTP10/HTTP10MessageGenerator.h"

void DrawMessageGenerator(ProtocolGuiState& state)
{
    auto& opt = state.genOptions;

    //
    // ------------------------------------------------------------
    // MESSAGE GENERATOR
    // ------------------------------------------------------------
    //
    SectionHeader("Message Generator");

    // Small card for Valid / Invalid (height ~80)
    if (CardBegin("validity", ImVec2(-1, 40)))
    {
        int validity = (opt.validity == HTTP10ExampleKind::Valid ? 0 : 1);

        ImGui::RadioButton("Valid", &validity, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Invalid", &validity, 1);

        opt.validity = (validity == 0
                        ? HTTP10ExampleKind::Valid
                        : HTTP10ExampleKind::Invalid);
    }
    CardEnd();


    //
    // ------------------------------------------------------------
    // REQUEST-LINE SETTINGS
    // ------------------------------------------------------------
    //
    SectionHeader("Request-Line Settings");

    // Medium card (height ~200)
    if (CardBegin("request_line", ImVec2(-1, 140)))
    {
        //
        // METHOD SELECT
        //
        static const char* methods[] = { "GET", "POST", "HEAD" };
        int methodIdx = (opt.method == "POST" ? 1 : (opt.method == "HEAD" ? 2 : 0));

        if (ComboField("Method", &methodIdx, methods, 3))
            opt.method = methods[methodIdx];

        //
        // PATH SEGMENTS
        //
        if (opt.path.size() < 3)
            opt.path.resize(3);

        InputTextField("Segment 1", opt.path[0]);
        InputTextField("Segment 2", opt.path[1]);
        InputTextField("Segment 3", opt.path[2]);

        //
        // EXTENSION
        //
        InputTextField("Extension", opt.extension);
    }
    CardEnd();


    //
    // ------------------------------------------------------------
    // HEADERS
    // ------------------------------------------------------------
    //
    SectionHeader("Headers");

    // Larger card for scrollable headers (height ~260)
    if (CardBegin("headers", ImVec2(-1, 260)))
    {
        for (size_t i = 0; i < opt.headers.size(); i++)
        {
            auto& h = opt.headers[i];
            ImGui::PushID((int)i);

            CheckboxField("Enable", h.enabled);
            InputTextField("Name", h.name);
            InputTextField("Value", h.value);
            CheckboxField("Force Invalid", h.forceInvalid);

            ImGui::Separator();
            ImGui::PopID();
        }
    }
    CardEnd();


    //
    // ------------------------------------------------------------
    // GENERATE BUTTON
    // ------------------------------------------------------------
    //
    ImGui::Spacing();
    if (ImGui::Button("Generate Message", ImVec2(220, 40)))
    {
        state.inputText = HTTP10MessageGenerator::generate(opt);
    }

    ImGui::Spacing();
}
