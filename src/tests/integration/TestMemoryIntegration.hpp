// Memory System Integration Tests
// Tests integration of working, episodic, and associative memory with neural activity

#pragma once

#include "IntegrationTestFramework.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <random>
#include <cmath>

namespace nlm {

class TestMemoryIntegration : public IntegrationTestBase {
public:
    explicit TestMemoryIntegration(std::shared_ptr<Config> cfg, IntegrationTestResult& results)
        : IntegrationTestBase(cfg, results) {}
    
    void runAllTests() {
        runTestCase("Working Memory Integration", "Memory Integration", [this]() {
            testWorkingMemoryIntegration();
        });
        
        runTestCase("Episodic Memory Integration", "Memory Integration", [this]() {
            testEpisodicMemoryIntegration();
        });
        
        runTestCase("Associative Memory Integration", "Memory Integration", [this]() {
            testAssociativeMemoryIntegration();
        });
        
        runTestCase("Memory-Neural Activity Coupling", "Memory Integration", [this]() {
            testMemoryNeuralActivityCoupling();
        });
        
        runTestCase("Memory Consolidation", "Memory Integration", [this]() {
            testMemoryConsolidation();
        });
        
        runTestCase("Memory Capacity Limits", "Memory Integration", [this]() {
            testMemoryCapacityLimits();
        });
        
        runTestCase("Memory Decay Dynamics", "Memory Integration", [this]() {
            testMemoryDecayDynamics();
        });
    }
    
private:
    void testWorkingMemoryIntegration() {
        setupSystem();
        
        auto* workingMemory = brain->getWorkingMemory();
        assert(workingMemory != nullptr);
        
        // Test capacity
        size_t initialCapacity = workingMemory->getCapacity();
        assert(initialCapacity > 0);
        
        // Store multiple items
        std::vector<NeuronId> neuronIds;
        std::vector<float> values;
        
        for (size_t i = 0; i < 10; ++i) {
            NeuronId neuron = static_cast<NeuronId>(i);
            float value = 0.5f + static_cast<float>(i) * 0.05f;
            workingMemory->store(neuron, value);
            neuronIds.push_back(neuron);
            values.push_back(value);
        }
        
        // Verify all items stored correctly
        assert(workingMemory->getCurrentSize() == neuronIds.size());
        
        for (size_t i = 0; i < neuronIds.size(); ++i) {
            assert(workingMemory->contains(neuronIds[i]));
            assert(std::abs(workingMemory->retrieve(neuronIds[i]) - values[i]) < 0.001f);
        }
        
        // Test overwriting
        workingMemory->store(neuronIds[0], 0.99f);
        assert(std::abs(workingMemory->retrieve(neuronIds[0]) - 0.99f) < 0.001f);
        
        // Test clearing
        workingMemory->clear();
        assert(workingMemory->getCurrentSize() == 0);
        
        teardownSystem();
    }
    
    void testEpisodicMemoryIntegration() {
        setupSystem();
        
        auto* episodicMemory = brain->getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        // Create multiple episodes
        std::vector<EpisodicMemoryItem> episodes;
        const size_t episodeCount = 5;
        
        for (size_t i = 0; i < episodeCount; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = static_cast<nlm::SimulationStep>(i * 1000);
            episode.neurons = {static_cast<NeuronId>(i), static_cast<NeuronId>(i + 10)};
            episode.values = {0.1f * static_cast<float>(i), 0.9f * static_cast<float>(i)};
            episode.metadata = "Episode " + std::to_string(i);
            
            episodicMemory->storeEpisode(episode);
            episodes.push_back(episode);
        }
        
        // Verify episode count
        assert(episodicMemory->getEpisodeCount() == episodeCount);
        
        // Test retrieval
        for (size_t i = 0; i < episodeCount; ++i) {
            auto retrieved = episodicMemory->retrieveEpisode(i);
            assert(retrieved.timestamp == episodes[i].timestamp);
            assert(retrieved.neurons == episodes[i].neurons);
            assert(retrieved.values == episodes[i].values);
            assert(retrieved.metadata == episodes[i].metadata);
        }
        
        // Test recent episodes retrieval
        auto recent = episodicMemory->getRecentEpisodes(3);
        assert(recent.size() <= 3);
        
        // Test consolidation
        episodicMemory->consolidate(0.5f);
        
        teardownSystem();
    }
    
    void testAssociativeMemoryIntegration() {
        setupSystem();
        
        auto* associativeMemory = brain->getAssociativeMemory();
        assert(associativeMemory != nullptr);
        
        // Create associations
        std::vector<std::pair<NeuronId, NeuronId>> pairs = {
            {1, 2}, {2, 3}, {3, 4}, {4, 5}
        };
        
        for (const auto& pair : pairs) {
            float strength = 0.5f + static_cast<float>(pair.first) * 0.1f;
            associativeMemory->associate(pair.first, pair.second, strength);
        }
        
        // Verify associations
        for (const auto& pair : pairs) {
            assert(associativeMemory->getAssociationStrength(pair.first, pair.second) > 0.4f);
            assert(associativeMemory->containsAssociation(pair.first, pair.second));
        }
        
        // Test reverse lookup
        std::vector<NeuronId> associations = associativeMemory->getAssociations(2);
        assert(std::find(associations.begin(), associations.end(), 1) != associations.end());
        assert(std::find(associations.begin(), associations.end(), 3) != associations.end());
        
        // Test updating associations
        associativeMemory->updateAssociation(1, 2, 0.2f);
        assert(std::abs(associativeMemory->getAssociationStrength(1, 2) - 0.7f) < 0.001f);
        
        teardownSystem();
    }
    
