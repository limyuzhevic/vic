#include "NeuromodulationControlCommands.hpp"

namespace nlm {
namespace cli {

void registerNeuromodulationCommands(AdvancedCLI& cli) {
    cli.registerCommand(std::make_unique<NeuromodulationCommand>());
}

} // namespace cli
} // namespace nlm
