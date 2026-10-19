// Memory System Tests
// Comprehensive tests for working memory, episodic memory, and associative memory

#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/Memory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_memory_systems {

// Working Memory Tests
void testWorkingMemoryBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.2f, 0.7f};
    workingMemory->store(pattern, 1.0f);
    
    std::vector<float> retrieved = workingMemory->retrieve();
    assert(retrieved.size() == pattern.size());
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        assert(fabs(retrieved[i] - pattern[i]) < 0.001f);
    }
    
    std::cout << "    testWorkingMemoryBasic passed" << std::endl;
}

void testWorkingMemoryNeuronAccess() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    nlm::NeuronId testNeuron(0);
    workingMemory->storeToNeuron(testNeuron, 0.8f);
    
    assert(workingMemory->contains(testNeuron));
    assert(fabs(workingMemory->getNeuronActivation(testNeuron) - 0.8f) < 0.001f);
    
    std::cout << "    testWorkingMemoryNeuronAccess passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    size_t originalCapacity = workingMemory->getCapacity();
    workingMemory->setCapacity(1000);
    assert(workingMemory->getCapacity() == 1000);
    
    workingMemory->setCapacity(originalCapacity);
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryDecay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    float originalDecayRate = workingMemory->getDecayRate();
    workingMemory->setDecayRate(0.01f);
    assert(fabs(workingMemory->getDecayRate() - 0.01f) < 0.001f);
    
    workingMemory->setDecayRate(originalDecayRate);
    
    std::cout << "    testWorkingMemoryDecay passed" << std::endl;
}

void testWorkingMemoryStrengthening() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test strengthenMemory method
    workingMemory->strengthenMemory(1.5f);
    
    std::cout << "    testWorkingMemoryStrengthening passed" << std::endl;
}

void testWorkingMemoryCompetition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    workingMemory->runCompetition();
    
    const std::vector<nlm::NeuronId>& winners = workingMemory->getWinners();
    
    std::cout << "    testWorkingMemoryCompetition passed" << std::endl;
}

void testWorkingMemoryMemoryActivity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    float activity = workingMemory->getMemoryActivity();
    assert(activity >= 0.0f && activity <= 1.0f);
    
    std::cout << "    testWorkingMemoryMemoryActivity passed" << std::endl;
}

// Episodic Memory Tests
void testEpisodicMemoryBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    episode.orientation = 0.5f;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 10.0f;
    episode.energy = 100.0f;
    episode.novelty = 0.3f;
    episode.resultingSensoryState = {0.5f, 0.7f, 0.9f};
    episode.resultingReward = 12.0f;
    episode.activeNeurons = {nlm::NeuronId(0), nlm::NeuronId(1), nlm::NeuronId(2)};
    episode.neuronActivations = {0.8f, 0.6f, 0.4f};
    episode.age = 0;
    
    episodicMemory->storeEpisode(episode);
    
    assert(episodicMemory->getEpisodeCount() == 1);
    
    const nlm::EpisodicMemoryItem* retrievedEpisode = episodicMemory->getEpisode(0);
    assert(retrievedEpisode != nullptr);
    assert(retrievedEpisode->reward == 10.0f);
    
    std::cout << "    testEpisodicMemoryBasic passed" << std::endl;
}

void testEpisodicMemoryRetrieval() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 0;
    episode1.sensoryState = {0.1f, 0.2f, 0.3f};
    episode1.action = nlm::ActionType::Wait;
    episode1.reward = 5.0f;
    
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 100;
    episode2.sensoryState = {0.4f, 0.5f, 0.6f};
    episode2.action = nlm::ActionType::Attack;
    episode2.reward = 10.0f;
    
    episodicMemory->storeEpisode(episode1);
    episodicMemory->storeEpisode(episode2);
    
    std::vector<const nlm::EpisodicMemoryItem*> recent = episodicMemory->getRecentEpisodes(1);
    assert(recent.size() == 1);
    assert(recent[0]->reward == 10.0f); // Most recent
    
    float avgReward = episodicMemory->getAverageReward();
    assert(fabs(avgReward - 7.5f) < 0.001f);
    
    std::cout << "    testEpisodicMemoryRetrieval passed" << std::endl;
}

void testEpisodicMemoryTemporalRetrieval() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Store multiple episodes with different timestamps
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 100;
        episode.reward = i * 2.0f;
        episode.sensoryState = {static_cast<float>(i) / 10.0f, 0.5f, 0.8f};
        episodicMemory->storeEpisode(episode);
    }
    
    std::vector<const nlm::EpisodicMemoryItem*> temporal = 
        episodicMemory->retrieveTemporal(150, 350, 5);
    
    assert(temporal.size() <= 5);
    for (const auto* episode : temporal) {
        assert(episode->timestamp >= 150 && episode->timestamp <= 350);
    }
    
    std::cout << "    testEpisodicMemoryTemporalRetrieval passed" << std::endl;
}

