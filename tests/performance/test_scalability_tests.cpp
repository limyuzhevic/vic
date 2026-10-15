// Performance Tests - Scalability Tests
// Comprehensive scalability testing for performance optimization

#include "tests/utils/TestUtils.hpp"
#include "src/performance/PerformanceMonitor.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>

namespace test_scalability_tests {

void testScalabilityMemoryUsage() {
    TestUtils::TestSession session("testScalabilityMemoryUsage");
    
    // Memory usage scalability test
    auto scalabilityFunc = [](size_t size) {
        std::vector<double> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = TestUtils::randomDouble();
        }
        
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        double mean = sum / size;
        
        // Estimate memory usage (rough approximation)
        size_t memoryBytes = size * sizeof(double);
        
        return std::pair<double, size_t>(mean, memoryBytes);
    };
    
    // Test different memory sizes
    for (size_t size : {1000, 10000, 100000}) {
        double startTime = TestUtils::now().time_since_epoch().count();
        
        auto result = scalabilityFunc(size);
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        TestAssert::true(result.first >= 0.0 && result.first <= 1.0, "Memory scalability should produce valid result");
        TestAssert::true(durationMs < 1000.0, "Memory scalability should complete quickly");
        
        // Log memory usage
        size_t memoryMB = result.second / (1024 * 1024);
        TestAssert::true(memoryMB > 0, "Memory usage should be positive");
    }
    
    std::cout << "    testScalabilityMemoryUsage passed" << std::endl;
    session.setSuccess(true);
}

void testScalabilityProcessingSpeed() {
    TestUtils::TestSession session("testScalabilityProcessingSpeed");
    
    // Processing speed scalability test
    auto processingFunc = [](size_t iterations) {
        double startTime = TestUtils::now().time_since_epoch().count();
        
        double sum = 0.0;
        for (size_t i = 0; i < iterations; ++i) {
            sum += TestUtils::randomDouble();
        }
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        double operationsPerSecond = iterations / (durationMs / 1000.0);
        
        return std::pair<double, double>(sum, operationsPerSecond);
    };
    
    // Test different iteration counts
    for (size_t iterations : {1000, 10000, 100000}) {
        auto result = processingFunc(iterations);
        
        TestAssert::true(result.first != 0.0, "Processing scalability should produce non-zero result");
        TestAssert::true(result.second > 0.0, "Processing speed should be positive");
        TestAssert::true(result.second < 1000000.0, "Processing speed should be reasonable");
    }
    
    std::cout << "    testScalabilityProcessingSpeed passed" << std::endl;
    session.setSuccess(true);
}

void testScalabilityConcurrency() {
    TestUtils::TestSession session("testScalabilityConcurrency");
    
    // Concurrency scalability test
    auto concurrencyFunc = [](size_t threadCount) {
        std::vector<std::future<double>> futures;
        futures.reserve(threadCount);
        
        double startTime = TestUtils::now().time_since_epoch().count();
        
        for (size_t i = 0; i < threadCount; ++i) {
            futures.push_back(std::async(std::launch::async, [i]() {
                double sum = 0.0;
                for (int j = 0; j < 1000; ++j) {
                    sum += TestUtils::randomDouble();
                }
                return sum;
            }));
        }
        
        double total = 0.0;
        for (auto& future : futures) {
            total += future.get();
        }
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        double operationsPerSecond = (threadCount * 1000) / (durationMs / 1000.0);
        
        return std::pair<double, double>(total, operationsPerSecond);
    };
    
    // Test different thread counts
    for (size_t threadCount : {1, 2, 4, 8}) {
        auto result = concurrencyFunc(threadCount);
        
        TestAssert::true(result.first != 0.0, "Concurrency scalability should produce non-zero result");
        TestAssert::true(result.second > 0.0, "Concurrency speed should be positive");
    }
    
    std::cout << "    testScalabilityConcurrency passed" << std::endl;
    session.setSuccess(true);
}

void testScalabilityDataStructures() {
    TestUtils::TestSession session("testScalabilityDataStructures");
    
    // Data structures scalability test
    auto testVectorPerformance = [](size_t size) {
        std::vector<double> vec;
        vec.reserve(size);
        
        double startTime = TestUtils::now().time_since_epoch().count();
        
        for (size_t i = 0; i < size; ++i) {
            vec.push_back(TestUtils::randomDouble());
        }
        
        // Access all elements
        double sum = 0.0;
        for (const auto& val : vec) {
            sum += val;
        }
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        return std::pair<double, double>(sum, durationMs);
    };
    
    // Test different vector sizes
    for (size_t size : {1000, 10000, 50000}) {
        auto result = testVectorPerformance(size);
        
        TestAssert::true(result.first != 0.0, "Vector performance should produce non-zero result");
        TestAssert::true(result.second < 1000.0, "Vector performance should complete quickly");
    }
    
    std::cout << "    testScalabilityDataStructures passed" << std::endl;
    session.setSuccess(true);
}

