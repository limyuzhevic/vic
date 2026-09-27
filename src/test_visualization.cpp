#include "visualization/VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "NLM VisualizationInterface Test" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Create a simple configuration
    auto config = std::make_shared<nlm::Config>();
    config->simulationSteps = 1000;
    config->maxNeurons = 1000;
    config->maxSynapses = 10000;
    
    // Create brain
    auto brain = std::make_unique<nlm::Brain>(config);
    
    // Create visualization interface
    auto visualization = std::make_unique<nlm::VisualizationInterface>();
    
    // Test initialization
    std::cout << "Initializing visualization..." << std::endl;
    if (!visualization->initialize()) {
        std::cerr << "Failed to initialize visualization!" << std::endl;
        return 1;
    }
    
    std::cout << "Visualization initialized successfully." << std::endl;
    
    // Test configuration
    visualization->setUpdateRate(60.0);
    visualization->setShowNetwork(true);
    visualization->setShowActivity(true);
    visualization->setShowSpikes(true);
    visualization->setShowWeights(true);
    visualization->setView(0); // Network view
    visualization->setNodeSize(3.0f);
    visualization->setConnectionWidth(0.5f);
    
    std::cout << "Visualization configured." << std::endl;
    std::cout << "Update rate: " << visualization->getUpdateRate() << " Hz" << std::endl;
    std::cout << "Current view: " << visualization->getCurrentView() << std::endl;
    std::cout << "Node size: " << visualization->getNodeSize() << std::endl;
    
    // Main loop (simulate for a few seconds)
    std::cout << "Starting visualization loop... (Press Ctrl+C to stop)" << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    double elapsed = 0.0;
    int frameCount = 0;
    
    while (visualization->isActive() && elapsed < 10.0) {
        visualization->update();
        visualization->render();
        
        frameCount++;
        
        // Check elapsed time
        auto currentTime = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration<double>(currentTime - startTime).count();
        
        // Sleep to maintain target frame rate
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 Hz
    }
    
    // Test data visualization functions
    std::cout << "\nTesting visualization functions..." << std::endl;
    
    // Create dummy spike data
    std::vector<nlm::SpikeEvent> spikes;
    for (int i = 0; i < 100; i++) {
        nlm::SpikeEvent spike;
        spike.source_neuron = nlm::NeuronId(i % 50);
        spike.timestamp = static_cast<double>(i) * 0.1;
        spike.step = i;
        spikes.push_back(spike);
    }
    
    visualization->visualizeSpikes(spikes);
    std::cout << "Spikes visualization completed (" << spikes.size() << " spikes)" << std::endl;
    
    // Test with brain (if brain has data)
    visualization->visualizeNetwork(*brain);
    visualization->visualizeActivity(*brain);
    visualization->visualizeWeights(*brain);
    
    std::cout << "All visualization functions completed successfully." << std::endl;
    
    // Cleanup
    visualization->close();
    
    std::cout << "\nVisualizationInterface test completed successfully!" << std::endl;
    return 0;
}
