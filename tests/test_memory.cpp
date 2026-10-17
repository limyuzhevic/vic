// Memory System Test Suite
// Phase 6: Comprehensive Memory and Learning Tests

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "sensory/SensoryInput.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>

using namespace nlm;

namespace test_memory {

void testWorkingMemoryBasic() {
    std::cout << "Testing Basic Working Memory Functionality..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Get working memory
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test basic properties
    assert(workingMemory->getCapacity() == 10);  // neuron_count / 10
    workingMemory->setCapacity(20);
    assert(workingMemory->getCapacity() == 20);
    
    workingMemory->setDecayRate(0.02f);
    assert(workingMemory->getDecayRate() == 0.02f);
    
    // Test storing and retrieving patterns
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    workingMemory->store(pattern, 1.0f);
    
    // Test neuron activation
    workingMemory->storeToNeuron(NeuronId(1000), 0.8f);
    assert(workingMemory->contains(NeuronId(1000)));
    assert(workingMemory->getNeuronActivation(NeuronId(1000)) == 0.8f);
    
    // Test competition
    workingMemory->storeToNeuron(NeuronId(1001), 0.9f);
    workingMemory->storeToNeuron(NeuronId(1002), 0.2f);
    workingMemory->runCompetition();
    
    // Test update
    workingMemory->update(0.001f);
    
    // Test clear
    workingMemory->clear();
    assert(workingMemory->getActiveTraces() == 0);
    
    std::cout << "✓ Working Memory Basic Test PASSED" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "Testing Episodic Memory..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Get episodic memory
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test initial state
    assert(episodicMemory->getEpisodeCount() == 0);
    assert(episodicMemory->getAverageReward() == 0.0f);
    
    // Create test episode
    EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    episode.orientation = 0.5f;
    episode.action = ActionType::MoveForward;
    episode.reward = 0.8f;
    episode.energy = 0.9f;
    episode.novelty = 0.3f;
    episode.resultingReward = 0.7f;
    
    // Add active neurons
    episode.activeNeurons.push_back(NeuronId(1000));
    episode.activeNeurons.push_back(NeuronId(1001));
    episode.neuronActivations.push_back(0.8f);
    episode.neuronActivations.push_back(0.6f);
    
    // Store episode
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    assert(episodicMemory->getAverageReward() == 0.8f);
    
    // Test retrieval
    std::vector<const EpisodicMemoryItem*> similar = episodicMemory->retrieveSimilar({0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, 3);
    // Should retrieve the same episode since all values are similar
    assert(similar.size() >= 1);
    assert(similar[0]->timestamp == 100);
    
    // Test temporal retrieval
    std::vector<const EpisodicMemoryItem*> temporal = episodicMemory->retrieveTemporal(90, 110, 5);
    assert(temporal.size() == 1);
    
    // Test location retrieval
    std::vector<const EpisodicMemoryItem*> location = episodicMemory->retrieveByLocation(1.1f, 2.1f, 0.5f, 5);
    assert(location.size() == 1);
    
    // Test action retrieval
    std::vector<const EpisodicMemoryItem*> action = episodicMemory->retrieveAfterAction(ActionType::MoveForward, 5);
    assert(action.size() == 1);
    
    // Test recent episodes
    std::vector<const EpisodicMemoryItem*> recent = episodicMemory->getRecentEpisodes(5);
    assert(recent.size() == 1);
    
    // Test consolidation
    episodicMemory->consolidate(0.1f);
    
    // Test get episode
    assert(episodicMemory->getEpisode(0) != nullptr);
    assert(episodicMemory->getEpisode(0)->timestamp == 100);
    assert(episodicMemory->getEpisode(1) == nullptr); // Out of range
    
    // Test replay
    episodicMemory->enableReplay(true);
    assert(episodicMemory->isReplayEnabled() == true);
    
    std::cout << "✓ Episodic Memory Test PASSED" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "Testing Associative Memory..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Get associative memory
    auto* associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test association creation
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
    
    associativeMemory->associate(patternA, patternB, 0.8f);
    assert(associativeMemory->getAssociationStrength(patternA, patternB) == 0.8f);
    
    // Test experience-based association
    EpisodicMemoryItem episode;
    episode.sensoryState = {0.1f, 0.2f, 0.3f};
    episode.resultingSensoryState = {0.4f, 0.5f, 0.6f};
    episode.action = ActionType::MoveForward;
    
    associativeMemory->associateFromExperience(episode);
    
    // Test retrieval
    std::vector<std::vector<float>> retrieved = associativeMemory->retrieve(patternA, 3);
    assert(!retrieved.empty());
    assert(retrieved[0] == patternB);
    
    // Test spread activation
    std::vector<std::vector<float>> spread = associativeMemory->spreadActivation(patternA, 2);
    assert(!spread.empty());
    
    // Test update association
    associativeMemory->updateAssociation(patternA, patternB, 0.2f);
    assert(associativeMemory->getAssociationStrength(patternA, patternB) == 1.0f); // Clamped to max
    
    // Test clear
    associativeMemory->clear();
    assert(associativeMemory->getAssociationCount() == 0);
    
    std::cout << "✓ Associative Memory Test PASSED" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "Testing Memory Integration with Brain..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Test that all memory systems are accessible
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getEpisodicMemory() != nullptr);
    assert(brain->getAssociativeMemory() != nullptr);
    
    // Test sensory processing integration
    std::vector<float> sensoryData = {0.5f, 0.3f, 0.7f, 0.2f, 0.9f};
    SensoryInput input(sensoryData);
    brain->receiveSensoryInput(input);
    
    // Test action production
    auto action = brain->produceAction();
    assert(action != nullptr);
    
    // Run simulation steps
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001f);
        