void testEpisodicMemoryReplay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 5.0f;
    episode.sensoryState = {0.5f, 0.7f};
    
    episodicMemory->storeEpisode(episode);
    
    size_t episodeId = episodicMemory->getEpisodeCount() - 1;
    
    episodicMemory->enableReplay(true);
    assert(episodicMemory->isReplayEnabled());
    
    episodicMemory->replayEpisode(episodicMemory->getEpisode(episodeId));
    
    std::cout << "    testEpisodicMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Store some episodes
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 50;
        episode.reward = static_cast<float>(i);
        episodicMemory->storeEpisode(episode);
    }
    
    size_t originalCount = episodicMemory->getEpisodeCount();
    episodicMemory->consolidate(0.5f); // Keep episodes with relevance > 0.5
    
    assert(episodicMemory->getEpisodeCount() <= originalCount);
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

// Associative Memory Tests
void testAssociativeMemoryBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    std::vector<float> patternA = {0.1f, 0.5f, 0.9f};
    std::vector<float> patternB = {0.2f, 0.6f, 0.8f};
    
    associativeMemory->associate(patternA, patternB, 0.7f);
    
    float strength = associativeMemory->getAssociationStrength(patternA, patternB);
    assert(fabs(strength - 0.7f) < 0.001f);
    
    std::cout << "    testAssociativeMemoryBasic passed" << std::endl;
}

void testAssociativeMemoryFromExperience() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.3f, 0.7f, 0.5f};
    episode.resultingSensoryState = {0.4f, 0.8f, 0.6f};
    
    associativeMemory->associateFromExperience(episode);
    
    std::cout << "    testAssociativeMemoryFromExperience passed" << std::endl;
}

void testAssociativeMemorySpreadingActivation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    std::vector<float> cuePattern = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
    
    // Create association first
    associativeMemory->associate(cuePattern, patternB, 0.8f);
    
    std::vector<std::vector<float>> spreadResults = 
        associativeMemory->spreadActivation(cuePattern, 2);
    
    // Should include the directly associated pattern
    bool foundPatternB = false;
    for (const auto& pattern : spreadResults) {
        if (pattern.size() == patternB.size()) {
            bool matches = true;
            for (size_t i = 0; i < pattern.size(); ++i) {
                if (fabs(pattern[i] - patternB[i]) > 0.001f) {
                    matches = false;
                    break;
                }
            }
            if (matches) {
                foundPatternB = true;
                break;
            }
        }
    }
    
    assert(foundPatternB);
    
    std::cout << "    testAssociativeMemorySpreadingActivation passed" << std::endl;
}

void testMemorySystemsIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all memory systems work together
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Store in working memory
    std::vector<float> pattern = {0.5f, 0.6f, 0.7f, 0.8f};
    workingMemory->store(pattern, 1.0f);
    
    // Create and store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = pattern;
    episode.reward = 10.0f;
    episodicMemory->storeEpisode(episode);
    
    // Create association
    std::vector<float> patternB = {0.2f, 0.3f, 0.4f, 0.5f};
    associativeMemory->associate(pattern, patternB, 0.9f);
    
    // Verify all systems are working
    assert(workingMemory->getActiveTraces() > 0);
    assert(episodicMemory->getEpisodeCount() > 0);
    assert(associativeMemory->getAssociationCount() > 0);
    
    std::cout << "    testMemorySystemsIntegration passed" << std::endl;
}

void testMemorySystemErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test that memory systems handle empty states gracefully
    assert(workingMemory->getMemoryNeurons().empty());
    assert(episodicMemory->getEpisodeCount() == 0);
    assert(associativeMemory->getAssociationCount() == 0);
    
    // Test operations on empty systems
    workingMemory->clear();
    episodicMemory->clear();
    associativeMemory->clear();
    
    std::cout << "    testMemorySystemErrorHandling passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory System tests..." << std::endl;
    
    try {
        testWorkingMemoryBasic();
        testWorkingMemoryNeuronAccess();
        testWorkingMemoryCapacity();
        testWorkingMemoryDecay();
        testWorkingMemoryStrengthening();
        testWorkingMemoryCompetition();
        testWorkingMemoryMemoryActivity();
        
        testEpisodicMemoryBasic();
        testEpisodicMemoryRetrieval();
        testEpisodicMemoryTemporalRetrieval();
        testEpisodicMemoryReplay();
        testEpisodicMemoryConsolidation();
        
        testAssociativeMemoryBasic();
        testAssociativeMemoryFromExperience();
        testAssociativeMemorySpreadingActivation();
        
        testMemorySystemsIntegration();
        testMemorySystemErrorHandling();
        
        std::cout << "  All Memory System tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_memory_systems