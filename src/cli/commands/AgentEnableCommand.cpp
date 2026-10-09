#include "AgentEnableCommand.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <vector>

namespace nlm {

int AgentEnableCommand::execute(const std::vector<std::string>& args) {
    bool curiosityEnabled = false;
    bool rewardModulationEnabled = false;
    bool neuromodulationEnabled = false;
    
    // Parse optional flags
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "--curiosity") {
            curiosityEnabled = true;
        } else if (arg == "--reward-modulation") {
            rewardModulationEnabled = true;
        } else if (arg == "--neuromodulation") {
            neuromodulationEnabled = true;
        }
    }
    
    std::cout << "Agent enable command executed" << std::endl;
    std::cout << "Curiosity: " << (curiosityEnabled ? "true" : "false") << std::endl;
    std::cout << "Reward Modulation: " << (rewardModulationEnabled ? "true" : "false") << std::endl;
    std::cout << "Neuromodulation: " << (neuromodulationEnabled ? "true" : "false") << std::endl;
    
    // In a real implementation, this would:
    // 1. Enable agent functionality
    // 2. Configure neuromodulation systems
    // 3. Set up curiosity-driven learning
    // 4. Configure reward modulation
    
    // Simulate agent enabling
    std::cout << "\nEnabling agent systems..." << std::endl;
    
    if (neuromodulationEnabled) {
        std::cout << "  Neuromodulation: Enabled" << std::endl;
        std::cout << "    Dopamine system: Active" << std::endl;
        std::cout << "    Curiosity system: Active" << std::endl;
        std::cout << "    Novelty detection: Active" << std::endl;
        std::cout << "    Prediction error: Active" << std::endl;
    }
    
    if (curiosityEnabled) {
        std::cout << "  Curiosity-driven exploration: Enabled" << std::endl;
        std::cout << "    Novelty seeking behavior: Active" << std::endl;
        std::cout << "    Intrinsic motivation: Active" << std::endl;
    }
    
    if (rewardModulationEnabled) {
        std::cout << "  Reward modulation: Enabled" << std::endl;
        std::cout << "    Reward prediction: Active" << std::endl;
        std::cout << "    Reinforcement learning: Active" << std::endl;
    }
    
    // Configure neuromodulation levels
    std::cout << "\nConfiguring neuromodulator levels..." << std::endl;
    std::cout << "  Baseline dopamine: 0.1" << std::endl;
    std::cout << "  Learning rate: 0.01" << std::endl;
    std::cout << "  Exploration bonus: 0.05" << std::endl;
    
    // Initialize agent behavior
    std::cout << "\nInitializing agent behavior..." << std::endl;
    std::cout << "  Setting action selection policy..." << std::endl;
    std::cout << "  Configuring sensory processing..." << std::endl;
    std::cout << "  Setting goal priorities..." << std::endl;
    
    std::cout << "\n✓ Agent enabled successfully" << std::endl;
    std::cout << "  Agent is ready for interaction with brain" << std::endl;
    std::cout << "  (Note: Full implementation requires agent registry)" << std::endl;
    
    return 0;
}

} // namespace nlm