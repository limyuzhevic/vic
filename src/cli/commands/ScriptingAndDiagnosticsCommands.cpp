#include "ScriptingAndDiagnosticsCommands.hpp"

namespace nlm {
namespace cli {

void registerScriptingAndDiagnosticsCommands(AdvancedCLI& cli) {
    cli.registerCommand(std::make_unique<ScriptCommand>());
    cli.registerCommand(std::make_unique<DiagnosticCommand>());
}

} // namespace cli
} // namespace nlm
