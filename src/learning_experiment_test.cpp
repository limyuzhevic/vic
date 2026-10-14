#include "learning_experiment.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>

using namespace nlm;

// Learning experiment test - standalone test for LearningExperiment struct
void runLearningExperimentTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment Test Suite ===");
    
    if (!brain) {
        NLM_LOG_ERROR("ERROR: Invalid brain pointer");
        return;
    }
    
    // Test 1: LearningExperiment basic functionality
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 1: LearningExperiment State Recording");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Inject some activity to trigger learning
    NLM_LOG_INFO("  Injecting learning activity...");
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
    
    // Test 2: LearningExperiment with plasticity enabled
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 2: LearningExperiment with Plasticity");
    
    LearningExperiment plasticityExperiment(brain, 43);
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    plasticityExperiment.recordInitialState();
    
    // Apply learning activity
    NLM_LOG_INFO("  Applying learning with plasticity active...");
    for (int step = 0; step < 500; ++step) {
        // Inject current to stimulate learning
        for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    plasticityExperiment.recordFinalState();
    plasticityExperiment.computeStatistics();
    
    // Test 3: LearningExperiment with multiple regions
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 3: LearningExperiment Multi-Region Analysis");
    
    LearningExperiment multiRegionExperiment(brain, 44);
    
    // Record initial state from all regions
    for (auto regionId : brain->getRegionIds()) {
        if (auto* region = brain->getRegion(regionId)) {
            NLM_LOG_INFO("  Initial state - Region " << regionId << ": " 
                        << region->getSynapseCount() << " synapses, " 
                        << region->getNeuronCount() << " neurons");
        }
    }
    
    // Apply activity
    NLM_LOG_INFO("  Applying cross-region activity...")
    for (int step = 0; step < 200; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 50 == 0) {
            NLM_LOG_INFO("    Step " << step << ": " 
                        << brain->getTotalSpikeCount() << " total spikes, " 
                        << brain->getFiringNeuronCount() << " active neurons");
        }
    }
    
    multiRegionExperiment.recordFinalState();
    multiRegionExperiment.computeStatistics();
    
    // Test 4: LearningExperiment edge cases
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 4: LearningExperiment Edge Cases");
    
    // Reset brain for clean test
    brain->reset();
    brain->initialize();
    
    LearningExperiment edgeExperiment(brain, 45);
    
    // Record with empty synapses (early initialization)
    edgeExperiment.recordInitialState();
    
    NLM_LOG_INFO("  Test with minimal initial state (may show empty metrics)");
    
    // Apply minimal activity
    for (int step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    edgeExperiment.recordFinalState();
    edgeExperiment.computeStatistics();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== LearningExperiment Test Suite Complete ===");
}

int main(int argc, char* argv[]) {
    std::cout << "===========================================\n";
    std::cout << "NLM LearningExperiment Test Suite\n";
    std::cout << "===========================================\n\n";
    
    try {
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", size_t(200));
        config->set("region_count", size_t(1));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", uint64_t(42));
        config->set("simulation_timestep", 0.001);
        
        // Create brain
        std::cout << "Initializing NLM brain...\n";
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!\n";
            return 1;
        }
        
        // Run learning experiments
        runLearningExperimentTest(brain);
        
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "LearningExperiment Test Suite Complete!\n";
        std::cout << std::string(60, '=') << "\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}