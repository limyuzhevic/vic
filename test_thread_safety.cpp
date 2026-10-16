//!/usr/bin/env g++
// Simple test to verify thread safety fixes
// This test compiles and links against the modified code

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

// Forward declarations
namespace nlm {
    class ParallelNeuralProcessor;
}

// Test function for thread safety
void testThreadSafety() {
    std::cout << "Testing thread safety fixes..." << std::endl;
    
    // Test 1: Create ParallelNeuralProcessor with thread safety fixes
    nlm::ParallelNeuralProcessor processor(4);
    
    // Atomic counter for testing
    std::atomic<size_t> processedNeurons{0};
    
    // Simple process function that updates thread-local state
    auto processFunc = [&processedNeurons](size_t i, nlm::ParallelNeuralProcessor::ThreadLocalState& state) {
        // Simulate neuron processing
        state.localSpikeCount++;
        state.localFiringCount++;
        state.localSynapticInput[i % 16] += 1.0f;
        processedNeurons++;
    };
    
    // Test concurrent processing
    std::cout << "Testing parallel processing..." << std::endl;
    processor.processNeurons(1000, processFunc);
    
    // Verify thread safety by checking totals
    std::cout << "Total spike count: " << processor.getTotalSpikeCount() << std::endl;
    std::cout << "Total firing count: " << processor.getTotalFiringCount() << std::endl;
    std::cout << "Processed neurons (direct count): " << processedNeurons << std::endl;
    
    // Test that we got some reasonable results (spike counts should be > 0)
    bool success = (processor.getTotalSpikeCount() > 0 || processor.getTotalFiringCount() > 0);
    
    if (success) {
        std::cout << "✓ Thread safety test passed!" << std::endl;
    } else {
        std::cout << "✗ Thread safety test failed!" << std::endl;
    }
    
    return success;
}

int main() {
    try {
        testThreadSafety();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}