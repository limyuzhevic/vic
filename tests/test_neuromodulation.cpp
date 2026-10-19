// Neuromodulation System Tests
// Comprehensive tests for all neuromodulation systems (dopamine, curiosity, novelty, prediction error)

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "environment/Observation.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_neuromodulation {

// Neuromodulator Base Class Tests
void testNeuromodulatorBase() {
    // Test base class interface
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test base interface
    assert(dopamine->getName() != nullptr);
    assert(std::string(dopamine->getName()) == "Dopamine");
    
    // Test initial level
    float initialLevel = dopamine->getLevel();
    assert(initialLevel >= 0.0f && initialLevel <= 1.0f);
    
    // Test level setting
    dopamine->setLevel(0.5f);
    assert(fabs(dopamine->getLevel() - 0.5f) < 0.001f);
    
    std::cout << "    testNeuromodulatorBase passed" << std::endl;
}

// Dopamine System Tests
void testDopamineBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test initial state
    float initialLevel = dopamine->getLevel();
    assert(initialLevel >= 0.0f && initialLevel <= 1.0f);
    
    // Test level setting
    dopamine->setLevel(0.8f);
    assert(fabs(dopamine->getLevel() - 0.8f) < 0.001f);
    
    // Test reward signaling
    dopamine->signalReward(10.0f);
    
    // Test reward prediction error signaling
    dopamine->signalRewardPredictionError(2.5f);
    
    // Test plasticity factor
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor >= 0.0f && plasticityFactor <= 2.0f);
    
    std::cout << "    testDopamineBasic passed" << std::endl;
}

void testDopamineIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test multiple dopamine signals
    for (int i = 0; i < 10; ++i) {
        float reward = static_cast<float>(i * 2);
        dopamine->signalReward(reward);
        
        float error = static_cast<float>(i * 0.5);
        dopamine->signalRewardPredictionError(error);
    }
    
    // Verify plasticity factor reflects accumulated signals
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor > 0.0f);
    
    std::cout << "    testDopamineIntegration passed" << std::endl;
}

void testDopamineErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test that dopamine handles negative rewards/errors gracefully
    dopamine->signalReward(-5.0f); // Negative reward
    dopamine->signalRewardPredictionError(-1.0f); // Negative prediction error
    
    // Should still be in valid range
    float level = dopamine->getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    std::cout << "    testDopamineErrorHandling passed" << std::endl;
}

// Curiosity System Tests
void CuriosityBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Test initial level
    float initialLevel = curiosity->getLevel();
    assert(initialLevel >= 0.0f && initialLevel <= 1.0f);
    
    // Test level setting via exploration drive
    float explorationDrive = curiosity->getExplorationDrive();
    assert(explorationDrive >= 0.0f && explorationDrive <= 1.0f);
    
    // Test update with novelty and prediction error
    curiosity->update(0.5f, 0.3f, 0.01f);
    
    // Test parameter setting
    curiosity->setNoveltyWeight(0.7f);
    curiosity->setPredictionErrorWeight(0.3f);
    
    std::cout << "    testCuriosityBasic passed" << std::endl;
}

void testCuriosityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Test multiple updates with different novelty/error combinations
    std::vector<std::pair<float, float>> inputs = {
        {0.1f, 0.1f},
        {0.5f, 0.2f},
        {0.9f, 0.8f},
        {0.3f, 0.7f},
        {0.2f, 0.4f}
    };
    
    for (const auto& input : inputs) {
        curiosity->update(input.first, input.second, 0.01f);
    }
    
    // Curiosity level should remain in valid range
    float level = curiosity->getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    std::cout << "    testCuriosityIntegration passed" << std::endl;
}

void testCuriosityWeightConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Test extreme weight values
    curiosity->setNoveltyWeight(0.0f);
    curiosity->setPredictionErrorWeight(1.0f);
    
    curiosity->update(0.5f, 0.5f, 0.01f); // Novelty has no effect, only prediction error matters
    
    curiosity->setNoveltyWeight(1.0f);
    curiosity->setPredictionErrorWeight(0.0f);
    
    curiosity->update(0.5f, 0.5f, 0.01f); // Prediction error has no effect, only novelty matters
    
    // Test parameter ranges
    curiosity->setNoveltyWeight(0.5f);
    curiosity->setPredictionErrorWeight(0.5f);
    
    std::cout << "    testCuriosityWeightConfiguration passed" << std::endl;
}

// Novelty System Tests
void NoveltyBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Test initial level
    float initialLevel = novelty->getLevel();
    assert(initialLevel >= 0.0f && initialLevel <= 1.0f);
    
    // Test level setting
    novelty->setLevel(0.6f);
    assert(fabs(novelty->getLevel() - 0.6f) < 0.001f);
    
    // Test novelty detection with patterns
    std::vector<float> currentPattern = {0.1f, 0.5f, 0.9f};
    std::vector<float> previousPattern = {0.2f, 0.4f, 0.8f};
    
    novelty->detectNovelty(currentPattern, previousPattern);
    
    // Test decay
    novelty->update(0.01f);
    
    // Test history
    const std::vector<float>& history = novelty->getHistory();
    assert(!history.empty());
    
    std::cout << "    testNoveltyBasic passed" << std::endl;
}

