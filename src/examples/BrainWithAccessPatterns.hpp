#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

class Config;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralRegion;

// Example class demonstrating new access patterns and utilities
class BrainWithAccessPatterns {
public:
    BrainWithAccessPatterns(std::shared_ptr<Config> config);
    
    // Initialize using SystemInitializer
    bool initialize();
    
    // Demonstrate new access patterns
    void demonstrateAccessPatterns();
    
    // Example of creating and initializing a neural system
    template<typename T>
    std::shared_ptr<T> createAndInitializeSystem(const std::string& systemName);
    
private:
    bool initializeWorkingMemory(std::shared_ptr<Config> config);
    bool initializeEpisodicMemory(std::shared_ptr<Config> config);
    std::vector<float> getCurrentActivations();
    void analyzeBrainState();
    
    std::vector<std::unique_ptr<NeuralRegion>> regions_;
    std::shared_ptr<Config> config_;
};

} // namespace nlm
