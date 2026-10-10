// NLM Advanced CLI - Display Status Implementation
#include "cli/status.hpp"
#include "cli/command_factory.hpp"
#include <iostream>

namespace nlm {

void displayStatus() {
    auto& state = CommandFactory::getState();
    
    std::cout << "=== System Status ===\n" << std::endl;
    
    if (state.config) {
        std::cout << "Configuration: Loaded" << std::endl;
        std::cout << "  Configuration source: " << (state.lastConfigFile.empty() ? "Default" : state.lastConfigFile) << std::endl;
        std::cout << "  Configuration keys: " << state.config->getKeys().size() << std::endl;
    } else {
        std::cout << "Configuration: Not loaded" << std::endl;
    }
    
    if (state.brain) {
        std::cout << "Brain: Initialized" << std::endl;
        std::cout << "  Neurons: " << state.brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << state.brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Firing neurons: " << state.brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Total spikes: " << state.brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Development stage: " << static_cast<int>(state.brain->getDevelopmentalStage()) << std::endl;
        std::cout << "  Plasticity enabled: " << (state.brain->isPlasticityEnabled() ? "Yes" : "No") << std::endl;
        std::cout << "  Current step: " << state.simulationStep << std::endl;
        std::cout << "  Simulation time: " << state.simulationTime << " s" << std::endl;
    } else {
        std::cout << "Brain: Not initialized" << std::endl;
    }
    
    if (state.terminalRenderer) {
        std::cout << "Terminal Renderer: Active" << std::endl;
    } else {
        std::cout << "Terminal Renderer: Not active" << std::endl;
    }
    
    if (state.debugTools) {
        std::cout << "Debug Tools: Active" << std::endl;
    } else {
        std::cout << "Debug Tools: Not active" << std::endl;
    }
    
    std::cout << "\n=== System Information ===\n" << std::endl;
    std::cout << "NLM Advanced CLI System v2.0.0" << std::endl;
    std::cout << "Built on: " << __DATE__ << " at " << __TIME__ << std::endl;
    std::cout << "C++ Standard: " << __cplusplus << std::endl;
    
    std::cout << "\nAvailable Commands:" << std::endl;
    auto commands = CommandFactory::getInstance().listCommands();
    for (const auto& cmd : commands) {
        std::cout << "  " << cmd << std::endl;
    }
}

} // namespace nlm

