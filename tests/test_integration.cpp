#include "../src/core/Logger/Logger.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Performance/PerformanceMetrics.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

namespace test_integration {

void testLoggerPerformance() {
    std::cout << "Testing Logger Performance Integration..." << std::endl;
    
    // Test basic logging
    nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);
    nlm::Logger::getGlobal().info("Test log message");
    
    // Test performance logging
    nlm::PerformanceMetrics metrics;
    metrics.totalWallClockTime = 10.0;
    metrics.avgFiringRate = 50.0;
    metrics.curiosityLevel = 0.8;
    metrics.actionSuccessRate = 0.9;
    
    std::string perfOutput = metrics.toString();
    assert(perfOutput.find("Performance Metrics:") != std::string::npos);
    assert(perfOutput.find("10.000") != std::string::npos);
    assert(perfOutput.find("50.000") != std::string::npos);
    assert(perfOutput.find("0.800") != std::string::npos);
    
    std::cout << "✓ Logger Performance Integration test passed" << std::endl;
}

void testConfigLoggerIntegration() {
    std::cout << "Testing Config Logger Integration..." << std::endl;
    
    nlm::Config config;
    config.set("test_setting", "value");
    config.set("test_number", 123);
    
    // Log config values
    nlm::Logger::getGlobal().info("Config loaded: " + config.summary());
    
    // Verify config works with logging
    assert(config.getOr<std::string>("test_setting", "") == "value");
    assert(config.getOr<int64_t>("test_number", 0) == 123);
    
    std::cout << "✓ Config Logger Integration test passed" << std::endl;
}

void testPerformanceConfigIntegration() {
    std::cout << "Testing Performance Config Integration..." << std::endl;
    
    nlm::Config config;
    config.set("max_steps", 10000);
    config.set("neuron_count", 1000);
    config.set("enable_checkpointing", true);
    
    nlm::PerformanceLogger perfLogger;
    
    // Simulate some performance tracking
    perfLogger.logStep("Neural Simulation", 0.001);
    perfLogger.logStep("Memory Update", 0.002);
    perfLogger.logStep("Plasticity Update", 0.0015);
    
    nlm::PerformanceMetrics metrics;
    metrics.totalWallClockTime = config.getOr<int64_t>("max_steps", 0) * 0.001; // Estimate
    metrics.avgFiringRate = 25.0;
    metrics.curiosityLevel = 0.6;
    metrics.actionSuccessRate = 0.85;
    
    perfLogger.logNeuralMetrics(metrics);
    
    nlm::PerformanceMetrics avg = perfLogger.getAverageMetrics();
    assert(avg.totalWallClockTime > 0.0);
    assert(avg.avgFiringRate > 0.0);
    
    std::cout << "✓ Performance Config Integration test passed" << std::endl;
}

void testAdvancedLoggerFeatures() {
    std::cout << "Testing Advanced Logger Features..." << std::endl;
    
    // Test color logging
    nlm::ConsoleLogger consoleLogger(nlm::LogLevel::Info);
    consoleLogger.setUseColors(false); // Disable colors for testing
    
    // Test file logging (create temporary file)
    std::string testFile = "/tmp/test_log.txt";
    consoleLogger.setOutputFile(testFile);
    
    // Log some messages
    nlm::LogEntry entry;
    entry.level = nlm::LogLevel::Info;
    entry.message = "Test advanced logger";
    entry.file = "test.cpp";
    entry.line = 42;
    entry.function = "testFunction";
    
    consoleLogger.log(entry);
    
    // Verify file was written
    std::ifstream file(testFile);
    assert(file.is_open());
    
    std::string line;
    std::getline(file, line);
    assert(line.find("Test advanced logger") != std::string::npos);
    
    file.close();
    std::remove(testFile.c_str());
    
    std::cout << "✓ Advanced Logger Features test passed" << std::endl;
}

void testBenchmarkIntegration() {
    std::cout << "Testing Benchmark Integration..." << std::endl;
    
    // Test benchmark with logging
    auto benchmarkFunc = []() {
        nlm::Logger::getGlobal().debug("Benchmark function started");
        // Simulate some work
        volatile int sum = 0;
        for (int i = 0; i < 10000; ++i) {
            sum += i;
        }
        nlm::Logger::getGlobal().debug("Benchmark function completed");
    };
    
    // Run benchmark (would use nlm::Benchmark in real implementation)
    auto start = std::chrono::high_resolution_clock::now();
    benchmarkFunc();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    
    assert(duration.count() > 0.0);
    
    std::cout << "✓ Benchmark Integration test passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== Integration Tests ===" << std::endl;
    
    try {
        testLoggerPerformance();
        testConfigLoggerIntegration();
        testPerformanceConfigIntegration();
        testAdvancedLoggerFeatures();
        testBenchmarkIntegration();
        
        std::cout << std::endl;
        std::cout << "=== All Integration Tests PASSED ===" << std::endl;
        return;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Integration Tests FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_integration

int main() {
    test_integration::runAllTests();
    return 0;
}