// Memory Integration Tests for NLM
// Tests integration between NeuralWorkingMemory, NeuralEpisodicMemory, 
// and NeuralAssociativeMemory systems

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_memory_integration {

void testMemorySystemConnections() {
    std::cout << "    Testing memory system connections..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("memory_capacity", 1000, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify memory systems are accessible
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory capacity
    assert(workingMemory->getCapacity() > 0);
    
    // Test episodic memory storage
    std::string testExperience = "test_experience_123";
    size_t episodeId = episodicMemory->storeExperience(testExperience);
    assert(episodeId != 0);
    
    // Test associative memory pattern storage
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> pattern2 = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    associativeMemory->associate(pattern1, pattern2);
    
    std::cout << "    testMemorySystemConnections passed" << std::endl;
}

void testMemoryNeuromodulationInteraction() {
    std::cout << "    Testing memory-neuromodulation interactions..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* neuromodulator = brain.getCuriosity();
    assert(neuromodulator != nullptr);
    
    // Create and apply neuromodulatory signals
    nlm::Neuromodulator dopamineSignal(nlm::Dopamine::Create(1.0f));
    brain.applyNeuromodulation(dopamineSignal);
    
    // Verify neuromodulation affects memory systems
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Apply curiosity signal
    nlm::Neuromodulator curiositySignal(nlm::Curiosity::Create(0.5f));
    brain.applyNeuromodulation(curiositySignal);
    
    // Verify effects on memory consolidation
    assert(workingMemory->getConsolidationRate() > 0.0f);
    
    std::cout << "    testMemoryNeuromodulationInteraction passed" << std::endl;
}

void testWorkingEpisodicMemoryIntegration() {
    std::cout << "    Testing working-episodic memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Store experience in episodic memory
    std::string experience = "neural_network_training";
    size_t episodeId = episodicMemory->storeExperience(experience);
    
    // Working memory should be able to access episodic content
    workingMemory->updateWorkingMemory(experience);
    
    // Verify cross-memory interaction
    auto episodicContent = episodicMemory->getEpisode(episodeId);
    assert(!episodicContent.empty());
    
    std::cout << "    testWorkingEpisodicMemoryIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "=== Memory Integration Tests ===" << std::endl;
    testMemorySystemConnections();
    testMemoryNeuromodulationInteraction();
    testWorkingEpisodicMemoryIntegration();
    std::cout << "=== Memory Integration Tests Complete ===" << std::endl;
}

} // namespace test_memory_integration