void testNoveltyPatternDetection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Test with identical patterns (should have low novelty)
    std::vector<float> pattern = {0.5f, 0.5f, 0.5f};
    novelty->detectNovelty(pattern, pattern);
    
    // Test with very different patterns (should have high novelty)
    std::vector<float> pattern1 = {0.0f, 0.0f, 0.0f};
    std::vector<float> pattern2 = {1.0f, 1.0f, 1.0f};
    novelty->detectNovelty(pattern1, pattern2);
    
    // Test with moderately different patterns
    std::vector<float> pattern3 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern4 = {0.8f, 0.7f, 0.6f};
    novelty->detectNovelty(pattern3, pattern4);
    
    // Verify history is maintained
    assert(novelty->getHistory().size() >= 3);
    
    std::cout << "    testNoveltyPatternDetection passed" << std::endl;
}

void testNoveltyDecay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Set high novelty
    novelty->setLevel(1.0f);
    
    // Update multiple times with small decay
    for (int i = 0; i < 10; ++i) {
        novelty->update(0.01f);
        float level = novelty->getLevel();
        assert(level >= 0.0f && level <= 1.0f);
    }
    
    // Clear history
    novelty->clearHistory();
    
    std::cout << "    testNoveltyDecay passed" << std::endl;
}

// Prediction Error System Tests
void PredictionErrorBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predError = brain.getPredictionErrorSignal();
    assert(predError != nullptr);
    
    // Test initial error
    float initialError = predError->getError();
    assert(initialError >= 0.0f && initialError <= 1.0f);
    
    // Test error computation
    predError->computeError(0.5f, 0.3f); // Error = 0.2
    assert(fabs(predError->getError() - 0.2f) < 0.001f);
    
    // Test prediction update
    predError->updatePrediction(0.8f);
    
    // Test error magnitude
    float magnitude = predError->getMagnitude();
    assert(magnitude >= 0.0f);
    
    std::cout << "    testPredictionErrorBasic passed" << std::endl;
}

void testPredictionErrorIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predError = brain.getPredictionErrorSignal();
    assert(predError != nullptr);
    
    // Test sequence of prediction updates and error computations
    std::vector<std::pair<float, float>> predictions = {
        {0.1f, 0.15f},
        {0.2f, 0.25f},
        {0.3f, 0.35f},
        {0.4f, 0.45f}
    };
    
    for (const auto& pred : predictions) {
        predError->updatePrediction(pred.first);
        predError->computeError(pred.first, pred.second);
    }
    
    // Error should reflect the computed differences
    assert(predError->getError() >= 0.0f);
    
    std::cout << "    testPredictionErrorIntegration passed" << std::endl;
}

void testPredictionErrorHistory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predError = brain.getPredictionErrorSignal();
    assert(predError != nullptr);
    
    // Generate some error history
    for (int i = 0; i < 5; ++i) {
        float predicted = static_cast<float>(i) * 0.1f;
        float actual = predicted + static_cast<float>(i) * 0.2f;
        predError->computeError(predicted, actual);
    }
    
    const std::vector<float>& history = predError->getHistory();
    assert(history.size() == 5);
    
    // Clear history
    predError->clearHistory();
    assert(predError->getHistory().empty());
    
    std::cout << "    testPredictionErrorHistory passed" << std::endl;
}

// Reward System Tests
void RewardBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* reward = brain.getDopamine(); // Note: getDopamine() for reward system
    assert(reward != nullptr);
    
    // Test initial value
    float initialValue = reward->getLevel();
    assert(initialValue >= 0.0f && initialValue <= 1.0f);
    
    // Test value setting
    reward->setLevel(0.3f);
    assert(fabs(reward->getLevel() - 0.3f) < 0.001f);
    
    // Test accumulation
    reward->add(0.5f);
    
    // Test reset
    reward->reset();
    
    std::cout << "    testRewardBasic passed" << std::endl;
}

void testRewardIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* reward = brain.getDopamine();
    assert(reward != nullptr);
    
    // Test multiple reward updates
    std::vector<float> rewards = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    for (float r : rewards) {
        reward->setLevel(r); // Note: using setLevel for reward testing
    }
    
    // Test negative rewards
    reward->setLevel(-2.0f); // Should handle gracefully
    
    std::cout << "    testRewardIntegration passed" << std::endl;
}

// Neuromodulation System Integration
void NeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predError != nullptr);
    
    // Simulate neuromodulation signals
    dopamine->setLevel(0.7f);
    novelty->setLevel(0.3f);
    
    curiosity->update(0.5f, 0.2f, 0.01f);
    
    predError->computeError(0.6f, 0.8f);
    
    // Apply neuromodulation to brain
    nlm::Neuromodulator* neuromodulator = dopamine;
    brain.applyNeuromodulation(*neuromodulator);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testNeuromodulationErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predError != nullptr);
    
    // Test neuromodulators with extreme values
    dopamine->setLevel(2.0f); // Should clamp or handle
    assert(dopamine->getLevel() >= 0.0f);
    
    curiosity->update(2.0f, 2.0f, 0.01f); // Should clamp
    assert(curiosity->getLevel() >= 0.0f);
    
    novelty->setLevel(-1.0f); // Should handle
    assert(novelty->getLevel() >= 0.0f);
    
    std::cout << "    testNeuromodulationErrorHandling passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neuromodulation tests..." << std::endl;
    
    try {
        testNeuromodulatorBase();
        
        testDopamineBasic();
        testDopamineIntegration();
        testDopamineErrorHandling();
        
        testCuriosityBasic();
        testCuriosityIntegration();
        testCuriosityWeightConfiguration();
        
        testNoveltyBasic();
        testNoveltyPatternDetection();
        testNoveltyDecay();
        
        testPredictionErrorBasic();
        testPredictionErrorIntegration();
        testPredictionErrorHistory();
        
        testRewardBasic();
        testRewardIntegration();
        
        testNeuromodulationIntegration();
        testNeuromodulationErrorHandling();
        
        std::cout << "  All Neuromodulation tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_neuromodulation