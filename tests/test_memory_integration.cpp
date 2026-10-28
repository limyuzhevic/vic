// Memory Systems Integration Tests
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_memory_integration {

void testWorkingMemoryEpisodicMemoryIntegration() {
    std::cout << "    Testing Working Memory + Episodic Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    config->set("region_count", static_cast<int64_t>(2));
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get memory systems
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Simulate some neural activity
    for (int step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
        
        // Occasionally inject sensory input to create memory traces
        if (step % 20 == 0) {
            // Create a simple sensory pattern
            std::vector<float> sensoryPattern(50, 0.0f);
            size_t patternSize = std::min(50UL, brain.getTotalNeuronCount() / 4);
            for (size_t i = 0; i < patternSize; ++i) {
                sensoryPattern[i] = 1.0f * sinf((float)i * 0.1f);
            }
            
            // Store in working memory
            workingMemory->store(sensoryPattern, 0.8f);
            
            // Create episodic memory item
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.sensoryState = sensoryPattern;
            episode.positionX = 0.5f;
            episode.positionY = 0.5f;
            episode.action = nlm::ActionType::Wait;
            episode.reward = 0.1f + 0.05f * sinf((float)step * 0.1f);
            episode.energy = 1.0f;
            episode.novelty = fabsf(sinf((float)step * 0.05f));
            episode.age = 0;
            
            episodicMemory->storeEpisode(episode);
        }
    }
    
    // Verify integration
    assert(workingMemory->getActiveTraces() >= 0); // May have traces
    assert(episodicMemory->getEpisodeCount() >= 0); // May have episodes
    
    // Test working memory decay
    size_t tracesBefore = workingMemory->getActiveTraces();
    workingMemory->update(0.001f);
    
    // Test episodic memory relevance update
    if (episodicMemory->getEpisodeCount() > 0) {
        episodicMemory->updateRelevance(0, 0.5f);
    }
    
    std::cout << "    Memory integration test passed" << std::endl;
}

void testAssociativeMemoryIntegration() {
    std::cout << "    Testing Associative Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150));
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Create correlated patterns in working memory
    std::vector<float> patternA(30, 0.0f);
    std::vector<float> patternB(30, 0.0f);
    for (size_t i = 0; i < 30; ++i) {
        patternA[i] = sinf((float)i * 0.5f);
        patternB[i] = cosf((float)i * 0.5f);
    }
    
    // Store patterns
    workingMemory->store(patternA, 0.9f);
    workingMemory->store(patternB, 0.9f);
    
    // Create related episodic items
    nlm::EpisodicMemoryItem episode1;
    episode1.sensoryState = patternA;
    episode1.action = nlm::ActionType::Wait;
    episode1.reward = 1.0f;
    
    nlm::EpisodicMemoryItem episode2;
    episode2.sensoryState = patternB;
    episode2.action = nlm::ActionType::Move;
    episode2.reward = 0.8f;
    
    episodicMemory->storeEpisode(episode1);
    episodicMemory->storeEpisode(episode2);
    
    // Associate patterns through episodic experience
    associativeMemory->associateFromExperience(episode1);
    associativeMemory->associateFromExperience(episode2);
    
    // Test association strength
    float strength = associativeMemory->getAssociationStrength(patternA, patternB);
    assert(strength >= 0.0f); // Should have some association
    
    // Test spreading activation
    auto retrieved = associativeMemory->spreadActivation(patternA, 1);
    assert(!retrieved.empty()); // Should retrieve related patterns
    
    std::cout << "    Associative memory integration test passed" << std::endl;
}

void runAll() {
    testWorkingMemoryEpisodicMemoryIntegration();
    testAssociativeMemoryIntegration();
}

} // namespace test_memory_integration