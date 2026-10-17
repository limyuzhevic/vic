#include "src/visualization/VisualizationInterface.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

int main() {
    std::cout << Colors::CYAN << "=== Testing Visualization Interface ===" << Colors::RESET << std::endl;
    
    // Create a simple configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100, ConfigSource::Default);
    config->set("region_count", 3, ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Initialize visualization interface
    VisualizationInterface viz;
    if (!viz.initialize()) {
        std::cerr << "Failed to initialize visualization" << std::endl;
        return 1;
    }
    
    std::cout << "Visualization initialized successfully" << std::endl;
    std::cout << "Active: " << (viz.isActive() ? "Yes" : "No") << std::endl;
    
    // Test visualization methods
    viz.setUpdateRate(10.0); // 10 Hz update rate
    
    std::cout << "Update rate: " << viz.getUpdateRate() << " Hz" << std::endl;
    
    // Generate some test spikes
    std::vector<SpikeEvent> testSpikes;
    for (int i = 0; i < 10; ++i) {
        testSpikes.emplace_back(NeuronId(i), 0.1f * i, i);
    }
    
    std::cout << "Testing spike visualization..." << std::endl;
    viz.visualizeSpikes(testSpikes);
    
    std::cout << "Testing network visualization..." << std::endl;
    viz.visualizeNetwork(*brain);
    
    std::cout << "Testing activity visualization..." << std::endl;
    viz.visualizeActivity(*brain);
    
    std::cout << "Testing weight visualization..." << std::endl;
    viz.visualizeWeights(*brain);
    
    // Update visualization
    std::cout << "Updating visualization..." << std::endl;
    viz.update();
    
    // Export data
    std::cout << "Testing data export..." << std::endl;
    viz.exportData("test_data", "{\"test\": \"data\"}", "json");
    
    // Close visualization
    std::cout << "Closing visualization..." << std::endl;
    viz.close();
    
    std::cout << Colors::GREEN << "Visualization test completed successfully!" << Colors::RESET << std::endl;
    
    return 0;
}
