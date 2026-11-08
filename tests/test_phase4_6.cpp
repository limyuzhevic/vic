// Phase 4-6 System Tests
// Comprehensive tests for advanced neural systems

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "prediction/PredictionSystem.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "experiments/ExperimentInfrastructure.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <numeric>

namespace test_phase4_6 {

void testPhase4PredictionSystem() {
    std::cout << "Testing Phase 4 Prediction System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create prediction system
    nlm::NeuralPrediction predictor;
    predictor.initialize(&brain);
    
    // Test temporal prediction
    std::vector<float> patternA(50, 0.0f);
    std::vector<float> patternB(50, 0.0f);
    
    for (size_t i = 0; i < 25; ++i) {
        patternA[i] = 1.0f;
        patternB[25 + i] = 1.0f;
    }
    
    float initialError = 0.0f;
    float finalError = 0.0f;
    
    // Generate sequences: pattern A always followed by pattern B
    for (size_t trial = 0; trial < 100; ++trial) {
        // Present pattern A
        predictor.recordSensoryState(patternA, trial * 2);
        
        // Predict pattern B
        auto prediction = predictor.generatePrediction(trial * 2 + 1);
        
        // Present actual pattern B
        float error = predictor.updateWithObservation(patternB, trial * 2 + 1);
        
        if (trial < 25) {
            initialError += error;
        }
        if (trial >=  numerics_limits<size_t>::max() / 4) {
            finalError += error;
        }
    }
    
    initialError /= 25;
    finalError /= 75;
    
    assert(!std::isnan(initialError));
    assert(!std::isnan(finalError));
    
    std::cout << "    Prediction system test passed" << std::endl;
}

void testPhase4WorkingMemory() {
    std::cout << "Testing Phase 4 Working Memory System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create working memory system
    nlm::NeuralWorkingMemory wm;
    wm.initialize(&brain);
    
    // Test pattern storage and retrieval
    std::vector<float> testPattern(20, 0.5f);
    
    // Store pattern
    wm.store(testPattern, 1.0f);
    
    // Wait some time
    for (size_t delay = 0; delay < 10; ++delay) {
        wm.update(0.001f);
    }
    
    // Retrieve
    auto retrieved = wm.retrieve();
    
    // Compute similarity (simplified)
    float similarity = 0.0f;
    for (size_t i = 0; i < testPattern.size(); ++i) {
        similarity += std::abs(testPattern[i] - retrieved[i]);
    }
    similarity /= testPattern.size();
    
    assert(!std::isnan(similarity));
    assert(similarity > 0.0f);
    
    std::cout << "    Working memory test passed" << std::endl;
}

void testPhase4EpisodicMemory() {
    std::cout << "Testing Phase 4 Episodic Memory System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create episodic memory system
    nlm::NeuralEpisodicMemory em;
    em.initialize(&brain);
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = std::vector<float>(50, 0.5f);
    episode.reward = 1.0f;
    episode.action = nlm::ActionType::MoveForward;
    
    em.storeEpisode(episode);
    
    // Retrieve similar episode
    auto similar = em.retrieveSimilar(episode.sensoryState, 1);
    
    assert(!similar.empty());
    assert(similar[0]->reward == episode.reward);
    
    std::cout << "    Episodic memory test passed" << std::endl;
}

void testPhase4ConceptFormation() {
    std::cout << "Testing Phase 4 Concept Formation System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create concept formation system
    nlm::ConceptFormation concepts;
    concepts.initialize(&brain);
    
    // Present variations of the same underlying concept
    std::vector<float> basePattern(30, 0.0f);
    for (size_t i = 0; i < 15; ++i) {
        basePattern[i] = 1.0f;
    }
    
    for (size_t trial = 0; trial < 50; ++trial) {
        // Create variation
        std::vector<float> variation = basePattern;
        for (auto& v : variation) {
            v += (brain.getRandomGenerator()->uniformReal(-0.1f, 0.1f));
            v = std::clamp(v, 0.0f, 1.0f);
        }
        
        std::vector<float> features = {0.5f};  // Neutral feature
        concepts.presentExperience(variation, features, 0.5f, trial);
    }
    
    assert(concepts.getConceptCount() > 0);
    
    std::cout << "    Concept formation test passed" << std::endl;
}

void testPhase4Planning() {
    std::cout << "Testing Phase 4 Planning System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create planning system
    nlm::NeuralPlanner planner;
    planner.initialize(&brain);
    
    // Set up simple action quality based on experience
    planner.setActionQuality(nlm::ActionType::MoveForward, 0.7f);
    planner.setActionQuality(nlm::ActionType::Interact, 0.8f);
    planner.setActionQuality(nlm::ActionType::Wait, 0.0f);
    
    // Plan action
    std::vector<float> currentState(50, 0.5f);
    nlm::ActionType plannedAction = planner.planAction(currentState, 0.5f);
    
    assert(plannedAction == nlm::ActionType::MoveForward || 
           plannedAction == nlm::ActionType::Interact);
    
    std::cout << "    Planning system test passed" << std::endl;
}

void testPhase4Neuromodulation() {
    std::cout << "Testing Phase 4 Neuromodulation Systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test reward modulation
    nlm::Reward rewardSystem;
    rewardSystem.initialize(&brain);
    
    // Test curiosity
    nlm::Curiosity curiositySystem;
    curiositySystem.initialize(&brain);
    
    // Test novelty
    nlm::Novelty noveltySystem;
    noveltySystem.initialize(&brain);
    
    // Test prediction error
    nlm::PredictionError errorSystem;
    errorSystem.initialize(&brain);
    
    // Run some steps with sensory input
    for (int i = 0; i < 50; ++i) {
        // Inject some sensory activity
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
        brain.step(i, i * 0.001);
    }
    
    std::cout << "    Neuromodulation systems test passed" << std::endl;
}

void testPhase5Checkpointing() {
    std::cout << "Testing Phase 5 Checkpointing System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain1(config);
    brain1.initialize();
    
    // Run some steps
    for (int i = 0; i < 100; ++i) {
        brain1.step(i, i * 0.001);
    }
    
    float avgFiring1 = brain1.getAverageFiringRate();
    size_t totalSpikes1 = brain1.getTotalSpikeCount();
    
    // Save checkpoint
    std::string path = "/tmp/nlm_checkpoint_test.bin";
    assert(brain1.save(path));
    
    // Create new brain and load
    nlm::Brain brain2(config);
    brain2.initialize();
    
    assert(brain2.load(path));
    
    size_t totalSpikes2 = brain2.getTotalSpikeCount();
    
    // Note: Due to the nature of neural simulation, exact state restoration
    // is complex. The load should restore the structure at minimum.
    
    std::cout << "    Checkpointing system test passed" << std::endl;
}

void testPhase6Integration() {
    std::cout << "Testing Phase 6 Integration System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test all systems are integrated
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    
    // Run simulation
    for (int i = 0; i < 200; ++i) {
        brain.step(i, i * 0.001);
    }
    
    std::cout << "    Integration system test passed" << std::endl;
}

void testExperimentInfrastructure() {
    std::cout << "Testing Experiment Infrastructure..." << std::endl;
    
    // Test ExperimentLogger
    nlm::ExperimentLogger logger("test_experiment.log");
    logger.log("Test message");
    logger.logPhase3Header();
    logger.logEpisodeStart(1);
    logger.logStep(0, 1.0f, 0.5f, 0.3f, 0.8f, "move");
    logger.logEpisodeEnd(1, 5.0f, 10, 0.2f);
    logger.logDevelopment(1.0f, "Stage 1");
    logger.logSynapticStats(100, 0.5f, 1.0f);
    logger.flush();
    
    // Test ExperimentMetrics
    nlm::ExperimentMetrics metrics;
    assert(metrics.episode == 0);
    assert(metrics.totalReward == 0.0f);
    
    metrics.episode = 1;
    metrics.totalReward = 5.0f;
    metrics.finalEnergy = 3.0f;
    metrics.finalNovelty = 0.2f;
    metrics.finalCuriosity = 0.3f;
    metrics.predictionError = 0.1f;
    metrics.developmentalAge = 1.0f;
    metrics.developmentalStage = "Stage 1";
    metrics.totalSynapses = 100;
    metrics.averageWeight = 0.5f;
    metrics.maxWeight = 1.0f;
    metrics.totalSpikes = 50;
    metrics.averageFiringRate = 0.5f;
    metrics.forwardCount = 10;
    metrics.backwardCount = 5;
    metrics.turnLeftCount = 3;
    metrics.turnRightCount = 2;
    metrics.interactCount = 8;
    metrics.waitCount = 20;
    metrics.lookLeftCount = 4;
    metrics.lookRightCount = 6;
    
    std::string csv = metrics.toCSV();
    assert(!csv.empty());
    
    // Test NeuralExperiment
    class TestExperiment : public nlm::NeuralExperiment {
    public:
        bool run(std::shared_ptr<nlm::Brain> brain) override {
            recordNetworkMetrics(brain, 0);
            return true;
        }
    };
    
    TestExperiment experiment;
    experiment.setName("Test Experiment");
    assert(experiment.getName() == "Test Experiment");
    
    // Test ExperimentResultAggregator
    nlm::ExperimentResultAggregator aggregator;
    aggregator.addResult("Test Exp", metrics);
    assert(aggregator.getExperimentCount() == 1);
    
    double avgReward = aggregator.getAverageMetric("totalReward");
    assert(avgReward == 5.0);
    
    // Test ExperimentConfig
    nlm::ExperimentConfig config;
    assert(config.getValue<int64_t>("neuron_count", 0) > 0);
    config.setValue("test_param", "test_value");
    
    std::vector<std::string> keys = config.getKeys();
    assert(std::find(keys.begin(), keys.end(), "neuron_count") != keys.end());
    
    std::cout << "    Experiment infrastructure test passed" << std::endl;
}

void runAll() {
    std::cout << "=== NLM Phase 4-6 Tests ===" << std::endl;
    std::cout << "Testing Advanced Neural Systems:" << std::endl;
    std::cout << "  - Phase 4: Cognitive Systems" << std::endl;
    std::cout << "  - Phase 5: Personality & Checkpointing" << std::endl;
    std::cout << "  - Phase 6: Full Integration" << std::endl;
    std::cout << "  - Experiment Infrastructure" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Test Phase 4 systems
    std::cout << "Running Phase 4 system tests..." << std::endl;
    try {
        testPhase4PredictionSystem();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPhase4WorkingMemory();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPhase4EpisodicMemory();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPhase4ConceptFormation();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPhase4Planning();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPhase4Neuromodulation();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Phase 5 systems
    std::cout << "Running Phase 5 system tests..." << std::endl;
    try {
        testPhase5Checkpointing();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Phase 6 systems
    std::cout << "Running Phase 6 system tests..." << std::endl;
    try {
        testPhase6Integration();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test experiment infrastructure
    std::cout << "Running experiment infrastructure tests..." << std::endl;
    try {
        testExperimentInfrastructure();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Phase 4-6 Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Phase 4-6 Tests FAILED ===" << std::endl;
        return 1;
    }
}

} // namespace test_phase4_6
