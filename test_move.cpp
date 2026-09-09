#include <iostream>
#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"

int main() {
    std::cout << "Testing AgentBrain move operations..." << std::endl;
    
    // Create a mock brain (simplified for testing)
    auto config = std::make_shared<Config>();
    config->set<uint64_t>("neuron_count", 100);
    config->set<size_t>("region_count", 1);
    config->set<float>("connection_probability", 0.1f);
    config->set<float>("simulation_timestep", 0.001);
    
    Brain brain(config);
    brain.initialize();
    
    // Create first AgentBrain
    AgentBrain agentBrain1(std::make_shared<Brain>(brain));
    
    std::cout << "Original AgentBrain created successfully." << std::endl;
    
    // Test move constructor
    AgentBrain agentBrain2(std::move(agentBrain1));
    
    std::cout << "Move constructor test passed." << std::endl;
    
    // Test move assignment operator
    AgentBrain agentBrain3(std::make_shared<Brain>(brain));
    agentBrain3 = std::move(agentBrain2);
    
    std::cout << "Move assignment operator test passed." << std::endl;
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
