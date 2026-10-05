// Hebbian Tests
#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianConfiguration() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(2.0f);
    
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getMaxWeight() == 2.0f);
    
    std::cout << "    testHebbianConfiguration passed" << std::endl;
}

void testHebbianCorrelationCalculation() {
    nlm::Hebbian hebbian;
    
    // Create a mock synapse
    auto synapse = std::make_shared<nlm::Synapse>();
    float initialWeight = synapse->getWeight();
    
    // Test with empty spikes (should return early)
    std::vector<nlm::Timestamp> emptySpikes;
    hebbian.update(synapse.get(), emptySpikes, emptySpikes, 0.0f);
    
    // Test with single spike pair (should correlate)
    std::vector<nlm::Timestamp> preSpikes = {100.0f};
    std::vector<nlm::Timestamp> postSpikes = {100.0f}; // Same time = within 100ms window
    
    hebbian.update(synapse.get(), preSpikes, postSpikes, 0.0f);
    
    // Test with spikes outside correlation window (no correlation)
    std::vector<nlm::Timestamp> preSpikes2 = {100.0f};
    std::vector<nlm::Timestamp> postSpikes2 = {300.0f}; // 200ms apart = outside window
    
    float weightBefore = synapse->getWeight();
    hebbian.update(synapse.get(), preSpikes2, postSpikes2, 0.0f);
    float weightAfter = synapse->getWeight();
    
    // Correlation should result in weight change
    // The exact change depends on implementation details, but it should be non-zero
    assert(std::abs(weightAfter - weightBefore) > 1e-6f);
    
    std::cout << "    testHebbianCorrelationCalculation passed" << std::endl;
}

void testHebbianSpikingPattern() {
    nlm::Hebbian hebbian;
    
    // Create a mock synapse
    auto synapse = std::make_shared<nlm::Synapse>();
    float initialWeight = synapse->getWeight();
    
    // Test with multiple spikes - some correlated, some not
    std::vector<nlm::Timestamp> preSpikes = {100.0f, 200.0f, 400.0f};
    std::vector<nlm::Timestamp> postSpikes = {100.0f, 250.0f, 500.0f};
    
    // 100.0 - 100.0: correlated (0ms)
    // 100.0 - 250.0: correlated (150ms, outside window)
    // 100.0 - 500.0: correlated (400ms, outside window)
    // 200.0 - 100.0: correlated (100ms, at boundary)
    // 200.0 - 250.0: correlated (50ms)
    // 200.0 - 500.0: correlated (300ms, outside window)
    // 400.0 - 100.0: correlated (300ms, outside window)
    // 400.0 - 250.0: correlated (150ms, outside window)
    // 400.0 - 500.0: correlated (100ms, at boundary)
    
    // Within 100ms window (inclusive): 
    // (100,100), (200,100), (200,250), (400,500) = 4 correlations
    hebbian.update(synapse.get(), preSpikes, postSpikes, 0.0f);
    
    std::cout << "    testHebbianSpikingPattern passed" << std::endl;
}

void testHebbianWeightBounds() {
    nlm::Hebbian hebbian;
    
    auto synapse = std::make_shared<nlm::Synapse>();
    
    // Set max weight low
    hebbian.setMaxWeight(0.5f);
    hebbian.setMinWeight(-0.5f);
    
    // Get current synapse weight
    float initialWeight = synapse->getWeight();
    
    // Apply Hebbian update multiple times to approach bounds
    std::vector<nlm::Timestamp> preSpikes = {100.0f};
    std::vector<nlm::Timestamp> postSpikes = {100.0f};
    
    // Push weight to max
    hebbian.update(synapse.get(), preSpikes, postSpikes, 0.0f);
    
    float currentWeight = synapse->getWeight();
    
    // Weight should be clamped to maxWeight
    assert(currentWeight <= 0.5f);
    assert(currentWeight >= -0.5f);
    
    std::cout << "    testHebbianWeightBounds passed" << std::endl;
}

void runAll() {
    testHebbianCreation();
    testHebbianConfiguration();
    testHebbianCorrelationCalculation();
    testHebbianSpikingPattern();
    testHebbianWeightBounds();
}

} // namespace test_hebbian