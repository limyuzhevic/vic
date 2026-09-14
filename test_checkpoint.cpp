// Simple test to verify checkpoint functionality

#include "Brain.hpp"
#include <iostream>
#include <fstream>

int main() {
    std::cout << "Testing checkpoint functionality..." << std::endl;
    
    // Create a brain with minimal configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize the brain
    bool initSuccess = brain.initialize();
    if (!initSuccess) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Test save functionality
    std::string checkpointFile = "./test_checkpoint.dat";
    bool saveSuccess = brain.save(checkpointFile);
    
    if (!saveSuccess) {
        std::cerr << "Failed to save brain state" << std::endl;
        return 1;
    }
    
    std::cout << "Brain checkpoint saved successfully" << std::endl;
    
    // Verify file exists and has content
    std::ifstream file(checkpointFile);
    if (!file.good()) {
        std::cerr << "Checkpoint file not created or empty" << std::endl;
        return 1;
    }
    
    // Get file size to verify it's not empty
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.close();
    
    std::cout << "Checkpoint file size: " << fileSize << " bytes" << std::endl;
    
    if (fileSize == 0) {
        std::cerr << "Checkpoint file is empty" << std::endl;
        return 1;
    }
    
    // Reset brain for load test
    brain.reset();
    
    // Test load functionality
    bool loadSuccess = brain.load(checkpointFile);
    
    if (!loadSuccess) {
        std::cerr << "Failed to load brain state" << std::endl;
        return 1;
    }
    
    std::cout << "Brain checkpoint loaded successfully" << std::endl;
    
    // Verify brain still has expected number of neurons
    size_t neuronCount = brain.getTotalNeuronCount();
    std::cout << "Brain has " << neuronCount << " neurons after load" << std::endl;
    
    if (neuronCount != 10) {
        std::cerr << "Expected 10 neurons but got " << neuronCount << std::endl;
        return 1;
    }
    
    // Test memory system accessors
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory == nullptr) {
        std::cerr << "getWorkingMemory() returned nullptr" << std::endl;
        return 1;
    }
    
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory == nullptr) {
        std::cerr << "getEpisodicMemory() returned nullptr" << std::endl;
        return 1;
    }
    
    auto* associativeMemory = brain.getAssociativeMemory();
    if (associativeMemory == nullptr) {
        std::cerr << "getAssociativeMemory() returned nullptr" << std::endl;
        return 1;
    }
    
    std::cout << "All memory system accessors return valid pointers" << std::endl;
    
    // Clean up test file
    std::remove(checkpointFile.c_str());
    
    std::cout << "\nAll checkpoint tests passed successfully!" << std::endl;
    return 0;
}