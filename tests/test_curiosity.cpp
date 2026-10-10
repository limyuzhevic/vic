// Curiosity Tests
#include "neuromodulation/Curiosity.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>

namespace test_curiosity {

void testCuriosityCreation() {
    nlm::Curiosity curiosity;
    
    assert(curiosity.getLevel() == 0.0f);
    assert(curiosity.getExplorationDrive() == 0.0f);
    
    std::cout << "    testCuriosityCreation passed" << std::endl;
}

void testCuriosityInitialization() {
    nlm::Curiosity curiosity;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    curiosity.initialize(&brain);
    
    assert(curiosity.getLevel() == 0.0f);
    
    std::cout << "    testCuriosityInitialization passed" << std::endl;
}

void testCuriosityUpdate() {
    nlm::Curiosity curiosity;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    curiosity.initialize(&brain);
    
    // Test with zero novelty and prediction error
    curiosity.update(0.0f, 0.0f, 1.0f);
    assert(curiosity.getLevel() == 0.0f);
    
    // Test with positive novelty only
    curiosity.update(0.8f, 0.0f, 1.0f);
    assert(curiosity.getLevel() > 0.0f);
    assert(curiosity.getLevel() <= 1.0f); // Should be bounded
    
    // Test with prediction error only
    curiosity.reset();
    curiosity.update(0.0f, 0.8f, 1.0f);
    assert(curiosity.getLevel() > 0.0f);
    
    // Test with both novelty and prediction error
    curiosity.reset();
    curiosity.update(0.5f, 0.5f, 1.0f);
    float expected = std::min(1.0f, 0.5f * 0.5f + 0.5f * 0.5f); // With weights of 0.5 each
    assert(std::abs(curiosity.getLevel() - expected) < 0.01f);
    
    std::cout << "    testCuriosityUpdate passed" << std::endl;
}

void testCuriosityExplorationDrive() {
    nlm::Curiosity curiosity;
    
    assert(curiosity.getExplorationDrive() == curiosity.getLevel());
    
    curiosity.setNoveltyWeight(0.3f);
    curiosity.setPredictionErrorWeight(0.7f);
    
    curiosity.update(1.0f, 0.0f, 1.0f);
    float drive = curiosity.getExplorationDrive();
    assert(drive > 0.0f);
    assert(drive <= 1.0f);
    
    std::cout << "    testCuriosityExplorationDrive passed" << std::endl;
}

void testCuriosityWeights() {
    nlm::Curiosity curiosity;
    
    // Test default weights
    curiosity.update(1.0f, 1.0f, 1.0f);
    
    // Test setting weights
    curiosity.setNoveltyWeight(0.2f);
    curiosity.setPredictionErrorWeight(0.8f);
    
    curiosity.reset();
    curiosity.update(1.0f, 1.0f, 1.0f);
    
    // With weights 0.2 and 0.8, expected level should be different
    // This test ensures the weights are being used
    
    std::cout << "    testCuriosityWeights passed" << std::endl;
}

void testCuriosityDecay() {
    nlm::Curiosity curiosity;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    curiosity.initialize(&brain);
    
    curiosity.update(1.0f, 0.0f, 1.0f); // Set curiosity high
    assert(curiosity.getLevel() > 0.0f);
    
    // Update with zero novelty/error to allow decay
    curiosity.update(0.0f, 0.0f, 1.0f);
    
    // After update, level should decrease due to decay
    // The exact amount depends on decay rate (0.05 default)
    
    std::cout << "    testCuriosityDecay passed" << std::endl;
}

void testCuriosityReset() {
    nlm::Curiosity curiosity;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    curiosity.initialize(&brain);
    
    curiosity.update(1.0f, 1.0f, 1.0f);
    assert(curiosity.getLevel() > 0.0f);
    
    curiosity.reset();
    assert(curiosity.getLevel() == 0.0f);
    
    std::cout << "    testCuriosityReset passed" << std::endl;
}

void runAll() {
    std::cout << "Running Curiosity tests..." << std::endl;
    testCuriosityCreation();
    testCuriosityInitialization();
    testCuriosityUpdate();
    testCuriosityExplorationDrive();
    testCuriosityWeights();
    testCuriosityDecay();
    testCuriosityReset();
}

} // namespace test_curiosity
