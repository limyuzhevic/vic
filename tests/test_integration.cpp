// Integration test for full system
#include <iostream>
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"

int test_integration() {
    std::cout << "Integration Test: Testing full system components..." << std::endl;
    
    // Test AgentBrain
    nlm::AgentBrain agent_brain;
    agent_brain.initialize(100, 50);
    
    std::cout << "AgentBrain initialized with " << agent_brain.getNumNeurons() << " neurons and " << agent_brain.getNumConnections() << " connections" << std::endl;
    
    // Test SimpleWorld
    nlm::SimpleWorld world;
    world.setSize(100.0f, 100.0f);
    world.setTimeStep(0.01f);
    
    std::cout << "SimpleWorld configured with size: " << world.getWidth() << " x " << world.getHeight() << " and dt: " << world.getTimeStep() << std::endl;
    
    return 0;
}

int main() {
    return test_integration();
}
