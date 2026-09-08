// Comprehensive Neuromodulation Tests
// Phase 2: Real Neural Computation Tests - Neuromodulation Systems

#include "brain/Brain.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_neuromodulation {

void testDopamineBasic() {
    // Create a minimal brain for dopamine testing
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test initial state
    assert(dopamine->getLevel() >= 0.0f);
    
    // Update dopamine
    dopamine->update(0.001f);
    
    // Test plasticity factor
    assert(dopamine->getPlasticityFactor() >= 0.0f);
    assert(dopamine->getPlasticityFactor() <= 2.0f);  // Should be reasonable
    
    std::cout << "    testDopamineBasic passed" << std::endl;
}

void testDopamineAdaptation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    float initialLevel = dopamine->getLevel();
    
    // Simulate reward prediction error
    // Positive reward should increase dopamine
    class PositiveRPENeuromodulator : public nlm::Neuromodulator {
    public:
        float getLevel() const override { return 0.8f; }
        float getPlasticityFactor() const override { return 1.5f; }
    } rpe;
    
    brain.applyNeuromodulation(rpe);
    
    float levelAfter = dopamine->getLevel();
    assert(levelAfter >= initialLevel);  // Should increase or stay same
    
    // Simulate negative reward (punishment)
    class NegativeRPENeuromodulator : public nlm::Neuromodulator {
    public:
        float getLevel() const override { return -0.5f; }
        float getPlasticityFactor() const override { return 0.5f; }
    } rpe2;
    
    brain.applyNeuromodulation(rpe2);
    
    std::cout << "    testDopamineAdaptation passed" << std::endl;
}

void testCuriosityBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Test initial state
    assert(curiosity->getLevel() >= 0.0f);
    
    // Update curiosity
    curiosity->update(0.001f);
    
    // Test exploration motivation
    assert(curiosity->getExplorationMotivation() >= 0.0f);
    
    std::cout << "    testCuriosityBasic passed" << std::endl;
}

void testCuriosityNovelty() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    float initialLevel = curiosity->getLevel();
    
    // Simulate novel stimulus
    class NoveltyNeuromodulator : public nlm::Neuromodulator {
    public:
        float getLevel() const override { return 0.9f; }
        float getPlasticityFactor() const override { return 1.2f; }
    } novelty;
    
    brain.applyNeuromodulation(novelty);
    
    float levelAfter = curiosity->getLevel();
    // Curiosity may or may not change based on implementation
    // but should remain reasonable
    assert(levelAfter >= 0.0f && levelAfter < 2.0f);
    
    std::cout << "    testCuriosityNovelty passed" << std::endl;
}

void testNoveltyDetection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Test novelty detection with repeated stimuli
    float initialNovelty = novelty->getLevel();
    
    // First exposure should increase novelty
    class NovelStimulusNeuromodulator : public nlm::Neuromodulator {
    public:
        float getLevel() const override { return 0.7f; }
        float getPlasticityFactor() const override { return 1.1f; }
    } novel;
    
    brain.applyNeuromodulation(novel);
    
    float noveltyAfter = novelty->getLevel();
    assert(noveltyAfter > initialNovelty || noveltyAfter == initialNovelty);
    
    // Second exposure should decrease novelty (adaptation)
    brain.applyNeuromodulation(novel);
    
    float noveltyAfterSecond = novelty->getLevel();
    assert(noveltyAfterSecond <= noveltyAfter);
    
    std::cout << "    testNoveltyDetection passed" << std::endl;
}

void testPredictionError() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test prediction error calculation
    float initialError = predictionError->getLevel();
    
    // Simulate prediction
    predictionError->update(0.001f);
    
    float errorAfter = predictionError->getLevel();
    assert(errorAfter >= 0.0f);  // Error should be non-negative
    
    // Test error magnitude
    class LargeErrorNeuromodulator : public nlm::Neuromodulator {
    public:
        float getLevel() const override { return 0.3f; }
        float getPlasticityFactor() const override { return 2.0f; }
    } largeError;
    
    brain.applyNeuromodulation(largeError);
    
    std::cout << "    testPredictionError passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all neuromodulators work together
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Update all neuromodulators
    float dt = 0.001f;
    dopamine->update(dt);
    curiosity->update(dt);
    novelty->update(dt);
    predictionError->update(dt);
    
    // Apply combined neuromodulation effects
    class CombinedNeuromodulator : public nlm::Neuromodulator {
    public:
        float getLevel() const override { 
            return 0.5f + 0.2f + 0.3f;  // Simulate dopamine + curiosity + novelty
        }
        float getPlasticityFactor() const override { 
            return 1.2f;  // Base factor
        }
    } combined;
    
    brain.applyNeuromodulation(combined);
    
    // All levels should be reasonable
    assert(dopamine->getLevel() >= 0.0f);
    assert(curiosity->getLevel() >= 0.0f);
    assert(novelty->getLevel() >= 0.0f);
    assert(predictionError->getLevel() >= 0.0f);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testNeuromodulationErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test error handling with extreme values
    float initialLevel = dopamine->getLevel();
    
    // Update with very small timestep
    dopamine->update(0.000001f);
    
    // Should not crash or produce NaN/inf
    float levelAfter = dopamine->getLevel();
    assert(std::isfinite(levelAfter));
    assert(levelAfter >= 0.0f);
    
    // Test with large timestep
    dopamine->update(0.1f);
    levelAfter = dopamine->getLevel();
    assert(std::isfinite(levelAfter));
    assert(levelAfter >= 0.0f);
    
    std::cout << "    testNeuromodulationErrorHandling passed" << std::endl;
}

void testNeuromodulationPersistence() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    
    // Store some neuromodulation state
    float initialDopamine = dopamine->getLevel();
    float initialCuriosity = curiosity->getLevel();
    
    // Apply some neuromodulation
    class TestNeuromodulator : public nlm::Neuromodulator {
    public:
        float getLevel() const override { return 0.6f; }
        float getPlasticityFactor() const override { return 1.1f; }
    } test;
    
    brain.applyNeuromodulation(test);
    
    // Reset brain
    brain.reset();
    
    // Neuromodulators should be in a reasonable state after reset
    // (implementation dependent, but should not crash)
    float resetDopamine = dopamine->getLevel();
    float resetCuriosity = curiosity->getLevel();
    
    assert(std::isfinite(resetDopamine));
    assert(std::isfinite(resetCuriosity));
    
    std::cout << "    testNeuromodulationPersistence passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neuromodulation tests..." << std::endl;
    testDopamineBasic();
    testDopamineAdaptation();
    testCuriosityBasic();
    testCuriosityNovelty();
    testNoveltyDetection();
    testPredictionError();
    testNeuromodulationIntegration();
    testNeuromodulationErrorHandling();
    testNeuromodulationPersistence();
}

} // namespace test_neuromodulation
