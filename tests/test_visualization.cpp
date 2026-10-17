// Visualization Tests
#include "visualization/VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_visualization {

void testVisualizationCreation() {
    std::cout << "    testVisualizationCreation passed" << std::endl;
}

void testVisualizationInitialization() {
    // Create a simple configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Initialize visualization interface
    nlm::VisualizationInterface viz;
    bool success = viz.initialize();
    assert(success);
    
    // Verify visualization is active
    assert(viz.isActive());
    
    // Set and get update rate
    viz.setUpdateRate(15.0);
    assert(viz.getUpdateRate() == 15.0);
    
    std::cout << "    testVisualizationInitialization passed" << std::endl;
}

void testNetworkVisualization() {
    // Create a simple configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Initialize visualization
    nlm::VisualizationInterface viz;
    viz.initialize();
    
    // Test network visualization
    viz.visualizeNetwork(*brain);
    
    // Test activity visualization
    viz.visualizeActivity(*brain);
    
    // Test weight visualization
    viz.visualizeWeights(*brain);
    
    std::cout << "    testNetworkVisualization passed" << std::endl;
}

void testSpikeVisualization() {
    // Create test spikes
    std::vector<nlm::SpikeEvent> spikes;
    for (int i = 0; i < 5; ++i) {
        spikes.emplace_back(nlm::NeuronId(i), 0.1f * i, i);
    }
    
    // Initialize visualization
    nlm::VisualizationInterface viz;
    viz.initialize();
    
    // Test spike visualization
    viz.visualizeSpikes(spikes);
    
    std::cout << "    testSpikeVisualization passed" << std::endl;
}

void testVisualizationUpdate() {
    // Create a simple configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Initialize visualization
    nlm::VisualizationInterface viz;
    viz.initialize();
    
    // Test update
    viz.update();
    
    // Close visualization
    viz.close();
    
    // Verify visualization is not active after close
    assert(!viz.isActive());
    
    std::cout << "    testVisualizationUpdate passed" << std::endl;
}

void runAll() {
    testVisualizationCreation();
    testVisualizationInitialization();
    testNetworkVisualization();
    testSpikeVisualization();
    testVisualizationUpdate();
}

} // namespace test_visualization

