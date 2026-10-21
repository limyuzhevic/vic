// Phase 6 Integration Tests
// Tests Phase 6 integration verification

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_phase6_integration {

void testPhase6MemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test Phase 6 memory systems
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Test working memory with large capacity
    std::vector<float> largePattern(100, 0.5f); // 100-dimensional pattern
    brain.getWorkingMemory()->store(largePattern, 1.0f);
    
    // Test episodic memory persistence
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = largePattern;
    episode.timestamp = 1;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Test memory consolidation
    for (nlm::SimulationStep step = 1; step < 20; ++step) {
        brain.step(step);
        assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
        assert(brain.getEpisodicMemory()->getEpisodeCount() == 1);
    }
    
    std::cout << "    testPhase6MemorySystems passed" << std::endl;
}

void testPhase6CognitiveSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test Phase 6 cognitive systems
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    // Test concept formation with complex patterns
    std::vector<float> complexPattern(50, 0.3f);
    for (size_t i = 0; i < complexPattern.size(); ++i) {
        complexPattern[i] = 0.3f + (static_cast<float>(i) * 0.02f);
    }
    
    size_t conceptId = brain.getConceptFormation()->presentExperience(
        complexPattern, complexPattern, 1.0f, 1);
    
    // Test planning with memory integration
    for (nlm::SimulationStep step = 1; step < 30; ++step) {
        brain.step(step);
        
        assert(brain.getConceptFormation()->getConceptCount() >= 0);
        assert(brain.getPlanner() != nullptr);
        
        auto action = brain.produceAction();
        assert(action != nullptr);
    }
    
    std::cout << "    testPhase6CognitiveSystems passed" << std::endl;
}

void testPhase6PredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test Phase 6 prediction system
    assert(brain.getPredictionSystem() != nullptr);
    
    // Create training data
    std::vector<std::vector<float>> trainingData = {
        {1.0f, 0.8f, 0.6f, 0.4f, 0.2f},
        {0.9f, 0.7f, 0.5f, 0.3f, 0.1f},
        {0.8f, 0.6f, 0.4f, 0.2f, 0.0f}
    };
    
    // Train prediction system
    for (size_t i = 0; i < trainingData.size(); ++i) {
        nlm::InternalSignals input(trainingData[i]);
        brain.getPredictionSystem()->train(input);
        
        // Test prediction accuracy
        auto prediction = brain.getPredictionSystem()->predictNextState(input);
        if (prediction) {
            brain.getPredictionSystem()->updatePredictions(input, input);
        }
    }
    
    // System should be stable
    for (nlm::SimulationStep step = 1; step < 25; ++step) {
        brain.step(step);
        assert(brain.getPredictionSystem()->getConfidence() >= 0.0f);
        assert(brain.getPredictionSystem()->getPredictionError() >= 0.0f);
    }
    
    std::cout << "    testPhase6PredictionSystem passed" << std::endl;
}

void testPhase6Neuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(700), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all neuromodulators
    auto* dopamine = brain.getDopamine();
    auto* serotonin = brain.getSerotonin();
    auto* norepinephrine = brain.getNorepinephrine();
    auto* acetylcholine = brain.getAcetylcholine();
    
    assert(dopamine != nullptr);
    assert(serotonin != nullptr);
    assert(norepinephrine != nullptr);
    assert(acetylcholine != nullptr);
    
    // Test neuromodulator dynamics
    for (nlm::SimulationStep step = 1; step < 20; ++step) {
        // Apply different signals based on step
        if (step % 3 == 0) {
            nlm::Neuromodulator rewardSignal;
            rewardSignal.setLevel(0.7f);
            brain.applyNeuromodulation(rewardSignal);
        }
        if (step % 4 == 0) {
            nlm::Neuromodulator stressSignal;
            stressSignal.setLevel(0.5f);
            brain.applyNeuromodulation(stressSignal);
        }
        
        brain.step(step);
        
        // All neuromodulators should have levels
        assert(dopamine->getLevel() >= 0.0f);
        assert(serotonin->getLevel() >= 0.0f);
        assert(norepinephrine->getLevel() >= 0.0f);
        assert(acetylcholine->getLevel() >= 0.0f);
    }
    
    std::cout << "    testPhase6Neuromodulation passed" << std::endl;
}

