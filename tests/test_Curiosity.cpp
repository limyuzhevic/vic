// Curiosity Tests
#include "neuromodulation/Curiosity.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_curiosity {

void testCuriosityCreation() {
    nlm::Curiosity curiosity;
    
    assert(curiosity.getLevel() == 0.0f);
    assert(curiosity.getExplorationDrive() == 0.0f);
    
    std::cout << "    testCuriosityCreation passed" << std::endl;
}

void testCuriosityInitialization() {
    nlm::Curiosity curiosity;
    
    // Test initialization (just ensure no crash)
    curiosity.initialize(nullptr);
    
    std::cout << "    testCuriosityInitialization passed" << std::endl;
}

void testCuriosityLevel() {
    nlm::Curiosity curiosity;
    
    // Test initial level
    assert(curiosity.getLevel() == 0.0f);
    assert(curiosity.getExplorationDrive() == 0.0f);
    
    // Set level
    curiosity.setLevel(0.5f);
    assert(curiosity.getLevel() == 0.5f);
    assert(curiosity.getExplorationDrive() == 0.5f);  // Same as level
    
    std::cout << "    testCuriosityLevel passed" << std::endl;
}

void testCuriosityUpdate() {
    nlm::Curiosity curiosity;
    
    // Test update with different inputs
    curiosity.update(0.2f, 0.1f, 1.0f);  // novelty=0.2, error=0.1
    
    // Level should have changed (implementation-dependent)
    float level = curiosity.getLevel();
    assert(level >= 0.0f);
    
    // Test with stronger novelty
    curiosity.update(0.8f, 0.5f, 1.0f);  // High novelty and error
    float newLevel = curiosity.getLevel();
    assert(newLevel >= 0.0f);
    
    std::cout << "    testCuriosityUpdate passed" << std::endl;
}

void testCuriosityParameters() {
    nlm::Curiosity curiosity;
    
    // Test default parameters (hard to test without accessing private members)
    // But we can test the public API
    
    // Set parameters
    curiosity.setNoveltyWeight(0.7f);
    curiosity.setPredictionErrorWeight(0.3f);
    
    // Update to use parameters
    curiosity.update(0.5f, 0.2f, 1.0f);
    
    // Should not crash
    float level = curiosity.getLevel();
    assert(level >= 0.0f);
    
    std::cout << "    testCuriosityParameters passed" << std::endl;
}

void testCuriosityReset() {
    nlm::Curiosity curiosity;
    
    // Set level
    curiosity.setLevel(0.5f);
    assert(curiosity.getLevel() == 0.5f);
    
    // Reset
    curiosity.reset();
    assert(curiosity.getLevel() == 0.0f);
    assert(curiosity.getExplorationDrive() == 0.0f);
    
    std::cout << "    testCuriosityReset passed" << std::endl;
}

void testCuriosityIntegration() {
    nlm::Curiosity curiosity;
    
    // Test multiple updates
    std::vector<std::tuple<float, float, float>> testCases = {
        {0.1f, 0.0f, 0.5f},
        {0.3f, 0.2f, 1.0f},
        {0.5f, 0.4f, 2.0f},
        {0.0f, 0.5f, 1.0f}
    };
    
    for (const auto& testCase : testCases) {
        float novelty = std::get<0>(testCase);
        float error = std::get<1>(testCase);
        float dt = std::get<2>(testCase);
        
        curiosity.update(novelty, error, dt);
        
        // Should remain valid
        float level = curiosity.getLevel();
        assert(level >= 0.0f);
        assert(curiosity.getExplorationDrive() == level);
    }
    
    // Reset to ensure clean state
    curiosity.reset();
    assert(curiosity.getLevel() == 0.0f);
    
    std::cout << "    testCuriosityIntegration passed" << std::endl;
}

void testCuriosityWeightedCombination() {
    nlm::Curiosity curiosity;
    
    // Set weights to create predictable behavior
    curiosity.setNoveltyWeight(0.6f);
    curiosity.setPredictionErrorWeight(0.4f);
    
    // Update with specific values
    curiosity.update(1.0f, 1.0f, 1.0f);
    
    // The exact level depends on implementation, but should be reasonable
    float level = curiosity.getLevel();
    assert(level >= 0.0f && level <= 1.0f);  // Likely bounded
    
    std::cout << "    testCuriosityWeightedCombination passed" << std::endl;
}

void runAll() {
    testCuriosityCreation();
    testCuriosityInitialization();
    testCuriosityLevel();
    testCuriosityUpdate();
    testCuriosityParameters();
    testCuriosityReset();
    testCuriosityIntegration();
    testCuriosityWeightedCombination();
}

} // namespace test_curiosity
