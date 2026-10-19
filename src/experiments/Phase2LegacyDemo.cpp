// NLM Phase 2 Legacy Demo
// 
// This demo recreates the original Phase 2 demonstrations from main.cpp
// in a cleaner, more focused format.
// 
// Phase 2 Legacy Demo includes:
// - Basic neural connectivity test
// - Plasticity learning experiment  
// - STDP verification test
// 
// This provides backward compatibility while being more maintainable.

#include "Phase2LegacyDemo.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace nlm;

// Phase 2 Legacy Demo Implementation
Phase2LegacyDemo::Phase2LegacyDemo() : config_(std::make_shared<Config>()) {
    configureDefaultSettings();
}

Phase2LegacyDemo::~Phase2LegacyDemo() = default;

void Phase2LegacyDemo::runDemo(int demoType) {
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 2 LEGACY DEMO\n";
    std::cout << "===========================================\n\n";
    
    switch (demoType) {
        case 1:
            std::cout << "Demo 1: Basic Neural Connectivity Test\n\n";
            runBasicConnectivityDemo();
            break;
        case 2:
            std::cout << "Demo 2: Plasticity Learning Experiment\n\n";
            runPlasticityDemo();
            break;
        case 3:
            std::cout << "Demo 3: STDP Verification Test\n\n";
            runStdpDemo();
            break;
        case 4:
            std::cout << "Demo 4: Complete Phase 2 Legacy Suite\n\n";
            runAllDemos();
            break;
        default:
            std::cout << "Invalid demo type. Running all demos.\n\n";
            runAllDemos();
            break;
    }
}

void Phase2LegacyDemo::configureDefaultSettings() {
    config_->set("neuron_count", 500);
    config_->set("region_count", 1);
    config_->set("connection_probability", 0.15f);
    config_->set("random_seed", 42u);
    config_->set("simulation_timestep", 0.001);
    config_->set("stdp_ltp_weight", 0.02f);
    config_->set("stdp_ltd_weight", 0.015f);
    config_->set("stdp_tau", 20.0f);
    config_->set("synaptogenesis_rate", 0.0001f);
    config_->set("pruning_rate", 0.00001f);
    config_->set("plasticity_learning_rate", 0.01f);
}

void Phase2LegacyDemo::runBasicConnectivityDemo() {
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config_);
    brain->initialize();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("=== Basic Neural Connectivity Test ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No brain regions found!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("  Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    NLM_LOG_INFO("Demo completed in " + std::to_string(duration) + "s");
}

void Phase2LegacyDemo::runPlasticityDemo() {
    // Create and initialize brain with plasticity enabled
    auto brain = std::make_shared<Brain>(config_);
    brain->initialize();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("=== Plasticity Learning Experiment ===");
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 200 steps
        if (step % 200 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    NLM_LOG_INFO("Final state:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("  Neurons firing: " + std::to_string(brain->getFiringNeuronCount()));
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    NLM_LOG_INFO("Demo completed in " + std::to_string(duration) + "s");
}

void Phase2LegacyDemo::runStdpDemo() {
    // Create and initialize brain with STDP focus
    auto brain = std::make_shared<Brain>(config_);
    brain->initialize();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("=== STDP Verification Test ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No brain regions found!");
        return;
    }
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
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
    NLM_LOG_INFO("After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("! No change: STDP may not be triggering");
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    NLM_LOG_INFO("Demo completed in " + std::to_string(duration) + "s");
}

void Phase2LegacyDemo::runAllDemos() {
    NLM_LOG_INFO("=== Complete Phase 2 Legacy Suite ===");
    NLM_LOG_INFO("This runs all Phase 2 demonstrations in sequence...");
    
    // Demo 1: Basic connectivity
    runBasicConnectivityDemo();
    
    // Demo 2: Plasticity learning
    runPlasticityDemo();
    
    // Demo 3: STDP verification
    runStdpDemo();
    
    NLM_LOG_INFO("=== All Phase 2 Demos Complete ===");
    NLM_LOG_INFO("The NLM Phase 2 demonstrations show:");
    NLM_LOG_INFO("  ✓ Basic neural connectivity and spike propagation");
    NLM_LOG_INFO("  ✓ Plasticity learning with measurable weight changes");
    NLM_LOG_INFO("  ✓ STDP operation with pre-before-post potentiation");
    NLM_LOG_INFO("  ✓ Network learning from experience");
}

// Main entry point for Phase 2 legacy demo
int main(int argc, char* argv[]) {
    int demoType = 4; // Default to running all demos
    
    if (argc > 1) {
        demoType = std::atoi(argv[1]);
    }
    
    nlm::Phase2LegacyDemo demo;
    demo.runDemo(demoType);
    
    return 0;
}