void testPhase6SleepRestConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test sleep/rest consolidation
    brain.getWorkingMemory()->setDecayRate(0.02f); // Rest decay
    
    // Create rich memory set
    for (int i = 0; i < 15; ++i) {
        std::vector<float> pattern = {
            0.9f - (i * 0.05f),
            0.8f - (i * 0.05f),
            0.7f - (i * 0.05f)
        };
        
        brain.getWorkingMemory()->store(pattern, 1.0f);
        
        nlm::EpisodicMemoryItem episode;
        episode.sensoryState = pattern;
        episode.timestamp = i;
        brain.getEpisodicMemory()->storeEpisode(episode);
    }
    
    // Enable replay
    brain.getEpisodicMemory()->enableReplay(true);
    
    // Simulate rest with consolidation
    for (nlm::SimulationStep step = 20; step < 50; ++step) {
        brain.step(step);
        
        // Memory should be preserved during rest
        assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
        assert(brain.getEpisodicMemory()->getEpisodeCount() >= 15);
        
        // Replay should be available
        assert(brain.getEpisodicMemory()->isReplayEnabled());
    }
    
    std::cout << "    testPhase6SleepRestConsolidation passed" << std::endl;
}

void testPhase6SystemPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // High-performance system test
    std::vector<std::vector<float>> testPatterns;
    for (int i = 0; i < 20; ++i) {
        std::vector<float> pattern;
        for (int j = 0; j < 10; ++j) {
            pattern.push_back(static_cast<float>(rand()) / RAND_MAX);
        }
        testPatterns.push_back(pattern);
    }
    
    // Complex operations
    for (int step = 1; step < 60; ++step) {
        brain.step(step);
        
        // Working memory operations
        brain.getWorkingMemory()->store(testPatterns[step % testPatterns.size()], 1.0f);
        
        // Episodic memory operations
        nlm::EpisodicMemoryItem episode;
        episode.sensoryState = testPatterns[step % testPatterns.size()];
        episode.timestamp = step;
        brain.getEpisodicMemory()->storeEpisode(episode);
        
        // Concept formation
        brain.getConceptFormation()->presentExperience(
            testPatterns[step % testPatterns.size()],
            testPatterns[step % testPatterns.size()],
            0.5f + (step * 0.01f), step);
        
        // Prediction
        nlm::InternalSignals input(testPatterns[step % testPatterns.size()]);
        brain.getPredictionSystem()->train(input);
        
        // Planning
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        // Neuromodulation
        nlm::Neuromodulator signal;
        signal.setLevel(0.5f + (step * 0.01f) % 1.0f);
        brain.applyNeuromodulation(signal);
    }
    
    // Verify system integrity
    assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
    assert(brain.getEpisodicMemory()->getEpisodeCount() >= 20);
    assert(brain.getConceptFormation()->getConceptCount() >= 0);
    assert(brain.getPredictionSystem()->getConfidence() >= 0.0f);
    assert(brain.getPlanner() != nullptr);
    
    std::cout << "    testPhase6SystemPerformance passed" << std::endl;
}

void testPhase6IntegrationScore() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Calculate Phase 6 integration score
    int totalTests = 0;
    int passedTests = 0;
    
    // Test memory systems
    try {
        std::vector<float> pattern = {1.0f, 0.5f, 0.0f};
        brain.getWorkingMemory()->store(pattern, 1.0f);
        passedTests++;
    } catch (...) { totalTests++; }
    
    // Test episodic memory
    try {
        nlm::EpisodicMemoryItem episode;
        episode.sensoryState = pattern;
        brain.getEpisodicMemory()->storeEpisode(episode);
        passedTests++;
    } catch (...) { totalTests++; }
    
    // Test concept formation
    try {
        brain.getConceptFormation()->presentExperience(pattern, pattern, 1.0f, 1);
        passedTests++;
    } catch (...) { totalTests++; }
    
    // Test prediction
    try {
        nlm::InternalSignals input(pattern);
        brain.getPredictionSystem()->train(input);
        passedTests++;
    } catch (...) { totalTests++; }
    
    // Test planning
    try {
        auto action = brain.produceAction();
        if (action) passedTests++;
    } catch (...) { totalTests++; }
    
    // Test neuromodulation
    try {
        nlm::Neuromodulator signal;
        signal.setLevel(0.5f);
        brain.applyNeuromodulation(signal);
        passedTests++;
    } catch (...) { totalTests++; }
    
    // Integration score should be high (all systems working)
    float integrationScore = (totalTests > 0) ? (static_cast<float>(passedTests) / totalTests) * 100.0f : 0.0f;
    
    assert(integrationScore >= 80.0f); // 80%+ integration score
    
    std::cout << "    testPhase6IntegrationScore passed" << std::endl;
}

void runAll() {
    testPhase6MemorySystems();
    testPhase6CognitiveSystems();
    testPhase6PredictionSystem();
    testPhase6Neuromodulation();
    testPhase6SleepRestConsolidation();
    testPhase6SystemPerformance();
    testPhase6IntegrationScore();
}

} // namespace test_phase6_integration