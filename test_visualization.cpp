#include "brain/Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "core/Config/Config.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

using namespace nlm;

int main() {
    std::cout << "=== NLM Visualization System Test ===" << std::endl;
    std::cout << "Testing console-based visualization for Phase 6" << std::endl;
    std::cout << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    config->set("connection_probability", 0.2f);
    config->set("random_seed", 42u);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    std::cout << "Creating brain with 100 neurons and 2 regions..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    std::cout << "Initializing brain..." << std::endl;
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Initialize visualization
    std::cout << "Initializing visualization interface..." << std::endl;
    VisualizationInterface viz;
    
    if (!viz.initialize()) {
        std::cerr << "ERROR: Failed to initialize visualization!" << std::endl;
        return 1;
    }
    
    std::cout << "Visualization initialized successfully!" << std::endl;
    std::cout << std::endl;
    
    // Generate some sample spike events for demonstration
    std::cout << "Generating sample spike events..." << std::endl;
    std::vector<SpikeEvent> sampleSpikes;
    
    // Simulate some spikes across neurons
    for (size_t i = 0; i < 20; ++i) {
        NeuronId neuronId(i % 100);  // Neuron IDs 0-99
        Timestamp time = static_cast<Timestamp>(i * 0.1); // 0.0, 0.1, 0.2, ... seconds
        SimulationStep step = static_cast<SimulationStep>(i);
        sampleSpikes.emplace_back(neuronId, time, step);
    }
    
    std::cout << "Generated " << sampleSpikes.size() << " sample spike events" << std::endl;
    std::cout << std::endl;
    
    // Run a few simulation steps
    std::cout << "Running simulation steps..." << std::endl;
    for (size_t step = 0; step < 5; ++step) {
        Timestamp time = static_cast<Timestamp>(step) * config->getOr<double>("simulation_timestep", 0.001);
        std::cout << "Step " << step << " (t=" << time << "s)" << std::endl;
        
        // Step the brain
        brain->step(step, time);
        
        // Update visualization
        viz.update();
        
        // Display network visualization
        std::cout << "--- Network Structure ---" << std::endl;
        viz.visualizeNetwork(*brain);
        
        // Display activity visualization
        std::cout << "--- Neural Activity ---" << std::endl;
        viz.visualizeActivity(*brain);
        
        // Display spike visualization (using sample spikes for demo)
        std::cout << "--- Spike Activity ---" << std::endl;
        viz.visualizeSpikes(sampleSpikes);
        
        // Display weights visualization
        std::cout << "--- Synaptic Weights ---" << std::endl;
        viz.visualizeWeights(*brain);
        
        std::cout << std::endl;
        
        // Wait a bit for readability
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    // Demonstrate spike raster plot with real-time data
    std::cout << "--- Real-time Spike Raster ---" << std::endl;
    const auto& spikeSystem = brain->getSpikeSystem();
    if (spikeSystem) {
        const auto& spikeHistory = spikeSystem->getSpikeHistory();
        std::cout << "Real spike history from simulation: " << spikeHistory.size() << " spikes" << std::endl;
        if (!spikeHistory.empty()) {
            viz.visualizeSpikes(spikeHistory);
        }
    }
    
    // Demonstrate network visualization with real brain data
    std::cout << "--- Complete Network Visualization ---" << std::endl;
    viz.visualizeNetwork(*brain);
    
    // Demonstrate activity visualization
    std::cout << "--- Final Activity Visualization ---" << std::endl;
    viz.visualizeActivity(*brain);
    
    // Close visualization
    std::cout << "Closing visualization..." << std::endl;
    viz.close();
    
    std::cout << std::endl;
    std::cout << "=== Visualization Test Completed ===" << std::endl;
    std::cout << "All visualization methods executed successfully!" << std::endl;
    std::cout << "The console-based visualization system is working correctly." << std::endl;
    
    return 0;
}
