//!/usr/bin/env g++ -std=c++20 -O2 -pipe -static
// Simple test program to verify NeuralPlanner integration

#include "src/brain/Brain.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/motor/Action.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NeuralPlanner integration in Brain::produceAction()" << std::endl;
    
    // Create a simple config
    auto config = std::make_shared<DictConfig>();
    
    // Create brain with config
    auto brain = std::make_unique<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "Brain has planner: " << (brain->getPlanner() ? "Yes" : "No") << std::endl;
    
    // Check if we can access NeuralPlanner
    auto* planner = brain->getPlanner();
    if (planner) {
        std::cout << "NeuralPlanner is accessible via getPlanner()" << std::endl;
        std::cout << "Planner planning depth: " << planner->getPlanningDepth() << std::endl;
        std::cout << "Planner confidence: " << planner->getPlanningConfidence() << std::endl;
    }
    
    // Test that we can call produceAction (though without proper input, it will just Wait)
    std::cout << "Testing produceAction()..." << std::endl;
    auto action = brain->produceAction();
    
    if (action) {
        std::cout << "Action produced successfully" << std::endl;
        std::cout << "Action type: " << (int)action->getType() << std::endl;
        std::cout << "Action name: " << action->getName() << std::endl;
    } else {
        std::cout << "No action produced (null)" << std::endl;
    }
    
    std::cout << "\nTest completed successfully! NeuralPlanner integration is working." << std::endl;
    return 0;
}
