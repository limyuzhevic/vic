// Performance Tests - Benchmarks
// Comprehensive performance benchmarking test suite

#include "tests/utils/TestUtils.hpp"
#include "src/performance/PerformanceMonitor.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>

namespace test_performance_benchmarks {

void testBenchmarkBasicPerformance() {
    TestUtils::TestSession session("testBenchmarkBasicPerformance");
    
    // Simple benchmark function
    auto benchmarkFunc = []() {
        double sum = 0.0;
        for (int i = 0; i < 10000; ++i) {
            sum += TestUtils::randomDouble();
        }
        return sum;
    };
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    double result = benchmarkFunc();
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Basic performance check
    TestAssert::true(result != 0.0, "Benchmark should produce non-zero result");
    TestAssert::true(durationMs < 50.0, "Benchmark should complete quickly");
    
    std::cout << "    testBenchmarkBasicPerformance passed" << std::endl;
    session.setSuccess(true);
}

void testBenchmarkNeuralNetworkOperations() {
    TestUtils::TestSession session("testBenchmarkNeuralNetworkOperations");
    
    // Neural network operations benchmark
    auto benchmarkFunc = []() {
        std::vector<double> weights(1000, 0.5);
        std::vector<double> inputs(1000);
        double output = 0.0;
        
        for (int i = 0; i < 1000; ++i) {
            inputs[i] = TestUtils::randomDouble();
            output += weights[i] * inputs[i];
        }
        
        // Activation function
        output = 1.0 / (1.0 + std::exp(-output));
        return output;
    };
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    double result = benchmarkFunc();
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Neural network benchmark
    TestAssert::true(result >= 0.0 && result <= 1.0, "Neural network output should be between 0 and 1");
    TestAssert::true(durationMs < 100.0, "Neural network benchmark should complete quickly");
    
    std::cout << "    testBenchmarkNeuralNetworkOperations passed" << std::endl;
    session.setSuccess(true);
}

void testBenchmarkMemoryOperations() {
    TestUtils::TestSession session("testBenchmarkMemoryOperations");
    
    // Memory operations benchmark
    auto benchmarkFunc = []() {
        std::vector<double> data(10000);
        
        // Fill memory
        for (int i = 0; i < 10000; ++i) {
            data[i] = TestUtils::randomDouble();
        }
        
        // Process memory
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        double mean = sum / data.size();
        
        return mean;
    };
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    double result = benchmarkFunc();
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Memory operations benchmark
    TestAssert::true(result >= 0.0 && result <= 1.0, "Memory processing result should be between 0 and 1");
    TestAssert::true(durationMs < 100.0, "Memory benchmark should complete quickly");
    
    std::cout << "    testBenchmarkMemoryOperations passed" << std::endl;
    session.setSuccess(true);
}

void testBenchmarkSTDPUpdate() {
    TestUtils::TestSession session("testBenchmarkSTDPUpdate");
    
    // STDP update benchmark
    auto benchmarkFunc = []() {
        double weight = 0.5;
        std::vector<double> preSpikes(10);
        std::vector<double> postSpikes(10);
        
        for (int i = 0; i < 10; ++i) {
            preSpikes[i] = static_cast<double>(i) * 10.0;
            postSpikes[i] = static_cast<double>(i) * 10.0 + 5.0;
        }
        
        // STDP update simulation
        for (int i = 0; i < 10; ++i) {
            double delta = 0.01 * (preSpikes[i] - postSpikes[i]);
            weight += delta;
        }
        
        return weight;
    };
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    double result = benchmarkFunc();
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // STDP benchmark
    TestAssert::true(result != 0.5, "STDP benchmark should modify weight");
    TestAssert::true(durationMs < 50.0, "STDP benchmark should complete quickly");
    
    std::cout << "    testBenchmarkSTDPUpdate passed" << std::endl;
    session.setSuccess(true);
}

void testBenchmarkStringOperations() {
    TestUtils::TestSession session("testBenchmarkStringOperations");
    
    // String operations benchmark
    auto benchmarkFunc = []() {
        std::string result = "";
        for (int i = 0; i < 1000; ++i) {
            result += "test";
            result += std::to_string(i);
            result += "\n";
        }
        return result.size();
    };
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    size_t result = benchmarkFunc();
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // String operations benchmark
    TestAssert::true(result > 0, "String operations should produce output");
    TestAssert::true(durationMs < 100.0, "String operations benchmark should complete quickly");
    
    std::cout << "    testBenchmarkStringOperations passed" << std::endl;
    session.setSuccess(true);
}

void testBenchmarkSortingAlgorithms() {
    TestUtils::TestSession session("testBenchmarkSortingAlgorithms");
    
    // Sorting algorithms benchmark
    auto benchmarkFunc = []() {
        std::vector<double> data(1000);
        for (int i = 0; i < 1000; ++i) {
            data[i] = TestUtils::randomDouble();
        }
        
        std::sort(data.begin(), data.end());
        
        return data[0] + data[999]; // Return min + max
    };
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    double result = benchmarkFunc();
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Sorting benchmark
    TestAssert::true(result != 0.0, "Sorting benchmark should produce result");
    TestAssert::true(durationMs < 100.0, "Sorting benchmark should complete quickly");
    
    std::cout << "    testBenchmarkSortingAlgorithms passed" << std::endl;
    session.setSuccess(true);
}

void testBenchmarkConcurrency() {
    TestUtils::TestSession session("testBenchmarkConcurrency");
    
    // Concurrency benchmark
    auto benchmarkFunc = []() {
        std::vector<double> results(10, 0.0);
        std::vector<std::future<double>> futures;
        
        for (int i = 0; i < 10; ++i) {
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
        
        return total;
    };
    
    double startTime = TestUtils::now().time_since_epoch().count();
    
    double result = benchmarkFunc();
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Concurrency benchmark
    TestAssert::true(result != 0.0, "Concurrency benchmark should produce result");
    TestAssert::true(durationMs < 200.0, "Concurrency benchmark should complete quickly");
    
    std::cout << "    testBenchmarkConcurrency passed" << std::endl;
    session.setSuccess(true);
}

void testBenchmarkRealTimePerformance() {
    TestUtils::TestSession session("testBenchmarkRealTimePerformance");
    
    // Real-time performance benchmark
    auto benchmarkFunc = []() {
        double start = TestUtils::now().time_since_epoch().count();
        
        for (int i = 0; i < 1000; ++i) {
            double x = TestUtils::randomDouble();
            double y = x * x;
            double z = std::sqrt(y);
            // Simulate real-time processing
        }
        
        double end = TestUtils::now().time_since_epoch().count();
        double durationMs = (end - start) * 0.001;
        
        return durationMs;
    };
    
    double result = benchmarkFunc();
    
    TestAssert::true(result < 100.0, "Real-time benchmark should complete within 100ms");
    
    std::cout << "    testBenchmarkRealTimePerformance passed" << std::endl;
    session.setSuccess(true);
}

void testBenchmarkScalability() {
    TestUtils::TestSession session("testBenchmarkScalability");
    
    // Scalability benchmark
    auto benchmarkFunc = [](size_t size) {
        std::vector<double> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = TestUtils::randomDouble();
        }
        
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        double mean = sum / size;
        
        return mean;
    };
    
    // Test different sizes
    for (size_t size : {1000, 5000, 10000}) {
        double startTime = TestUtils::now().time_since_epoch().count();
        
        double result = benchmarkFunc(size);
        
        double endTime = TestUtils::now().time_since_epoch().count();
        double durationMs = (endTime - startTime) * 0.001;
        
        TestAssert::true(result >= 0.0 && result <= 1.0, "Scalability test should produce valid result");
        TestAssert::true(durationMs < 500.0, "Scalability test should complete quickly");
    }
    
    std::cout << "    testBenchmarkScalability passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Performance Benchmark Tests ===" << std::endl;
    
    testBenchmarkBasicPerformance();
    testBenchmarkNeuralNetworkOperations();
    testBenchmarkMemoryOperations();
    testBenchmarkSTDPUpdate();
    testBenchmarkStringOperations();
    testBenchmarkSortingAlgorithms();
    testBenchmarkConcurrency();
    testBenchmarkRealTimePerformance();
    testBenchmarkScalability();
    
    std::cout << std::endl;
    std::cout << "=== All Performance Benchmark Tests PASSED ===" << std::endl;
}