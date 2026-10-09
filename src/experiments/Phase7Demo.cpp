// Phase 7 Demo - Advanced Expert Features
//
// This demo showcases advanced expert features for the NLM brain system:
// - Brain inspection and debugging tools
// - Memory system analysis
// - Neuromodulator control
// - Development stage management
// - Advanced configuration options
// - Performance profiling tools

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "development/DevelopmentSystem.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "development/Maturation.hpp"
#include "development/Pruning.hpp"
#include "development/Synaptogenesis.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <memory>

using namespace nlm;

// Forward declarations for expert functions
void expertBrainInspection(std::shared_ptr<Brain> brain);
void expertMemoryAnalysis(std::shared_ptr<Brain> brain);
void expertNeuromodulatorControl(std::shared_ptr<Brain> brain, const Config& config);
void expertDevelopmentStageManagement(std::shared_ptr<Brain> brain, uint64_t targetStage);
void expertPerformanceProfiling(std::shared_ptr<Brain> brain, uint64_t steps);
void expertConfigurationDisplay(const Config& config);

// Simplified expert functions for demo purposes
void expertBrainInspection(std::shared_ptr<Brain> brain) {
    std::cout << "\n=== Brain Inspection ===" << std::endl;
    std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
}

void expertMemoryAnalysis(std::shared_ptr<Brain> brain) {
    std::cout << "\n=== Memory Analysis ===" << std::endl;
    if (auto* workingMem = brain->getWorkingMemory()) {
        std::cout << "Working memory: Active" << std::endl;
    }
    if (auto* episodicMem = brain->getEpisodicMemory()) {
        std::cout << "Episodic memory: Active" << std::endl;
    }
    if (auto* semanticMem = brain->getSemanticMemory()) {
        std::cout << "Semantic memory: Active" << std::endl;
    }
}

void expertNeuromodulatorControl(std::shared_ptr<Brain> brain, const Config& config) {
    std::cout << "\n=== Neuromodulator Control ===" << std::endl;
    std::cout << "Dopamine level: " << config.getOr<float>("dopamine_level", 0.0f) << std::endl;
    std::cout << "Curiosity level: " << config.getOr<float>("curiosity_level", 0.0f) << std::endl;
    std::cout << "Prediction error level: " << config.getOr<float>("prediction_error_level", 0.0f) << std::endl;
    std::cout << "Novelty level: " << config.getOr<float>("novelty_level", 0.0f) << std::endl;
}

void expertDevelopmentStageManagement(std::shared_ptr<Brain> brain, uint64_t targetStage) {
    std::cout << "\n=== Development Stage Management ===" << std::endl;
    std::cout << "Target stage: " << targetStage << std::endl;
    if (auto* devSystem = brain->getDevelopmentSystem()) {
        std::cout << "Development system: Active" << std::endl;
    }
}

