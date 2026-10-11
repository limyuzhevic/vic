# Example Experiment Files for NLM Advanced Features

## 1. Config Operations Demo
# File: config_demo.cpp
# Description: Demonstration of advanced config file operations

#include "core/Config/Config.hpp"
#include <iostream>

void demonstrateConfigOperations() {
    std::cout << "=== Advanced Config Operations Demo ===" << std::endl;
    
    // Create and load a config
    nlm::Config config;
    config.loadFromFile("configs/neural_network.json");
    
    std::cout << "Config loaded: " << config.getOr<int64_t>("parameters.neuron_count", 0) << " neurons" << std::endl;
    
    // Extract specific keys
    nlm::Config extracted = config.extractKeys({"neuron_count", "region_count"});
    std::cout << "Extracted " << extracted.getKeys().size() << " key(s)" << std::endl;
    
    // Generate template
    nlm::Config templateConfig = config.generateTemplate("neural");
    std::cout << "Generated template with " << templateConfig.getKeys().size() << " parameters" << std::endl;
    
    // Show statistics
    std::string stats = config.getStatistics();
    std::cout << "Config statistics:" << std::endl;
    std::cout << stats << std::endl;
}

void demonstrateBrainCommands() {
    std::cout << "=== Brain System Commands Demo ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (brain->initialize()) {
        brain->logStatus();
        
        // Show plasticity report
        auto* stdp = brain->getSTDP();
        if (stdp) {
            std::cout << "STDP LTP weight: " << stdp->getLTPWeight() << std::endl;
        }
        
        // Show neuromodulation status
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            std::cout << "Dopamine level: " << dopamine->getCurrentLevel() << std::endl;
        }
    }
}

void demonstrateAgentCommands() {
    std::cout << "=== Agent Commands Demo ===" << std::endl;
    
    // Agent learn command
    std::cout << "Agent learn with parameters: learning_rate=0.01, episodes=1000" << std::endl;
    
    // Agent profile command
    std::cout << "Agent profile information:" << std::endl;
    std::cout << "  Capabilities: Real spiking neural computation, plasticity, neuromodulation" << std::endl;
    std::cout << "  Limitations: CPU-bound, requires extensive memory" << std::endl;
    
    // Agent save/load example
    std::cout << "Agent save: Config saved to 'agent_state.json' with metadata" << std::endl;
}

void demonstrateEnvironmentCommands() {
    std::cout << "=== Environment Commands Demo ===" << std::endl;
    
    std::cout << "Available environments:" << std::endl;
    std::cout << "  simple - Basic neural environment" << std::endl;
    std::cout << "  complex - Advanced neural environment with obstacles" << std::endl;
    std::cout << "  adaptive - Environment that adapts to agent performance" << std::endl;
    
    std::cout << "\nEnvironment create example:" << std::endl;
    std::cout << "  env create simulated 100x100x10 --rewards.goal=100 --rewards.step=-0.01" << std::endl;
}

void demonstrateWorldCommands() {
    std::cout << "=== World Commands Demo ===" << std::endl;
    
    std::cout << "World reset: Resets world state to initial conditions" << std::endl;
    std::cout << "World pause: Pauses/resumes world simulation" << std::endl;
    std::cout << "World step: Steps world simulation forward specified number of steps" << std::endl;
    
    std::cout << "\nUsage examples:" << std::endl;
    std::cout << "  world reset" << std::endl;
    std::cout << "  world pause 1" << std::endl;
    std::cout << "  world step 100" << std::endl;
}

int main() {
    demonstrateConfigOperations();
    std::cout << std::endl;
    
    demonstrateBrainCommands();
    std::cout << std::endl;
    
    demonstrateAgentCommands();
    std::cout << std::endl;
    
    demonstrateEnvironmentCommands();
    std::cout << std::endl;
    
    demonstrateWorldCommands();
    
    return 0;
}
