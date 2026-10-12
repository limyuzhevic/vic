#include "../core/Performance/PerformanceMetrics.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

namespace test_performance {

void testPerformanceMetricsBasic() {
    std::cout << "Testing Performance Metrics Basic..." << std::endl;
    
    nlm::PerformanceMetrics metrics;
    metrics.totalWallClockTime = 10.5;
    metrics.avgFiringRate = 25.3;
    metrics.totalSpikeCount = 1000;
    metrics.totalNeuronCount = 500;
    metrics.curiosityLevel = 0.7;
    metrics.actionSuccessRate = 0.85;
    
    std::string output = metrics.toString();
    assert(output.find("Performance Metrics:") != std::string::npos);
    assert(output.find("10.500") != std::string::npos);  // Wall Clock Time
    assert(output.find("25.300") != std::string::npos);  // Average Firing Rate
    assert(output.find("0.700") != std::string::npos);   // Curiosity Level
    assert(output.find("85.000%") != std::string::npos); // Action Success Rate
    
    std::cout << "✓ Basic Performance Metrics test passed" << std::endl;
}

void testPerformanceLogger() {
    std::cout << "Testing Performance Logger..." << std::endl;
    
    nlm::PerformanceLogger logger;
    
    // Add some step timings
    logger.logStep("Neural Step", 0.001);
    logger.logStep("Memory Step", 0.002);
    logger.logStep("Plasticity Step", 0.0015);
    
    // Should have logged step summary
    // (Implementation details would verify this)
    
    nlm::PerformanceMetrics metrics;
    metrics.totalWallClockTime = 5.0;
    metrics.avgFiringRate = 20.0;
    metrics.curiosityLevel = 0.5;
    
    logger.logNeuralMetrics(metrics);
    
    nlm::PerformanceMetrics avg = logger.getAverageMetrics();
    assert(avg.totalWallClockTime > 0.0);
    assert(avg.avgFiringRate > 0.0);
    assert(avg.curiosityLevel > 0.0);
    
    std::cout << "✓ Performance Logger test passed" << std::endl;
}

void testBenchmark() {
    std::cout << "Testing Benchmark..." << std::endl;
    
    auto dummyFunc = []() {
        volatile int sum = 0;
        for (int i = 0; i < 1000; ++i) {
            sum += i;
        }
    };
    
    nlm::Benchmark::runBenchmark("Dummy Computation", dummyFunc, 5);
    
    std::cout << "✓ Benchmark test passed" << std::endl;
}

void testPerformanceIntegration() {
    std::cout << "Testing Performance Integration..." << std::endl;
    
    // Test integration with logging system
    nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);
    nlm::Logger::getGlobal().info("Test performance integration");
    
    // Test with config system
    nlm::Config::getGlobal().set("test_metric", 1.5);
    assert(nlm::Config::getGlobal().getOr<double>("test_metric", 0.0) > 0.0);
    
    std::cout << "✓ Performance Integration test passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== Performance Tests ===" << std::endl;
    
    try {
        testPerformanceMetricsBasic();
        testPerformanceLogger();
        testBenchmark();
        testPerformanceIntegration();
        
        std::cout << std::endl;
        std::cout << "=== All Performance Tests PASSED ===" << std::endl;
        return;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Performance Tests FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_performance

int main() {
    test_performance::runAllTests();
    return 0;
}