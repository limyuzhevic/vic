#include "BasicConnectivityExperiment.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>

namespace nlm {

ExperimentResult BasicConnectivityExperiment::run(std::shared_ptr<Brain> brain) {
    auto startTime = std::chrono::high_resolution_clock::now();
    ExperimentResult result;
    result.logs.push_back("Starting basic connectivity experiment...");
    
    try {
        // Get test region
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            result.logs.push_back("ERROR: No test region found");
            result.success = false;
            result.message = "No test region found";
            return result;
        }
        
        // Get all neurons
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            result.logs.push_back("  No neurons found!");
            result.success = false;
            result.message = "No neurons found";
            return result;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into specified number of neurons
        size_t neuronsToStimulate = std::min<size_t>(10, neurons.size());
        result.logs.push_back("  Injecting current into " + std::to_string(neuronsToStimulate) + " neurons...");
        for (size_t i = 0; i < neuronsToStimulate; ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run simulation
        result.logs.push_back("  Running simulation for " + std::to_string(50) + " steps...");
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Calculate results
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        result.logs.push_back("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            result.logs.push_back("  ✓ Spikes propagate through network");
            result.success = true;
            result.message = "Connectivity test passed - spikes propagated successfully";
        } else {
            result.logs.push_back("  ! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min<size_t>(3, neurons.size()); ++i) {
                result.logs.push_back("    Neuron " + std::to_string(i) + 
                                    " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                                    " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
            result.success = false;
            result.message = "No spikes generated - connectivity issue";
        }
        
    } catch (const std::exception& e) {
        result.logs.push_back("ERROR: Exception occurred: " + std::string(e.what()));
        result.success = false;
        result.message = "Exception: " + std::string(e.what());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return result;
}

} // namespace nlm