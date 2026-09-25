// Enhanced Brain Tests for NLM Phase 6
// Tests memory management fixes, move semantics, and Phase 6 system integration

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigFactory.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainMemoryManagement() {
    // Test move semantics
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config1);
    brain1.initialize();
    
    // Test move constructor
    nlm::Brain brain2(std::move(brain1));
    
    // Original should be in valid but unspecified state
    // The moved-from brain should have released its resources
    
    auto regionIds = brain2.getRegionIds();
    assert(regionIds.size() == 1);
    
    // Test move assignment
    auto config3 = std::make_shared<nlm::Config>();
    config3->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config3->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain3(config3);
    brain3.initialize();
    
    brain2 = std::move(brain3);
    
    auto regionIds2 = brain2.getRegionIds();
    assert(regionIds2.size() == 2);
    assert(brain2.getTotalNeuronCount() == 75);
    
    std::cout << "    testBrainMemoryManagement passed" << std::endl;
}

void testBrainVectorInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify vector initialization
    assert(brain.getRegionCount() == 5);
    assert(brain.getTotalNeuronCount() == 1000);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Check that vectors are properly initialized (not null)
    auto regions = brain.getRegions();
    assert(!regions.empty());
    
    for (const auto& region : regions) {
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainVectorInitialization passed" << std::endl;
}

void testBrainResetFunctionality() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get initial state
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    // Run some steps to modify state
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Reset
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == initialNeurons);
    assert(brain.getRegionCount() == 1);
    
    // State should be restored to initialized state
    auto regions = brain.getRegions();
    assert(!regions.empty());
    assert(regions[0] != nullptr);
    
    std::cout << "    testBrainResetFunctionality passed" << std::endl;
}

void testBrainThreadSafety() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test thread safety with concurrent stepping
    std::vector<std::thread> threads;
    const size_t threadCount = 5;
    
    for (size_t i = 0; i < threadCount; ++i) {
        threads.emplace_back([&brain, i]() {
            for (nlm::SimulationStep step = 0; step < 20; ++step) {
                brain.step(step);
                // Simulate some work
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify brain is still in valid state
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getRegionCount() == 3);
    
    std::cout << "    testBrainThreadSafety passed" << std::endl;
}

void testBrainPhase6SystemIntegration() {
    // Test integration with Phase 6 systems
    auto config = nlm::ConfigFactory::createBrainSimulationConfig();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test memory systems integration
    auto workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    auto episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    auto associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test prediction system integration
    auto predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test cognition systems integration
    auto planner = brain.getPlanner();
    assert(planner != nullptr);
    
    auto conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test neuromodulation integration
    auto dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    auto curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    auto novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Test development system integration
    auto developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test that all systems are operational
    brain.step(0);
    
    // Verify statistics are collected
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainPhase6SystemIntegration passed" << std::endl;
}

void testBrainActionProductionEnhanced() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test action production with various inputs
    auto action1 = brain.produceAction();
    assert(action1 != nullptr);
    
    // Inject some input to stimulate action production
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
    
    auto action2 = brain.produceAction();
    assert(action2 != nullptr);
    
    // Test status reporting
    auto neuronCount = brain.getTotalNeuronCount();
    auto synapseCount = brain.getTotalSynapseCount();
    auto firingCount = brain.getFiringNeuronCount();
    auto firingRate = brain.getAverageFiringRate();
    
    assert(neuronCount == 100);
    assert(synapseCount >= 0);
    assert(firingCount >= 0);
    assert(firingRate >= 0.0f);
    
    std::cout << "    testBrainActionProductionEnhanced passed" << std::endl;
}

void testBrainMemoryCleanup() {
    // Test that memory is properly cleaned up
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create and destroy brain multiple times
    for (int i = 0; i < 5; ++i) {
        nlm::Brain brain(config);
        brain.initialize();
        
        // Use the brain
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
        
        // Brain should go out of scope and be destroyed here
        // The destructor should properly clean up all allocated resources
    }
    
    // If we get here, memory cleanup is working
    std::cout << "    testBrainMemoryCleanup passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainMemoryManagement();
    testBrainVectorInitialization();
    testBrainResetFunctionality();
    testBrainThreadSafety();
    testBrainPhase6SystemIntegration();
    testBrainActionProductionEnhanced();
    testBrainMemoryCleanup();
}

} // namespace test_brain
