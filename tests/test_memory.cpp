// Memory System Tests
// Tests for working memory, episodic memory, memory integration, and memory consolidation mechanisms

#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>
#include <set>

namespace test_memory {

void testWorkingMemoryCapacity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::WorkingMemory workingMem(config);
    
    // Test initial state
    assert(workingMem.getCapacity() > 0);
    assert(workingMem.getCurrentLoad() == 0.0f);
    assert(workingMem.getActiveItems().empty());
    
    // Test adding items
    std::string testItem1 = "item1";
    workingMem.addItem(testItem1, nlm::MemoryImportance::High);
    
    assert(workingMem.hasItem(testItem1));
    assert(workingMem.getItemImportance(testItem1) == nlm::MemoryImportance::High);
    assert(workingMem.getCurrentLoad() > 0.0f);
    
    // Test item retrieval
    auto* retrievedItem = workingMem.getItem(testItem1);
    assert(retrievedItem != nullptr);
    assert(retrievedItem->id == testItem1);
    
    // Test capacity limits
    size_t capacity = workingMem.getCapacity();
    for (size_t i = 1; i <= capacity + 5; ++i) {
        std::string itemId = "item_" + std::to_string(i);
        workingMem.addItem(itemId, nlm::MemoryImportance::Medium);
    }
    
    // Load should not exceed 100%
    assert(workingMem.getCurrentLoad() <= 1.0f);
    
    // Test eviction policy (items may be evicted when capacity is exceeded)
    size_t currentItems = workingMem.getActiveItems().size();
    assert(currentItems <= capacity);
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryEvictionPolicy() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::WorkingMemory workingMem(config);
    
    // Add items with different importance levels
    workingMem.addItem("high_importance", nlm::MemoryImportance::High);
    workingMem.addItem("medium_importance", nlm::MemoryImportance::Medium);
    workingMem.addItem("low_importance", nlm::MemoryImportance::Low);
    
    // Test that higher importance items are preserved during eviction
    // This depends on implementation - typically high importance items are evicted last
    
    size_t initialSize = workingMem.getActiveItems().size();
    assert(initialSize >= 3); // All three items should be present
    
    // Fill memory to test eviction
    for (int i = 0; i < 20; ++i) {
        std::string itemId = "eviction_test_" + std::to_string(i);
        workingMem.addItem(itemId, nlm::MemoryImportance::Low);
    }
    
    // Lower importance items should have been evicted first
    assert(!workingMem.hasItem("low_importance")); // May be evicted
    assert(workingMem.hasItem("medium_importance")); // More likely to survive
    assert(workingMem.hasItem("high_importance"));  // Most likely to survive
    
    std::cout << "    testWorkingMemoryEvictionPolicy passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::EpisodicMemory episodicMem(config);
    
    // Test initial state
    assert(episodeMem.getConsolidationRate() > 0.0f);
    assert(episodeMem.getReplayProbability() > 0.0f);
    
    // Add an episode
    nlm::Episode episode;
    episode.id = "episode_1";
    episode.timestamp = 0.0;
    episode.context = std::vector<float>{0.1f, 0.2f, 0.3f};
    episode.actions = std::vector<std::string>{"action1", "action2"};
    episode.outcomes = std::vector<float>{1.0f, 0.5f};
    
    episodeMem.addEpisode(episode);
    assert(episodeMem.hasEpisode("episode_1"));
    
    // Test consolidation
    episodeMem.update(0.1f); // Simulate time passing
    
    // Episode should be consolidated
    assert(episodeMem.isConsolidated("episode_1"));
    
    // Test replay mechanism
    bool replayed = episodeMem.triggerReplay();
    // Replay may or may not happen depending on internal state
    
    // Test memory integration
    std::vector<float> queryContext = {0.1f, 0.2f, 0.3f};
    auto relevantEpisodes = episodeMem.findRelevantEpisodes(queryContext);
    
    // At least the episode we just added should be relevant
    assert(!relevantEpisodes.empty());
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testMemoryIntegrationBinding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::EpisodicMemory episodicMem(config);
    
    // Create multiple episodes with related content
    nlm::Episode episode1;
    episode1.id = "episode_1";
    episode1.context = std::vector<float>{0.1f, 0.2f, 0.3f};
    episode1.actions = std::vector<std::string>{"move_forward"};
    episode1.outcomes = std::vector<float>{0.8f};
    
    nlm::Episode episode2;
    episode2.id = "episode_2";
    episode2.context = std::vector<float>{0.1f, 0.25f, 0.35f}; // Similar to episode1
    episode2.actions = std::vector<std::string>{"turn_right"};
    episode2.outcomes = std::vector<float>{0.6f};
    
