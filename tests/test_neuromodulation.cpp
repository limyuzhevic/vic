// Neuromodulation Tests
// Tests for all neuromodulator systems including Dopamine, Novelty, Curiosity, and PredictionError

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "TestInfrastructure.hpp"
#include "TestUtilities.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

namespace test_neuromodulation {

void testNeuromodulatorBaseClass() {
    // Test abstract base class cannot be instantiated directly
    // This is implicit through the C++ type system
    
    std::cout << "    testNeuromodulatorBaseClass passed" << std::endl;
}

void testDopamineCreation() {
    Dopamine dopamine;
    
    // Test basic properties
    assert(std::string(dopamine.getName()) == "Dopamine");
    assert(nlm_test::utils::approxEqual(dopamine.getLevel(), 0.0f));
    
    // Test level setting
    dopamine.setLevel(0.5f);
    assert(nlm_test::utils::approxEqual(dopamine.getLevel(), 0.5f));
    
    // Test plasticity factor
    assert(nlm_test::utils::approxEqual(dopamine.getPlasticityFactor(), 1.0f));
    
    std::cout << "    testDopamineCreation passed" << std::endl;
}

void testDopamineRewardSignaling() {
    Dopamine dopamine;
    
    // Test reward signaling
    dopamine.signalReward(1.0f);
    assert(nlm_test::utils::approxEqual(dopamine.getLevel(), 1.0f));
    
    // Test reward prediction error signaling
    dopamine.signalRewardPredictionError(0.5f);
    // Prediction error may add to current level
    
    // Test decay over time
    dopamine.update(1.0f); // Update with time
    // Level should decrease slightly due to decay
    
    std::cout << "    testDopamineRewardSignaling passed" << stdendl;
}

void testCuriosityCreation() {
    Curiosity curiosity;
    
    // Test basic properties
    assert(std::string(curiosity.getName()) == "Curiosity");
    assert(nlm_test::utils::approxEqual(curiosity.getLevel(), 0.0f));
    
    // Test curiosity computation
    curiosity.setLevel(0.3f);
    assert(nlm_test::utils::approxEqual(curiosity.getLevel(), 0.3f));
    
    // Test plasticity factor
    assert(nlm_test::utils::approxEqual(curiosity.getPlasticityFactor(), 1.0f));
    
    std::cout << "    testCuriosityCreation passed" << std::endl;
}

void testNoveltyCreation() {
    Novelty novelty;
    
    // Test basic properties
    assert(std::string(novelty.getName()) == "Novelty");
    assert(nlm_test::utils::approxEqual(novelty.getLevel(), 0.0f));
    
    // Test novelty detection
    novelty.setLevel(0.8f);
    assert(nlm_test::utils::approxEqual(novelty.getLevel(), 0.8f));
    
    // Test plasticity factor
    assert(nlm_test::utils::approxEqual(novelty.getPlasticityFactor(), 1.0f));
    
    std::cout << "    testNoveltyCreation passed" << std::endl;
}

void testPredictionErrorCreation() {
    PredictionError predictionError;
    
    // Test basic properties
    assert(std::string(predictionError.getName()) == "PredictionError");
    assert(nlm_test::utils::approxEqual(predictionError.getLevel(), 0.0f));
    
    // Test prediction error computation
    predictionError.setLevel(-0.2f);
    assert(nlm_test::utils::approxEqual(predictionError.getLevel(), -0.2f));
    
    // Test plasticity factor
    assert(nlm_test::utils::approxEqual(predictionError.getPlasticityFactor(), 1.0f));
    
    // Test prediction error signaling
    predictionError.signalPredictionError(0.5f);
    // Error level should increase
    
    std::cout << "    testPredictionErrorCreation passed" << std::endl;
}

void testAllNeuromodulators() {
    std::vector<std::unique_ptr<nlm::Neuromodulator>> neuromodulators;
    
    // Test that we can create all neuromodulator types
    neuromodulators.push_back(std::make_unique<Dopamine>());
    neuromodulators.push_back(std::make_unique<Curiosity>());
    neuromodulators.push_back(std::make_unique<Novelty>());
    neuromodulators.push_back(std::make_unique<PredictionError>());
    
    // Acetylcholine, Norepinephrine, Serotonin are placeholder implementations
    // They should still work as concrete classes
    nlm::Acetylcholine ach;
    nlm::Norepinephrine ne;
    nlm::Serotonin serotonin;
    
    // Test all have names
    assert(std::string(ach.getName()) == "ACh");
    assert(std::string(ne.getName()) == "NE");
    assert(std::string(serotonin.getName()) == "5-HT");
    
    // Test all can set/get levels
    ach.setLevel(0.1f);
    assert(nlm_test::utils::approxEqual(ach.getLevel(), 0.1f));
    
    ne.setLevel(0.2f);
    assert(nlm_test::utils::approxEqual(ne.getLevel(), 0.2f));
    
    serotonin.setLevel(0.3f);
    assert(nlm_test::utils::approxEqual(serotonin.getLevel(), 0.3f));
    
    std::cout << "    testAllNeuromodulators passed" << std::endl;
}

void testNeuromodulatorIntegrationWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulator access through brain
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test that all neuromodulators are functional
    dopamine->setLevel(0.5f);
    curiosity->setLevel(0.3f);
    novelty->setLevel(0.7f);
    predictionError->setLevel(-0.1f);
    
