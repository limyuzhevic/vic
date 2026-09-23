// Synaptogenesis Tests
#include "brain/Synaptogenesis.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_synaptogenesis {

void testSynaptogenesisCreation() {
    nlm::Synaptogenesis synaptogenesis;
    
    float rate = synaptogenesis.getFormationRate();
    assert(rate >= 0.0f && rate <= 1.0f);
    
    float density = synaptogenesis.getTargetDensity();
    assert(density >= 0.0f);
    
    std::cout << "    testSynaptogenesisCreation passed" << std::endl;
}

void testSynaptogenesisConfiguration() {
    nlm::Synaptogenesis synaptogenesis;
    
    // Set formation rate
    synaptogenesis.setFormationRate(0.05f);
    assert(std::abs(synaptogenesis.getFormationRate() - 0.05f) < 0.001f);
    
    // Set target density
    synaptogenesis.setTargetDensity(0.3f);
    assert(std::abs(synaptogenesis.getTargetDensity() - 0.3f) < 0.001f);
    
    // Test boundary values
    synaptogenesis.setFormationRate(0.0f);
    assert(synaptogenesis.getFormationRate() == 0.0f);
    
    synaptogenesis.setFormationRate(1.0f);
    assert(synaptogenesis.getFormationRate() == 1.0f);
    
    std::cout << "    testSynaptogenesisConfiguration passed" << std::endl;
}

void testSynaptogenesisUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Synaptogenesis synaptogenesis;
    nlm::RandomGenerator rng(42);
    
    // Test update
    synaptogenesis.update(&brain, rng);
    
    std::cout << "    testSynaptogenesisUpdate passed" << std::endl;
}

void testSynaptogenesisEdgeCases() {
    nlm::Synaptogenesis synaptogenesis;
    
    // Test extreme formation rates
    synaptogenesis.setFormationRate(0.0f);
    assert(synaptogenesis.getFormationRate() == 0.0f);
    
    synaptogenesis.setFormationRate(0.999f);
    assert(std::abs(synaptogenesis.getFormationRate() - 0.999f) < 0.001f);
    
    // Test zero target density
    synaptogenesis.setTargetDensity(0.0f);
    assert(synaptogenesis.getTargetDensity() == 0.0f);
    
    // Test high target density
    synaptogenesis.setTargetDensity(1.0f);
    assert(synaptogenesis.getTargetDensity() == 1.0f);
    
    std::cout << "    testSynaptogenesisEdgeCases passed" << std::endl;
}

void runAll() {
    std::cout << "Running Synaptogenesis tests..." << std::endl;
    testSynaptogenesisCreation();
    testSynaptogenesisConfiguration();
    testSynaptogenesisUpdate();
    testSynaptogenesisEdgeCases();
}

} // namespace test_synaptogenesis