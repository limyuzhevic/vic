// Basic connectivity demo - Test neural signal propagation

#include "demos/BasicConnectivityDemo.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

namespace nlm {

DemoResult BasicConnectivityDemo::run(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Basic Connectivity Demo ===");
    
    DemoResult result;
    result.success = true;
    result.message = "Basic connectivity test completed";
    
    // Get neurons and test connectivity
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        result.success = false;
        result.message = "No brain regions found";
        return result;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        result.success = false;
        result.message = "No neurons found";
        return result;
    }
    
    // Record initial state
    size_t initialSpikes = brain->getTotalSpikeCount();
    NLM_LOG_INFO("Initial spike count: " + std::to_string(initialSpikes));
    
    // Inject current into first 10 neurons
    NLM_LOG_INFO("Injecting current into first 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
        reportProgress("Injecting neurons", i + 1, std::min(size_t(10), neurons.size()));
    }
    
    // Run simulation steps
    size_t stepsToRun = config->getOr<size_t>("steps", 50);
    NLM_LOG_INFO("Running " + std::to_string(stepsToRun) + " simulation steps...");
    
    for (SimulationStep step = 0; step < stepsToRun; ++step) {
        brain->step(step, step * 0.001);
        reportProgress("Simulating", step + 1, stepsToRun);
    }
    
    // Calculate results
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t spikesGenerated = finalSpikes - initialSpikes;
    
    result.durationSteps = stepsToRun;
    result.metrics["initial_spikes"] = std::to_string(initialSpikes);
    result.metrics["final_spikes"] = std::to_string(finalSpikes);
    result.metrics["spikes_generated"] = std::to_string(spikesGenerated);
    
    NLM_LOG_INFO("Final spike count: " + std::to_string(finalSpikes));
    NLM_LOG_INFO("Spikes generated: " + std::to_string(spikesGenerated));
    
    if (spikesGenerated > 0) {
        NLM_LOG_INFO("✓ SUCCESS: Neural signals propagated through the network");
        result.message = "Neural connectivity verified - " + std::to_string(spikesGenerated) + " spikes generated";
    } else {
        NLM_LOG_WARNING("! WARNING: No spikes detected - checking neuron parameters");
        result.success = false;
        result.message = "No spikes generated despite current injection";
        
        // Log neuron parameters for debugging
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
    
    return result;
}

void BasicConnectivityDemo::printInfo() const {
    std::cout << "Basic Connectivity Demo" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "Tests basic neural signal propagation through the network." << std::endl;
    std::cout << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Injects current into sensory neurons" << std::endl;
    std::cout << "  - Measures spike propagation" << std::endl;
    std::cout << "  - Reports network connectivity" << std::endl;
    std::cout << std::endl;
}