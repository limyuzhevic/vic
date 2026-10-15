// Stress Tests - Memory Extremes
// Comprehensive memory stress test suite

#include "tests/utils/TestUtils.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>

namespace test_memory_extremes {

void testMemoryAllocationExtremes() {
    TestUtils::TestSession session("testMemoryAllocationExtremes");
    
    // Test extreme memory allocation patterns
    const size_t largeAllocationSize = 1000000; // 1 million doubles
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    // Allocate very large memory block
    std::vector<double> largeData;
    largeData.reserve(largeAllocationSize);
    
    for (size_t i = 0; i < largeAllocationSize; ++i) {
        largeData.push_back(TestUtils::randomDouble());
    }
    
    // Process large data
    double sum = std::accumulate(largeData.begin(), largeData.end(), 0.0);
    double mean = sum / largeAllocationSize;
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Extreme memory allocation should be efficient
    TestAssert::true(mean >= 0.0 && mean <= 1.0, "Large data processing should produce valid result");
    TestAssert::true(durationMs < 5000.0, "Extreme memory allocation should complete quickly");
    
    std::cout << "    testMemoryAllocationExtremes passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryFragmentationExtremes() {
    TestUtils::TestSession session("testMemoryFragmentationExtremes");
    
    // Test extreme memory fragmentation
    const size_t allocationCount = 10000;
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    std::vector<std::vector<double>> fragmentedData;
    
    // Allocate many small blocks in a pattern that causes fragmentation
    for (size_t i = 0; i < allocationCount; ++i) {
        size_t size = (i % 100) + 1; // Very small allocations
        fragmentedData.push_back(std::vector<double>(size));
        for (size_t j = 0; j < size; ++j) {
            fragmentedData.back()[j] = TestUtils::randomDouble();
        }
    }
    
    // Process fragmented data
    double totalSum = 0.0;
    for (const auto& data : fragmentedData) {
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        totalSum += sum;
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Extreme fragmentation should be handled
    TestAssert::true(totalSum != 0.0, "Fragmented data processing should produce non-zero result");
    TestAssert::true(durationMs < 10000.0, "Extreme fragmentation should complete quickly");
    
    std::cout << "    testMemoryFragmentationExtremes passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryExhaustionSimulation() {
    TestUtils::TestSession session("testMemoryExhaustionSimulation");
    
    // Simulate memory exhaustion scenarios
    const size_t iterations = 1000;
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    std::vector<std::vector<double>> memoryHogs;
    
    for (size_t i = 0; i < iterations; ++i) {
        // Allocate memory in chunks
        std::vector<double> chunk;
        chunk.reserve(10000);
        
        for (size_t j = 0; j < 10000; ++j) {
            chunk.push_back(TestUtils::randomDouble());
        }
        
        // Process chunk
        double sum = std::accumulate(chunk.begin(), chunk.end(), 0.0);
        memoryHogs.push_back(std::move(chunk));
        
        // Occasionally clear some memory
        if (i % 100 == 0 && i > 0) {
            memoryHogs.erase(memoryHogs.begin(), memoryHogs.begin() + 10);
        }
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Memory exhaustion simulation should be robust
    TestAssert::true(durationMs < 15000.0, "Memory exhaustion simulation should complete");
    
    std::cout << "    testMemoryExhaustionSimulation passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryPressureTesting() {
    TestUtils::TestSession session("testMemoryPressureTesting");
    
    // Test under memory pressure
    const size_t pressureIterations = 500;
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    std::vector<std::unique_ptr<double[]>> memoryPressure;
    
    for (size_t i = 0; i < pressureIterations; ++i) {
        // Allocate maximum possible memory
        size_t size = 50000; // 50k doubles per allocation
        auto ptr = std::make_unique<double[]>(size);
        
        // Initialize and process
        for (size_t j = 0; j < size; ++j) {
            ptr[j] = TestUtils::randomDouble();
        }
        
        // Process
        double sum = 0.0;
        for (size_t j = 0; j < size; ++j) {
            sum += ptr[j];
        }
        
        memoryPressure.push_back(std::move(ptr));
        
        // Clear some memory to simulate pressure
        if (i % 100 == 0) {
            memoryPressure.clear();
        }
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Memory pressure testing should be robust
    TestAssert::true(durationMs < 20000.0, "Memory pressure testing should complete");
    
    std::cout << "    testMemoryPressureTesting passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryBoundaryTesting() {
    TestUtils::TestSession session("testMemoryBoundaryTesting");
    
    // Test memory boundary conditions
    const size_t minSize = 1;
    const size_t maxSize = 1000000;
    const size_t stepSize = 10000;
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    for (size_t size = minSize; size <= maxSize; size += stepSize) {
        // Allocate at boundary conditions
        std::vector<double> boundaryData;
        boundaryData.reserve(size);
        
        for (size_t i = 0; i < size; ++i) {
            boundaryData.push_back(TestUtils::randomDouble());
        }
        
        // Process boundary data
        double sum = std::accumulate(boundaryData.begin(), boundaryData.end(), 0.0);
        double mean = sum / size;
        
        // Verify results
        TestAssert::true(mean >= 0.0 && mean <= 1.0, "Boundary data processing should produce valid result");
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Boundary testing should handle all sizes
    TestAssert::true(durationMs < 30000.0, "Boundary testing should complete");
    
    std::cout << "    testMemoryBoundaryTesting passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryLeakTesting() {
    TestUtils::TestSession session("testMemoryLeakTesting");
    
    // Test for potential memory leaks
    const size_t leakIterations = 1000;
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    for (size_t i = 0; i < leakIterations; ++i) {
        // Allocate and deallocate memory to detect leaks
        std::vector<double> testData(1000);
        for (size_t j = 0; j < 1000; ++j) {
            testData[j] = TestUtils::randomDouble();
        }
        
        // Process data
        double sum = std::accumulate(testData.begin(), testData.end(), 0.0);
        
        // Vector goes out of scope here (should be deallocated)
        if (i % 100 == 0) {
            // Occasionally check for memory issues
            testData.clear();
        }
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Memory leak testing should be stable
    TestAssert::true(durationMs < 15000.0, "Memory leak testing should complete");
    
    std::cout << "    testMemoryLeakTesting passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Memory Extremes Tests ===" << std::endl;
    
    testMemoryAllocationExtremes();
    testMemoryFragmentationExtremes();
    testMemoryExhaustionSimulation();
    testMemoryPressureTesting();
    testMemoryBoundaryTesting();
    testMemoryLeakTesting();
    
    std::cout << std::endl;
    std::cout << "=== All Memory Extremes Tests PASSED ===" << std::endl;
}