    void testMemoryNeuralActivityCoupling() {
        setupSystem();
        
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* associativeMemory = brain->getAssociativeMemory();
        
        // Simulate neural activity coupling with memory
        std::vector<NeuronId> activeNeurons = {1, 3, 5, 7, 9};
        std::vector<float> activityLevels = {0.8f, 0.6f, 0.4f, 0.3f, 0.9f};
        
        for (size_t i = 0; i < activeNeurons.size(); ++i) {
            NeuronId neuron = activeNeurons[i];
            float level = activityLevels[i];
            
            // Store in working memory
            workingMemory->store(neuron, level);
            
            // Create episodic record
            EpisodicMemoryItem episode;
            episode.timestamp = static_cast<nlm::SimulationStep>(i * 100);
            episode.neurons = {neuron};
            episode.values = {level};
            episode.metadata = "Activity coupling test";
            
            episodicMemory->storeEpisode(episode);
            
            // Create associations between active neurons
            for (size_t j = 0; j < activeNeurons.size(); ++j) {
                if (i != j) {
                    associativeMemory->associate(neuron, activeNeurons[j], level * activityLevels[j]);
                }
            }
        }
        
        // Verify coupling
        for (size_t i = 0; i < activeNeurons.size(); ++i) {
            assert(workingMemory->contains(activeNeurons[i]));
            assert(std::abs(workingMemory->retrieve(activeNeurons[i]) - activityLevels[i]) < 0.001f);
            
            auto retrievedEpisode = episodicMemory->retrieveEpisode(i);
            assert(retrievedEpisode.values[0] == activityLevels[i]);
        }
        
        // Verify associative strengths
        for (size_t i = 0; i < activeNeurons.size(); ++i) {
            for (size_t j = 0; j < activeNeurons.size(); ++j) {
                if (i != j) {
                    float expectedStrength = activityLevels[i] * activityLevels[j];
                    float actualStrength = associativeMemory->getAssociationStrength(
                        activeNeurons[i], activeNeurons[j]);
                    assert(std::abs(actualStrength - expectedStrength) < 0.01f);
                }
            }
        }
        
        teardownSystem();
    }
    
    void testMemoryConsolidation() {
        setupSystem();
        
        auto* episodicMemory = brain->getEpisodicMemory();
        
        // Create episodes with varying relevance
        std::vector<EpisodicMemoryItem> episodes;
        for (size_t i = 0; i < 10; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = static_cast<nlm::SimulationStep>(i * 100);
            episode.neurons = {static_cast<NeuronId>(i)};
            episode.values = {0.5f + static_cast<float>(i) * 0.05f};
            
            // Vary relevance
            if (i % 3 == 0) {
                episode.metadata = "High relevance";
            } else if (i % 3 == 1) {
                episode.metadata = "Medium relevance";
            } else {
                episode.metadata = "Low relevance";
            }
            
            episodicMemory->storeEpisode(episode);
            episodes.push_back(episode);
        }
        
        // Get episode count before consolidation
        size_t beforeConsolidation = episodicMemory->getEpisodeCount();
        
        // Consolidate with threshold
        episodicMemory->consolidate(0.6f);
        
        // Some episodes should be consolidated
        size_t afterConsolidation = episodicMemory->getEpisodeCount();
        
        // Test consolidation threshold effect
        episodicMemory->consolidate(0.1f); // Lower threshold
        
        teardownSystem();
    }
    
    void testMemoryCapacityLimits() {
        setupSystem();
        
        auto* workingMemory = brain->getWorkingMemory();
        
        // Test working memory capacity
        size_t capacity = workingMemory->getCapacity();
        
        // Fill beyond capacity
        for (size_t i = 0; i < capacity + 5; ++i) {
            workingMemory->store(static_cast<NeuronId>(i), 0.5f);
        }
        
        // System should handle overflow gracefully
        assert(workingMemory->getCurrentSize() <= capacity);
        
        // Clear and test again
        workingMemory->clear();
        assert(workingMemory->getCurrentSize() == 0);
        
        // Test episodic memory limits
        auto* episodicMemory = brain->getEpisodicMemory();
        
        // Store many episodes
        for (size_t i = 0; i < 100; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = static_cast<nlm::SimulationStep>(i);
            episode.neurons = {static_cast<NeuronId>(i % 10)};
            episode.values = {0.1f};
            
            episodicMemory->storeEpisode(episode);
        }
        
        // System should handle large number of episodes
        assert(episodicMemory->getEpisodeCount() > 0);
        
        teardownSystem();
    }
    
    void testMemoryDecayDynamics() {
        setupSystem();
        
        auto* workingMemory = brain->getWorkingMemory();
        
        // Store items
        for (size_t i = 0; i < 10; ++i) {
            workingMemory->store(static_cast<NeuronId>(i), 0.9f);
        }
        
        // Apply decay
        float decayRate = 0.1f;
        workingMemory->decay(decayRate);
        
        // Verify decay effect
        for (size_t i = 0; i < 10; ++i) {
            float value = workingMemory->retrieve(static_cast<NeuronId>(i));
            float expected = 0.9f * std::pow(1.0f - decayRate, 1.0f); // Simplified decay model
            assert(std::abs(value - expected) < 0.01f);
        }
        
        teardownSystem();
    }
};

} // namespace nlm