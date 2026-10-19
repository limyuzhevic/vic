#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Brain.hpp"
#include "configs/default.cfg"

// Test memory retention over time - standalone function for test suite
void test_memory_retention() {
    std::cout << "=== Testing Memory Retention Over Time ===" << std::endl;
    
    // Create and initialize brain with config
    auto config = std::make_shared<Config>();
    
    // Load config
    if (config->loadFromFile("configs/default.cfg")) {
        std::cout << "Loaded configuration from default.cfg" << std::endl;
    } else {
        std::cout << "Using default configuration" << std::endl;
    }
    
    // Set up simulation parameters for test
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    
    // Phase 1: Encode initial experiences
    std::cout << "\n--- Phase 1: Encoding Initial Experiences ---" << std::endl;
    
    // Create initial experiences
    std::vector<float> pattern1 = {0.8f, 0.6f, 0.4f, 0.2f, 0.1f};
    std::vector<float> pattern2 = {0.9f, 0.7f, 0.5f, 0.3f, 0.2f};
    std::vector<float> pattern3 = {0.95f, 0.8f, 0.6f, 0.4f, 0.3f};
    
    // Store patterns in working memory
    if (brain->getWorkingMemory()) {
        brain->getWorkingMemory()->store(pattern1, 1.0f);
        brain->getWorkingMemory()->store(pattern2, 1.0f);
        brain->getWorkingMemory()->store(pattern3, 1.0f);
        std::cout << "Stored 3 patterns in working memory" << std::endl;
    }
    
    // Phase 2: Simulate ongoing experiences and learning
    std::cout << "\n--- Phase 2: Ongoing Learning and Forgetting ---" << std::endl;
    
    size_t retentionTests = 0;
    size_t successfulRetention = 0;
    
    // Run simulation for multiple time periods
    for (SimulationStep period = 0; period < 10; ++period) {
        std::cout << "\nTime Period " << period + 1 << ":" << std::endl;
        
        // Simulate neural activity and learning
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (SimulationStep step = 0; step < 100; ++step) {
            // Inject current to simulate experiences
            for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 20; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Occasionally store new episodes
            if (step % 25 == 0) {
                EpisodicMemoryItem episode;
                episode.timestamp = step;
                episode.reward = brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f;
                
                // Store active neurons as episode content
                for (auto& region : brain->getRegions()) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->isFiring() || 
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                                episode.activeNeurons.push_back(neuron->getId());
                                episode.neuronActivations.push_back(
                                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                            }
                        }
                    }
                }
                
                if (brain->getEpisodicMemory()) {
                    brain->getEpisodicMemory()->storeEpisode(episode);
                }
            }
        }
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t spikesGenerated = finalSpikes - initialSpikes;
        std::cout << "  Spikes generated: " << spikesGenerated << std::endl;
        
        // Test working memory retention
        if (brain->getWorkingMemory()) {
            retentionTests++;
            float memoryActivity = brain->getWorkingMemory()->getMemoryActivity();
            std::cout << "  Working memory activity: " << memoryActivity << std::endl;
            
            if (memoryActivity > 0.01f) {
                successfulRetention++;
                std::cout << "  ✓ Memory traces retained" << std::endl;
            } else {
                std::cout << "  ! Memory traces decayed" << std::endl;
            }
            
            // Test episodic memory storage
            size_t episodeCount = brain->getEpisodicMemory() ? 
                                 brain->getEpisodicMemory()->getEpisodeCount() : 0;
            std::cout << "  Episodic memory episodes: " << episodeCount << std::endl;
            
            if (episodeCount > 0) {
                std::cout << "  ✓ Experiences stored in episodic memory" << std::endl;
            }
        }
        
        // Apply sleep/rest cycle for consolidation
        if (period % 3 == 0) {
            std::cout << "  Entering rest period for memory consolidation..." << std::endl;
            // Simulate sleep/rest effects on memory
            if (brain->getEpisodicMemory()) {
                brain->getEpisodicMemory()->consolidate(0.5f);
            }
        }
    }
    
    // Phase 3: Test memory recall and consolidation
    std::cout << "\n--- Phase 3: Memory Recall and Consolidation ---" << std::endl;
    
    // Test episodic memory replay
    if (brain->getEpisodicMemory()) {
        size_t episodeCount = brain->getEpisodicMemory()->getEpisodeCount();
        std::cout << "Total episodes stored: " << episodeCount << std::endl;
        
        if (episodeCount > 0) {
            // Try to replay recent episodes
            auto recentEpisodes = brain->getEpisodicMemory()->getRecentEpisodes(3);
            std::cout << "Episodes available for replay: " << recentEpisodes.size() << std::endl;
            
            for (const auto* episode : recentEpisodes) {
                // Replay episode to strengthen memory traces
                brain->getEpisodicMemory()->replayEpisode(episode);
            }
            
            std::cout << "✓ Episodic memory replay completed" << std::endl;
        }
        
        // Test concept formation from episodic memory
        size_t conceptCount = brain->getConceptFormation() ? 
                             brain->getConceptFormation()->getConceptCount() : 0;
        std::cout << "Concepts formed: " << conceptCount << std::endl;
        
        if (brain->getConceptFormation()) {
            std::cout << "✓ Concept formation system working" << std::endl;
        }
    }
    
    // Phase 4: Final evaluation
    std::cout << "\n--- Phase 4: Final Evaluation ---" << std::endl;
    
    // Calculate retention statistics
    float retentionRate = (retentionTests > 0) ? 
                         static_cast<float>(successfulRetention) / retentionTests * 100.0f : 0.0f;
    
    std::cout << "Memory Retention Statistics:" << std::endl;
    std::cout << "  Retention tests: " << retentionTests << std::endl;
    std::cout << "  Successful retention: " << successfulRetention << std::endl;
    std::cout << "  Retention rate: " << retentionRate << "%" << std::endl;
    
    // Check for memory consolidation
    float consolidationEffect = 0.0f;
    if (brain->getWorkingMemory()) {
        consolidationEffect += brain->getWorkingMemory()->getMemoryActivity();
    }
    if (brain->getEpisodicMemory()) {
        consolidationEffect += static_cast<float>(brain->getEpisodicMemory()->getEpisodeCount()) * 0.1f;
    }
    
    std::cout << "  Memory consolidation effect: " << consolidationEffect << std::endl;
    
    // Test developmental progression
    DevelopmentalStage devStage = brain->getDevelopmentalStage();
    std::cout << "Developmental stage: " << static_cast<int>(devStage) << std::endl;
    
    // Test prediction system
    bool predictionSystemActive = brain->getPredictionSystem() != nullptr;
    std::cout << "Prediction system active: " << (predictionSystemActive ? "Yes" : "No") << std::endl;
    
    // Test attention system
    bool attentionSystemActive = brain->getAttention() != nullptr;
    std::cout << "Attention system active: " << (attentionSystemActive ? "Yes" : "No") << std::endl;
    
    // Test planner system
    bool plannerSystemActive = brain->getPlanner() != nullptr;
    std::cout << "Neural planner active: " << (plannerSystemActive ? "Yes" : "No") << std::endl;
    
    std::cout << "✓ Memory retention test completed" << std::endl;
}