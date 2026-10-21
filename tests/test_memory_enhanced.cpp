// MemoryEnhanced Tests
// Comprehensive tests for enhanced memory systems

#include "memory/MemoryEnhanced.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_set>

namespace test_memory_enhanced {

void testMemoryEnhancedCreation() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    assert(memory.getCapacity() == 100);
    assert(memory.getDecayRate() == 0.01f);
    assert(memory.getMemoryActivity() == 0.0f);
    assert(memory.getActiveTraces() == 0);
    
    std::cout << "    testMemoryEnhancedCreation passed" << std::endl;
}

void testMemoryEnhancedConfiguration() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    memory.setCapacity(200);
    memory.setDecayRate(0.005f);
    memory.setDecayTimeConstant(2000.0f);
    
    assert(memory.getCapacity() == 200);
    assert(memory.getDecayRate() == 0.005f);
    assert(memory.getDecayTimeConstant() == 2000.0f);
    
    std::cout << "    testMemoryEnhancedConfiguration passed" << std::endl;
}

void testMemoryEnhancedStoreRetrieve() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    // Create test pattern
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    
    memory.store(pattern, 1.0f);
    
    // Retrieve with perfect match
    std::vector<float> retrieved = memory.retrieve(nullptr);
    
    assert(retrieved.size() == pattern.size());
    assert(std::abs(retrieved[0] - pattern[0]) < 0.01f);
    assert(std::abs(retrieved[1] - pattern[1]) < 0.01f);
    assert(std::abs(retrieved[2] - pattern[2]) < 0.01f);
    
    std::cout << "    testMemoryEnhancedStoreRetrieve passed" << std::endl;
}

void testMemoryEnhancedPartialCue() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    // Store full pattern
    std::vector<float> fullPattern = {0.0f, 0.3f, 0.6f, 0.9f, 1.0f};
    memory.store(fullPattern, 1.0f);
    
    // Retrieve with partial cue (first 3 elements)
    std::vector<float> partialCue = {0.0f, 0.2f, 0.5f};
    std::vector<float> retrieved = memory.retrieve(&partialCue);
    
    // Should have completed the pattern
    assert(retrieved.size() == fullPattern.size());
    assert(std::abs(retrieved[0] - 0.0f) < 0.1f);  // Cue match
    assert(std::abs(retrieved[1] - 0.3f) < 0.1f);  // Cue match
    assert(std::abs(retrieved[2] - 0.6f) < 0.1f);  // Cue match
    assert(std::abs(retrieved[3] - 0.9f) < 0.1f);  // Completion
    assert(std::abs(retrieved[4] - 1.0f) < 0.1f);  // Completion
    
    std::cout << "    testMemoryEnhancedPartialCue passed" << std::endl;
}

void testMemoryEnhancedCapacity() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    memory.setCapacity(2);
    assert(memory.getCapacity() == 2);
    
    std::cout << "    testMemoryEnhancedCapacity passed" << std::endl;
}

void testMemoryEnhancedDecay() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    std::vector<float> pattern = {0.5f, 0.8f, 0.3f};
    memory.store(pattern, 1.0f);
    
    float initialActivity = memory.getMemoryActivity();
    assert(initialActivity > 0.0f);
    
    // Apply decay over time
    memory.update(100.0f, false);  // 100 time units
    
    float decayedActivity = memory.getMemoryActivity();
    assert(decayedActivity <= initialActivity);  // Should decay
    
    std::cout << "    testMemoryEnhancedDecay passed" << std::endl;
}

void testMemoryEnhancedInterference() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    std::vector<float> pattern1 = {1.0f, 0.0f, 0.0f};
    std::vector<float> pattern2 = {0.0f, 1.0f, 0.0f};
    
    memory.store(pattern1, 1.0f);
    memory.store(pattern2, 1.0f);
    
    // Register interference between patterns
    memory.registerInterference(NeuronId(1), NeuronId(2));
    
    // Store another pattern that interferes
    std::vector<float> pattern3 = {0.5f, 0.5f, 0.0f};
    memory.store(pattern3, 1.0f);
    
    // Update with current activity
    std::vector<float> currentActivity = {0.6f, 0.6f, 0.0f};
    memory.resolveInterference(currentActivity);
    
    std::cout << "    testMemoryEnhancedInterference passed" << std::endl;
}

void testMemoryEnhancedCompetition() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    std::vector<float> pattern1 = {0.8f, 0.2f, 0.1f};
    std::vector<float> pattern2 = {0.3f, 0.7f, 0.2f};
    std::vector<float> pattern3 = {0.1f, 0.1f, 0.8f};
    
    memory.store(pattern1, 1.0f);
    memory.store(pattern2, 1.0f);
    memory.store(pattern3, 1.0f);
    
    // Run advanced competition to determine winners
    memory.runAdvancedCompetition();
    
    // Check that some neurons are winning
    bool hasWinners = false;
    for (size_t i = 0; i < memory.getMemoryNeurons().size(); ++i) {
        if (memory.isWinning(memory.getMemoryNeurons()[i])) {
            hasWinners = true;
            break;
        }
    }
    
    assert(hasWinners);
    
    std::cout << "    testMemoryEnhancedCompetition passed" << std::endl;
}

