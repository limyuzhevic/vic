#include "StateAnalysisCommands.hpp"

namespace nlm {
namespace cli {

void registerStateCommands(AdvancedCLI& cli) {
    cli.registerCommand(std::make_unique<StateAnalysisCommand>());
    cli.registerCommand(std::make_unique<PerformanceCommand>());
}

} // namespace cli
} // namespace nlm
