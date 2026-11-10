// Neuromodulator Tests
// Tests the improved neuromodulator implementations

#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_neuromodulators {

void testDopamineBasics() {
    nlm::Dopamine dopamine;
    
    assert(std::string(dopamine.getName()) == "DA");
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getBaselineLevel() == 0.0f);
    assert(dopamine.getMaxLevel() == 1.0f);
    
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    std::cout << "    testDopamineBasics passed" << std::endl;
}

void testDopaminePlasticityFactor() {
    nlm::Dopamine dopamine;
    
    // Test plasticity factor at different levels
    dopamine.setLevel(0.0f);  // Baseline
    float factor0 = dopamine.getPlasticityFactor();
    assert(factor0 > 0.0f && factor0 < 2.0f);
    
    dopamine.setLevel(0.5f);  // Medium level
    float factor1 = dopamine.getPlasticityFactor();
    assert(factor1 > 0.0f && factor1 < 2.0f);
    
    dopamine.setLevel(1.0f);  // High level
    float factor2 = dopamine.getPlasticityFactor();
    assert(factor2 > 0.0f && factor2 < 2.0f);
    
    std::cout << "    testDopaminePlasticityFactor passed" << std::endl;
}

void testAcetylcholineBasics() {
    nlm::Acetylcholine ach;
    
    assert(std::string(ach.getName()) == "ACh");
    assert(ach.getBaselineLevel() == 0.1f);
    assert(ach.getMaxLevel() == 1.0f);
    
    ach.setLevel(0.8f);
    assert(ach.getLevel() == 0.8f);
    
    std::cout << "    testAcetylcholineBasics passed" << std::endl;
}

void testAcetylcholineAttentionSignal() {
    nlm::Acetylcholine ach;
    
    ach.signalAttention(0.5f);
    assert(ach.getLevel() > 0.0f);
    
    ach.signalNovelty(0.3f);
    assert(ach.getLevel() > 0.0f);
    
    std::cout << "    testAcetylcholineAttentionSignal passed" << std::endl;
}

void testNorepinephrineBasics() {
    nlm::Norepinephrine ne;
    
    assert(std::string(ne.getName()) == "NE");
    assert(ne.getBaselineLevel() == 0.05f);
    assert(ne.getMaxLevel() == 1.0f);
    
    ne.setLevel(0.9f);
    assert(ne.getLevel() == 0.9f);
    
    std::cout << "    testNorepinephrineBasics passed" << std::endl;
}

void testSerotoninBasics() {
    nlm::Serotonin serotonin;
    
    assert(std::string(serotonin.getName()) == "5-HT");
    assert(serotonin.getBaselineLevel() == 0.1f);
    assert(serotonin.getMaxLevel() == 1.0f);
    
    serotonin.setLevel(0.6f);
    assert(serotonin.getLevel() == 0.6f);
    
    std::cout << "    testSerotoninBasics passed" << std::endl;
}

void testNeuromodulatorValidation() {
    nlm::Dopamine dopamine;
    
    // Test level clamping
    dopamine.setLevel(1.5f); // Should be clamped to 1.0
    assert(dopamine.getLevel() == 1.0f);
    
    dopamine.setLevel(-0.5f); // Should be clamped to 0.0
    assert(dopamine.getLevel() == 0.0f);
    
    // Test update function
    dopamine.setLevel(0.5f);
    dopamine.update(0.001); // Small time step
    assert(dopamine.getLevel() < 0.5f); // Should decay towards baseline
    
    std::cout << "    testNeuromodulatorValidation passed" << std::endl;
}

