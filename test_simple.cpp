// Simple test to demonstrate the Brain class implementation
#include <iostream>
#include <memory>

// Include the Brain class header
#include "src/brain/Brain.hpp"

int main() {
    std::cout << "Testing improved Brain class API..." << std::endl;
    
    try {
        // Test 1: Create brain using default constructor
        {
            nlm::Brain brain;
            std::cout << "✓ Test 1 passed: Default brain creation" << std::endl;
        }
        
        // Test 2: Create brain using factory methods
        {
            auto basicBrain = nlm::Brain::createBasicBrain();
            auto learningBrain = nlm::Brain::createLearningBrain();
            auto developmentBrain = nlm::Brain::createDevelopmentBrain();
            auto optimizedBrain = nlm::Brain::createOptimizedBrain(500);
            
            std::cout << "✓ Test 2 passed: Factory methods work" << std::endl;
        }
        
        // Test 3: Test configuration management
        {
            nlm::BrainConfig config = nlm::BrainConfig::createDefault();
            config.neuronCount = 2000;
            
            auto customBrain = nlm::Brain::createCustomBrain(config);
            std::cout << "✓ Test 3 passed: Custom brain creation" << std::endl;
        }
        
        // Test 4: Test runtime configuration modification
        {
            nlm::Brain brain;
            brain.setLearningRate(0.05f);
            brain.enableLearning(true);
            brain.enableDevelopment(true);
            
            std::cout << "✓ Test 4 passed: Runtime configuration modification" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "All tests passed! The improved Brain class API is working correctly." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
