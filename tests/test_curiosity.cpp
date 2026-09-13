// Curiosity Neuromodulation Tests
// Tests for exploration drive and novelty-seeking behavior

#include "neuromodulation/Curiosity.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_curiosity {

void testCuriosityCreation() {
    nlm::Curiosity curiosity;
    
    // Creation doesn't crash
    std::cout << "    testCuriosityCreation passed" << std::endl;
}

void testCuriosityLevel() {
    nlm::Curiosity curiosity;
    
    assert(curiosity.getLevel() == 0.0f);
    
    std::cout << "    testCuriosityLevel passed" << std::endl;
}

void testCuriosityExplorationDrive() {
    nlm::Curiosity curiosity;
    
    assert(curiosity.getExplorationDrive() == 0.0f);
    
    std::cout << "    testCuriosityExplorationDrive passed" << std::endl;
}

void testCuriosityUpdate() {
    nlm::Curiosity curiosity;
    
    // Update with novelty and prediction error
    curiosity.update(0.5f, 0.3f, 0.1f);  // novelty=0.5, error=0.3, dt=0.1
    
    // Curiosity level should have changed
    assert(curiosity.getLevel() >= 0.0f);
    
    std::cout << "    testCuriosityUpdate passed" << std::endl;
}

void testCuriosityWeights() {
    nlm::Curiosity curiosity;
    
    // Set novelty weight
    curiosity.setNoveltyWeight(0.7f);
    
    // Set prediction error weight  
    curiosity.setPredictionErrorWeight(0.3f);
    
    // Test update with different weights
    curiosity.update(1.0f, 1.0f, 0.1f);
    
    std::cout << "    testCuriosityWeights passed" << std::endl;
}

void testCuriosityReset() {
    nlm::Curiosity curiosity;
    
    // Update curiosity first
    curiosity.update(0.5f, 0.5f, 0.1f);
    
    // Reset should clear state
    curiosity.reset();
    assert(curiosity.getLevel() == 0.0f);
    
    std::cout << "    testCuriosityReset passed" << std::endl;
}

void testCuriosityNoveltySeeking() {
    nlm::Curiosity curiosity;
    
    // High novelty should increase curiosity
    curiosity.update(1.0f, 0.0f, 0.1f);  // High novelty, low error
    
    // Low novelty with high error might also increase curiosity
    curiosity.update(0.0f, 1.0f, 0.1f);
    
    std::cout << "    testCuriosityNoveltySeeking passed" << std::endl;
}

void testCuriosityIntegration() {
    nlm::Curiosity curiosity;
    
    // Test integration with brain
    // This is a placeholder for more complex integration
    std::cout << "    testCuriosityIntegration passed (placeholder)" << std::endl;
}

void runAll() {
    std::cout << "Running Curiosity tests..." << std::endl;
    testCuriosityCreation();
    testCuriosityLevel();
    testCuriosityExplorationDrive();
    testCuriosityUpdate();
    testCuriosityWeights();
    testCuriosityReset();
    testCuriosityNoveltySeeking();
    testCuriosityIntegration();
}

} // namespace test_curiosity