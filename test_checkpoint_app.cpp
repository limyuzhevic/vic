// Checkpoint test application
// Tests the checkpoint save/load functionality

#include "Brain.hpp"
#include <fstream>
#include <iostream>
#include <cassert>

class CheckpointTest {
public:
    void runTests() {
        std::cout << "Running checkpoint tests..." << std::endl;
        
        // Create a brain with test configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        // Initialize the brain
        bool initSuccess = brain.initialize();
        assert(initSuccess == true);
        
        std::cout << "Brain initialized successfully" << std::endl;
        
        // Run simulation for a few steps to create some activity
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
        
        std::cout << "Brain simulation completed" << std::endl;
        
        // Test save functionality
        std::string checkpointFile = "./test_checkpoint.dat";
        bool saveSuccess = brain.save(checkpointFile);
        assert(saveSuccess == true);
        
        std::cout << "Brain checkpoint saved successfully" << std::endl;
        
        // Verify file exists
        std::ifstream file(checkpointFile);
        assert(file.good());
        file.close();
        
        // Reset brain for load test
        brain.reset();
        
        // Test load functionality
        bool loadSuccess = brain.load(checkpointFile);
        assert(loadSuccess == true);
        
        std::cout << "Brain checkpoint loaded successfully" << std::endl;
        
        // Verify brain still has neurons after load
        assert(brain.getTotalNeuronCount() == 100);
        
        // Test memory system accessors
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory != nullptr);
        
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        auto* associativeMemory = brain.getAssociativeMemory();
        assert(associativeMemory != nullptr);
        
        std::cout << "All memory system accessors return valid pointers" << std::endl;
        
        // Clean up
        std::remove(checkpointFile.c_str());
        
        std::cout << "\nAll checkpoint tests passed!" << std::endl;
    }
};

int main() {
    CheckpointTest test;
    test.runTests();
    return 0;
}