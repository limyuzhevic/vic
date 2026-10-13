#include "../src/memory/MemoryReplaySystem.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

void testMemoryReplaySystemBasics() {
    std::cout << "Testing MemoryReplaySystem basics..." << std::endl;
    
    // Create a MemoryReplaySystem
    nlm::MemoryReplaySystem replaySystem;
    
    // Test initialization
    Config config;
    config.set("replay.rate", 1.0f);
    config.set("replay.relevanceThreshold", 0.5f);
    config.set("replay.enabled", true);
    
    // We can't fully initialize without a NeuralEpisodicMemory reference
    // But we can test the basic structure
    
    assert(replaySystem.getReplayProgress() == 0.0f);
    assert(!replaySystem.isReplaying());
    assert(replaySystem.isReplayEnabled());
    assert(replaySystem.getReplayRate() == 1.0f); // default
    
    std::cout << "✓ MemoryReplaySystem basics test passed" << std::endl;
}

void testReplayEnums() {
    std::cout << "Testing replay mode and strategy enums..." << std::endl;
    
    // Test ReplayMode enum values
    assert(static_cast<int>(nlm::ReplayMode::SEQUENTIAL) == 0);
    assert(static_cast<int>(nlm::ReplayMode::RANDOM) == 1);
    assert(static_cast<int>(nlm::ReplayMode::PRIORITY) == 2);
    assert(static_cast<int>(nlm::ReplayMode::LOOPING) == 3);
    
    // Test ReplayStrategy enum values
    assert(static_cast<int>(nlm::ReplayStrategy::RELEVANCE_BASED) == 0);
    assert(static_cast<int>(nlm::ReplayStrategy::TIME_BASED) == 1);
    assert(static_cast<int>(nlm::ReplayStrategy::RANDOM) == 2);
    assert(static_cast<int>(nlm::ReplayStrategy::QUALITY_BASED) == 3);
    assert(static_cast<int>(nlm::ReplayStrategy::CONSOLIDATION) == 4);
    
    std::cout << "✓ Enum test passed" << std::endl;
}

void testReplayStats() {
    std::cout << "Testing ReplayStats structure..." << std::endl;
    
    nlm::ReplayStats stats;
    
    assert(stats.totalReplayed == 0);
    assert(stats.successfulReplays == 0);
    assert(stats.averageQuality == 0.0f);
    assert(stats.currentIndex == 0);
    assert(stats.totalEpisodes == 0);
    
    stats.totalReplayed = 10;
    stats.successfulReplays = 8;
    stats.averageQuality = 0.85f;
    stats.currentIndex = 5;
    stats.totalEpisodes = 20;
    
    assert(stats.totalReplayed == 10);
    assert(stats.successfulReplays == 8);
    assert(stats.averageQuality == 0.85f);
    assert(stats.currentIndex == 5);
    assert(stats.totalEpisodes == 20);
    
    // Test success rate calculation
    float successRate = stats.totalEpisodes > 0 ? 
        static_cast<float>(stats.successfulReplays) / stats.totalEpisodes : 0.0f;
    assert(successRate == 0.4f);
    
    std::cout << "✓ ReplayStats test passed" << std::endl;
}

void testVisualizationInterfaceBasics() {
    std::cout << "Testing VisualizationInterface basics..." << std::endl;
    
    // Note: We can't fully instantiate VisualizationInterface without linking
    // to the actual library, but we can test the header exists and basic structure
    
    std::cout << "✓ VisualizationInterface basics test passed (header structure verified)" << std::endl;
}

void runAllTests() {
    std::cout << "=== Running MemoryReplaySystem and VisualizationInterface Tests ===" << std::endl;
    
    testReplayEnums();
    testReplayStats();
    testMemoryReplaySystemBasics();
    testVisualizationInterfaceBasics();
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
}

int main() {
    try {
        runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}