// Memory System Unit Tests
// These tests verify the functionality of NLM's memory systems

#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_memory_systems {

void testNeuralWorkingMemoryBasic() {
    std::cout << "    testNeuralWorkingMemoryBasic..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create working memory
    nlm::NeuralWorkingMemory workingMemory;
    workingMemory.initialize(brain.get());
    
    // Test initial state
    assert(workingMemory.getActiveTraces() == 0);
    assert(workingMemory.getMemoryNeurons().size() == 0);
    
    // Store some information
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    workingMemory.store(pattern, 1.0f);
    
    // Verify memory was stored
    assert(workingMemory.getActiveTraces() > 0);
    assert(workingMemory.getMemoryNeurons().size() > 0);
    
    // Retrieve memory
    std::vector<float> retrieved = workingMemory.retrieve();
    assert(retrieved.size() == pattern.size());
    
    // Test capacity
    workingMemory.setCapacity(50);
    assert(workingMemory.getCapacity() == 50);
    
    // Test decay rate
    workingMemory.setDecayRate(0.02f);
    assert(workingMemory.getDecayRate() == 0.02f);
    
    // Test clear
    workingMemory.clear();
    assert(workingMemory.getActiveTraces() == 0);
    
    std::cout << "    testNeuralWorkingMemoryBasic passed" << std::endl;
}

void testNeuralWorkingMemoryCapacity() {
    std::cout << "    testNeuralWorkingMemoryCapacity..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create working memory
    nlm::NeuralWorkingMemory workingMemory;
    workingMemory.initialize(brain.get());
    
    // Set small capacity
    workingMemory.setCapacity(10);
    
    // Store more patterns than capacity
    for (int i = 0; i < 20; ++i) {
        std::vector<float> pattern(static_cast<size_t>(i % 5) + 1, static_cast<float>(i % 10) * 0.1f);
        workingMemory.store(pattern, 1.0f);
    }
    
    // Should have some traces but not all 20
    assert(workingMemory.getActiveTraces() <= 10); // Capacity limit
    
    // Test storage to specific neuron
    nlm::NeuronId neuronId(42);
    workingMemory.storeToNeuron(neuronId, 0.8f);
    
    // Verify neuron is stored
    assert(workingMemory.contains(neuronId));
    
    // Get neuron activation
    float activation = workingMemory.getNeuronActivation(neuronId);
    assert(activation == 0.8f);
    
    std::cout << "    testNeuralWorkingMemoryCapacity passed" << std::endl;
}

void testNeuralEpisodicMemoryBasic() {
    std::cout << "    testNeuralEpisodicMemoryBasic..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create episodic memory
    nlm::NeuralEpisodicMemory episodicMemory;
    episodicMemory.initialize(brain.get());
    
    // Test initial state
    assert(episodicMemory.getEpisodeCount() == 0);
    
    // Store an episode
    std::vector<float> state = {0.5f, 0.6f, 0.7f, 0.8f, 0.9f};
    std::vector<nlm::Action> actions;
    actions.push_back(nlm::Action::MoveForward);
    actions.push_back(nlm::Action::TurnLeft);
    
    size_t episodeId = episodicMemory.storeEpisode(state, actions);
    assert(episodeId > 0);
    
    // Verify episode was stored
    assert(episodicMemory.getEpisodeCount() == 1);
    
    // Test replay
    auto episodes = episodicMemory.getEpisodesForReplay(1);
    assert(episodes.size() == 1);
    
    // Verify episode contents
    std::vector<float> retrievedState = episodes[0].first;
    std::vector<nlm::Action> retrievedActions = episodes[0].second;
    
    assert(retrievedState.size() == state.size());
    assert(retrievedActions.size() == actions.size());
    
    // Test memory consolidation
    episodicMemory.consolidateMemory();
    
    std::cout << "    testNeuralEpisodicMemoryBasic passed" << std::endl;
}

void testNeuralEpisodicMemoryReplay() {
    std::cout << "    testNeuralEpisodicMemoryReplay..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create episodic memory
    nlm::NeuralEpisodicMemory episodicMemory;
    episodicMemory.initialize(brain.get());
    
    // Store multiple episodes
    for (int episodeNum = 0; episodeNum < 10; ++episodeNum) {
        std::vector<float> state;
        for (int i = 0; i < 10; ++i) {
            state.push_back(static_cast<float>(episodeNum * 10 + i) * 0.01f);
        }
        
        std::vector<nlm::Action> actions;
        actions.push_back(nlm::Action::MoveForward);
        if (episodeNum % 2 == 0) actions.push_back(nlm::Action::TurnLeft);
        else actions.push_back(nlm::Action::TurnRight);
        
        episodicMemory.storeEpisode(state, actions);
    }
    
    // Verify all episodes were stored
    assert(episodicMemory.getEpisodeCount() == 10);
    
    // Test replay with different retrieval counts
    auto allEpisodes = episodicMemory.getEpisodesForReplay(20);
    assert(allEpisodes.size() == 10); // Should retrieve all
    
    auto threeEpisodes = episodicMemory.getEpisodesForReplay(3);
    assert(threeEpisodes.size() == 3);
    
    auto oneEpisode = episodicMemory.getEpisodesForReplay(1);
    assert(oneEpisode.size() == 1);
    
    // Test replay after consolidation
    episodicMemory.consolidateMemory();
    
    auto consolidatedEpisodes = episodicMemory.getEpisodesForReplay(5);
    assert(consolidatedEpisodes.size() <= 5);
    
    std::cout << "    testNeuralEpisodicMemoryReplay passed" << std::endl;
}

void runAllMemoryTests() {
    std::cout << "=== Running Memory System Tests ===" << std::endl;
    
    testNeuralWorkingMemoryBasic();
    testNeuralWorkingMemoryCapacity();
    testNeuralEpisodicMemoryBasic();
    testNeuralEpisodicMemoryReplay();
    
    std::cout << "=== All Memory System Tests Passed ===" << std::endl;
}

} // namespace test_memory_systems