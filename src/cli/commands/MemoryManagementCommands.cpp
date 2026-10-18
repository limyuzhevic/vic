#include "MemoryManagementCommands.hpp"

namespace nlm {
namespace cli {

void registerMemoryCommands(AdvancedCLI& cli) {
    cli.registerCommand(std::make_unique<WorkingMemoryCommand>());
    cli.registerCommand(std::make_unique<EpisodicMemoryCommand>());
    cli.registerCommand(std::make_unique<PredictionAnalysisCommand>());
}

} // namespace cli
} // namespace nlm
