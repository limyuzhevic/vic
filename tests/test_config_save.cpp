// Test for Config.saveToFile fix
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_save_to_file {
    
    void testSaveToFile() {
        nlm::Config config;
        
        // Set some configuration values
        config.set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
        config.set("brain.synapse_density", 0.1f, nlm::ConfigSource::Runtime);
        config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Runtime);
        config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
        config.set("learning.rate", 0.01f, nlm::ConfigSource::Runtime);
        
        // Test saving to file
        std::string test_file = "test_config_output.cfg";
        
        bool save_result = config.saveToFile(test_file);
        assert(save_result);
        
        // Read back the file and verify content
        std::ifstream file(test_file);
        assert(file.is_open());
        
        std::string line;
        bool found_neuron_count = false;
        bool found_synapse_density = false;
        bool found_random_seed = false;
        bool found_simulation_timestep = false;
        bool found_learning_rate = false;
        
        while (std::getline(file, line)) {
            // Skip comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Check for our configuration values
            if (line.find("brain.neuron_count = 1000") != std::string::npos) {
                found_neuron_count = true;
            }
            if (line.find("brain.synapse_density = 0.1") != std::string::npos) {
                found_synapse_density = true;
            }
            if (line.find("random_seed = 42") != std::string::npos) {
                found_random_seed = true;
            }
            if (line.find("simulation_timestep = 0.001") != std::string::npos) {
                found_simulation_timestep = true;
            }
            if (line.find("learning.rate = 0.01") != std::string::npos) {
                found_learning_rate = true;
            }
        }
        
        file.close();
        
        // Clean up
        std::remove(test_file.c_str());
        
        // Verify all values were saved correctly
        assert(found_neuron_count);
        assert(found_synapse_density);
        assert(found_random_seed);
        assert(found_simulation_timestep);
        assert(found_learning_rate);
        
        std::cout << "    testSaveToFile passed" << std::endl;
    }
    
    void runAll() {
        testSaveToFile();
    }
}

// Main function for the test
int main() {
    std::cout << "=== Config.saveToFile Test ===" << std::endl;
    test_save_to_file::runAll();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
