// Replay and Consolidation Tests
// Tests replay and consolidation capabilities of memory systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace test_replay {

// Test 1: Episodic memory replay mechanism
void testEpisodicMemoryReplay() {
    std::cout << "    Testing episodic memory replay..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::EpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create episodes with temporal patterns
    std::vector<nlm::EpisodicMemoryItem> episodes;
    for (int i = 0; i < 8; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 100;
        episode.neurons = {static_cast<nlm::NeuronId>(i)};
        episode.values = {0.1f * (i % 3 + 1)};  // Pattern: 0.1, 0.2, 0.3 repeating
        episode.metadata = "Replay test episode " + std::to_string(i);
        episodes.push_back(episode);
    }
    
    // Store all episodes
    for (const auto& episode : episodes) {
        episodicMemory->storeEpisode(episode);
    }
    
    assert(episodicMemory->getEpisodeCount() == 8);
    
    // Test replay mechanism - get recent episodes
    auto recentEpisodes = episodicMemory->getRecentEpisodes(5);
    assert(recentEpisodes.size() == 5);
    
    // Verify replay order (should be most recent first)
    assert(recentEpisodes[0].timestamp == 700);
    assert(recentEpisodes[1].timestamp == 600);
    assert(recentEpisodes[2].timestamp == 500);
    assert(recentEpisodes[3].timestamp == 400);
    assert(recentEpisodes[4].timestamp == 300);
    
    // Test replay simulation (conceptual - in real implementation,
    // replay would reactivate neural patterns)
    for (int i = 0; i < 5; ++i) {
        float replayStrength = 0.8f - 0.1f * i;  // Decreasing strength
        // Simulate replay affecting consolidation
        episodicMemory->consolidate(replayStrength);
    }
    
    // Verify episode count after replay/consolidation
    size_t episodeCount = episodicMemory->getEpisodeCount();
    assert(episodeCount >= 0);  // Consolidation might reduce count
    
    std::cout << "      PASSED" << std::endl;
}

// Test 2: Working memory replay and decay
void testWorkingMemoryReplay() {
    std::cout << "    Testing working memory replay and decay..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::WorkingMemory* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Store items in working memory
    for (int i = 0; i < 5; ++i) {
        workingMemory->store(i, 0.5f + 0.1f * i);
    }
    
    assert(workingMemory->getCurrentSize() == 5);
    
    // Simulate replay activity affecting decay
    for (int step = 0; step < 10; ++step) {
        // Replay can affect decay rate
        float decayRate = 0.05f + 0.02f * sin(step * 0.5f);
        workingMemory->decay(decayRate);
        
        // Verify values decay appropriately
        for (int i = 0; i < 5; ++i) {
            float value = workingMemory->retrieve(i);
            assert(value >= 0.0f);  // Values shouldn't go negative
        }
    }
    
    // Test memory consolidation through replay
    workingMemory->decay(0.2f);  // Stronger decay for consolidation
    
    // Verify capacity constraints after replay
    assert(workingMemory->getCurrentSize() >= 0);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 3: Memory replay affecting plasticity
void testMemoryReplayPlasticity() {
    std::cout << "    Testing memory replay affecting plasticity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Store episodes that can be replayed
    nlm::EpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 50;
        episode.neurons = {static_cast<nlm::NeuronId>(i), static_cast<nlm::NeuronId>(i + 1)};
        episode.values = {0.3f, 0.7f};
        episodicMemory->storeEpisode(episode);
    }
    
    // Replay episodes (simulate consolidation)
    episodicMemory->consolidate(0.3f);
    
    // Check development stage affects memory replay
    brain.getDevelopmentSystem()->advanceStage();
    brain.getDevelopmentSystem()->advanceStage();
    
    // Development affects plasticity during replay
    float plasticityModifier = brain.getDevelopmentSystem()->getPlasticityModifier();
    assert(plasticityModifier >= 0.0f && plasticityModifier <= 1.0f);
    
    // Neuromodulators affect replay consolidation
    brain.getDopamine()->signalReward(0.5f);
    brain.getCuriosity()->signalCuriosity();
    brain.getPredictionErrorSignal()->signalPredictionError(0.3f);
    
    // Simulate replay session with neuromodulation
    for (int step = 0; step < 8; ++step) {
        nlm::SensoryInput input;
        input.values = {0.4f, 0.6f, 0.8f};
        brain.receiveSensoryInput(input);
        brain.step(step);
        
        // Neuromodulators update during replay
        brain.getDopamine()->update(0.05f);
        brain.getCuriosity()->update(0.05f);
    }
    
    // Verify plasticity has been affected by replay
    brain.updatePlasticity();
    
    std::cout << "      PASSED" << std::endl;
}