void testScalabilityNeuralNetwork() {
    TestUtils::TestSession session("testScalabilityNeuralNetwork");
    
    // Neural network scalability test
    auto neuralNetworkFunc = [](size_t layerSize) {
        std::vector<std::vector<double>> weights(layerSize, std::vector<double>(layerSize, 0.5));
        std::vector<double> inputs(layerSize);
        double output = 0.0;
        
        double startTime = TestUtils::now().time_since_epoch().count();
        
        // Neural network computation
        for (size_t i = 0; i < layerSize; ++i) {
            inputs[i] = TestUtils::randomDouble();
            for (size_t j = 0; j < layerSize; ++j) {
                output += weights[i][j] * inputs[j];
            }
        }
        
        // Activation function
        output = 1.0 / (1.0 + std::exp(-output));
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        return std::pair<double, double>(output, durationMs);
    };
    
    // Test different layer sizes
    for (size_t layerSize : {10, 50, 100}) {
        auto result = neuralNetworkFunc(layerSize);
        
        TestAssert::true(result.first >= 0.0 && result.first <= 1.0, "Neural network should produce valid output");
        TestAssert::true(result.second < 100.0, "Neural network scalability should complete quickly");
    }
    
    std::cout << "    testScalabilityNeuralNetwork passed" << std::endl;
    session.setSuccess(true);
}

void testScalabilityRealTimeConstraints() {
    TestUtils::TestSession session("testScalabilityRealTimeConstraints");
    
    // Real-time constraints scalability test
    auto realTimeFunc = [](size_t iterations) {
        double startTime = TestUtils::now().time_since_epoch().count();
        
        for (size_t i = 0; i < iterations; ++i) {
            double x = TestUtils::randomDouble();
            double y = x * x;
            double z = std::sqrt(y);
            // Simulate real-time processing with 1ms deadline
            if (TestUtils::elapsedMs(std::chrono::steady_clock::now().time_since_epoch().count()) > 1.0) {
                break; // Deadline exceeded
            }
        }
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        return durationMs;
    };
    
    // Test with real-time constraints
    size_t iterations = 1000;
    double duration = realTimeFunc(iterations);
    
    TestAssert::true(duration < 1000.0, "Real-time constraints should be met");
    
    std::cout << "    testScalabilityRealTimeConstraints passed" << std::endl;
    session.setSuccess(true);
}

void testScalabilityMemoryEfficiency() {
    TestUtils::TestSession session("testScalabilityMemoryEfficiency");
    
    // Memory efficiency scalability test
    auto memoryEfficiencyFunc = [](size_t size) {
        // Test memory allocation/deallocation efficiency
        std::vector<std::vector<double>> matrix;
        matrix.reserve(size);
        
        double startTime = TestUtils::now().time_since_epoch().count();
        
        for (size_t i = 0; i < size; ++i) {
            matrix.push_back(std::vector<double>(size, TestUtils::randomDouble()));
        }
        
        // Process matrix
        double sum = 0.0;
        for (const auto& row : matrix) {
            for (const auto& val : row) {
                sum += val;
            }
        }
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        return std::pair<double, double>(sum, durationMs);
    };
    
    // Test different matrix sizes
    for (size_t size : {10, 50, 100}) {
        auto result = memoryEfficiencyFunc(size);
        
        TestAssert::true(result.first != 0.0, "Memory efficiency should produce non-zero result");
        TestAssert::true(result.second < 1000.0, "Memory efficiency should complete quickly");
    }
    
    std::cout << "    testScalabilityMemoryEfficiency passed" << std::endl;
    session.setSuccess(true);
}

void testScalabilityParallelProcessing() {
    TestUtils::TestSession session("testScalabilityParallelProcessing");
    
    // Parallel processing scalability test
    auto parallelProcessingFunc = [](size_t dataSize) {
        std::vector<double> data(dataSize);
        for (size_t i = 0; i < dataSize; ++i) {
            data[i] = TestUtils::randomDouble();
        }
        
        double startTime = TestUtils::now().time_since_epoch().count();
        
        // Parallel processing (simulated with threads)
        std::vector<std::future<double>> futures;
        size_t chunkSize = dataSize / 4;
        
        for (size_t i = 0; i < 4; ++i) {
            size_t start = i * chunkSize;
            size_t end = (i == 3) ? dataSize : (i + 1) * chunkSize;
            
            futures.push_back(std::async(std::launch::async, [data, start, end]() {
                double sum = 0.0;
                for (size_t i = start; i < end; ++i) {
                    sum += data[i];
                }
                return sum;
            }));
        }
        
        double total = 0.0;
        for (auto& future : futures) {
            total += future.get();
        }
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        return std::pair<double, double>(total, durationMs);
    };
    
    // Test different data sizes
    for (size_t dataSize : {1000, 10000, 50000}) {
        auto result = parallelProcessingFunc(dataSize);
        
        TestAssert::true(result.first != 0.0, "Parallel processing should produce non-zero result");
        TestAssert::true(result.second < 500.0, "Parallel processing should complete quickly");
    }
    
    std::cout << "    testScalabilityParallelProcessing passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Scalability Tests ===" << std::endl;
    
    testScalabilityMemoryUsage();
    testScalabilityProcessingSpeed();
    testScalabilityConcurrency();
    testScalabilityDataStructures();
    testScalabilityNeuralNetwork();
    testScalabilityRealTimeConstraints();
    testScalabilityMemoryEfficiency();
    testScalabilityParallelProcessing();
    
    std::cout << std::endl;
    std::cout << "=== All Scalability Tests PASSED ===" << std::endl;
}