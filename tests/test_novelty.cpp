// Novelty Tests
#include "brain/Novelty.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_novelty {

void testNoveltyCreation() {
    nlm::Novelty novelty;
    
    float level = novelty.getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    std::cout << "    testNoveltyCreation passed" << std::endl;
}

void testNoveltyConfiguration() {
    nlm::Novelty novelty;
    
    // Test initial level
    float level = novelty.getLevel();
    assert(level == 0.0f);
    
    // Set level
    novelty.setLevel(0.7f);
    assert(std::abs(novelty.getLevel() - 0.7f) < 0.001f);
    
    // Test boundary values
    novelty.setLevel(0.0f);
    assert(novelty.getLevel() == 0.0f);
    
    novelty.setLevel(1.0f);
    assert(novelty.getLevel() == 1.0f);
    
    std::cout << "    testNoveltyConfiguration passed" << std::endl;
}

void testNoveltyDetection() {
    nlm::Novelty novelty;
    
    // Test novelty detection with patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> pattern2 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};  // Very different
    
    novelty.detectNovelty(pattern1, pattern2);
    
    float level = novelty.getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    // Similar patterns should produce lower novelty
    std::vector<float> pattern3 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};  // Same as pattern1
    novelty.detectNovelty(pattern1, pattern3);
    
    float similarLevel = novelty.getLevel();
    assert(similarLevel >= 0.0f && similarLevel <= 1.0f);
    
    std::cout << "    testNoveltyDetection passed" << std::endl;
}

void testNoveltyUpdate() {
    nlm::Novelty novelty;
    
    // Set some novelty level
    novelty.setLevel(0.8f);
    
    // Update with different time steps
    novelty.update(0.001);
    novelty.update(0.01);
    novelty.update(0.1);
    
    // Level should have decayed
    float level = novelty.getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    std::cout << "    testNoveltyUpdate passed" << std::endl;
}

void testNoveltyHistory() {
    nlm::Novelty novelty;
    
    // Initial history should be empty
    const std::vector<float>& history = novelty.getHistory();
    assert(history.empty());
    
    // Set some novelty
    novelty.setLevel(0.5f);
    
    // Update with pattern detection
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.9f, 0.8f, 0.7f};
    novelty.detectNovelty(pattern1, pattern2);
    
    const std::vector<float>& history2 = novelty.getHistory();
    assert(!history2.empty());
    
    // Clear history
    novelty.clearHistory();
    const std::vector<float>& history3 = novelty.getHistory();
    assert(history3.empty());
    
    std::cout << "    testNoveltyHistory passed" << std::endl;
}

void runAll() {
    std::cout << "Running Novelty tests..." << std::endl;
    testNoveltyCreation();
    testNoveltyConfiguration();
    testNoveltyDetection();
    testNoveltyUpdate();
    testNoveltyHistory();
}

} // namespace test_novelty