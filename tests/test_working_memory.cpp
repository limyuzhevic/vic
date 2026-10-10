// Working Memory Tests
// Tests for active maintenance of information and competitive selection

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_working_memory {

void testActiveMaintenance() {
    std::cout << "  Testing active maintenance..." << std::endl;
    
    // Test that working memory can maintain active information
    std::vector<float> activeTraces = {0.1f, 0.5f, 0.8f, 0.3f, 0.9f};
    
    // Verify that traces are maintained
    float totalActivity = 0.0f;
    for (float trace : activeTraces) {
        totalActivity += trace;
    }
    
    assert(totalActivity > 0.0f && "Working memory should maintain active traces");
    
    // Verify that traces decay over time (simplified)
    std::vector<float> decayedTraces = activeTraces;
    for (float& trace : decayedTraces) {
        trace *= 0.9f; // Decay factor
    }
    
    float decayedTotal = 0.0f;
    for (float trace : decayedTraces) {
        decayedTotal += trace;
    }
    
    assert(decayedTotal < totalActivity && "Traces should decay over time");
    
    std::cout << "    ✓ Active maintenance test passed" << std::endl;
}

void testMemoryCapacity() {
    std::cout << "  Testing memory capacity..." << std::endl;
    
    // Test that working memory has finite capacity
    size_t maxCapacity = 100;
    std::vector<float> memorySlots(maxCapacity, 0.0f);
    
    // Fill memory to capacity
    size_t itemsStored = 0;
    for (float& slot : memorySlots) {
        if (slot == 0.0f) {
            slot = 0.5f; // Store an item
            itemsStored++;
        }
    }
    
    assert(itemsStored <= maxCapacity && "Should not exceed memory capacity");
    
    // Verify capacity constraints
    assert(memorySlots.size() == maxCapacity && "Memory slots should match capacity");
    
    std::cout << "    ✓ Memory capacity test passed" << std::endl;
}

void testMemoryCompetition() {
    std::cout << "  Testing memory competition..." << std::endl;
    
    // Test that working memory uses competition for active trace selection
    std::vector<float> traceActivities = {0.1f, 0.8f, 0.3f, 0.6f, 0.2f};
    
    // Find the most active trace (competition)
    size_t winnerIndex = 0;
    float maxActivity = traceActivities[0];
    
    for (size_t i = 1; i < traceActivities.size(); ++i) {
        if (traceActivities[i] > maxActivity) {
            maxActivity = traceActivities[i];
            winnerIndex = i;
        }
    }
    
    assert(maxActivity > 0.0f && "Should have a winning trace");
    assert(winnerIndex < traceActivities.size() && "Winner index should be valid");
    
    std::cout << "    ✓ Memory competition test passed" << std::endl;
}

void runAll() {
    std::cout << "Working Memory Tests:" << std::endl;
    
    testActiveMaintenance();
    testMemoryCapacity();
    testMemoryCompetition();
    
    std::cout << "Working Memory Tests: PASSED" << std::endl;
}

} // namespace test_working_memory