void expertPerformanceProfiling(std::shared_ptr<Brain> brain, uint64_t steps) {
    std::cout << "\n=== Performance Profiling (" << steps << " steps) ===" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (uint64_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Duration: " << duration.count() << "ms" << std::endl;
}

void expertConfigurationDisplay(const Config& config) {
    std::cout << "\n=== Configuration Display ===" << std::endl;
    std::vector<std::string> keys = config.getKeys();
    for (const auto& key : keys) {
        auto value = config.get<std::string>(key);
        if (value) {
            std::cout << "  " << key << " = " << *value << std::endl;
        }
    }
}

void runExpertBrainInspectionDemo() {
    std::cout << "=== Phase 7A: Expert Brain Inspection Demo ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Inject some activity
    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
    
    // Run a few steps
    for (uint64_t step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Expert brain inspection
    expertBrainInspection(brain);
    
    // Memory system analysis
    expertMemoryAnalysis(brain);
}

void runExpertNeuromodulatorControlDemo() {
    std::cout << "\n=== Phase 7B: Expert Neuromodulator Control Demo ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 300);
    config->set("dopamine_level", 0.5f);
    config->set("curiosity_level", 0.8f);
    config->set("prediction_error_level", 0.3f);
    config->set("novelty_level", 0.6f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Apply neuromodulator control
    expertNeuromodulatorControl(brain, *config);
    
    // Inject activity and test neuromodulation effects
    for (uint64_t step = 0; step < 200; ++step) {
        // Create interesting patterns to trigger neuromodulators
        if (step % 50 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
        }
        
        if (step % 75 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Motor, 25.0f);
        }
        
        brain->step(step, step * 0.001);
    }
    
    expertBrainInspection(brain);
}

void runExpertDevelopmentStageDemo() {
    std::cout << "\n=== Phase 7C: Expert Development Stage Management Demo ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 250);
    config->set("synaptogenesis_rate", 0.001f);
    config->set("pruning_rate", 0.0001f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Test development stage management
    expertDevelopmentStageManagement(brain, 1); // Critical period
    
    // Run development simulation
    for (uint64_t step = 0; step < 300; ++step) {
        if (step % 50 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 35.0f);
        }
        
        brain->step(step, step * 0.001);
        brain->develop(); // Apply development
    }
    
    expertBrainInspection(brain);
    
    // Test different developmental stages
    for (int stage = 0; stage < 5; ++stage) {
        std::cout << "\n--- Testing Developmental Stage " << stage << " ---" << std::endl;
        expertDevelopmentStageManagement(brain, stage);
        brain->reset();
        brain->initialize();
    }
}

void runExpertPerformanceProfilingDemo() {
    std::cout << "\n=== Phase 7D: Expert Performance Profiling Demo ===" << std::endl;
    
    // Test different brain sizes for performance profiling
    std::vector<int> sizes = {100, 200, 500};
    
    for (size_t sizeIdx = 0; sizeIdx < sizes.size(); ++sizeIdx) {
        int size = sizes[sizeIdx];
        std::cout << "\n--- Performance Test with " << size << " neurons ---" << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", size);
        config->set("region_count", 2);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            continue;
        }
        
        // Profile performance
        expertPerformanceProfiling(brain, size * 2);
    }
}

void runExpertConfigurationDemo() {
    std::cout << "\n=== Phase 7E: Expert Configuration Management Demo ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Set advanced plasticity parameters
    config->set("stdp_ltp_weight", 0.05f);
    config->set("stdp_ltd_weight", 0.04f);
    config->set("stdp_time_constant", 15.0f);
    config->set("plasticity_learning_rate", 0.05f);
    
    // Set development trajectory control
    config->set("development_synaptogenesis_rate", 0.002f);
    config->set("development_pruning_rate", 0.0005f);
    config->set("development_sensitivity", 1.2f);
    
    // Set neuromodulation parameters
    config->set("dopamine_baseline", 0.2f);
    config->set("curiosity_baseline", 0.7f);
    config->set("novelty_threshold", 0.3f);
    
    // Display full configuration
    expertConfigurationDisplay(*config);
    
    // Create brain with advanced configuration
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "\nBrain initialized with advanced configuration:" << std::endl;
    expertBrainInspection(brain);
}

void runExpertIntegrationDemo() {
    std::cout << "\n=== Phase 7F: Expert Integration Demo ===" << std::endl;
    
    // Create advanced Phase 6 experiment configuration
    Phase6Config config;
    config.neuronCount = 400;
    config.maxSteps = 1000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.regionCount = 3;
    config.connectionProbability = 0.15f;
    
    Phase6IntegratedExperiment experiment;
    
    std::cout << "Running Phase 6 integration with expert features..." << std::endl;
    auto result = experiment.run(config);
    
    std::cout << "\n=== Integration Results ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 7 Advanced Expert Features Demo ===" << std::endl;
    std::cout << "Demonstrating advanced expert features for NLM Phase 6+ integration" << std::endl;
    
    try {
        // Run all expert feature demos
        runExpertBrainInspectionDemo();
        runExpertNeuromodulatorControlDemo();
        runExpertDevelopmentStageDemo();
        runExpertPerformanceProfilingDemo();
        runExpertConfigurationDemo();
        runExpertIntegrationDemo();
        
        std::cout << "\n=== Phase 7 Complete ===" << std::endl;
        std::cout << "All advanced expert features demonstrated successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
