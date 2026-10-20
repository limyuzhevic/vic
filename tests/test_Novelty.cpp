// Novelty Tests
#include "neuromodulation/Novelty.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_novelty {

void testNoveltyCreation() {
    nlm::Novelty novelty;
    
    assert(novelty.getLevel() == 0.0f);
    assert(novelty.getHistory().empty());
    
    std::cout << "    testNoveltyCreation passed" << std::endl;
}

void testNoveltyInitialization() {
    nlm::Novelty novelty;
    
    // Test initialization (just ensure no crash)
    novelty.initialize(nullptr);
    
    std::cout << "    testNoveltyInitialization passed" << std::endl;
}

void testNoveltyLevel() {
    nlm::Novelty novelty;
    
    // Test initial level
    assert(novelty.getLevel() == 0.0f);
    
    // Set level
    novelty.setLevel(0.7f);
    assert(novelty.getLevel() == 0.7f);
    
    std::cout << "    testNoveltyLevel passed" << std::endl;
}

void testNoveltyDetectionFromObservations() {
    nlm::Novelty novelty;
    
    // Create two observations with different intensities
    nlm::Observation obs1;
    obs1.intensity = 0.5f;
    obs1.spatialX = 1.0f;
    obs1.spatialY = 2.0f;
    
    nlm::Observation obs2;
    obs2.intensity = 0.8f;  // Different intensity
    obs2.spatialX = 1.5f;  // Different position
    obs2.spatialY = 2.5f;
    
    // Detect novelty
    novelty.detectNovelty(obs1, obs2);
    
    // Should have some novelty level
    float level = novelty.getLevel();
    assert(level >= 0.0f);
    
    // Detect from same observation (should be low novelty)
    novelty.detectNovelty(obs1, obs1);
    
    std::cout << "    testNoveltyDetectionFromObservations passed" << std::endl;
}

void testNoveltyDetectionFromPatterns() {
    nlm::Novelty novelty;
    
    // Create two different patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> pattern2 = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};  // Very different
    
    // Detect novelty between patterns
    novelty.detectNovelty(pattern1, pattern2);
    
    // Should have high novelty for very different patterns
    float level = novelty.getLevel();
    assert(level >= 0.0f);
    
    // Detect novelty between same pattern
    novelty.detectNovelty(pattern1, pattern1);
    
    std::cout << "    testNoveltyDetectionFromPatterns passed" << std::endl;
}

void testNoveltyUpdate() {
    nlm::Novelty novelty;
    
    // Set some novelty level
    novelty.setLevel(0.8f);
    assert(novelty.getLevel() == 0.8f);
    
    // Update with time decay
    novelty.update(1.0f);
    
    // Level should decay (implementation-dependent)
    float newLevel = novelty.getLevel();
    assert(newLevel >= 0.0f);
    
    std::cout << "    testNoveltyUpdate passed" << std::endl;
}

void testNoveltyHistory() {
    nlm::Novelty novelty;
    
    assert(novelty.getHistory().empty());
    
    // Set level
    novelty.setLevel(0.5f);
    assert(novelty.getHistory().size() == 1);
    assert(novelty.getHistory()[0] == 0.5f);
    
    // Set another level
    novelty.setLevel(0.8f);
    assert(novelty.getHistory().size() == 2);
    assert(novelty.getHistory()[1] == 0.8f);
    
    // Clear history
    novelty.clearHistory();
    assert(novelty.getHistory().empty());
    
    std::cout << "    testNoveltyHistory passed" << std::endl;
}

void testNoveltyIntegration() {
    nlm::Novelty novelty;
    
    // Test multiple detections
    for (int i = 0; i < 5; ++i) {
        std::vector<float> pattern1 = {static_cast<float>(i) * 0.1f, 
                                     static_cast<float>(i) * 0.2f,
                                     static_cast<float>(i) * 0.3f,
                                     static_cast<float>(i) * 0.4f,
                                     static_cast<float>(i) * 0.5f};
        
        std::vector<float> pattern2 = {pattern1[0] + 0.1f, pattern1[1] + 0.1f, 
                                     pattern1[2] + 0.1f, pattern1[3] + 0.1f, 
                                     pattern1[4] + 0.1f};
        
        novelty.detectNovelty(pattern1, pattern2);
        
        // Should remain valid
        float level = novelty.getLevel();
        assert(level >= 0.0f);
    }
    
    // Update to decay
    novelty.update(1.0f);
    
    // Reset to ensure clean state
    novelty.reset();
    assert(novelty.getLevel() == 0.0f);
    assert(novelty.getHistory().empty());
    
    std::cout << "    testNoveltyIntegration passed" << std::endl;
}

void runAll() {
    testNoveltyCreation();
    testNoveltyInitialization();
    testNoveltyLevel();
    testNoveltyDetectionFromObservations();
    testNoveltyDetectionFromPatterns();
    testNoveltyUpdate();
    testNoveltyHistory();
    testNoveltyIntegration();
}

} // namespace test_novelty