void testWorkingMemoryBasics() {
    nlm::WorkingMemory memory;
    
    assert(memory.getCapacity() > 0);
    assert(memory.getCurrentSize() == 0);
    
    memory.store(nlm::NeuronId(1), 0.5f);
    assert(memory.getCurrentSize() == 1);
    assert(memory.contains(nlm::NeuronId(1)));
    
    assert(memory.retrieve(nlm::NeuronId(1)) == 0.5f);
    
    std::cout << "    testWorkingMemoryBasics passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    nlm::WorkingMemory memory;
    
    // Test that capacity limits are respected
    size_t originalCapacity = memory.getCapacity();
    
    // Add neurons until we reach capacity
    for (size_t i = 1; i <= originalCapacity + 10; ++i) {
        memory.store(nlm::NeuronId(i), 1.0f);
    }
    
    // Current size should not exceed capacity
    assert(memory.getCurrentSize() <= originalCapacity);
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryDecay() {
    nlm::WorkingMemory memory;
    
    memory.store(nlm::NeuronId(1), 1.0f);
    memory.store(nlm::NeuronId(2), 0.5f);
    
    assert(memory.getCurrentSize() == 2);
    
    memory.decay(0.1f); // 10% decay
    
    // Values should have decayed
    assert(memory.retrieve(nlm::NeuronId(1)) < 1.0f);
    assert(memory.retrieve(nlm::NeuronId(2)) < 0.5f);
    
    std::cout << "    testWorkingMemoryDecay passed" << std::endl;
}

void testEpisodicMemoryBasics() {
    nlm::EpisodicMemory memory;
    
    assert(memory.getEpisodeCount() == 0);
    
    // Create a test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.neurons.push_back(nlm::NeuronId(1));
    episode.values.push_back(0.7f);
    episode.metadata = "test_episode";
    
    memory.storeEpisode(episode);
    assert(memory.getEpisodeCount() == 1);
    
    auto retrieved = memory.retrieveEpisode(0);
    assert(retrieved.timestamp == 100);
    assert(retrieved.neurons.size() == 1);
    assert(retrieved.values.size() == 1);
    
    std::cout << "    testEpisodicMemoryBasics passed" << std::endl;
}

void testEpisodicMemoryRecentEpisodes() {
    nlm::EpisodicMemory memory;
    
    // Add several episodes
    for (size_t i = 0; i < 20; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.metadata = std::to_string(i);
        memory.storeEpisode(episode);
    }
    
    // Test getting recent episodes
    auto recent = memory.getRecentEpisodes(5);
    assert(recent.size() == 5);
    
    // Should be the most recent (highest timestamp)
    assert(recent[0].timestamp == 19);
    assert(recent[1].timestamp == 18);
    
    std::cout << "    testEpisodicMemoryRecentEpisodes passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    nlm::EpisodicMemory memory;
    
    // Add many episodes
    for (size_t i = 0; i < 100; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.neurons.push_back(nlm::NeuronId(i % 10));
        episode.values.push_back(static_cast<float>(i % 5 + 1));
        episode.metadata = std::to_string(i);
        memory.storeEpisode(episode);
    }
    
    assert(memory.getEpisodeCount() == 100);
    
    // Consolidate with low threshold
    memory.consolidate(0.1f);
    assert(memory.getEpisodeCount() < 100);
    assert(memory.getEpisodeCount() > 0);
    
    // Consolidate with high threshold
    memory.consolidate(0.9f);
    assert(memory.getEpisodeCount() < 100);
    assert(memory.getEpisodeCount() > 0);
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void runAllNeuromodulatorTests() {
    std::cout << "Running neuromodulator tests..." << std::endl;
    
    testDopamineBasics();
    testDopaminePlasticityFactor();
    testAcetylcholineBasics();
    testAcetylcholineAttentionSignal();
    testNorepinephrineBasics();
    testSerotoninBasics();
    testNeuromodulatorValidation();
    
    std::cout << "Neuromodulator tests completed successfully!" << std::endl;
    std::cout << std::endl;
}

void runAllWorkingMemoryTests() {
    std::cout << "Running working memory tests..." << std::endl;
    
    testWorkingMemoryBasics();
    testWorkingMemoryCapacity();
    testWorkingMemoryDecay();
    
    std::cout << "Working memory tests completed successfully!" << std::endl;
    std::cout << std::endl;
}

void runAllEpisodicMemoryTests() {
    std::cout << "Running episodic memory tests..." << std::endl;
    
    testEpisodicMemoryBasics();
    testEpisodicMemoryRecentEpisodes();
    testEpisodicMemoryConsolidation();
    
    std::cout << "Episodic memory tests completed successfully!" << std::endl;
    std::cout << std::endl;
}

void runAllTests() {
    runAllNeuromodulatorTests();
    runAllWorkingMemoryTests();
    runAllEpisodicMemoryTests();
    
    std::cout << "All tests completed successfully!" << std::endl;
}

} // namespace test_neuromodulators

// Test runner function
void test_neuromodulators_main() {
    test_neuromodulators::runAllTests();
}