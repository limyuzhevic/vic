#include "SimulationControlCommands.hpp"

namespace nlm {
namespace cli {

void registerSimulationCommands(AdvancedCLI& cli) {
    cli.registerCommand(std::make_unique<SimulationControlCommand>());
    cli.registerCommand(std::make_unique<VisualizationCommand>());
    cli.registerCommand(std::make_unique<ConfigurationCommand>());
}

} // namespace cli
} // namespace nlm
