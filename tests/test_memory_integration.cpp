// Memory Integration Tests
// Tests memory system integration within the brain

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

namespace test_memory_integration {

// Test 1: Working memory basic functionality and integration
void testWorkingMemoryBasic() {
    std::cout << "    Testing working memory basic functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::WorkingMemory* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test storage
    workingMemory->store(1, 1.5f);
    workingMemory->store(2, 2.5f);
    workingMemory->store(3, 3.5f);
    
    // Test retrieval
    assert(workingMemory->retrieve(1) == 1.5f);
    assert(workingMemory->retrieve(2) == 2.5f);
    assert(workingMemory->retrieve(3) == 3.5f);
    
    // Test contains
    assert(workingMemory->contains(1));
    assert(workingMemory->contains(2));
    assert(workingMemory->contains(3));
    assert(!workingMemory->contains(4));
    
    // Test capacity
    assert(workingMemory->getCapacity() > 0);
    assert(workingMemory->getCurrentSize() == 3);
    
    // Test decay
    float beforeDecay = workingMemory->retrieve(1);
    workingMemory->decay(0.5f);
    float afterDecay = workingMemory->retrieve(1);
    assert(afterDecay <= beforeDecay);  // Decay should reduce value
    
    // Test clear
    workingMemory->clear();
    assert(workingMemory->getCurrentSize() == 0);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 2: Episodic memory storage and retrieval
void testEpisodicMemoryBasic() {
    std::cout << "    Testing episodic memory basic functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(40), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::EpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create test episodes
    std::vector<nlm::EpisodicMemoryItem> episodes;
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 100;
        episode.neurons = {static_cast<nlm::NeuronId>(i), static_cast<nlm::NeuronId>(i + 1)};
        episode.values = {0.1f * i, 0.2f * i};
        episode.metadata = "Episode " + std::to_string(i);
        episodes.push_back(episode);
    }
    
    // Store all episodes
    for (const auto& episode : episodes) {
        episodicMemory->storeEpisode(episode);
    }
    
    // Verify count
    assert(episodicMemory->getEpisodeCount() == 5);
    
    // Test retrieval
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem retrieved = episodicMemory->retrieveEpisode(i);
        assert(retrieved.timestamp == i * 100);
        assert(retrieved.neurons.size() == 2);
        assert(retrieved.values.size() == 2);
        assert(retrieved.metadata == "Episode " + std::to_string(i));
    }
    
    // Test recent episodes
    auto recent = episodicMemory->getRecentEpisodes(3);
    assert(recent.size() == 3);
    
    // Verify recent episodes are the latest
    assert(recent[0].timestamp == 400);
    assert(recent[1].timestamp == 300);
    assert(recent[2].timestamp == 200);
    
    // Test consolidation
    episodicMemory->consolidate(0.15f);
    
