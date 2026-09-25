// Neuromodulation Tests
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/Curiosity.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_neuromodulation {

TEST_CASE("Neuromodulator base class functionality", "[neuromodulation][base][neuromodulator]") {
    nlm::Neuromodulator neuromodulator;
    
    REQUIRE(neuromodulator.getName() != nullptr);
    REQUIRE(neuromodulator.getEffectType() != nullptr);
    REQUIRE(neuromodulator.getInfluenceStrength() > 0.0f);
}

TEST_CASE("Reward system functionality", "[neuromodulation][reward]") {
    nlm::Reward reward;
    
    REQUIRE(std::string(reward.getName()) == "Reward");
    REQUIRE(reward.getPredictionError() > 0.0f);
    REQUIRE(reward.getLearningRate() > 0.0f);
}

TEST_CASE("Prediction error system functionality", "[neuromodulation][prediction_error]") {
    nlm::PredictionError predictionError;
    
    REQUIRE(std::string(predictionError.getName()) == "PredictionError");
    REQUIRE(predictionError.getErrorMagnitude() > 0.0f);
    REQUIRE(predictionError.getErrorThreshold() > 0.0f);
}

TEST_CASE("Novelty detection system functionality", "[neuromodulation][novelty]") {
    nlm::Novelty novelty;
    
    REQUIRE(std::string(novelty.getName()) == "Novelty");
    REQUIRE(novelty.getSurpriseValue() > 0.0f);
    REQUIRE(novelty.getDetectionThreshold() > 0.0f);
}

TEST_CASE("Curiosity system functionality", "[neuromodulation][curiosity]") {
    nlm::Curiosity curiosity;
    
    REQUIRE(std::string(curiosity.getName()) == "Curiosity");
    REQUIRE(curiosity.getExplorationDrive() > 0.0f);
    REQUIRE(curiosity.getExploitationBalance() > 0.0f);
}

TEST_CASE("Neuromodulator effect application", "[neuromodulation][effect][application]") {
    nlm::Reward reward;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setMembranePotential(-60.0f);
    
    float initialPotential = neuron.getMembranePotential();
    
    // Apply reward effect
    reward.applyEffect(&neuron, 1.0f);
    
    // Neuron should have been modified by neuromodulator
    REQUIRE(neuron.getMembranePotential() != initialPotential);
}

TEST_CASE("Reward prediction error calculation", "[neuromodulation][reward][error]") {
    nlm::Reward reward;
    
    float expected = 1.0f;
    float actual = 0.8f;
    
    float error = reward.calculatePredictionError(expected, actual);
    
    REQUIRE(error != 0.0f);
    REQUIRE(std::abs(error) > 0.0f);
}

TEST_CASE("Novelty detection threshold tuning", "[neuromodulation][novelty][threshold]") {
    nlm::Novelty novelty;
    
    float initialThreshold = novelty.getDetectionThreshold();
    
    novelty.setDetectionThreshold(0.5f);
    REQUIRE(novelty.getDetectionThreshold() == 0.5f);
}

TEST_CASE("Curiosity exploration-exploitation balance", "[neuromodulation][curiosity][balance]") {
    nlm::Curiosity curiosity;
    
    float exploration = curiosity.getExplorationDrive();
    float exploitation = curiosity.getExploitationBalance();
    
    REQUIRE(exploration + exploitation <= 1.0f);
    REQUIRE(exploration >= 0.0f);
    REQUIRE(exploitation >= 0.0f);
}

TEST_CASE("Neuromodulator system integration", "[neuromodulation][integration][system]") {
    // Create a scenario with multiple neuromodulators
    nlm::Reward reward;
    nlm::Novelty novelty;
    nlm::Curiosity curiosity;
    
    // Verify all neuromodulators are functioning
    REQUIRE(reward.getPredictionError() > 0.0f);
    REQUIRE(novelty.getSurpriseValue() > 0.0f);
    REQUIRE(curiosity.getExplorationDrive() > 0.0f);
}

TEST_CASE("Neuromodulator interaction with neurons", "[neuromodulation][neuron][interaction]") {
    nlm::Novelty novelty;
    nlm::Curiosity curiosity;
    
    nlm::Neuron neuron(nlm::NeuronId(2));
    neuron.setMembranePotential(-65.0f);
    
    float initialPotential = neuron.getMembranePotential();
    
    // Apply novelty effect
    novelty.applyEffect(&neuron, 0.3f);
    
    // Apply curiosity effect
    curiosity.applyEffect(&neuron, 0.7f);
    
    // Neuron should have been modified by combined effects
    REQUIRE(neuron.getMembranePotential() != initialPotential);
}

} // namespace test_neuromodulation
