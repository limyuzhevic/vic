// Novelty Tests
// Comprehensive testing of novelty neuromodulation system

#include "neuromodulation/Novelty.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>

namespace test_novelty {

void testNoveltyCreation() {
    nlm::Novelty novelty;
    
    const char* name = novelty.getName();
    if (!name || std::string(name) != "Novelty") {
        throw nlm::NLMTestException("Novelty creation failed: name should be 'Novelty'");
    }
    
    std::cout << "    testNoveltyCreation passed" << std::endl;
}

void testNoveltyName() {
    nlm::Novelty novelty;
    
    const char* name = novelty.getName();
    if (!name) {
        throw nlm::NLMTestException("Novelty name failed: name is null");
    }
    if (std::string(name) != "Novelty") {
        throw nlm::NLMTestException("Novelty name failed: expected 'Novelty', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testNoveltyName passed" << std::endl;
}

void testNoveltyUpdate() {
    nlm::Novelty novelty;
    
    float initialLevel = novelty.getLevel();
    
    // Update with positive input
    novelty.update(0.5f);
    
    float newLevel = novelty.getLevel();
    
    // Level should have increased
    if (newLevel <= initialLevel) {
        throw nlm::NLMTestException("Novelty update test failed: level should increase with positive input, " + 
                                   "initial: " + std::to_string(initialLevel) + ", new: " + 
                                   std::to_string(newLevel));
    }
    
    std::cout << "    testNoveltyUpdate passed (level: " << initialLevel << " -> " << newLevel << ")" << std::endl;
}

void testNoveltyZeroInput() {
    nlm::Novelty novelty;
    
    float initialLevel = novelty.getLevel();
    novelty.update(0.0f); // Zero input
    float newLevel = novelty.getLevel();
    
    // Level should have decayed towards zero
    if (newLevel > initialLevel * 1.1f) {
        throw nlm::NLMTestException("Novelty zero input test failed: level should decay with zero input, " + 
                                   "initial: " + std::to_string(initialLevel) + ", new: " + 
                                   std::to_string(newLevel));
    }
    
    std::cout << "    testNoveltyZeroInput passed (level: " << initialLevel << " -> " << newLevel << ")" << std::endl;
}

void testNoveltyReset() {
    nlm::Novelty novelty;
    
    // Set some level
    novelty.update(0.8f);
    
    float nonZeroLevel = novelty.getLevel();
    if (nonZeroLevel <= 0.0f) {
        throw nlm::NLMTestException("Novelty reset test failed: level should be non-zero before reset");
    }
    
    // Reset
    novelty.reset();
    
    float resetLevel = novelty.getLevel();
    if (resetLevel != 0.0f) {
        throw nlm::NLMTestException("Novelty reset test failed: level should be 0.0f after reset, got " + 
                                   std::to_string(resetLevel));
    }
    
    std::cout << "    testNoveltyReset passed (level reset from " << nonZeroLevel << " to 0.0f)" << std::endl;
}

void testNoveltyDecay() {
    nlm::Novelty novelty;
    
    // Set high level
    novelty.update(1.0f);
    
    float initialLevel = novelty.getLevel();
    
    // Wait (decay over time)
    novelty.update(0.0f); // Zero input - should decay
    float levelAfterOneStep = novelty.getLevel();
    
    // Level should have decayed
    if (levelAfterOneStep >= initialLevel) {
        throw nlm::NLMTestException("Novelty decay test failed: level should decay over time, " + 
                                   "initial: " + std::to_string(initialLevel) + ", after step: " + 
                                   std::to_string(levelAfterOneStep));
    }
    
    std::cout << "    testNoveltyDecay passed (level: " << initialLevel << " -> " << levelAfterOneStep << ")" << std::endl;
}

void testNoveltyTimeDependency() {
    nlm::Novelty novelty;
    
    // Reset for clean test
    novelty.reset();
    
    float initialLevel = novelty.getLevel();
    
    // Update with input
    novelty.update(0.5f);
    float levelAfterUpdate = novelty.getLevel();
    
    // Reset and update with input (should be the same without decay in between)
    novelty.reset();
    novelty.update(0.5f);
    float levelAfterReset = novelty.getLevel();
    
    // Levels should be very close (decay is small)
    if (std::abs(levelAfterUpdate - levelAfterReset) > 0.01f) {
        throw nlm::NLMTestException("Novelty time dependency test failed: levels should be close when reset between updates, " + 
                                   "update: " + std::to_string(levelAfterUpdate) + ", reset: " + 
                                   std::to_string(levelAfterReset));
    }
    
    std::cout << "    testNoveltyTimeDependency passed (update: " << levelAfterUpdate << ", reset: " << levelAfterReset << ")" << std::endl;
}

void testNoveltyMaxLevel() {
    nlm::Novelty novelty;
    
    // Set very high input
    novelty.update(1.0f);
    
    float level = novelty.getLevel();
    
    // Level should be reasonable (not infinite)
    if (level > 1.0f) {
        throw nlm::NLMTestException("Novelty max level test failed: level should not exceed 1.0, got " + 
                                   std::to_string(level));
    }
    
    std::cout << "    testNoveltyMaxLevel passed (level: " << level << ")" << std::endl;
}

void testNoveltyIntegration() {
    // Test integration with Curiosity system conceptually
    nlm::Novelty novelty;
    
    // Get level for integration check
    float level = novelty.getLevel();
    
    // Update
    novelty.update(0.3f);
    
    float newLevel = novelty.getLevel();
    
    if (newLevel <= level) {
        throw nlm::NLMTestException("Novelty integration test failed: level should increase with input, " + 
                                   "initial: " + std::to_string(level) + ", new: " + 
                                   std::to_string(newLevel));
    }
    
    std::cout << "    testNoveltyIntegration passed (level: " << level << " -> " << newLevel << ")" << std::endl;
}

void runAll() {
    std::cout << "Running Novelty tests..." << std::endl;
    testNoveltyCreation();
    testNoveltyName();
    testNoveltyUpdate();
    testNoveltyZeroInput();
    testNoveltyReset();
    testNoveltyDecay();
    testNoveltyTimeDependency();
    testNoveltyMaxLevel();
    testNoveltyIntegration();
}

} // namespace test_novelty
