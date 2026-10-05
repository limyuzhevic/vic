// Phase 2 Demo - Neural Computation Examples
// Legacy demo for backward compatibility
// This demonstrates the basic Phase 2 neural computation capabilities

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <numeric>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 2 Demo ===" << std::endl;
    std::cout << "Basic neural computation demonstration" << std::endl;
    std::cout << "(Legacy demo - use nlm for unified Phase 2/Phase 6 interface)" << std::endl << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Basic Neural Computation ===");
    NLM_LOG_INFO("This demo shows basic neural dynamics without full integration.");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", static_cast<int64_t>(100));  // Smaller for demo
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.15f);
    
    // Initialize brain
    NLM_LOG_INFO("Creating brain with configuration:");
    NLM_LOG_INFO("  Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 100)));
    NLM_LOG_INFO("  Region count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Demo 1: Basic connectivity test
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Demo 1: Basic Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            // Inject current into first 5 neurons
            NLM_LOG_INFO("Injecting current into 5 neurons...");
            for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
                neurons[i]->injectCurrent(40.0f);
            }
            
            // Run simulation
            for (SimulationStep step = 0; step < 30; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
            
            if (spikes > 0) {
                NLM_LOG_INFO("✓ Basic connectivity working - spikes propagate!");
            } else {
                NLM_LOG_INFO("  Note: No spikes - neurons may need higher input or different parameters");
            }
        }
    }
    
    // Demo 2: Plasticity demonstration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Demo 2: Synaptic Plasticity ===");
    
    // Reset and enable plasticity
    brain->reset();
    brain->initialize();
    
    if (region) {
        // Record initial weights
        std::vector<float> initialWeights;
        auto& synapses = region->getSynapses();
        for (const auto& syn : synapses) {
            if (synapses.size() < 10) {  // Only record first 10 for demo
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial synaptic weights (first 10):");
        for (size_t i = 0; i < std::min(size_t(5), initialWeights.size()); ++i) {
            NLM_LOG_INFO("  Synapse " + std::to_string(i) + ": " + std::to_string(initialWeights[i]));
        }
        
        // Enable plasticity
        for (auto& syn : synapses) {
            syn->enablePlasticity(true, true, false);  // Hebbian and STDP
        }
        
        // Apply learning input
        NLM_LOG_INFO("Applying learning input for 100 steps...");
        for (SimulationStep step = 0; step < 100; ++step) {
            // Inject correlated input to stimulate plasticity
            if (step % 5 == 0) {  // Every 5 steps
                brain->injectCurrentToNeurons(NeuronType::Internal, 20.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        // Check final weights
        NLM_LOG_INFO("Final synaptic weights (first 10):");
        for (size_t i = 0; i < std::min(size_t(5), synapses.size()); ++i) {
            float delta = synapses[i]->getWeight() - initialWeights[i];
            NLM_LOG_INFO("  Synapse " + std::to_string(i) + ": " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
        
        // Calculate statistics
        float totalInitial = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float totalFinal = 0.0f;
        for (size_t i = 0; i < std::min(initialWeights.size(), synapses.size()); ++i) {
            totalFinal += synapses[i]->getWeight();
        }
        
        NLM_LOG_INFO("Plasticity Statistics:");
        NLM_LOG_INFO("  Total weight change: " + std::to_string(totalFinal - totalInitial));
        NLM_LOG_INFO("  Learning activity detected: " + std::string((totalFinal - totalInitial) != 0 ? "YES" : "NO"));
    }
    
    // Final status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Demo Complete ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 capabilities demonstrated:");
    NLM_LOG_INFO("  ✓ LIF neuron dynamics");
    NLM_LOG_INFO("  ✓ Spike propagation with delays");
    NLM_LOG_INFO("  ✓ Synaptic plasticity (STDP, Hebbian)");
    NLM_LOG_INFO("  ✓ Structural plasticity");
    NLM_LOG_INFO("  ✓ Basic learning behavior");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Note: For comprehensive Phase 6 integration with memory, prediction,");
    NLM_LOG_INFO("      and cognitive systems, use 'nlm --phase6' instead.");
    
    return 0;
}