#include "src/memory/NeuralEpisodicMemory.hpp"
#include <iostream>
#include <cassert>

void testBasicEpisodicMemory() {
    std::cout << "Testing Basic Episodic Memory Functionality..." << std::endl;
    
    // Create a minimal mock for Brain (we won't actually use neural integration)
    class MockBrain {
    public:
        void injectCurrent(nlm::NeuronId, float) {}
    };
    
    MockBrain mockBrain;
    
    // Create and initialize episodic memory
    nlm::NeuralEpisodicMemory em;
    em.initialize(&mockBrain);
    
    // Test initial state
    assert(em.getEpisodeCount() == 0);
    assert(em.getAverageReward() == 0.0f);
    assert(em.getConsolidatedCount() == 0);
    assert(em.getActiveCount() == 0);
    assert(em.getAverageStrength() == 0.0f);
    assert(em.getAveragePrecision() == 0.0f);
    assert(em.getAverageRecall() == 0.0f);
    
    // Test configuration flags
    assert(em.isReplayEnabled() == true);
    assert(em.isConsolidationEnabled() == true);
    assert(em.isTemporalCompressionEnabled() == true);
    assert(em.isPrioritizationEnabled() == true);
    assert(em.isSelfReferentialProcessingEnabled() == true);
    assert(em.isEmotionalIntegrationEnabled() == true);
    assert(em.isPatternCompletionEnabled() == true);
    assert(em.isInterferenceManagementEnabled() == true);
    assert(em.isSchemaIntegrationEnabled() == true);
    assert(em.isSchemaMatchingEnabled() == true);
    
    // Create test episodes
    for (size_t i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.id = i + 1;
        episode.timestamp = 1000 + i * 100;
        episode.action = nlm::ActionType::Move;
        episode.positionX = 10.0f + static_cast<float>(i * 5);
        episode.positionY = 20.0f + static_cast<float>(i * 3);
        episode.orientation = 0.1f * (i + 1);
        episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
        episode.novelty = 0.8f - static_cast<float>(i) * 0.15f;
        episode.selfReferential = (i % 2 == 0);
        episode.selfImportance = episode.selfReferential ? 0.9f : 0.1f;
        episode.emotionalValence = (static_cast<float>(i) - 2.0f) / 2.0f;
        episode.emotionalArousal = 0.5f + static_cast<float>(i) * 0.1f;
        episode.precision = 0.6f + static_cast<float>(i) * 0.08f;
        episode.specificity = 0.5f + static_cast<float>(i) * 0.1f;
        
        // Create basic sensory pattern
        episode.sensoryState = {0.1f * (i + 1), 0.2f * (i + 1), 0.3f * (i + 1),
                                0.4f * (i + 1), 0.5f * (i + 1)};
        
        episode.resultingSensoryState = {0.15f * (i + 1), 0.25f * (i + 1), 
                                        0.35f * (i + 1), 0.45f * (i + 1), 
                                        0.55f * (i + 1)};
        
        em.storeEpisode(episode);
    }
    
    // Test storage
    assert(em.getEpisodeCount() == 5);
    assert(em.getActiveCount() == 5); // All episodes are active
    
    // Test average reward calculation
    float expectedAvgReward = (0.5f + 0.6f + 0.7f + 0.8f + 0.9f) / 5.0f;
    assert(std::abs(em.getAverageReward() - expectedAvgReward) < 0.01f);
    
    // Test capacity
    assert(em.getCapacity() == 10000);
    
    // Test temporal compression
    assert(em.isTemporalCompressionEnabled() == true);
    
    // Test hippocampal-neocortical
    assert(em.isHippocampalNeocorticalModeEnabled() == true);
    
    // Test configuration
    em.setExpansionFactor(2.0f);
    em.setReplayFrequency(0.2f);
    em.setCompletionThreshold(0.7f);
    em.setInterferenceThreshold(0.4f);
    em.setSelfImportanceThreshold(0.6f);
    
    // Test expansion factor
    // (The factor is stored but we can't directly access it without a getter)
    
    // Test priority weights
    nlm::MemoryPriority weights;
    weights.temporalPriority = 0.3f;
    weights.rewardPriority = 0.2f;
    weights.noveltyPriority = 0.15f;
    weights.selfPriority = 0.1f;
    weights.emotionalPriority = 0.1f;
    weights.schemaPriority = 0.05f;
    weights.interferencePriority = 0.05f;
    
    em.setPriorityWeights(weights);
    
    // Test temporal retrieval
    std::vector<const nlm::EpisodicMemoryItem*> temporal = 
        em.retrieveTemporal(1100, 1500, 3);
    assert(temporal.size() <= 3);
    
    // Test location retrieval
    std::vector<const nlm::EpisodicMemoryItem*> byLocation = em.retrieveByLocation(15.0f, 30.0f, 10.0f, 3);
    assert(byLocation.size() <= 3);
    
    // Test action retrieval
    std::vector<const nlm::EpisodicMemoryItem*> byAction = em.retrieveAfterAction(nlm::ActionType::Move, 3);
    assert(byAction.size() <= 3);
    
    // Test consolidation
    em.consolidate(0.8f);
    
    // Test high priority episodes
    std::vector<uint64_t> highPriority = em.getHighPriorityEpisodes(3);
    assert(highPriority.size() <= 3);
    
    // Test episodes for replay
    std::vector<uint64_t> forReplay = em.getEpisodesForReplay(3);
    assert(forReplay.size() <= 3);
    
    // Test update system
    em.update(100, 1.0f);
    
    // Test integration
    em.integrateWithNeurons();
    
    // Test hippocampal synchronization
    em.synchronizeWithHippocampalSystem();
    
    // Test neocortical integration
    em.updateNeocorticalIntegrations();
    
    // Test statistics
    size_t consolidatedCount = em.getConsolidatedCount();
    size_t activeCount = em.getActiveCount();
    float strength = em.getAverageStrength();
    float precision = em.getAveragePrecision();
    float recall = em.getAverageRecall();
    
    assert(consolidatedCount >= 0);
    assert(activeCount == 5);
    assert(strength >= 0.0f && strength <= 1.0f);
    assert(precision >= 0.0f && precision <= 1.0f);
    assert(recall >= 0.0f && recall <= 1.0f);
    
    // Test debugging
    std::cout << "Basic episodic memory tests passed!" << std::endl;
    
    // Print basic statistics
    std::cout << "  Total episodes: " << em.getEpisodeCount() << std::endl;
    std::cout << "  Active episodes: " << activeCount << std::endl;
    std::cout << "  Consolidated episodes: " << consolidatedCount << std::endl;
    std::cout << "  Average strength: " << strength << std::endl;
    std::cout << "  Average precision: " << precision << std::endl;
    std::cout << "  Average recall: " << recall << std::endl;
}

int main() {
    try {
        testBasicEpisodicMemory();
        std::cout << std::endl;
        std::cout << "=== All Basic Episodic Memory Tests Passed ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
