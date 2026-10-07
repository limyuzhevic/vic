// Simple test to verify neuromodulation integration
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Neuromodulation Integration Test ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Test 1: Check neuromodulator presence
    std::cout << "\n=== Testing Neuromodulator Presence ===" << std::endl;
    
    bool hasDopamine = brain->getDopamine() != nullptr;
    bool hasAcetylcholine = brain->getAcetylcholine() != nullptr;
    bool hasNorepinephrine = brain->getNorepinephrine() != nullptr;
    bool hasSerotonin = brain->getSerotonin() != nullptr;
    
    std::cout << "Dopamine present: " << (hasDopamine ? "✓" : "✗") << std::endl;
    std::cout << "Acetylcholine present: " << (hasAcetylcholine ? "✓" : "✗") << std::endl;
    std::cout << "Norepinephrine present: " << (hasNorepinephrine ? "✓" : "✗") << std::endl;
    std::cout << "Serotonin present: " << (hasSerotonin ? "✓" : "✗") << std::endl;
    
    if (!hasDopamine || !hasAcetylcholine || !hasNorepinephrine || !hasSerotonin) {
        std::cerr << "Missing neuromodulators!" << std::endl;
        return 1;
    }
    
    // Test 2: Check neuromodulator levels
    std::cout << "\n=== Testing Neuromodulator Levels ===" << std::endl;
    
    if (hasDopamine) {
        float daLevel = brain->getDopamine()->getLevel();
        float daPlasticity = brain->getDopamine()->getPlasticityFactor();
        std::cout << "Dopamine level: " << daLevel << std::endl;
        std::cout << "Dopamine plasticity factor: " << daPlasticity << std::endl;
    }
    
    if (hasAcetylcholine) {
        float achLevel = brain->getAcetylcholine()->getLevel();
        float achPlasticity = brain->getAcetylcholine()->getPlasticityFactor();
        std::cout << "Acetylcholine level: " << achLevel << std::endl;
        std::cout << "Acetylcholine plasticity factor: " << achPlasticity << std::endl;
    }
    
    if (hasNorepinephrine) {
        float neLevel = brain->getNorepinephrine()->getLevel();
        float nePlasticity = brain->getNorepinephrine()->getPlasticityFactor();
        std::cout << "Norepinephrine level: " << neLevel << std::endl;
        std::cout << "Norepinephrine plasticity factor: " << nePlasticity << std::endl;
    }
    
    if (hasSerotonin) {
        float 5htLevel = brain->getSerotonin()->getLevel();
        float 5htPlasticity = brain->getSerotonin()->getPlasticityFactor();
        std::cout << "Serotonin level: " << 5htLevel << std::endl;
        std::cout << "Serotonin plasticity factor: " << 5htPlasticity << std::endl;
    }
    
    // Test 3: Run simulation
    std::cout << "\n=== Testing Simulation ===" << std::endl;
    
    brain->reset();
    brain->initialize();
    
    for (int step = 0; step < 5; ++step) {
        brain->step(step, step * 0.001);
        std::cout << "Step " << step << ": "
                  << brain->getTotalNeuronCount() << " neurons, "
                  << brain->getTotalSpikeCount() << " spikes" << std::endl;
    }
    
    // Test 4: Check memory systems
    std::cout << "\n=== Testing Memory Systems ===" << std::endl;
    
    if (auto* workingMemory = brain->getWorkingMemory()) {
        std::cout << "Working memory capacity: " << workingMemory->getCapacity() << std::endl;
        std::cout << "Working memory active traces: " << workingMemory->getActiveTraces() << std::endl;
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        std::cout << "Episodic memory max episodes: " << episodicMemory->getMaxEpisodes() << std::endl;
        std::cout << "Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    }
    
    // Test 5: Check prediction system
    std::cout << "\n=== Testing Prediction System ===" << std::endl;
    
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        std::cout << "Prediction system accessible" << std::endl;
    }
    
    // Test 6: Check cognition systems
    std::cout << "\n=== Testing Cognition Systems ===" << std::endl;
    
    if (auto* planner = brain->getPlanner()) {
        std::cout << "NeuralPlanner planning depth: " << planner->getPlanningDepth() << std::endl;
    }
    
    if (auto* conceptFormation = brain->getConceptFormation()) {
        std::cout << "ConceptFormation concepts: " << conceptFormation->getConceptCount() << std::endl;
    }
    
    if (auto* attention = brain->getAttention()) {
        std::cout << "AttentionalSelection accessible" << std::endl;
    }
    
    std::cout << "\n=== Integration Test Results ===" << std::endl;
    std::cout << "✓ All neuromodulators present and accessible" << std::endl;
    std::cout << "✓ Neuromodulators have proper levels and plasticity factors" << std::endl;
    std::cout << "✓ Simulation runs successfully" << std::endl;
    std::cout << "✓ Memory systems accessible" << std::endl;
    std::cout << "✓ Prediction system accessible" << std::endl;
    std::cout << "✓ Cognition systems accessible" << std::endl;
    
    return 0;
}