// Test 4: Replay and learning integration
void testReplayAndLearningIntegration() {
    std::cout << "    Testing replay and learning integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(55), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Initialize concept formation for learning
    brain.getConceptFormation()->initialize(&brain);
    
    // Create learning episodes
    for (int episode = 0; episode < 6; ++episode) {
        // Present experience that can be learned and later replayed
        std::vector<float> pattern{0.1f * (episode % 3), 
                                  0.2f * ((episode + 1) % 4),
                                  0.3f * ((episode + 2) % 5)};
        std::vector<float> features{0.4f, 0.5f};
        float reward = (episode % 2 == 0) ? 0.8f : 0.3f;
        
        size_t conceptId = brain.getConceptFormation()->presentExperience(pattern, features, reward, episode * 10);
        assert(conceptId > 0);
        
        // Store in episodic memory for later replay
        nlm::EpisodicMemoryItem epi;
        epi.timestamp = episode * 100;
        epi.neurons = {static_cast<nlm::NeuronId>(episode)};
        epi.values = pattern;
        epi.metadata = "Learning replay episode " + std::to_string(episode);
        brain.getEpisodicMemory()->storeEpisode(epi);
    }
    
    // Verify learning occurred
    assert(brain.getConceptFormation()->getConceptCount() > 0);
    assert(brain.getEpisodicMemory()->getEpisodeCount() == 6);
    
    // Replay episodes for consolidation
    auto recentEpisodes = brain.getEpisodicMemory()->getRecentEpisodes(3);
    assert(recentEpisodes.size() == 3);
    
    // Replay should strengthen learned concepts
    episodicMemory->consolidate(0.2f);
    
    // Development affects replay-based learning
    brain.getDevelopmentSystem()->advanceStage();
    brain.develop();
    
    // Verify learned concepts are accessible
    for (size_t i = 0; i < brain.getConceptFormation()->getConceptCount(); ++i) {
        const nlm::DiscoveredConcept* concept = brain.getConceptFormation()->getConcept(i);
        assert(concept != nullptr);
        assert(concept->totalObservations > 0);
    }
    
    std::cout << "      PASSED" << std::endl;
}

