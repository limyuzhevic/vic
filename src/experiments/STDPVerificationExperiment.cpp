#include "STDPVerificationExperiment.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <numeric>

namespace nlm {

ExperimentResult StdpVerificationExperiment::run(std::shared_ptr<Brain> brain) {
    auto startTime = std::chrono::high_resolution_clock::now();
    ExperimentResult result;
    result.logs.push_back("Starting STDP verification experiment...");
    
    try {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            result.logs.push_back("ERROR: No test region found");
            result.success = false;
            result.message = "No test region found";
            return result;
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            result.logs.push_back("  Not enough synapses for STDP test");
            result.success = false;
            result.message = "Not enough synapses for STDP test";
            return result;
        }
        
        result.logs.push_back("  Testing STDP on " + std::to_string(std::min<size_t>(5, synapses.size())) + " synapses:");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            result.logs.push_back("    Synapse " + std::to_string(i) + 
                                " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        result.logs.push_back("");
        result.logs.push_back("  Creating correlated pre->post activity (potentiation)...");
        
        auto neurons = region->getAllNeurons();
        
        for (int trial = 0; trial < 50; ++trial) {
            // Fire pre-synaptic neuron
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights
        result.logs.push_back("  After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            result.logs.push_back("    Synapse " + std::to_string(i) + 
                                " after: " + std::to_string(synapses[i]->getWeight()) +
                                " (Δ=" + std::to_string(delta) + ")");
        }
        
        // Check if weights increased (LTP)
        result.logs.push_back("");
        if (totalDelta > 0.001f) {
            result.logs.push_back("  ✓ STDP WORKING: Pre-before-post produced potentiation");
            result.success = true;
            result.message = "STDP verification successful - LTP detected";
        } else if (totalDelta < -0.001f) {
            result.logs.push_back("  ! STDP reversed: Check parameters");
            result.success = false;
            result.message = "STDP showed LTD instead of expected LTP";
        } else {
            result.logs.push_back("  ! No change: STDP may not be triggering");
            result.success = false;
            result.message = "STDP verification failed - no significant weight change";
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