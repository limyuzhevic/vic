#include <iostream>
#include "agent/AgentBrain.hpp"
#include "environment/Environment.hpp"
#include "world/SimpleWorld.hpp"
#include "cognition/NeuralPlanner.hpp"

int main() {
    std::cout << "NLM Neural Simulation - Integration Demo" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    // Create integrated agent with environment
    nlm::agent::AgentBrain agent(100, 10, 5.0);
    nlm::environment::Environment environment(100.0, 50.0, 25.0);
    nlm::world::SimpleWorld world(&environment);
    
    std::cout << "Agent initialized with sensory capabilities" << std::endl;
    std::cout << "Environment configured with spatial bounds" << std::endl;
    std::cout << "World system created for agent interaction" << std::endl;
    
    // Demonstrate integration
    auto observation = environment.getObservation();
    agent.sense(observation);
    
    auto action = agent.think();
    environment.applyAction(action);
    
    std::cout << "\nAgent completed integrated sensory-motor cycle" << std::endl;
    std::cout << "Demonstrates successful module integration!" << std::endl;
    
    return 0;
}
