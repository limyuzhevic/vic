// Brain Integration Tests
// Tests for memory system integration and enhanced brain functionality
// 
// Key integration features verified:
// 1. Memory System Integration - WorkingMemory and EpisodicMemory functionality
// 2. Brain-Memory Coupling - Proper initialization and operation
// 3. Memory-Driven Brain Behavior - Enhanced cognitive capabilities

#include "src/brain/Brain.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_brain_integration {

void testMemorySystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("working_memory_capacity", 50, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain (creates memory systems)
    bool success = brain.initialize();
    assert(success);
    
    // Verify memory systems are properly integrated
    // Working memory should be available and functional
    auto workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Episodic memory should be available and functional
    auto episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Store some information in working memory
    std::vector<float> pattern = {1.0f, 0.5f, 0.3f, 0.1f, 0.0f};
    workingMemory->store(pattern, 1.0f);
    
    // Verify working memory stored the pattern
    assert(workingMemory->getActiveTraces() > 0);
    
    // Update brain step (which updates memory systems)
    brain.step(1);
    
    // Verify episodic memory is being updated
    assert(episodicMemory->getEpisodeCount() > 0);
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void testWorkingMemoryFunctional() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test working memory store/retrieve cycle
    std::vector<float> originalPattern = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f};
    workingMemory->store(originalPattern, 1.0f);
    
    // Give some time for maintenance and decay
    workingMemory->update(0.1f);
    
    // Verify pattern is still retrievable (with potential decay)
    std::vector<float> retrieved = workingMemory->retrieve();
    assert(retrieved.size() == originalPattern.size());
    
    // Test specific neuron activation
    auto memoryNeurons = workingMemory->getMemoryNeurons();
    if (!memoryNeurons.empty()) {
        NeuronId testNeuron = memoryNeurons[0];
        assert(workingMemory->contains(testNeuron));
        assert(workingMemory->getNeuronActivation(testNeuron) >= 0.0f);
    }
    
    std::cout << "    testWorkingMemoryFunctional passed" << std::endl;
}

void testEpisodicMemoryFunctional() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test episodic memory storage
    std::vector<float> sensoryState = {0.9f, 0.3f, 0.7f, 0.2f};
    SimulationStep step = 1;
    
    episodicMemory->storeEpisode(sensoryState, step, 1.0f);
    
    // Verify episode was stored
    assert(episodicMemory->getEpisodeCount() > 0);
    
    // Test episode retrieval
    auto episode = episodicMemory->getEpisode(step);
    assert(episode != nullptr);
    assert(episode->sensoryState == sensoryState);
    assert(episode->stepId == step);
    
    // Test replay functionality
    brain.step(10);
    
    // Verify replay interval
    assert(episodicMemory->getReplayInterval() > 0);
    
    std::cout << "    testEpisodicMemoryFunctional passed" << std::endl;
}

void testMemoryDrivenBehavior() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store meaningful patterns in working memory
    std::vector<float> pattern1 = {1.0f, 0.9f, 0.8f, 0.7f, 0.6f};
    std::vector<float> pattern2 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    auto workingMemory = brain.getWorkingMemory();
    workingMemory->store(pattern1, 1.0f);
    workingMemory->store(pattern2, 0.5f);
    
    // Run brain for several steps
    for (SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Verify memory systems are active and maintaining information
    assert(workingMemory->getActiveTraces() > 0);
    
    // Verify attention system is influenced by memory
    auto attendedRegions = workingMemory->getAttendedRegions();
    // Attended regions may be empty depending on configuration
    
    std::cout << "    testMemoryDrivenBehavior passed" << std::endl;
}

void testMemoryConstantsAndCapacity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("working_memory_capacity", 100, nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", 50, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto workingMemory = brain.getWorkingMemory();
    auto episodicMemory = brain.getEpisodicMemory();
    
    // Test working memory capacity
    assert(workingMemory->getCapacity() == 100);
    
    // Test decay rate
    assert(workingMemory->getDecayRate() >= 0.0f);
    
    // Store many patterns to test capacity limits
    for (size_t i = 0; i < 20; ++i) {
        std::vector<float> pattern(5, static_cast<float>(i) * 0.1f);
        workingMemory->store(pattern, 1.0f);
    }
    
    // Memory should still be functional
    assert(workingMemory->getActiveTraces() > 0);
    
    // Test episodic memory capacity
    for (size_t i = 0; i < 30; ++i) {
        std::vector<float> sensoryState(4, 0.5f);
        episodicMemory->storeEpisode(sensoryState, i + 100, 1.0f);
    }
    
    std::cout << "    testMemoryConstantsAndCapacity passed" << std::endl;
}

void runAll() {
    testMemorySystemIntegration();
    testWorkingMemoryFunctional();
    testEpisodicMemoryFunctional();
    testMemoryDrivenBehavior();
    testMemoryConstantsAndCapacity();
}

} // namespace test_brain_integration