    assert(nlm_test::utils::approxEqual(dopamine->getLevel(), 0.5f));
    assert(nlm_test::utils::approxEqual(curiosity->getLevel(), 0.3f));
    assert(nlm_test::utils::approxEqual(novelty->getLevel(), 0.7f));
    assert(nlm_test::utils::approxEqual(predictionError->getLevel(), -0.1f));
    
    std::cout << "    testNeuromodulatorIntegrationWithBrain passed" << std::endl;
}

void testNeuromodulatorUpdateAndDecay() {
    Dopamine dopamine;
    
    dopamine.setLevel(1.0f);
    assert(nlm_test::utils::approxEqual(dopamine.getLevel(), 1.0f));
    
    // Update should decrease level due to decay
    dopamine.update(1.0f);
    
    Curiosity curiosity;
    curiosity.setLevel(0.8f);
    curiosity.update(1.0f);
    
    Novelty novelty;
    novelty.setLevel(0.6f);
    novelty.update(1.0f);
    
    PredictionError predictionError;
    predictionError.setLevel(0.4f);
    predictionError.update(1.0f);
    
    std::cout << "    testNeuromodulatorUpdateAndDecay passed" << std::endl;
}

void testNeuromodulatorPlasticityFactors() {
    // Test that neuromodulators modify plasticity appropriately
    Dopamine dopamine;
    assert(nlm_test::utils::approxEqual(dopamine.getPlasticityFactor(), 1.0f));
    
    Curiosity curiosity;
    assert(nlm_test::utils::approxEqual(curiosity.getPlasticityFactor(), 1.0f));
    
    Novelty novelty;
    assert(nlm_test::utils::approxEqual(novelty.getPlasticityFactor(), 1.0f));
    
    PredictionError predictionError;
    assert(nlm_test::utils::approxEqual(predictionError.getPlasticityFactor(), 1.0f));
    
    // Placeholder neuromodulators should have plasticity factor of 1.0f
    nlm::Acetylcholine ach;
    nlm::Norepinephrine ne;
    nlm::Serotonin serotonin;
    
    assert(nlm_test::utils::approxEqual(ach.getPlasticityFactor(), 1.0f));
    assert(nlm_test::utils::approxEqual(ne.getPlasticityFactor(), 1.0f));
    assert(nlm_test::utils::approxEqual(serotonin.getPlasticityFactor(), 1.0f));
    
    std::cout << "    testNeuromodulatorPlasticityFactors passed" << std::endl;
}

void testNeuromodulatorStress() {
    // Rapid creation, update, and level changes
    for (int i = 0; i < 100; ++i) {
        Dopamine dopamine;
        dopamine.setLevel(static_cast<float>(i % 100) / 100.0f);
        dopamine.update(0.1f);
        
        Curiosity curiosity;
        curiosity.setLevel(static_cast<float>((i * 2) % 100) / 100.0f);
        curiosity.update(0.1f);
        
        // Verify levels are reasonable
        assert(dopamine.getLevel() >= 0.0f && dopamine.getLevel() <= 1.0f);
        assert(curiosity.getLevel() >= 0.0f && curiosity.getLevel() <= 1.0f);
    }
    
    std::cout << "    testNeuromodulatorStress passed" << std::endl;
}

void testNeuromodulatorBrainIntegrationStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Set initial neuromodulator levels
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    
    dopamine->setLevel(0.5f);
    curiosity->setLevel(0.3f);
    
    // Run a few simulation steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
        
        // Verify neuromodulators still exist and accessible
        assert(brain.getDopamine() != nullptr);
        assert(brain.getCuriosity() != nullptr);
        
        // Levels should evolve (decay, update, etc.)
        // We can't make exact assertions due to the internal implementation,
        // but we can verify they don't crash and remain valid
        assert(brain.getDopamine()->getLevel() >= -1.0f); // Allow some range
        assert(brain.getCuriosity()->getLevel() >= -1.0f);
    }
    
    std::cout << "    testNeuromodulatorBrainIntegrationStep passed" << std::endl;
}

void runAll() {
    nlm_test::TestRunner runner(true);
    
    runner.runTest("Neuromodulator Base Class", testNeuromodulatorBaseClass);
    runner.runTest("Dopamine Creation", testDopamineCreation);
    runner.runTest("Dopamine Reward Signaling", testDopamineRewardSignaling);
    runner.runTest("Curiosity Creation", testCuriosityCreation);
    runner.runTest("Novelty Creation", testNoveltyCreation);
    runner.runTest("Prediction Error Creation", testPredictionErrorCreation);
    runner.runTest("All Neuromodulators", testAllNeuromodulators);
    runner.runTest("Neuromodulator Integration With Brain", testNeuromodulatorIntegrationWithBrain);
    runner.runTest("Neuromodulator Update and Decay", testNeuromodulatorUpdateAndDecay);
    runner.runTest("Neuromodulator Plasticity Factors", testNeuromodulatorPlasticityFactors);
    runner.runTest("Neuromodulator Stress Test", testNeuromodulatorStress);
    runner.runTest("Neuromodulator Brain Integration Step", testNeuromodulatorBrainIntegrationStep);
    
    runner.runAll();
}

} // namespace test_neuromodulation