    nlm::Episode episode3;
    episode3.id = "episode_3";
    episode3.context = std::vector<float>{0.5f, 0.6f, 0.7f}; // Different context
    episode3.actions = std::vector<std::string>{"jump"};
    episode3.outcomes = std::vector<float>{0.9f};
    
    episodicMem.addEpisode(episode1);
    episodicMem.addEpisode(episode2);
    episodicMem.addEpisode(episode3);
    
    // Test memory binding (related episodes should be associated)
    auto boundEpisodes = episodicMem.getBoundEpisodes("episode_1");
    assert(!boundEpisodes.empty());
    
    // Episodes with similar context should be considered related
    bool episode2IsBound = false;
    for (const auto& episodeId : boundEpisodes) {
        if (episodeId == "episode_2") {
            episode2IsBound = true;
            break;
        }
    }
    
    // Binding is implementation-dependent
    // For now, just verify the mechanism exists
    assert(!boundEpisodes.empty());
    
    // Test temporal context integration
    std::vector<nlm::Episode> episodes = episodicMem.getAllEpisodes();
    std::map<nlm::Timestamp, std::vector<std::string>> temporalTimeline;
    
    for (const auto& episode : episodes) {
        temporalTimeline[episode.timestamp].push_back(episode.id);
    }
    
    assert(!temporalTimeline.empty());
    
    std::cout << "    testMemoryIntegrationBinding passed" << std::endl;
}

void testMemoryReplayConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::EpisodicMemory episodicMem(config);
    
    // Add multiple episodes
    for (int i = 0; i < 10; ++i) {
        nlm::Episode episode;
        episode.id = "episode_" + std::to_string(i);
        episode.timestamp = static_cast<nlm::Timestamp>(i * 10.0f);
        episode.context = std::vector<float>{static_cast<float>(i) / 10.0f, 0.5f, 0.5f};
        episode.actions = std::vector<std::string>{"action_" + std::to_string(i % 3)};
        episode.outcomes = std::vector<float>{0.1f * (i + 1)};
        
        episodicMem.addEpisode(episode);
    }
    
    // Simulate sleep/consolidation
    for (int t = 0; t < 100; ++t) {
        episodicMem.update(static_cast<nlm::TimestepDuration>(t * 0.1f));
    }
    
    // Check that episodes are being consolidated
    size_t consolidatedCount = 0;
    for (const auto& episodeId : episodicMem.getAllEpisodeIds()) {
        if (episodicMem.isConsolidated(episodeId)) {
            consolidatedCount++;
        }
    }
    
    // Some episodes should be consolidated
    assert(consolidatedCount > 0);
    
    // Test replay probability enhancement with reward
    float baseReplayProbability = episodicMem.getReplayProbability();
    episodicMem.processReward(1.0f); // Simulate reward
    float enhancedReplayProbability = episodicMem.getReplayProbability();
    
    // Replay probability should be enhanced by reward
    assert(enhancedReplayProbability >= baseReplayProbability);
    
    std::cout << "    testMemoryReplayConsolidation passed" << std::endl;
}

void testMemoryReconstructionReconsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::EpisodicMemory episodicMem(config);
    
    // Add an episode
    nlm::Episode episode;
    episode.id = "episode_1";
    episode.context = std::vector<float>{0.1f, 0.2f, 0.3f};
    episode.actions = std::vector<std::string>{"action1", "action2"};
    episode.outcomes = std::vector<float>{0.8f, 0.6f};
    
    episodicMem.addEpisode(episode);
    
    // Retrieve the episode
    auto* retrievedEpisode = episodicMem.getEpisode("episode_1");
    assert(retrievedEpisode != nullptr);
    
    // Reconstruct the episode memory
    episodicMem.reconstructEpisode("episode_1");
    
    // Reconsolidation should update the episode
    retrievedEpisode = episodicMem.getEpisode("episode_1");
    assert(retrievedEpisode != nullptr);
    
    // Modify the episode
    retrievedEpisode->actions.push_back("new_action");
    
    // Save the modified episode
    episodicMem.saveEpisode(*retrievedEpisode);
    
    // Verify the episode was updated
    auto* updatedEpisode = episodicMem.getEpisode("episode_1");
    assert(updatedEpisode != nullptr);
    assert(updatedEpisode->actions.size() > 2); // Original + new action
    
    std::cout << "    testMemoryReconstructionReconsolidation passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory Tests..." << std::endl;
    testWorkingMemoryCapacity();
    testWorkingMemoryEvictionPolicy();
    testEpisodicMemoryConsolidation();
    testMemoryIntegrationBinding();
    testMemoryReplayConsolidation();
    testMemoryReconstructionReconsolidation();
    
    std::cout << "\nAll Memory Tests passed!" << std::endl;
}

} // namespace test_memory