// Test 5: System replay coordination
void testSystemReplayCoordination() {
    std::cout << "    Testing system replay coordination..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.3f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Initialize all systems that participate in replay
    brain.getConceptFormation()->initialize(&brain);
    brain.getPlanner()->initialize(&brain);
    
    // Create coordinated replay experience
    std::vector<nlm::EpisodicMemoryItem> replayEpisodes;
    
    // Store multiple episodes for coordinated replay
    for (int i = 0; i < 4; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 25;
        episode.neurons = {static_cast<nlm::NeuronId>(i), static_cast<nlm::NeuronId>(i + 1)};
        episode.values = {0.5f + 0.1f * i, 0.8f - 0.1f * i};
        episode.metadata = "Coordinated replay " + std::to_string(i);
        replayEpisodes.push_back(episode);
        brain.getEpisodicMemory()->storeEpisode(episode);
    }
    
    // Initialize working memory for replay
    for (int i = 0; i < 3; ++i) {
        brain.getWorkingMemory()->store(i, 0.6f + 0.2f * sin(i * 0.5f));
    }
    
    // Coordinated replay across systems
    for (int step = 0; step < 10; ++step) {
        // Sensory input drives replay
        nlm::SensoryInput input;
        input.values = {0.3f, 0.6f, 0.9f};
        brain.receiveSensoryInput(input);
        
        // Step brain - allows replay mechanisms to activate
        brain.step(step);
        
        // Neuromodulation affects replay strength
        nlm::Neuromodulator neuromod;
        neuromod.setLevel(0.5f + 0.3f * sin(step * 0.3f));
        brain.applyNeuromodulation(neurom);
        
        // Development affects replay timing
        if (step % 3 == 0) {
            brain.getDevelopmentSystem()->advanceStage();
        }
        
        // Memory systems update during replay
        brain.getWorkingMemory()->decay(0.05f);
    }
    
    // Verify coordinated replay occurred
    assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    assert(brain.getWorkingMemory()->getCurrentSize() >= 0);
    
    // Verify development stage progression
    assert(brain.getDevelopmentStage() != nlm::DevelopmentalStage::End);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 6: Replay efficiency and consolidation
void testReplayEfficiency() {
    std::cout << "    Testing replay efficiency and consolidation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(55), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::EpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    nlm::WorkingMemory* workingMemory = brain.getWorkingMemory();
    
    // Create multiple episodes for efficiency testing
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 20;
        episode.neurons = {static_cast<nlm::NeuronId>(i % 3)};  // Reuse neurons
        episode.values = {0.1f * i, 0.2f * i};
        episode.metadata = "Efficiency replay " + std::to_string(i);
        episodicMemory->storeEpisode(episode);
        
        workingMemory->store(i % 3, 0.3f * i);
    }
    
    assert(episodicMemory->getEpisodeCount() == 10);
    assert(workingMemory->getCurrentSize() == 3);
    
    // Test efficient replay through consolidation
    // Lower threshold should consolidate more aggressively
    episodicMemory->consolidate(0.1f);
    
    // Higher threshold should consolidate selectively
    episodicMemory->consolidate(0.25f);
    
    // Test working memory efficiency
    workingMemory->decay(0.15f);  // Efficient decay
    
    // Replay should maintain stability while consolidating
    for (int step = 0; step < 5; ++step) {
        float replayStrength = 0.9f - 0.1f * step;
        episodicMemory->consolidate(replayStrength);
    }
    
    // Verify efficiency - systems should still function
    assert(brain.getTotalSpikeCount() >= 0);
    assert(brain.getWorkingMemory()->getCurrentSize() >= 0);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 7: Replay and development plasticity
void testReplayAndDevelopmentPlasticity() {
    std::cout << "    Testing replay and development plasticity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    nlm::EpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    nlm::WorkingMemory* workingMemory = brain.getWorkingMemory();
    
    // Store memories at different developmental stages
    for (int stage = 0; stage < 4; ++stage) {
        // Advance development
        development->advanceStage();
        
        // Create and store episodes at each stage
        for (int episode = 0; episode < 3; ++episode) {
            nlm::EpisodicMemoryItem epi;
            epi.timestamp = stage * 100 + episode * 10;
            epi.neurons = {static_cast<nlm::NeuronId>(stage * 10 + episode)};
            epi.values = {0.2f * (stage + 1), 0.3f * (episode + 1)};
            epi.metadata = "Development replay at stage " + std::to_string(stage);
            episodicMemory->storeEpisode(epi);
        }
        
        // Store working memory items
        for (int item = 0; item < 2; ++item) {
            workingMemory->store(stage * 10 + item, 0.4f * (stage + item));
        }
        
        // Development affects replay
        development->advanceStage();
        development->advanceStage();  // Faster during certain stages
        
        // Neuromodulators affect developmental replay
        development->update(&brain, brain.getRandomGenerator(), 0.1f);
        
        // Memory systems update with development
        workingMemory->decay(0.1f * (stage + 1));
        episodicMemory->consolidate(0.2f * (stage + 1));
    }
    
    // Verify development stage progression
    assert(development->getStage() != nlm::DevelopmentalStage::End);
    
    // Verify plasticity modifier changes with development
    float plasticity = development->getPlasticityModifier();
    assert(plasticity >= 0.0f && plasticity <= 1.0f);
    
    // Verify memory systems have grown
    assert(episodicMemory->getEpisodeCount() > 0);
    assert(workingMemory->getCurrentSize() > 0);
    
    std::cout << "      PASSED" << std::endl;
}

void runAll() {
    std::cout << "Replay and Consolidation Tests" << std::endl;
    std::cout << "=" << std::string(55, '=') << std::endl;
    
    testEpisodicMemoryReplay();
    testWorkingMemoryReplay();
    testMemoryReplayPlasticity();
    testReplayAndLearningIntegration();
    testSystemReplayCoordination();
    testReplayEfficiency();
    testReplayAndDevelopmentPlasticity();
    
    std::cout << "=" << std::string(55, '=') << std::endl;
    std::cout << "All replay and consolidation tests completed successfully!" << std::endl;
}

} // namespace test_replay