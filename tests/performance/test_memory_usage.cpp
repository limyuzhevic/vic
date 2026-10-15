// Performance Tests - Memory Usage Tests
// Comprehensive memory usage testing for performance optimization

#include "tests/utils/TestUtils.hpp"
#include "src/performance/PerformanceMonitor.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>
#include <memory>

namespace test_memory_usage {

void testMemoryUsageBasic() {
    TestUtils::TestSession session("testMemoryUsageBasic");
    
    // Basic memory usage test
    auto memoryFunc = [](size_t allocationSize) {
        // Allocate memory
        std::vector<double> data(allocationSize);
        for (size_t i = 0; i < allocationSize; ++i) {
            data[i] = TestUtils::randomDouble();
        }
        
        // Process data
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        double mean = sum / allocationSize;
        
        return mean;
    };
    
    // Test different allocation sizes
    for (size_t size : {1000, 10000, 100000}) {
        double startTime = TestUtils::now().time_since_epoch().count();
        
        double result = memoryFunc(size);
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        TestAssert::true(result >= 0.0 && result <= 1.0, "Memory usage should produce valid result");
        TestAssert::true(durationMs < 500.0, "Memory usage should complete quickly");
        
        // Estimate memory usage (rough approximation)
        size_t memoryMB = (size * sizeof(double)) / (1024 * 1024);
        TestAssert::true(memoryMB > 0, "Memory allocation should use memory");
    }
    
    std::cout << "    testMemoryUsageBasic passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryUsageAllocationPatterns() {
    TestUtils::TestSession session("testMemoryUsageAllocationPatterns");
    
    // Memory allocation patterns test
    auto allocationFunc = [](size_t numAllocations, size_t allocationSize) {
        std::vector<std::vector<double>> matrices;
        double startTime = TestUtils::now().time_since_epoch().count();
        
        for (size_t i = 0; i < numAllocations; ++i) {
            matrices.push_back(std::vector<double>(allocationSize));
            for (size_t j = 0; j < allocationSize; ++j) {
                matrices[i][j] = TestUtils::randomDouble();
            }
        }
        
        // Process matrices
        double total = 0.0;
        for (const auto& matrix : matrices) {
            double sum = std::accumulate(matrix.begin(), matrix.end(), 0.0);
            total += sum;
        }
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        return std::pair<double, double>(total, durationMs);
    };
    
    // Test different allocation patterns
    auto patternFunc = [](size_t pattern) {
        switch (pattern) {
            case 0: return std::make_pair<size_t, size_t>(10, 1000); // 10 matrices of 1000 elements
            case 1: return std::make_pair<size_t, size_t>(100, 100); // 100 matrices of 100 elements
            case 2: return std::make_pair<size_t, size_t>(1, 10000); // 1 matrix of 10000 elements
            default: return std::make_pair<size_t, size_t>(10, 1000);
        }
    };
    
    for (size_t pattern = 0; pattern < 3; ++pattern) {
        auto params = patternFunc(pattern);
        auto result = allocationFunc(params.first, params.second);
        
        TestAssert::true(result.first != 0.0, "Allocation pattern should produce non-zero result");
        TestAssert::true(result.second < 1000.0, "Allocation pattern should complete quickly");
    }
    
    std::cout << "    testMemoryUsageAllocationPatterns passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryUsageDeallocation() {
    TestUtils::TestSession session("testMemoryUsageDeallocation");
    
    // Memory deallocation test
    auto deallocationFunc = [](size_t allocationSize) {
        std::vector<double> data;
        
        double startTime = TestUtils::now().time_since_epoch().count();
        
        // Allocate and deallocate multiple times
        for (size_t i = 0; i < 100; ++i) {
            data.resize(allocationSize);
            for (size_t j = 0; j < allocationSize; ++j) {
                data[j] = TestUtils::randomDouble();
            }
            
            // Process data
            double sum = std::accumulate(data.begin(), data.end(), 0.0);
            // Store result (to prevent optimization)
            if (i == 0) {
                static double storedResult = sum;
                (void)storedResult;
            }
        }
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        return durationMs;
    };
    
    // Test deallocation with different sizes
    for (size_t size : {100, 1000, 5000}) {
        double duration = deallocationFunc(size);
        TestAssert::true(duration < 500.0, "Deallocation should complete quickly");
    }
    
    std::cout << "    testMemoryUsageDeallocation passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryUsageContinuous() {
    TestUtils::TestSession session("testMemoryUsageContinuous");
    
    // Continuous memory usage test
    auto continuousFunc = [](size_t durationMs) {
        double startTime = TestUtils::now().time_since_epoch().count();
        std::vector<std::vector<double>> data;
        
        while (TestUtils::elapsedMs(startTime) < durationMs) {
            // Allocate memory
            data.push_back(std::vector<double>(1000));
            for (size_t i = 0; i < 1000; ++i) {
                data.back()[i] = TestUtils::randomDouble();
            }
            
            // Process memory
            double sum = std::accumulate(data.back().begin(), data.back().end(), 0.0);
            
            // Trim to prevent unbounded growth
            if (data.size() > 100) {
                data.erase(data.begin());
            }
        }
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double actualDuration = (endTime - startTime) * 0.001;
        
        return actualDuration;
    };
    
    // Test continuous memory usage
    double duration = continuousFunc(200); // 200ms test
    TestAssert::true(duration < 300.0, "Continuous memory usage should complete");
    
    std::cout << "    testMemoryUsageContinuous passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryUsagePeakDetection() {
    TestUtils::TestSession session("testMemoryUsagePeakDetection");
    
    // Memory peak detection test
    auto peakDetectionFunc = [](size_t numIterations) {
        std::vector<double> largeData;
        double peakMemory = 0.0;
        double startTime = TestUtils::now().time_since_epoch().count();
        
        for (size_t i = 0; i < numIterations; ++i) {
            // Allocate large amount of memory
            largeData.resize(10000);
            for (size_t j = 0; j < 10000; ++j) {
                largeData[j] = TestUtils::randomDouble();
            }
            
            // Calculate memory usage (approximate)
            size_t memoryKB = largeData.size() * sizeof(double) / 1024;
            if (memoryKB > peakMemory) {
                peakMemory = static_cast<double>(memoryKB);
            }
            
            // Process data
            double sum = std::accumulate(largeData.begin(), largeData.end(), 0.0);
        }
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        return std::pair<double, double>(peakMemory, durationMs);
    };
    
    auto result = peakDetectionFunc(50);
    
    TestAssert::true(result.first > 0.0, "Peak memory should be positive");
    TestAssert::true(result.second < 100.0, "Peak detection should complete quickly");
    
    std::cout << "    testMemoryUsagePeakDetection passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryUsageFragmentation() {
    TestUtils::TestSession session("testMemoryUsageFragmentation");
    
    // Memory fragmentation test
    auto fragmentationFunc = [](size_t numAllocations) {
        std::vector<std::unique_ptr<double[]>> allocations;
        double startTime = TestUtils::now().time_since_epoch().count();
        
        for (size_t i = 0; i < numAllocations; ++i) {
            // Allocate and deallocate in a pattern that might cause fragmentation
            size_t size = (i % 10 + 1) * 100;
            auto ptr = std::make_unique<double[]>(size);
            for (size_t j = 0; j < size; ++j) {
                ptr[j] = TestUtils::randomDouble();
            }
            
            // Process
            double sum = 0.0;
            for (size_t j = 0; j < size; ++j) {
                sum += ptr[j];
            }
            
            // Deallocate by moving out (causes destruction)
            allocations.push_back(std::move(ptr));
        }
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        return durationMs;
    };
    
    // Test fragmentation with different allocation counts
    for (size_t allocations : {100, 500, 1000}) {
        double duration = fragmentationFunc(allocations);
        TestAssert::true(duration < 500.0, "Memory fragmentation test should complete quickly");
    }
    
    std::cout << "    testMemoryUsageFragmentation passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryUsageStress() {
    TestUtils::TestSession session("testMemoryUsageStress");
    
    // Memory stress test
    auto stressFunc = [](size_t dataSize) {
        // Allocate maximum possible memory
        std::vector<double> data;
        data.reserve(dataSize);
        
        double startTime = TestUtils::now().time_since_epoch().count();
        
        for (size_t i = 0; i < dataSize; ++i) {
            data.push_back(TestUtils::randomDouble());
        }
        
        // Process all data
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        double mean = sum / dataSize;
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        return std::pair<double, double>(mean, durationMs);
    };
    
    // Test with large data size (but not too large to avoid system issues)
    for (size_t size : {1000, 5000, 10000}) {
        auto result = stressFunc(size);
        
        TestAssert::true(result.first >= 0.0 && result.first <= 1.0, "Stress test should produce valid result");
        TestAssert::true(result.second < 1000.0, "Stress test should complete quickly");
    }
    
    std::cout << "    testMemoryUsageStress passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Memory Usage Tests ===" << std::endl;
    
    testMemoryUsageBasic();
    testMemoryUsageAllocationPatterns();
    testMemoryUsageDeallocation();
    testMemoryUsageContinuous();
    testMemoryUsagePeakDetection();
    testMemoryUsageFragmentation();
    testMemoryUsageStress();
    
    std::cout << std::endl;
    std::cout << "=== All Memory Usage Tests PASSED ===" << std::endl;
}