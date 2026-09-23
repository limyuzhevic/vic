// Maturation Tests
#include "brain/Maturation.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_maturation {

void testMaturationCreation() {
    nlm::Maturation maturation;
    
    float progress = maturation.getProgress();
    assert(progress >= 0.0f && progress <= 1.0f);
    
    std::cout << "    testMaturationCreation passed" << std::endl;
}

void testMaturationProgress() {
    nlm::Maturation maturation;
    
    // Set progress
    maturation.setProgress(0.3f);
    assert(std::abs(maturation.getProgress() - 0.3f) < 0.001f);
    
    maturation.setProgress(0.7f);
    assert(std::abs(maturation.getProgress() - 0.7f) < 0.001f);
    
    // Test boundary values
    maturation.setProgress(0.0f);
    assert(maturation.getProgress() == 0.0f);
    
    maturation.setProgress(1.0f);
    assert(maturation.getProgress() == 1.0f);
    
    std::cout << "    testMaturationProgress passed" << std::endl;
}

void testMaturationMatureProperties() {
    nlm::Maturation maturation;
    
    // Get mature properties
    nlm::MembranePotential threshold = maturation.getMatureThreshold();
    nlm::MembranePotential resting = maturation.getMatureRestingPotential();
    float timeConstant = maturation.getMatureTimeConstant();
    
    // These should return reasonable values
    assert(std::abs(threshold) >= 0.0f && std::abs(threshold) <= 100.0f);
    assert(std::abs(resting) >= -100.0f && std::abs(resting) <= 0.0f);
    assert(timeConstant >= 0.0f && timeConstant <= 100.0f);
    
    std::cout << "    testMaturationMatureProperties passed" << std::endl;
}

void testMaturationUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Maturation maturation;
    
    // Test update with current step
    maturation.update(&brain, 0);
    
    maturation.update(&brain, 100);
    
    std::cout << "    testMaturationUpdate passed" << std::endl;
}

void testMaturationEdgeCases() {
    nlm::Maturation maturation;
    
    // Test progress boundary values
    maturation.setProgress(0.0f);
    assert(maturation.getProgress() == 0.0f);
    
    maturation.setProgress(0.5f);
    assert(std::abs(maturation.getProgress() - 0.5f) < 0.001f);
    
    maturation.setProgress(1.0f);
    assert(maturation.getProgress() == 1.0f);
    
    std::cout << "    testMaturationEdgeCases passed" << std::endl;
}

void runAll() {
    std::cout << "Running Maturation tests..." << std::endl;
    testMaturationCreation();
    testMaturationProgress();
    testMaturationMatureProperties();
    testMaturationUpdate();
    testMaturationEdgeCases();
}

} // namespace test_maturation