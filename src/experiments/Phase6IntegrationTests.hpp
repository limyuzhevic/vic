// Phase 6 Integration Tests Header
// Comprehensive integration testing for the complete brain loop functionality
// Validates integration of all subsystems (memory, prediction, neuromodulation, cognition, sensory, motor)

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <ctime>

namespace nlm {

// ============================================================================
// Test Type Enums
// ============================================================================

enum class TestType {
    Integration,      // System integration tests
    BrainLoop,        // Complete brain loop functionality
    DataFlow,         // Data transfer between components
    ErrorHandling,    // Error recovery and graceful failure handling
    Performance,      // Performance and resource usage
    Checkpointing,    // Save/load functionality
    Reproducibility,  // Deterministic behavior with given seeds
    EdgeCases,        // Edge cases and boundary conditions
    Concurrent        // Concurrent access testing
};

// ============================================================================
// Result Structures
// ============================================================================

struct PerformanceMetrics {
    double testsPerSecond;
    double averageTestDuration;
    std::string performanceGrade;
    
    PerformanceMetrics()
        : testsPerSecond(0.0)
        , averageTestDuration(0.0)
        , performanceGrade("UNKNOWN") {}
};

struct IntegrationTestResult {
    std::string testName;
    TestType testType;
    bool success;
    std::string error;
    int testCount;
    int passedCount;
    int failedCount;
    double totalDuration;
    PerformanceMetrics performance;
    
    IntegrationTestResult()
        : testType(TestType::Integration)
        , success(false)
        , testCount(0)
        , passedCount(0)
        , failedCount(0)
        , totalDuration(0.0) {}
};

// ============================================================================
// Phase6IntegrationTests Class
// ============================================================================

class Phase6IntegrationTests {
public:
    Phase6IntegrationTests();
    ~Phase6IntegrationTests();
    
    // Run all integration tests
    IntegrationTestResult runAllTests();
    
    // Print test results to console
    void printTestResults() const;
    
    // Test suite methods
    IntegrationTestResult runSystemIntegrationTests();
    IntegrationTestResult runBrainLoopTests();
    IntegrationTestResult runDataFlowTests();
    IntegrationTestResult runErrorHandlingTests();
    IntegrationTestResult runPerformanceTests();
    IntegrationTestResult runCheckpointTests();
    IntegrationTestResult runReproducibilityTests();
    IntegrationTestResult runEdgeCaseTests();
    IntegrationTestResult runConcurrentAccessTests();
    
    // Helper methods
    PerformanceMetrics calculatePerformanceMetrics() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
