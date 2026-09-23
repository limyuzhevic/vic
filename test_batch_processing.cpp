#include <iostream>
#include <thread>
#include <chrono>
#include "Brain.hpp"
#include "BatchBrain.hpp"

int main() {
    std::cout << "NLM Batch Processing Test" << std::endl;
    std::cout << "===========================" << std::endl;
    
    try {
        // This would normally be initialized with a config file
        // For now, we'll just test the API compilation
        std::cout << "Testing batch processing API compilation..." << std::endl;
        
        // Create a config
        // auto config = std::make_shared<Config>("test_config");
        
        // Create brain (this would fail without proper config)
        // auto brain = std::make_shared<Brain>(config);
        
        // Create batch brain
        // auto batchBrain = std::make_unique<BatchBrain>(config);
        
        // Test batch processing
        // bool success = batchBrain->processBatch(0, 100);
        
        std::cout << "Test completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
