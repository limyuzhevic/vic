// Simple test to verify enhanced AgentBrain functionality
#include "src/agent/AgentBrain.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Testing Enhanced AgentBrain ===" << std::endl;
    
    // Initialize global logger
    nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Brain created and initialized successfully" << std::endl;
    
    // Create agent brain
    nlm::AgentBrain agent(brain);
    
    // Create simple world
    nlm::SimpleWorld world;
    world.initialize(16, 16);
    
    std::cout << "✓ AgentBrain created successfully" << std::endl;
    
    // Initialize agent
    agent.initialize(world);
    
    std::cout << "✓ AgentBrain initialized successfully" << std::endl;
    
    // Test sensory input size
    assert(agent.getSensoryInputSize() > 0);
    std::cout << "✓ Sensory input size is valid: " << agent.getSensoryInputSize() << std::endl;
    
    // Test motor output size
    assert(agent.getMotorOutputSize() > 0);
    std::cout << "✓ Motor output size is valid: " << agent.getMotorOutputSize() << std::endl;
    
    // Test action preferences
    agent.updateActionPreferences();
    std::cout << "✓ Action preferences updated successfully" << std::endl;
    
    // Test state variables
    agent.updateStateVariables(std::vector<float>(4, 0.5f));
    assert(agent.getStateVariable("energy") >= 0.0);
    assert(agent.getStateVariable("health") >= 0.0);
    std::cout << "✓ State variables updated successfully" << std::endl;
    
    // Test motivation
    agent.updateMotivation();
    float motivation = agent.getMotivation();
    assert(motivation >= 0.0 && motivation <= 1.0);
    std::cout << "✓ Motivation calculated successfully: " << motivation << std::endl;
    
    // Test configuration
    agent.enableRewardModulation(true);
    assert(agent.isRewardModulationEnabled() == true);
    agent.enableDevelopment(false);
    assert(agent.isDevelopmentEnabled() == false);
    std::cout << "✓ Configuration settings work correctly" << std::endl;
    
    // Test reset
    agent.reset();
    assert(agent.getStateVariable("energy") == 1.0);
    assert(agent.getStateVariable("health") == 1.0);
    std::cout << "✓ Reset functionality works correctly" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== All Enhanced AgentBrain Tests PASSED ===" << std::endl;
    
    return 0;
}

// Include necessary headers for the types used above
namespace nlm {
    extern std::shared_ptr<Brain> createBrainFromConfig(std::shared_ptr<Config> config);
}