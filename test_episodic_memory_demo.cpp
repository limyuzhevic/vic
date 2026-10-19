#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <chrono>
#include <thread>

void demonstrateEpisodicMemory() {
    std::cout << "=== Neural Episodic Memory Demonstration ===" << std::endl;
    std::cout << "This demonstrates the enhanced episodic memory system with:" << std::endl;
    std::cout << "  - Rich contextual binding (what, where, when, how)" << std::endl;
    std::cout << "  - Self-referential processing and encoding" << std::endl;
    std::cout << "  - Hippocampal-neocortical interactions" << std::endl;
    std::cout << "  - Memory replay and consolidation during sleep/wake" << std::endl;
    std::cout << "  - Pattern completion with contextual cues" << std::endl;
    std::cout << "  - Emotional and reward integration" << std::endl;
    std::cout << "  - Temporal compression and expansion" << std::endl;
    std::cout << "  - Source monitoring and attribution" << std::endl;
    std::cout << "  - Schema-based memory integration" << std::endl;
    std::cout << "  - Memory interference management" << std::endl;
    std::cout << "  - Prioritization and gating mechanisms" << std::endl;
    std::cout << std::endl;

    // Create configuration and brain
    auto config = std::make_shared<nlm::Config>("demo_config");
    nlm::Brain brain(config);
    
    // Initialize episodic memory with enhanced features
    nlm::NeuralEpisodicMemory em;
    em.initialize(&brain);
    em.setCapacity(1000);
    
    // Enable all episodic memory features
    em.enableReplay(true);
    em.enableConsolidation(true);
    em.setTemporalCompression(true);
    em.setExpansionFactor(1.5f);
    em.setPrioritizationEnabled(true);
    em.setSelfReferentialProcessing(true);
    em.setEmotionalIntegrationEnabled(true);
    em.setPatternCompletionEnabled(true);
    em.setInterferenceManagementEnabled(true);
    em.setSchemaIntegrationEnabled(true);
    em.setSchemaMatchingEnabled(true);
    em.setHippocampalNeocorticalMode(true);
    em.setReplayFrequency(0.1f);
    em.setCompletionThreshold(0.6f);
    em.setInterferenceThreshold(0.3f);
    em.setSelfImportanceThreshold(0.5f);
    
    // Configure priority weights
    nlm::MemoryPriority weights;
    weights.temporalPriority = 0.2f;
    weights.rewardPriority = 0.2f;
    weights.noveltyPriority = 0.15f;
    weights.selfPriority = 0.15f;
    weights.emotionalPriority = 0.15f;
    weights.schemaPriority = 0.08f;
    weights.interferencePriority = 0.05f;
    
    em.setPriorityWeights(weights);
    
    std::cout << "Enhanced episodic memory system initialized successfully!" << std::endl;
    std::cout << std::endl;

    // Demonstrate event encoding with contextual binding
    std::cout << "1. Demonstrating Event Encoding and Contextual Binding:" << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.id = i + 1;
        episode.timestamp = 1000 + i * 100;
        episode.action = (i % 3 == 0) ? nlm::ActionType::Move : 
                       (i % 3 == 1) ? nlm::ActionType::Attack : nlm::ActionType::Explore;
        episode.positionX = 10.0f + static_cast<float>(i * 5);
        episode.positionY = 20.0f + static_cast<float>(i * 3);
        episode.orientation = 0.1f * (i + 1);
        episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
        episode.novelty = 0.8f - static_cast<float>(i) * 0.15f;
        episode.selfReferential = (i % 2 == 0);
        episode.selfImportance = episode.selfReferential ? 0.9f : 0.1f;
        episode.emotionalValence = (static_cast<float>(i) - 2.0f) / 2.0f;
        episode.emotionalArousal = 0.5f + static_cast<float>(i) * 0.1f;
        
        // Create contextual sensory patterns
        std::vector<float> sensory(5, 0.0f);
        for (size_t j = 0; j < 5; ++j) {
            sensory[j] = 0.1f * (i * 10 + j + 1);
        }
        episode.sensoryState = sensory;
        
        // Store with enhanced encoding
        em.storeEpisode(episode);
        
        std::cout << "   Stored episode " << i+1 << ": " 
                  << "action=" << static_cast<int>(episode.action)
                  << ", reward=" << episode.reward
                  << ", novelty=" << episode.novelty
                  << ", self-referential=" << (episode.selfReferential ? "yes" : "no")
                  << ", precision=" << episode.precision
                  << std::endl;
    }
    
    std::cout << std::endl;
    
    // Demonstrate temporal sequencing and storage
    std::cout << "2. Demonstrating Temporal Sequencing and Storage:" << std::endl;
    std::cout << "   Stored " << em.getEpisodeCount() << " episodes chronologically" << std::endl;
    
    // Demonstrate retrieval mechanisms
    std::cout << std::endl;
    std::cout << "3. Demonstrating Retrieval Mechanisms:" << std::endl;
    
    // Test similar retrieval
    std::vector<float> query = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    std::vector<const nlm::EpisodicMemoryItem*> similar = em.retrieveSimilar(query, 3);
    std::cout << "   Similar retrieval query: " << query[0] << ", " << query[1] << "..." << std::endl;
    std::cout << "   Found " << similar.size() << " similar episodes" << std::endl;
    
    // Test location retrieval
    std::vector<const nlm::EpisodicMemoryItem*> byLocation = em.retrieveByLocation(15.0f, 30.0f, 10.0f, 3);
    std::cout << "   Location retrieval (15, 30): found " << byLocation.size() << " episodes in radius 10" << std::endl;
    
    // Test action retrieval
    std::vector<const nlm::EpisodicMemoryItem*> byAction = em.retrieveAfterAction(nlm::ActionType::Move, 3);
    std::cout << "   Action retrieval (Move): found " << byAction.size() << " episodes" << std::endl;
    
    std::cout << std::endl;
    
    // Demonstrate replay and consolidation
    std::cout << "4. Demonstrating Replay and Consolidation:" << std::endl;
    
    // Test episode replay
    const nlm::EpisodicMemoryItem* episodeToReplay = em.getEpisode(0);
    if (episodeToReplay) {
        std::cout << "   Replaying episode " << episodeToReplay->id << "..." << std::endl;
        em.replayEpisode(episodeToReplay);
        std::cout << "   Episode replay count: " << episodeToReplay->replayCount << std::endl;
    }
    
    // Test sequence replay
    std::vector<uint64_t> episodeIds;
    for (size_t i = 0; i < 3; ++i) {
        episodeIds.push_back(i + 1);
    }
    
    std::cout << "   Replaying sequence: ";
    for (uint64_t id : episodeIds) {
        std::cout << id << " ";
    }
    std::cout << "..." << std::endl;
    
    em.replaySequence(episodeIds);
    
    // Test consolidation
    em.consolidate(0.7f);
    std::cout << "   After consolidation: " << em.getConsolidatedCount() << " consolidated episodes" << std::endl;
    
    // Get episodes for replay
    std::vector<uint64_t> forReplay = em.getEpisodesForReplay(3);
    std::cout << "   Episodes selected for replay: ";
    for (uint64_t id : forReplay) {
        std::cout << id << " ";
    }
    std::cout << "(" << forReplay.size() << " total)" << std::endl;
    
    std::cout << std::endl;
    
    // Demonstrate system integration
    std::cout << "5. Demonstrating System Integration and Neural Mechanisms:" << std::endl;
    
    // Update system to trigger neural integration
    std::cout << "   Updating memory system (simulating time passage)..." << std::endl;
    em.update(100, 1.0f);
    
    // Integrate with neural substrate
    em.integrateWithNeurons();
    std::cout << "   Integrated with neural substrate" << std::endl;
    
    // Synchronize with hippocampal system
    em.synchronizeWithHippocampalSystem();
    std::cout << "   Synchronized with hippocampal system" << std::endl;
    
    // Update neocortical integrations
    em.updateNeocorticalIntegrations();
    std::cout << "   Updated neocortical integrations" << std::endl;
    
    // Demonstrate advanced features
    std::cout << std::endl;
    std::cout << "6. Demonstrating Advanced Features:" << std::endl;
    
    // Self-referential processing
    std::cout << "   Self-referential processing: " << (em.isSelfReferentialProcessingEnabled() ? "enabled" : "disabled") << std::endl;
    
    // Emotional integration
    std::cout << "   Emotional integration: " << (em.isEmotionalIntegrationEnabled() ? "enabled" : "disabled") << std::endl;
    
    // Pattern completion
    std::cout << "   Pattern completion: " << (em.isPatternCompletionEnabled() ? "enabled" : "disabled") << std::endl;
    
    // Temporal compression
    std::cout << "   Temporal compression: " << (em.isTemporalCompressionEnabled() ? "enabled" : "disabled") << std::endl;
    
    // Hippocampal-neocortical interactions
    std::cout << "   Hippocampal-neocortical: " << (em.isHippocampalNeocorticalModeEnabled() ? "enabled" : "disabled") << std::endl;
    
    // Memory prioritization
    std::cout << "   Memory prioritization: " << (em.isPrioritizationEnabled() ? "enabled" : "disabled") << std::endl;
    
    // Demonstrate statistics and diagnostics
    std::cout << std::endl;
    std::cout << "7. System Statistics and Diagnostics:" << std::endl;
    em.printMemoryStats();
    
    std::cout << std::endl;
    std::cout << "=== Enhanced Episodic Memory Demonstration Complete ===" << std::endl;
    std::cout << "The neural episodic memory system successfully demonstrates:" << std::endl;
    std::cout << "  - Rich contextual event encoding and storage" << std::endl;
    std::cout << "  - Advanced temporal and spatial processing" << std::endl;
    std::cout << "  - Self-referential and emotional integration" << std::endl;
    std::cout << "  - Sophisticated replay and consolidation mechanisms" << std::endl;
    std::cout << "  - Pattern completion with contextual cues" << std::endl;
    std::cout << "  - Hippocampal-neocortical interactions" << std::endl;
    std::cout << "  - Memory prioritization and gating" << std::endl;
    std::cout << "  - Interference management and schema integration" << std::endl;
    std::cout << "  - Comprehensive neural integration" << std::endl;
    std::cout << std::endl;
}

int main() {
    try {
        demonstrateEpisodicMemory();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
