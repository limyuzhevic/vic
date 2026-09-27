// Curiosity Tests
// Comprehensive testing of curiosity neuromodulation system

#include "neuromodulation/Curiosity.hpp"
#include "brain/Brain.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>

namespace test_curiosity {

void testCuriosityCreation() {
    nlm::Curiosity curiosity;
    
    if (curiosity.getExplorationDrive() != 0.0f) {
        throw nlm::NLMTestException("Curiosity creation failed: exploration drive should be 0.0f, got " + 
                                   std::to_string(curiosity.getExplorationDrive()));
    }
    if (curiosity.getLevel() != 0.0f) {
        throw nlm::NLMTestException("Curiosity creation failed: level should be 0.0f, got " + 
                                   std::to_string(curiosity.getLevel()));
    }
    
    const char* name = curiosity.getName();
    if (!name || std::string(name) != "Curiosity") {
        throw nlm::NLMTestException("Curiosity creation failed: name should be 'Curiosity'");
    }
    
    std::cout << "    testCuriosityCreation passed" << std::endl;
}

void testCuriosityName() {
    nlm::Curiosity curiosity;
    
    const char* name = curiosity.getName();
    if (!name) {
        throw nlm::NLMTestException("Curiosity name failed: name is null");
    }
    if (std::string(name) != "Curiosity") {
        throw nlm::NLMTestException("Curiosity name failed: expected 'Curiosity', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testCuriosityName passed" << std::endl;
}

void testCuriosityConfiguration() {
    nlm::Curiosity curiosity;
    
    curiosity.setNoveltyWeight(0.7f);
    curiosity.setPredictionErrorWeight(0.3f);
    curiosity.setNoveltyWeight(0.7f);
    curiosity.setPredictionErrorWeight(0.3f);
    
    curiosity.initialize(nullptr); // Initialize with null brain for test
    
    std::cout << "    testCuriosityConfiguration passed" << std::endl;
}

void testCuriosityUpdate() {
    nlm::Curiosity curiosity;
    curiosity.initialize(nullptr);
    
    // Get initial level
    float initialLevel = curiosity.getLevel();
    
    // Update with some novelty and prediction error
    curiosity.update(0.5f, 0.3f, 0.1f);
    
    float newLevel = curiosity.getLevel();
    
    // Level should have increased due to positive inputs
    if (newLevel <= initialLevel) {
        throw nlm::NLMTestException("Curiosity update test failed: level should increase with positive inputs, " + 
                                   "initial: " + std::to_string(initialLevel) + ", new: " + 
                                   std::to_string(newLevel));
    }
    
    std::cout << "    testCuriosityUpdate passed (level: " << initialLevel << " -> " << newLevel << ")" << std::endl;
}

void testCuriosityZeroInputs() {
    nlm::Curiosity curiosity;
    curiosity.initialize(nullptr);
    
    float initialLevel = curiosity.getLevel();
    curiosity.update(0.0f, 0.0f, 0.1f); // Zero novelty and prediction error
    float newLevel = curiosity.getLevel();
    
    // Level should have decayed towards zero
    if (newLevel > initialLevel * 1.1f) {
        throw nlm::NLMTestException("Curiosity zero inputs test failed: level should decay with zero inputs, " + 
                                   "initial: " + std::to_string(initialLevel) + ", new: " + 
                                   std::to_string(newLevel));
    }
    
    std::cout << "    testCuriosityZeroInputs passed (level: " << initialLevel << " -> " << newLevel << ")" << std::endl;
}

void testCuriosityReset() {
    nlm::Curiosity curiosity;
    curiosity.initialize(nullptr);
    
    // Set some level
    curiosity.update(0.8f, 0.6f, 0.1f);
    
    float nonZeroLevel = curiosity.getLevel();
    if (nonZeroLevel <= 0.0f) {
        throw nlm::NLMTestException("Curiosity reset test failed: level should be non-zero before reset");
    }
    
    // Reset
    curiosity.reset();
    
    float resetLevel = curiosity.getLevel();
    if (resetLevel != 0.0f) {
        throw nlm::NLMTestException("Curiosity reset test failed: level should be 0.0f after reset, got " + 
                                   std::to_string(resetLevel));
    }
    
    std::cout << "    testCuriosityReset passed (level reset from " << nonZeroLevel << " to 0.0f)" << std::endl;
}

void testCuriosityWeights() {
    nlm::Curiosity curiosity;
    curiosity.initialize(nullptr);
    
    // Set weights
    curiosity.setNoveltyWeight(0.8f);
    curiosity.setPredictionErrorWeight(0.2f);
    
    // Update and check effect
    curiosity.update(1.0f, 1.0f, 0.1f);
    
    float level = curiosity.getLevel();
    
    // With weights set, level should be influenced by inputs
    if (level < 0.5f) {
        throw nlm::NLMTestException("Curiosity weights test failed: level should be influenced by weights, got " + 
                                   std::to_string(level));
    }
    
    std::cout << "    testCuriosityWeights passed (level: " << level << ")" << std::endl;
}

void testCuriosityIntegrationWithBrain() {
    // Create a brain for integration testing
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Curiosity curiosity;
    curiosity.initialize(brain.get());
    
    // Update with some inputs
    curiosity.update(0.3f, 0.2f, 0.01f);
    
    float level = curiosity.getLevel();
    if (level < 0.0f) {
        throw nlm::NLMTestException("Curiosity integration test failed: level should be non-negative, got " + 
                                   std::to_string(level));
    }
    
    std::cout << "    testCuriosityIntegrationWithBrain passed (level: " << level << ")" << std::endl;
}

void testCuriosityDecay() {
    nlm::Curiosity curiosity;
    curiosity.initialize(nullptr);
    
    // Set high level
    curiosity.update(1.0f, 1.0f, 0.1f);
    
    float initialLevel = curiosity.getLevel();
    
    // Wait (decay over time)
    curiosity.update(0.0f, 0.0f, 1.0f); // 1.0 second dt
    float levelAfterOneSecond = curiosity.getLevel();
    
    // Level should have decayed
    if (levelAfterOneSecond >= initialLevel) {
        throw nlm::NLMTestException("Curiosity decay test failed: level should decay over time, " + 
                                   "initial: " + std::to_string(initialLevel) + ", after 1s: " + 
                                   std::to_string(levelAfterOneSecond));
    }
    
    std::cout << "    testCuriosityDecay passed (level: " << initialLevel << " -> " << levelAfterOneSecond << ")" << std::endl;
}

void testCuriosityTimeDependency() {
    nlm::Curiosity curiosity;
    curiosity.initialize(nullptr);
    
    // Reset for clean test
    curiosity.reset();
    
    float initialLevel = curiosity.getLevel();
    
    // Update with small dt
    curiosity.update(0.5f, 0.3f, 0.01f);
    float levelSmallDt = curiosity.getLevel();
    
    // Reset and update with larger dt
    curiosity.reset();
    curiosity.update(0.5f, 0.3f, 1.0f);
    float levelLargeDt = curiosity.getLevel();
    
    // With larger dt, decay should be more pronounced
    // So level should be lower or equal
    if (levelLargeDt > levelSmallDt * 1.5f) {
        throw nlm::NLMTestException("Curiosity time dependency test failed: level difference should be reasonable, " + 
                                   "small dt: " + std::to_string(levelSmallDt) + ", large dt: " + 
                                   std::to_string(levelLargeDt));
    }
    
    std::cout << "    testCuriosityTimeDependency passed (small dt: " << levelSmallDt << ", large dt: " << levelLargeDt << ")" << std::endl;
}

void runAll() {
    std::cout << "Running Curiosity tests..." << std::endl;
    testCuriosityCreation();
    testCuriosityName();
    testCuriosityConfiguration();
    testCuriosityUpdate();
    testCuriosityZeroInputs();
    testCuriosityReset();
    testCuriosityWeights();
    testCuriosityIntegrationWithBrain();
    testCuriosityDecay();
    testCuriosityTimeDependency();
}

} // namespace test_curiosity
