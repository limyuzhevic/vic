// Novelty Detection Tests
// Tests for difference detection from previous experiences

#include "neuromodulation/Novelty.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_novelty {

void testNoveltyCreation() {
    nlm::Novelty novelty;
    
    // Creation doesn't crash
    std::cout << "    testNoveltyCreation passed" << std::endl;
}

void testNoveltyLevel() {
    nlm::Novelty novelty;
    
    assert(novelty.getLevel() == 0.0f);
    
    novelty.setLevel(0.5f);
    assert(novelty.getLevel() == 0.5f);
    
    std::cout << "    testNoveltyLevel passed" << std::endl;
}

void testNoveltyInitialization() {
    nlm::Novelty novelty;
    
    // Initialize with null brain (placeholder)
    novelty.initialize(nullptr);
    
    std::cout << "    testNoveltyInitialization passed" << std::endl;
}

void testNoveltyDetection() {
    nlm::Novelty novelty;
    
    // Test novelty detection with same patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.1f, 0.2f, 0.3f};  // Same as pattern1
    
    novelty.detectNovelty(pattern1, pattern2);
    
    // Low novelty for identical patterns
    assert(novelty.getLevel() < 0.1f);
    
    std::cout << "    testNoveltyDetection passed" << std::endl;
}

void testNoveltyDifferentPatterns() {
    nlm::Novelty novelty;
    
    // Test novelty detection with different patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.9f, 0.8f, 0.7f};  // Different from pattern1
    
    novelty.detectNovelty(pattern1, pattern2);
    
    // High novelty for very different patterns
    assert(novelty.getLevel() > 0.5f);
    
    std::cout << "    testNoveltyDifferentPatterns passed" << std::endl;
}

void testNoveltyEmptyPatterns() {
    nlm::Novelty novelty;
    
    std::vector<float> pattern1 = {};
    std::vector<float> pattern2 = {1.0f};
    
    novelty.detectNovelty(pattern1, pattern2);
    
    // Empty or mismatched patterns result in low novelty
    assert(novelty.getLevel() == 0.0f);
    
    std::cout << "    testNoveltyEmptyPatterns passed" << std::endl;
}

void testNoveltyHistory() {
    nlm::Novelty novelty;
    
    std::vector<float> pattern1 = {0.1f, 0.2f};
    std::vector<float> pattern2 = {0.9f, 0.8f};
    
    novelty.detectNovelty(pattern1, pattern2);
    
    // Add to history
    std::vector<float> pattern3 = {0.5f, 0.5f};
    novelty.detectNovelty(pattern2, pattern3);
    
    // Check history
    const auto& history = novelty.getHistory();
    assert(history.size() >= 2);
    
    std::cout << "    testNoveltyHistory passed" << std::endl;
}

void testNoveltyUpdate() {
    nlm::Novelty novelty;
    
    novelty.setLevel(0.8f);
    
    // Update with decay
    novelty.update(0.1f);  // 100ms
    
    // Level should have decayed
    assert(novelty.getLevel() < 0.8f);
    
    std::cout << "    testNoveltyUpdate passed" << std::endl;
}

void testNoveltyClearHistory() {
    nlm::Novelty novelty;
    
    std::vector<float> pattern1 = {0.1f, 0.2f};
    std::vector<float> pattern2 = {0.9f, 0.8f};
    
    novelty.detectNovelty(pattern1, pattern2);
    
    assert(!novelty.getHistory().empty());
    
    novelty.clearHistory();
    assert(novelty.getHistory().empty());
    
    std::cout << "    testNoveltyClearHistory passed" << std::endl;
}

void runAll() {
    std::cout << "Running Novelty tests..." << std::endl;
    testNoveltyCreation();
    testNoveltyLevel();
    testNoveltyInitialization();
    testNoveltyDetection();
    testNoveltyDifferentPatterns();
    testNoveltyEmptyPatterns();
    testNoveltyHistory();
    testNoveltyUpdate();
    testNoveltyClearHistory();
}

} // namespace test_novelty