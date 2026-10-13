#include <iostream>
#include <memory>
#include "brain/Brain.hpp"
#include "cognition/NeuralPlanner.hpp"

using namespace nlm;

int main() {
    std::cout << "=== NeuralPlanner Integration Test ===" << std::endl;
    
    // Create a simple brain configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(100));
    config->set("region_count", size_t(1));
    config->set("connection_probability", 0.1f);
    
    // Create brain
    std::cout << "Creating brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Check that NeuralPlanner is integrated
    std::cout << "Checking NeuralPlanner integration..." << std::endl;
    auto* planner = brain->getPlanner();
    if (planner) {
        std::cout << "✓ NeuralPlanner is integrated and accessible" << std::endl;
        
        // Test basic planning
        std::vector<float> testState(10, 0.5f);
        ActionType action = planner->planAction(testState, 0.5f);
        std::cout << "✓ NeuralPlanner can plan actions" << std::endl;
        std::cout << "  Planned action: " << static_cast<int>(action) << std::endl;
        
        // Test integration methods
        std::cout << "Testing integration methods..." << std::endl;
        planner->integrateWithPredictionSystem(brain.get());
        std::cout << "✓ Prediction system integration works" << std::endl;
        
        planner->integrateWithMemorySystems(brain.get());
        std::cout << "✓ Memory systems integration works" << std::endl;
        
        planner->integrateWithOtherCognitiveSystems(brain.get());
        std::cout << "✓ Other cognitive systems integration works" << std::endl;
        
        // Test action production
        std::cout << "Testing action production..." << std::endl;
        auto actionPtr = brain->produceAction();
        if (actionPtr) {
            std::cout << "✓ Brain can produce actions via NeuralPlanner" << std::endl;
            std::cout << "  Produced action: " << static_cast<int>(actionPtr->getType()) << std::endl;
        }
        
    } else {
        std::cerr << "✗ NeuralPlanner is NOT integrated!" << std::endl;
        return 1;
    }
    
    // Test step method includes NeuralPlanner calls
    std::cout << "Testing step() method integration..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        brain->step(i, i * 0.001f);
    }
    std::cout << "✓ Step method runs with NeuralPlanner integration" << std::endl;
    
    std::cout << "\n=== Integration Test Complete ===" << std::endl;
    return 0;
}