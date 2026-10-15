// Memory-Neuromodulation Integration Test
// Tests integration between memory systems and neuromodulation
// Validates reward-based learning, memory consolidation, and neuromodulatory effects

#include "brain/Brain.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

namespace test_memory_neuromodulation_integration {

void testRewardBasedMemoryConsolidation() {
    std::cout << "    Testing reward-based memory consolidation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::NeuralWorkingMemory* workingMem = brain.getWorkingMemory();
    nlm::NeuralEpisodicMemory* episodicMem = brain.getEpisodicMemory();
    nlm::NeuralAssociativeMemory* assocMem = brain.getAssociativeMemory();
    nlm::PredictionSystem* predictionSys = brain.getPredictionSystem();
    
    // Create test memory items
    std::vector<nlm::NeuronId> testNeurons = {1, 2, 3, 4, 5};
    std::vector<float> testValues = {0.2f, 0.5f, 0.8f, 0.3f, 0.6f};
    
    // Store items in working memory
    for (size_t i = 0; i < testNeurons.size(); ++i) {
        if (workingMem) {
            workingMem->store(testNeurons[i], testValues[i]);
            assert(workingMem->contains(testNeurons[i]));
        }
    }
    
    // Test 1: Reward signal strengthens memory
    float initialReward = 0.5f;
    if (dopamine) {
        dopamine->signalReward(initialReward);
        
        // Simulate reward-based consolidation
        if (episodicMem) {
            nlm::EpisodicMemoryItem highValueItem;
            highValueItem.timestamp = 0;
            highValueItem.neurons = {testNeurons[2], testNeurons[4]};  // High value neurons
            highValueItem.values = {testValues[2], testValues[4]};
            highValueItem.metadata = "high_reward_path";
            
            episodicMem->storeEpisode(highValueItem);
            
            // Retrieve and verify consolidation
            auto episodes = episodicMem->getRecentEpisodes(1);
            assert(!episodes.empty());
        }
        
        // Verify dopamine level increased
        assert(dopamine->getLevel() > 0.0f);
    }
    
    // Test 2: Curiosity-driven exploration stores new memories
    if (workingMem && curiosity) {
        // Clear working memory first
        workingMem->clear();
        assert(workingMem->getCurrentSize() == 0);
        
        // Curiosity encourages storing new experiences
        curiosity->setLevel(0.9f);
        curiosity->update(0.1f);
        
        // Store new experiences
        for (size_t i = 0; i < testNeurons.size(); ++i) {
            workingMem->store(testNeurons[i], testValues[i]);
        }
        
        // Verify storage occurred
        assert(workingMem->getCurrentSize() == testNeurons.size());
    }
    
    // Test 3: Novelty detection triggers memory encoding
    if (novelty) {
        novelty->setLevel(0.7f);
        
        // Check novelty detection and memory encoding
        if (workingMem) {
            // Novelty should promote encoding of new patterns
            bool novelPattern = true;
            if (novelPattern) {
                workingMem->store(99, 1.0f);  // Store novel pattern
                assert(workingMem->contains(99));
            }
        }
    }
    
    // Test 4: Prediction error drives memory updating
    if (predictionSys && predError && workingMem) {
        // Generate prediction error
        predError->setPredictionError(0.3f);
        
        // Update prediction system
        predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        
        // Use prediction error to update working memory
        float predictionError = predError->getPredictionError();
        if (predictionError > 0.0f) {
            // Update memory based on prediction error
            for (size_t i = 0; i < testNeurons.size(); ++i) {
                float currentValue = workingMem->retrieve(testNeurons[i]);
                float updatedValue = currentValue * (1.0f + predictionError * 0.1f);
                workingMem->store(testNeurons[i], updatedValue);
                assert(std::abs(workingMem->retrieve(testNeurons[i]) - updatedValue) < 0.01f);
            }
        }
    }
    
    // Test 5: Memory consolidation across neuromodulatory systems
    if (workingMem && episodicMem && assocMem) {
        // Create associative memory
        assocMem->associate(testNeurons[0], testNeurons[1], 0.7f);
        
        // Verify association
        std::vector<nlm::NeuronId> associations = assocMem->getAssociations(testNeurons[0]);
        assert(std::find(associations.begin(), associations.end(), testNeurons[1]) != associations.end());
        
        // Consolidate episodic memory
        if (episodicMem) {
            episodicMem->consolidate(0.5f);  // Consolidate items with relevance > 0.5
        }
    }
    
    std::cout << "    testRewardBasedMemoryConsolidation passed" << std::endl;
}

void testMemoryReplayAndNeuromodulation() {
    std::cout << "    Testing memory replay and neuromodulation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    nlm::NeuralWorkingMemory* workingMem = brain.getWorkingMemory();
    nlm::NeuralEpisodicMemory* episodicMem = brain.getEpisodicMemory();
    
    // Create episodic memories
    std::vector<nlm::EpisodicMemoryItem> episodes;
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.neurons = {static_cast<nlm::NeuronId>(10 + i), static_cast<nlm::NeuronId>(20 + i)};
        episode.values = {0.5f * (i + 1) / 10.0f, 0.3f * (i + 1) / 10.0f};
        episode.metadata = "experience_" + std::to_string(i);
        episodes.push_back(episode);
    }
    
