#!/usr/bin/env g++
// src/testing/TestFrameworkDemo.cpp
// Demonstration of the NLM testing framework

#include "TestFramework.hpp"
#include "Assert.hpp"
#include <iostream>
#include <vector>

namespace nlm {

class DemoTestCase : public TestCase {
public:
    std::string getName() const override {
        return "DemoTestCase";
    }
    
    void run() override {
        std::cout << "Running DemoTestCase..." << std::endl;
        
        // Test basic arithmetic
        Assert::equals(2 + 2, 4, "Basic arithmetic");
        
        // Test floating point comparison
        Assert::equals(3.14159, 3.1416, 0.00001, "Pi approximation");
        
        // Test string comparison
        Assert::equals(std::string("test"), std::string("test"), "String comparison");
        
        // Test vector operations
        std::vector<int> vec = {1, 2, 3, 4, 5};
        Assert::equals(vec.size(), 5, "Vector size");
        Assert::equals(vec[0], 1, "Vector first element");
        
        std::cout << "DemoTestCase completed successfully" << std::endl;
    }
};

class DemoBenchmark {
public:
    static void simpleMath() {
        volatile int result = 0;
        for (int i = 0; i < 1000000; ++i) {
            result += i * 2;
        }
    }
    
    static void stringOperations() {
        volatile std::string result;
        for (int i = 0; i < 100000; ++i) {
            result = "test" + std::to_string(i);
        }
    }
};

class DemoProfiler {
public:
    static void intensiveComputation() {
        volatile double result = 0.0;
        for (int i = 0; i < 100000; ++i) {
            result += std::sin(i * 0.1) * std::cos(i * 0.2);
        }
    }
};

} // namespace nlm

int main() {
    std::cout << "=== NLM Testing Framework Demo ===" << std::endl;
    
    // Create and run a test case
    auto testCase = std::make_unique<nlm::DemoTestCase>();
    
    std::cout << "Test Case Name: " << testCase->getName() << std::endl;
    testCase->run();
    
    // Run benchmarks
    std::cout << "\n=== Running Benchmarks ===" << std::endl;
    
    auto mathBenchmark = nlm::Benchmark::benchmark("Simple Math", nlm::DemoBenchmark::simpleMath, 100);
    std::cout << "Math Benchmark: " << mathBenchmark.timeMs << " ms (" << mathBenchmark.averageTimeMs << " ms avg)" << std::endl;
    
    auto stringBenchmark = nlm::Benchmark::benchmark("String Operations", nlm::DemoBenchmark::stringOperations, 10);
    std::cout << "String Operations Benchmark: " << stringBenchmark.timeMs << " ms (" << stringBenchmark.averageTimeMs << " ms avg)" << std::endl;
    
    // Generate benchmark report
    nlm::Benchmark::saveResults({mathBenchmark, stringBenchmark}, "demo_benchmarks.json");
    
    // Profile code
    std::cout << "\n=== Running Profiler ===" << std::endl;
    
    nlm::Profiler profiler;
    
    auto session = profiler.startSession("Intensive Computation");
    for (int i = 0; i < 10; ++i) {
        nlm::DemoProfiler::intensiveComputation();
    }
    profiler.endSession(session);
    
    profiler.generateReport("demo_profiler_report.html");
    
    std::cout << "\n=== Demo completed successfully ===" << std::endl;
    return 0;
}