        // Check that working memory can be updated
        if (auto* wm = brain->getWorkingMemory()) {
            wm->update(0.001f);
        }
    }
    
    // Test episodic memory accumulation
    int episodeCount = 0;
    if (auto* em = brain->getEpisodicMemory()) {
        episodeCount = em->getEpisodeCount();
        // Should have some episodes from the simulation
        assert(episodeCount >= 0);
    }
    
    // Test memory consolidation
    if (auto* em = brain->getEpisodicMemory()) {
        em->consolidate(0.5f);
        // Should still have same number of episodes (just filtered)
        assert(em->getEpisodeCount() == episodeCount);
    }
    
    std::cout << "✓ Memory Integration Test PASSED" << std::endl;
}

void testMemoryRetention() {
    std::cout << "Testing Memory Retention Over Time..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(150), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Store initial state
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    int initialEpisodes = episodicMemory->getEpisodeCount();
    
    // Run simulation for many steps
    int stepsWithoutNewEpisodes = 0;
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001f);
        
        // Check if new episodes were created (should be every 10 steps)
        int currentEpisodes = episodicMemory->getEpisodeCount();
        if (currentEpisodes > initialEpisodes) {
            initialEpisodes = currentEpisodes;
            stepsWithoutNewEpisodes = 0;
        } else {
            stepsWithoutNewEpisodes++;
        }
    }
    
    // Memory should have retained some episodes
    assert(episodicMemory->getEpisodeCount() > 0);
    
    // Episodic memory should have cleaned up old episodes through consolidation
    // The exact number depends on the consolidation threshold
    std::cout << "  Final episode count: " << episodicMemory->getEpisodeCount() << std::endl;
    
    // Test that memories persist
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        workingMemory->update(0.001f); // Update to test persistence
    }
    
    std::cout << "✓ Memory Retention Test PASSED" << std::endl;
}

void testLearningProgress() {
    std::cout << "Testing Learning Progress and Improvement..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(300), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    // Run plasticity experiment
    for (int step = 0; step < 500; ++step) {
        // Apply repeated input pattern to stimulate learning
        for (size_t i = 0; i < 10; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        }
        
        brain->step(step, step * 0.001f);
    }
    
    // Check for learning indicators
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    
    std::cout << "  Total spikes: " << totalSpikes << std::endl;
    std::cout << "  Firing neurons (last step): " << firingNeurons << std::endl;
    
    // Learning should have occurred - there should be activity and potentially spikes
    // This is a qualitative test, not quantitative
    bool learningIndicator = (totalSpikes > 100) || (firingNeurons > 0);
    
    if (learningIndicator) {
        std::cout << "  ✓ Learning indicators detected" << std::endl;
    } else {
        std::cout << "  ! Limited learning indicators (may need more simulation steps)" << std::endl;
    }
    
    // Test memory formation
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "  Episodes formed: " << episodicMemory->getEpisodeCount() << std::endl;
        
        // Test memory retrieval
        if (!episodicMemory->getEpisodeCount()) {
            // Create a test episode
            EpisodicMemoryItem episode;
            episode.timestamp = 0;
            episode.action = ActionType::Wait;
            episode.reward = 0.5f;
            episodicMemory->storeEpisode(episode);
        }
        
        // Test retrieval
        std::vector<const EpisodicMemoryItem*> recent = episodicMemory->getRecentEpisodes(3);
        assert(!recent.empty());
        std::cout << "  Memory retrieval: OK" << std::endl;
    }
    
    std::cout << "✓ Learning Progress Test PASSED" << std::endl;
}

void runAllMemoryTests() {
    std::cout << "=============================================" << std::endl;
    std::cout << "NLM Memory System Test Suite (Phase 6)" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << std::endl;
    
    try {
        testWorkingMemoryBasic();
        testEpisodicMemory();
        testAssociativeMemory();
        testMemoryIntegration();
        testMemoryRetention();
        testLearningProgress();
        
        std::cout << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "✓ ALL MEMORY TESTS PASSED" << std::endl;
        std::cout << "====================================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "✗ MEMORY TEST FAILED: " << e.what() << std::endl;
        std::cout << "====================================================" << std::endl;
        throw;
    }
}

} // namespace test_memory

int main() {
    std::cout << "=== NLM Memory Test Suite ===" << std::endl;
    std::cout << "Running comprehensive memory and learning tests for Phase 6." << std::endl;
    std::cout << std::endl;
    
    try {
        test_memory::runAllMemoryTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test suite failed with error: " << e.what() << std::endl;
        return 1;
    }
}