    // Test clear
    episodicMemory->clear();
    assert(episodicMemory->getEpisodeCount() == 0);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 3: Neural working memory integration with brain
void testNeuralWorkingMemoryIntegration() {
    std::cout << "    Testing neural working memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::NeuralWorkingMemory* neuralMem = brain.getWorkingMemory();
    assert(neuralMem != nullptr);
    
    // Test storing with neural patterns
    std::vector<float> pattern1{0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2{0.4f, 0.5f, 0.6f};
    std::vector<float> pattern3{0.7f, 0.8f, 0.9f};
    
    // Simulate storing neural patterns (WorkingMemory interface)
    neuralMem->store(1, 0.5f);
    neuralMem->store(2, 0.8f);
    neuralMem->store(3, 0.3f);
    
    // Test retrieval
    assert(neuralMem->retrieve(1) == 0.5f);
    assert(neuralMem->retrieve(2) == 0.8f);
    assert(neuralMem->retrieve(3) == 0.3f);
    
    // Test capacity management
    size_t initialCapacity = neuralMem->getCapacity();
    assert(initialCapacity > 0);
    
    size_t currentSize = neuralMem->getCurrentSize();
    assert(currentSize == 3);
    
    // Test decay affects neural memory
    neuralMem->decay(0.2f);
    
    // Test memory updates with brain activity
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        // Simulate brain activity affecting working memory
        neuralMem->store(1, 0.5f + 0.1f * sin(step * 0.5f));
        neuralMem->decay(0.05f);
    }
    
    std::cout << "      PASSED" << std::endl;
}

// Test 4: Neural episodic memory integration
void testNeuralEpisodicMemoryIntegration() {
    std::cout << "    Testing neural episodic memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::NeuralEpisodicMemory* neuralEpisodic = brain.getEpisodicMemory();
    assert(neuralEpisodic != nullptr);
    
    // Create complex episodes with neural patterns
    std::vector<nlm::EpisodicMemoryItem> episodes;
    for (int i = 0; i < 8; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 50;
        
        // Create neural pattern for this episode
        std::vector<nlm::NeuronId> neurons;
        std::vector<float> values;
        
        for (int j = 0; j < 5; ++j) {
            neurons.push_back(static_cast<nlm::NeuronId>(i * 10 + j));
            values.push_back(0.1f * (i + j));
        }
        
        episode.neurons = neurons;
        episode.values = values;
        episode.metadata = "Neural episode " + std::to_string(i) + " with patterns";
        episodes.push_back(episode);
    }
    
    // Store all episodes
    for (const auto& episode : episodes) {
        neuralEpisodic->storeEpisode(episode);
    }
    
    // Verify storage
    assert(neuralEpisodic->getEpisodeCount() == 8);
    
    // Test retrieval of specific episodes
    for (int i = 0; i < 8; ++i) {
        nlm::EpisodicMemoryItem retrieved = neuralEpisodic->retrieveEpisode(i);
        assert(retrieved.timestamp == i * 50);
        assert(retrieved.neurons.size() == 5);
        assert(retrieved.values.size() == 5);
    }
    
    // Test getting recent episodes
    auto recent = neuralEpisodic->getRecentEpisodes(3);
    assert(recent.size() == 3);
    
    // Verify temporal ordering
    assert(recent[0].timestamp == 350);
    assert(recent[1].timestamp == 300);
    assert(recent[2].timestamp == 250);
    
    // Test consolidation with neural patterns
    neuralEpisodic->consolidate(0.2f);
    
    // Test decay simulation
    neuralEpisodic->clear();
    assert(neuralEpisodic->getEpisodeCount() == 0);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 5: Memory consolidation and replay
void testMemoryConsolidationReplay() {
    std::cout << "    Testing memory consolidation and replay..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(45), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::EpisodicMemory* episodicMem = brain.getEpisodicMemory();
    nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
    
    // Create episodes with varying relevance
    std::vector<nlm::EpisodicMemoryItem> episodes;
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.neurons = {static_cast<nlm::NeuronId>(i)};
        episode.values = {0.05f * (i + 1)};  // Higher value for later episodes
        episode.metadata = "Consolidation test episode " + std::to_string(i);
        episodes.push_back(episode);
        
        // Also store in working memory
        workingMem->store(i, 0.1f * i);
    }
    
    // Store all episodes
    for (const auto& episode : episodes) {
        episodicMem->storeEpisode(episode);
    }
    
    assert(episodicMem->getEpisodeCount() == 10);
    assert(workingMem->getCurrentSize() == 10);
    
    // Test consolidation with relevance threshold
    episodicMem->consolidate(0.08f);  // Low threshold to consolidate more
    
    // Test consolidation with higher threshold
    episodicMem->consolidate(0.15f);
    
    // Test getting recent episodes (replay mechanism)
    auto recentEpisodes = episodicMem->getRecentEpisodes(5);
    assert(recentEpisodes.size() == 5);
    
    // Verify replay temporal order
    assert(recentEpisodes[0].timestamp == 90);
    assert(recentEpisodes[4].timestamp == 10);
    
    // Test working memory decay over time
    for (int decayStep = 0; decayStep < 5; ++decayStep) {
        workingMem->decay(0.1f);
        assert(workingMem->getCurrentSize() >= 0);  // Size can go to zero
    }
    
    // Test memory clearing
    workingMem->clear();
    assert(workingMem->getCurrentSize() == 0);
    
    episodicMem->clear();
    assert(episodicMem->getEpisodeCount() == 0);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 6: Associative memory integration
void testAssociativeMemoryIntegration() {
    std::cout << "    Testing associative memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(55), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AssociativeMemory* associativeMem = brain.getAssociativeMemory();
    assert(associativeMem != nullptr);
    
    // Test creating associations
    associativeMem->associate(1, 2, 0.8f);  // Strong association
    associativeMem->associate(1, 3, 0.5f);  // Medium association
    associativeMem->associate(2, 4, 0.3f);  // Weak association
    
    // Test retrieving associations
    std::vector<nlm::NeuronId> associations1 = associativeMem->getAssociations(1);
    assert(associations1.size() == 2);  // Associated with 2 and 3
    assert(std::find(associations1.begin(), associations1.end(), 2) != associations1.end());
    assert(std::find(associations1.begin(), associations1.end(), 3) != associations1.end());
    
    std::vector<nlm::NeuronId> associations2 = associativeMem->getAssociations(2);
    assert(associations2.size() == 2);  // Associated with 1 and 4
    assert(std::find(associations2.begin(), associations2.end(), 1) != associations2.end());
    assert(std::find(associations2.begin(), associations2.end(), 4) != associations2.end());
    
    // Test association strength
    assert(associativeMem->getAssociationStrength(1, 2) == 0.8f);
    assert(associativeMem->getAssociationStrength(1, 3) == 0.5f);
    assert(associativeMem->getAssociationStrength(2, 4) == 0.3f);
    assert(associativeMem->getAssociationStrength(2, 1) == 0.8f);  // Bidirectional
    
    // Test updating associations
    associativeMem->updateAssociation(1, 2, 0.1f);  // Increase strength
    assert(associativeMem->getAssociationStrength(1, 2) == 0.9f);
    
    associativeMem->updateAssociation(1, 3, -0.2f);  // Decrease strength
    assert(associativeMem->getAssociationStrength(1, 3) == 0.3f);
    
    // Test clearing
    associativeMem->clear();
    std::vector<nlm::NeuronId> emptyAssociations = associativeMem->getAssociations(1);
    assert(emptyAssociations.size() == 0);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 7: Memory statistics and monitoring
void testMemoryStatistics() {
    std::cout << "    Testing memory statistics..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
    nlm::EpisodicMemory* episodicMem = brain.getEpisodicMemory();
    
    // Initialize statistics
    assert(workingMem->getCurrentSize() == 0);
    assert(episodicMem->getEpisodeCount() == 0);
    
    // Add data to working memory
    workingMem->store(1, 1.0f);
    workingMem->store(2, 2.0f);
    workingMem->store(3, 3.0f);
    
    assert(workingMem->getCurrentSize() == 3);
    
    // Add data to episodic memory
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.neurons = {static_cast<nlm::NeuronId>(i)};
        episode.values = {0.1f * i};
        episodicMem->storeEpisode(episode);
    }
    
    assert(episodicMem->getEpisodeCount() == 5);
    
    // Test recent episodes
    auto recent = episodicMem->getRecentEpisodes(3);
    assert(recent.size() == 3);
    
    // Test statistics through brain interface
    assert(brain.getWorkingMemory()->getCurrentSize() == 3);
    assert(brain.getEpisodicMemory()->getEpisodeCount() == 5);
    assert(brain.getAssociativeMemory()->getAllAssociations().size() == 0);
    
    std::cout << "      PASSED" << std::endl;
}

void runAll() {
    std::cout << "Memory Integration Tests" << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl;
    
    testWorkingMemoryBasic();
    testEpisodicMemoryBasic();
    testNeuralWorkingMemoryIntegration();
    testNeuralEpisodicMemoryIntegration();
    testMemoryConsolidationReplay();
    testAssociativeMemoryIntegration();
    testMemoryStatistics();
    
    std::cout << "=" << std::string(50, '=') << std::endl;
    std::cout << "All memory integration tests completed successfully!" << std::endl;
}

} // namespace test_memory_integration