// Curiosity Tests
#include "brain/Curiosity.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_curiosity {

void testCuriosityCreation() {
    nlm::Curiosity curiosity;
    
    float level = curiosity.getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    std::cout << "    testCuriosityCreation passed" << std::endl;
}

void testCuriosityConfiguration() {
    nlm::Curiosity curiosity;
    
    // Test initial level
    float level = curiosity.getLevel();
    assert(level == 0.0f);
    
    // Update with novelty and prediction error
    curiosity.update(0.8f, 0.3f, 0.001);
    
    float newLevel = curiosity.getLevel();
    assert(newLevel >= 0.0f && newLevel <= 1.0f);
    
    // Test exploration drive
    float exploration = curiosity.getExplorationDrive();
    assert(exploration >= 0.0f && exploration <= 1.0f);
    
    std::cout << "    testCuriosityConfiguration passed" << std::endl;
}

void testCuriosityParameters() {
    nlm::Curiosity curiosity;
    
    // Set curiosity parameters
    curiosity.setNoveltyWeight(0.7f);
    curiosity.setPredictionErrorWeight(0.3f);
    
    // Update and check level is affected by parameters
    curiosity.update(0.5f, 0.4f, 0.001);
    
    float level = curiosity.getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    std::cout << "    testCuriosityParameters passed" << std::endl;
}

void testCuriosityUpdate() {
    nlm::Curiosity curiosity;
    
    // Update with different combinations
    curiosity.update(0.0f, 0.0f, 0.001);
    curiosity.update(1.0f, 0.0f, 0.001);
    curiosity.update(0.0f, 1.0f, 0.001);
    curiosity.update(1.0f, 1.0f, 0.001);
    
    float level = curiosity.getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    std::cout << "    testCuriosityUpdate passed" << std::endl;
}

void testCuriosityReset() {
    nlm::Curiosity curiosity;
    
    // Set some curiosity level
    curiosity.update(0.8f, 0.6f, 0.001);
    
    float levelBefore = curiosity.getLevel();
    assert(levelBefore > 0.0f);
    
    // Reset
    curiosity.reset();
    
    float levelAfter = curiosity.getLevel();
    assert(levelAfter == 0.0f);
    
    std::cout << "    testCuriosityReset passed" << std::endl;
}

void runAll() {
    std::cout << "Running Curiosity tests..." << std::endl;
    testCuriosityCreation();
    testCuriosityConfiguration();
    testCuriosityParameters();
    testCuriosityUpdate();
    testCuriosityReset();
}

} // namespace test_curiosity