void testMemoryEnhancedPatternSimilarity() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> pattern2 = {0.11f, 0.19f, 0.31f, 0.41f, 0.49f};  // Similar
    std::vector<float> pattern3 = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};  // Different
    
    memory.store(pattern1, 1.0f);
    memory.store(pattern2, 1.0f);
    memory.store(pattern3, 1.0f);
    
    float similarity12 = memory.getPatternSimilarity(pattern1, pattern2);
    float similarity13 = memory.getPatternSimilarity(pattern1, pattern3);
    float similarity22 = memory.getPatternSimilarity(pattern2, pattern2);
    
    assert(similarity12 > 0.9f);  // Should be very similar
    assert(similarity13 > 0.0f && similarity13 < 0.5f);  // Should be different
    assert(std::abs(similarity22 - 1.0f) < 0.01f);  // Perfect similarity with self
    
    std::cout << "    testMemoryEnhancedPatternSimilarity passed (sim12=" 
              << similarity12 << ", sim13=" << similarity13 << ")" << std::endl;
}

void testMemoryEnhancedChunking() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    // Store multiple similar patterns to test chunking
    std::vector<std::vector<float>> patterns = {
        {0.1f, 0.2f, 0.3f, 0.4f},
        {0.11f, 0.19f, 0.31f, 0.39f},
        {0.12f, 0.18f, 0.32f, 0.38f}
    };
    
    for (const auto& pattern : patterns) {
        memory.store(pattern, 1.0f);
    }
    
    memory.chunkMemoryTraces();
    
    // Get compressed memory
    std::vector<std::vector<float>> compressed = memory.getCompressedMemory();
    assert(compressed.size() > 0);
    
    std::cout << "    testMemoryEnhancedChunking passed (compressed into " 
              << compressed.size() << " chunks)" << std::endl;
}

void testMemoryEnhancedTemporalSequence() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    std::vector<std::vector<float>> sequence = {
        {0.1f, 0.2f, 0.3f},
        {0.2f, 0.3f, 0.4f},
        {0.3f, 0.4f, 0.5f}
    };
    
    memory.storeTemporalSequence(sequence, 1.0f);
    
    std::cout << "    testMemoryEnhancedTemporalSequence passed" << std::endl;
}

void testMemoryEnhancedSleepConsolidation() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    std::vector<float> pattern = {0.5f, 0.7f, 0.3f};
    memory.store(pattern, 1.0f);
    
    float initialActivity = memory.getMemoryActivity();
    
    // Process sleep rest
    memory.processSleepRest(false, 0.8f);
    memory.consolidateWorkingMemory(0.5f);
    
    float consolidatedActivity = memory.getMemoryActivity();
    assert(std::abs(consolidatedActivity - initialActivity) < 0.01f);
    
    std::cout << "    testMemoryEnhancedSleepConsolidation passed" << std::endl;
}

void testMemoryEnhancedStrengthening() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    std::vector<float> pattern = {0.3f, 0.5f, 0.7f};
    memory.store(pattern, 1.0f);
    
    float initialActivity = memory.getMemoryActivity();
    
    // Strengthen memory
    memory.strengthenMemory(1.5f);
    
    float strengthenedActivity = memory.getMemoryActivity();
    assert(strengthenedActivity > initialActivity);
    
    std::cout << "    testMemoryEnhancedStrengthening passed (activity: " 
              << initialActivity << " -> " << strengthenedActivity << ")" << std::endl;
}

void testMemoryEnhancedRecentActivity() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    std::vector<float> pattern = {0.6f, 0.7f, 0.8f};
    memory.store(pattern, 1.0f);
    
    // Get recent activity
    auto recent = memory.getRecentActivity(5);
    assert(!recent.empty());
    assert(recent.size() <= 5);
    
    std::cout << "    testMemoryEnhancedRecentActivity passed (recent entries: " 
              << recent.size() << ")" << std::endl;
}

void testMemoryEnhancedPatternCompletion() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    // Store a reference pattern
    std::vector<float> referencePattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    memory.store(referencePattern, 1.0f);
    
    // Create partial cue
    std::vector<float> partialCue = {0.1f, 0.15f, 0.25f};  // Partial match
    
    // Complete the pattern
    std::vector<float> completed = memory.completePattern(partialCue);
    
    assert(completed.size() == referencePattern.size());
    assert(std::abs(completed[0] - 0.1f) < 0.05f);
    assert(std::abs(completed[1] - 0.2f) < 0.05f);
    assert(std::abs(completed[2] - 0.3f) < 0.05f);
    assert(std::abs(completed[3] - 0.4f) < 0.05f);
    assert(std::abs(completed[4] - 0.5f) < 0.05f);
    
    std::cout << "    testMemoryEnhancedPatternCompletion passed" << std::endl;
}

void testMemoryEnhancedStrengthPattern() {
    nlm::NeuralWorkingMemoryEnhanced memory;
    
    std::vector<float> pattern = {0.2f, 0.4f, 0.6f, 0.8f};
    float strength = memory.getPatternStability(pattern);
    
    // Store the same pattern again with same strength
    memory.store(pattern, 1.0f);
    
    std::cout << "    testMemoryEnhancedStrengthPattern passed (stability: " 
              << strength << ")" << std::endl;
}

void runAll() {
    std::cout << "Running MemoryEnhanced tests..." << std::endl;
    testMemoryEnhancedCreation();
    testMemoryEnhancedConfiguration();
    testMemoryEnhancedStoreRetrieve();
    testMemoryEnhancedPartialCue();
    testMemoryEnhancedCapacity();
    testMemoryEnhancedDecay();
    testMemoryEnhancedInterference();
    testMemoryEnhancedCompetition();
    testMemoryEnhancedPatternSimilarity();
    testMemoryEnhancedChunking();
    testMemoryEnhancedTemporalSequence();
    testMemoryEnhancedSleepConsolidation();
    testMemoryEnhancedStrengthening();
    testMemoryEnhancedRecentActivity();
    testMemoryEnhancedPatternCompletion();
    testMemoryEnhancedStrengthPattern();
}

} // namespace test_memory_enhanced