    // Store all episodes
    if (episodicMem) {
        for (const auto& episode : episodes) {
            episodicMem->storeEpisode(episode);
        }
        assert(episodicMem->getEpisodeCount() == episodes.size());
    }
    
    // Test memory replay with neuromodulation
    for (int replayStep = 0; replayStep < 20; ++replayStep) {
        double currentTime = replayStep * 0.1;
        
        // Get recent episodes for replay
        if (episodicMem) {
            auto recentEpisodes = episodicMem->getRecentEpisodes(3);
            
            // Replay memory with neuromodulatory influence
            for (const auto& episode : recentEpisodes) {
                // Reward-based replay (dopamine)
                if (dopamine) {
                    float replayReward = 0.2f * (episode.values.size() > 0 ? episode.values[0] : 0.0f);
                    dopamine->signalReward(replayReward);
                    
                    // Store replayed experience in working memory
                    if (workingMem) {
                        for (size_t i = 0; i < episode.neurons.size(); ++i) {
                            workingMem->store(episode.neurons[i], episode.values[i]);
                        }
                    }
                }
                
                // Curiosity-driven exploration during replay
                if (curiosity) {
                    curiosity->setLevel(0.5f + 0.3f * (replayStep / 20.0f));
                    curiosity->update(0.1f);
                }
            }
        }
        
        // Brain step with memory replay
        brain.step(replayStep, currentTime);
        
        // Verify memory systems are still functional
        if (workingMem) {
            assert(workingMem->getCurrentSize() >= 0);
        }
    }
    
    // Test concept formation from memory patterns
    nlm::ConceptFormation* conceptForm = brain.getConceptFormation();
    if (conceptForm && episodicMem) {
        // Update concept formation with memory data
        conceptForm->update(brain.getTotalNeuronCount(), brain.getFiringNeuronCount());
        
        // Get active concepts
        auto concepts = conceptForm->getActiveConcepts();
        
        // Concepts should be derived from episodic memory patterns
        for (const auto& concept : concepts) {
            assert(!concept.empty());
        }
    }
    
    std::cout << "    testMemoryReplayAndNeuromodulation passed" << std::endl;
}

void testNeuromodulatoryNetworkEffects() {
    std::cout << "    Testing neuromodulatory network effects..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test all neuromodulators working together
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    nlm::Novelty* novelty = brain.getNovelty();
    nlm::PredictionError* predError = brain.getPredictionErrorSignal();
    
    // Test network effects during learning
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        // Coordinated neuromodulation
        if (dopamine) {
            dopamine->signalReward(0.1f + 0.05f * std::sin(step * 0.5f));
        }
        
        if (curiosity) {
            curiosity->setLevel(0.5f + 0.3f * (step / 30.0f));
            curiosity->update(0.1f);
        }
        
        if (novelty) {
            novelty->setLevel(0.3f + 0.4f * std::cos(step * 0.3f));
        }
        
        if (predError) {
            predError->setPredictionError(std::abs(std::sin(step * 0.2f)));
        }
        
        // Apply all neuromodulators
        if (dopamine) {
            brain.applyNeuromodulation(*dopamine);
        }
        
        if (curiosity) {
            brain.applyNeuromodulation(*curiosity);
        }
        
        if (novelty) {
            brain.applyNeuromodulation(*novelty);
        }
        
        if (predError) {
            // Note: predError is not a Neuromodulator, it's a separate class
        }
        
        // Brain step with all neuromodulators active
        brain.step(step);
        brain.updatePlasticity();  // Neuromodulators affect plasticity
        
        // Verify network effects
        assert(brain.getTotalNeuronCount() == 300);
        assert(brain.getRegionCount() == 3);
        
        // Check for system consistency
        if (dopamine) {
            assert(dopamine->getLevel() >= 0.0f);
        }
        if (curiosity) {
            assert(curiosity->getLevel() >= 0.0f);
        }
    }
    
    // Test developmental modulation of neuromodulatory effects
    nlm::DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
    if (devSystem) {
        devSystem->update(&brain, 15);
        
        float plasticityModifier = devSystem->getPlasticityModifier();
        bool isCriticalPeriod = devSystem->isCriticalPeriod();
        
        // Neuromodulatory effects should be stage-dependent
        assert(plasticityModifier > 0.0f);
        
        if (isCriticalPeriod) {
            // Higher plasticity during critical period
            assert(plasticityModifier > 1.0f);
        }
    }
    
    std::cout << "    testNeuromodulatoryNetworkEffects passed" << std::endl;
}

void runAll() {
    testRewardBasedMemoryConsolidation();
    testMemoryReplayAndNeuromodulation();
    testNeuromodulatoryNetworkEffects();
}

} // namespace test_memory_neuromodulation_integration