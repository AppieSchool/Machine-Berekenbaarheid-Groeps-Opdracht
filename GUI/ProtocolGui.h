#pragma once

#include <string>
#include <vector>
#include "HTTP10Checker.h"
#include "imgui.h"
#include "../protocols/HTTP10/HTTP10MessageGenerator.h"

// --------------------------
// Protocol selection enum
// --------------------------
enum class ProtocolKind {
    HTTP10
};

// --------------------------
// GUI State container
// --------------------------
struct ProtocolGuiState {
    ProtocolGuiState();
    ProtocolKind selectedProtocol = ProtocolKind::HTTP10;

    // NEW: generator configuration
    HTTP10MessageOptions genOptions;

    std::string inputText;
    bool isChecking = false;
    bool hasResult = false;
    double checkStartTime = 0.0;
    double fakeCheckDuration = 0.3;

    ProtocolCheckResult lastResult;
};
// --------------------------
// Main function to render ALL GUI panels
// --------------------------
void DrawProtocolGui(ProtocolGuiState& state);
