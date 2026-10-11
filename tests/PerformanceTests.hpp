// Performance Tests Header
// Performance infrastructure tests for Phase 6 integration
// Validates memory pool efficiency, event queue performance, SIMD optimizations,
// and parallel processing integration

#ifndef PERFORMANCE_TESTS_HPP
#define PERFORMANCE_TESTS_HPP

#include <vector>
#include <chrono>
#include <string>
#include <memory>

namespace nlm {
    class Config;
    class WorkingMemory;
    class EpisodicMemory;
    class AttentionalSelection;
    class PredictionSystem;
    class ConceptFormation;
    class Neuromodulator;
    class SleepManager;
    class DevelopmentSystem;
    class PerformanceProfiler;
}

class PerformanceTestSuite {
public:
    enum class EventType { Cognitive, Memory, Prediction, Concept, Neuromodulation, Replay, Sleep, Development };

    struct Event {
        int id;
        EventType type;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string data;
    };

    PerformanceTestSuite();
    ~PerformanceTestSuite();

    void initialize();
    void cleanup();
    void runAllTests();

    // Memory Pool Tests
    void runMemoryPoolTests();
    
    // Event Queue Tests
    void runEventQueueTests();
    
    // SIMD Integration Tests
    void runSIMDIntegrationTests();
    
    // Parallel Processing Tests
    void runParallelProcessingTests();
    
    // Performance Infrastructure Tests
    void runPerformanceInfrastructureTests();
    
    // Memory Bandwidth Tests
    void runMemoryBandwidthTests();
    
    // CPU Performance Tests
    void runCPUPerformanceTests();
    
    // Scalability Tests
    void runScalabilityTests();
    
    // Resource Management Tests
    void runResourceManagementTests();

private:
    // Helper functions for performance testing
    double calculateThroughput(int operations, std::chrono::microseconds duration);
    double calculateBandwidth(size_t bytes, std::chrono::microseconds duration);
};

// Performance test runner function
void run_performance_tests();

#endif // PERFORMANCE_TESTS_HPP