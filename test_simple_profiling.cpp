// Simple test to verify enhanced logging and profiling compilation
test_simple_profiling.cpp

#include <iostream>
#include "core/EnhancedLogger.hpp"
#include "profiling/ProfilingTimer.hpp"
#include "debug/SystemMonitor.hpp"

int main() {
    std::cout << "Testing NLM Enhanced Logging and Profiling" << std::endl;
    
    // Test 1: Enhanced logger initialization
    std::cout << "Test 1: Enhanced logger initialization" << std::endl;
    auto& enhancedLogger = nlm::getGlobalEnhancedLogger();
    
    // Test 2: Structured logging
    std::cout << "Test 2: Structured logging" << std::endl;
    enhancedLogger.logStructured("Test", "TestEvent", {
        {"key1", "value1"},
        {"key2", "value2"}
    });
    
    // Test 3: Profiling timer
    std::cout << "Test 3: Profiling timer" << std::endl;
    nlm::ProfilingTimer timer("TestTimer", true);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    double elapsed = timer.stop();
    std::cout << "Timer elapsed: " << elapsed << " ms" << std::endl;
    
    // Test 4: System monitor registry
    std::cout << "Test 4: System monitor registry" << std::endl;
    nlm::SystemMonitorRegistry& registry = nlm::SystemMonitorRegistry::getInstance();
    
    // Test 5: Performance monitoring
    std::cout << "Test 5: Performance monitoring" << std::endl;
    enhancedLogger.logPerformance("TestOperation", {
        {"executionTime", 10.5},
        {"operationCount", 100}
    }, "TestContext");
    
    std::cout << "All tests passed successfully!" << std::endl;
    
    return 0;
}
