// Memory System Integration Tests
// Integration tests for NLM memory systems and neuromodulation interaction

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "brain/Brain.hpp"
#include "memory/Memory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <memory>

namespace test_memory_integration {

void testWorkingMemoryIntegration() {
    std::cout << "    Testing Working Memory Integration..." << std::endl;
    
    // Create neural working memory system
    nlm::NeuralWorkingMemory workingMemory;
    
    // Test basic storage and retrieval
    nlm::NeuronId neuron1 = nlm::NeuronId(1);
    nlm::NeuronId neuron2 = nlm::NeuronId(2);
    
    workingMemory.storeToNeuron(neuron1, 0.8f);
    workingMemory.storeToNeuron(neuron2, 0.5f);
    
    assert(workingMemory.contains(neuron1));
    assert(workingMemory.contains(neuron2));
    
    assert(std::abs(workingMemory.getNeuronActivation(neuron1) - 0.8f) < 0.001f);
    assert(std::abs(workingMemory.getNeuronActivation(neuron2) - 0.5f) < 0.001f);
    
    // Test decay
    workingMemory.update(0.001f);  // Update with timestep
    
    // Test clear
    workingMemory.clear();
    assert(!workingMemory.contains(neuron1));
    assert(!workingMemory.contains(neuron2));
    
    // Test capacity
    workingMemory.setCapacity(200);
    assert(workingMemory.getCapacity() == 200);
    
    std::cout << "        Working Memory Integration passed" << std::endl;
}

void testEpisodicMemoryIntegration() {
    std::cout << "    Testing Episodic Memory Integration..." << std::endl;
    
    // Create episodic memory system
    nlm::NeuralEpisodicMemory episodicMemory;
    
    // Create test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.metadata = "test_episode";
    
    nlm::NeuronId neuron1 = nlm::NeuronId(1);
    nlm::NeuronId neuron2 = nlm::NeuronId(2);
    
    episode.activeNeurons.push_back(neuron1);
    episode.activeNeurons.push_back(neuron2);
    episode.neuronActivations.push_back(0.7f);
    episode.neuronActivations.push_back(0.9f);
    
    // Store episode
    episodicMemory.storeEpisode(episode);
    assert(episodicMemory.getEpisodeCount() == 1);
    
    // Retrieve episode
    const nlm::EpisodicMemoryItem* retrieved = episodicMemory.getEpisode(0);
    assert(retrieved->timestamp == 100);
    assert(retrieved->metadata == "test_episode");
    assert(retrieved->activeNeurons.size() == 2);
    assert(retrieved->neuronActivations.size() == 2);
    
    // Test recent episodes
    auto recent = episodicMemory.getRecentEpisodes(10);
    assert(recent.size() == 1);
    
    // Test clear
    episodicMemory.clear();
    assert(episodicMemory.getEpisodeCount() == 0);
    
    // Test replay
    episodicMemory.enableReplay(true);
    assert(episodicMemory.isReplayEnabled() == true);
    
    std::cout << "        Episodic Memory Integration passed" << std::endl;
}

void testAssociativeMemoryIntegration() {
    std::cout << "    Testing Associative Memory Integration..." << std::endl;
    
    // Create associative memory system
    nlm::NeuralAssociativeMemory associativeMemory;
    
    // Test association creation
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
    
    associativeMemory.associate(patternA, patternB, 0.8f);
    
    // Test association retrieval
    auto retrieved = associativeMemory.retrieve(patternA, 5);
    assert(retrieved.size() == 1);
    assert(retrieved[0].size() == 3);
    
    // Test association strength
    float strength = associativeMemory.getAssociationStrength(patternA, patternB);
    assert(std::abs(strength - 0.8f) < 0.001f);
    
    // Test clear
    associativeMemory.clear();
    assert(associativeMemory.getAssociationCount() == 0);
    
    std::cout << "        Associative Memory Integration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    Testing Neuromodulation Integration..." << std::endl;
    
    // Test novelty detector
    nlm::Novelty novelty;
    assert(novelty.getLevel() >= 0.0f && novelty.getLevel() <= 1.0f);
    
    // Test curiosity detector
    nlm::Curiosity curiosity;
    assert(curiosity.getLevel() >= 0.0f && curiosity.getLevel() <= 1.0f);
    
    // Test prediction error detector
    nlm::PredictionError predictionError;
    assert(predictionError.getLevel() >= 0.0f && predictionError.getLevel() <= 1.0f);
    
    std::cout << "        Neuromodulation Integration passed" << std::endl;
}

void testMemoryNeuromodulationInteraction() {
    std::cout << "    Testing Memory-Neuromodulation Interaction..." << std::endl;
    
    // Create memory and neuromodulation systems
    nlm::NeuralWorkingMemory workingMemory;
    nlm::Novelty novelty;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("simulation_timestep", 0.001);
    
    // Test neuromodulation level
    float noveltyLevel = novelty.getLevel();
    assert(noveltyLevel >= 0.0f && noveltyLevel <= 1.0f);
    
    // Test working memory interaction with novelty
    nlm::NeuronId neuron = nlm::NeuronId(1);
    workingMemory.storeToNeuron(neuron, 0.8f);
    assert(workingMemory.contains(neuron));
    assert(std::abs(workingMemory.getNeuronActivation(neuron) - 0.8f) < 0.001f);
    
    // Update neuromodulation
    novelty.update(0.001f);
    
    std::cout << "        Memory-Neuromodulation Interaction passed" << std::endl;
}

void testBrainMemoryIntegration() {
    std::cout << "    Testing Brain-Memory Integration..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test memory system access
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    nlm::NeuralAssociativeMemory* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test neuromodulation system access
    nlm::Novelty* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    nlm::Curiosity* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    nlm::PredictionError* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test configuration access
    auto configRef = brain.getConfig();
    assert(configRef != nullptr);
    
    // Test random generator access
    nlm::RandomGenerator* rng = brain.getRandomGenerator();
    assert(rng != nullptr);
    
    std::cout << "        Brain-Memory Integration passed" << std::endl;
}

void testMemoryPerformance() {
    std::cout << "    Testing Memory Performance..." << std::endl;
    
    // Create neural working memory system
    nlm::NeuralWorkingMemory workingMemory;
    
    // Test capacity
    assert(workingMemory.getCapacity() == 100); // Default capacity
    
    // Fill memory
    for (int i = 0; i < 50; ++i) {
        nlm::NeuronId neuron = nlm::NeuronId(i);
        workingMemory.storeToNeuron(neuron, static_cast<float>(i) / 100.0f);
    }
    
    assert(workingMemory.getActiveTraces() == 50);
    
    // Test decay performance
    workingMemory.update(0.001f);
    
    // Test clear performance
    workingMemory.clear();
    assert(workingMemory.getActiveTraces() == 0);
    
    std::cout << "        Memory Performance passed" << std::endl;
}

void testIntegration() {
    std::cout << "    Testing Full Integration..." << std::endl;
    
    // Create complete brain with all systems
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 123);
    config->set("neuron_count", 200);
    config->set("region_count", 3);
    config->set("simulation_timestep", 0.001);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test step function (this will exercise all integrated systems)
    for (int i = 0; i < 10; ++i) {
        brain.step(i, static_cast<nlm::Timestamp>(i) * 0.001);
    }
    
    // Verify systems are still functioning
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getRegionCount() == 3);
    
    std::cout << "        Full Integration passed" << std::endl;
}

void runAll() {
    std::cout << "=== Memory System Integration Tests ===" << std::endl;
    
    testWorkingMemoryIntegration();
    testEpisodicMemoryIntegration();
    testAssociativeMemoryIntegration();
    testNeuromodulationIntegration();
    testMemoryNeuromodulationInteraction();
    testBrainMemoryIntegration();
    testMemoryPerformance();
    testIntegration();
    
    std::cout << "\n=== All Memory Integration Tests Passed ===" << std::endl;
}

} // namespace test_memory_integration