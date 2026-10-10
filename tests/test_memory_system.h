/*
 * Memory System Integration Test Framework
 * 
 * This file provides a comprehensive test framework for the NLM neural simulation's memory systems.
 * It includes tests for:
 * - NeuralWorkingMemory
 * - NeuralEpisodicMemory
 * - NeuralAssociativeMemory
 * - Integration tests across memory systems
 * - Performance and stress tests
 * - Error handling tests
 * 
 * The framework uses a simple unit testing approach with:
 * - Test registration and execution
 * - Comprehensive assertions with detailed error reporting
 * - Performance benchmarking capabilities
 * - Memory persistence verification
 * - Integration with the Brain system
 */

#pragma once

#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <cmath>
#include <string>
#include <iomanip>
#include <fstream>

// Forward declarations
namespace nlm {
    class Brain;
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class NeuralAssociativeMemory;
    struct EpisodicMemoryItem;
    enum class ActionType;
}

// Test result tracking
struct TestResult {
    std::string name;
    bool passed;
    double durationMs;
    std::string errorMessage;
    
    TestResult(const std::string& testName, bool success, double duration, const std::string& error = "")
        : name(testName), passed(success), durationMs(duration), errorMessage(error) {}
};

class MemorySystemTestFramework {
public:
    MemorySystemTestFramework() : 
        passedTests_(0), failedTests_(0), totalTests_(0), startTime_(getCurrentTime()) {}
    
    virtual ~MemorySystemTestFramework() = default;
    
    // Test registration
    virtual void registerTest(const std::string& name, std::function<bool()> testFunc);
    
    // Test execution
    void runAllTests();
    bool runTest(const std::string& name);
    
    // Test results
    const std::vector<TestResult>& getTestResults() const { return results_; }
    size_t getPassedTests() const { return passedTests_; }
    size_t getFailedTests() const { return failedTests_; }
    size_t getTotalTests() const { return totalTests_; }
    double getTotalDurationMs() const { return totalDurationMs_; }
    
    // Test utilities
    template<typename T>
    bool assertEqual(const T& expected, const T& actual, const std::string& message);
    
    template<typename T>
    bool assertNotEqual(const T& expected, const T& actual, const std::string& message);
    
    bool assertTrue(bool condition, const std::string& message);
    bool assertFalse(bool condition, const std::string& message);
    bool assertGreaterThan(float value, float threshold, const std::string& message);
    bool assertLessThan(float value, float threshold, const std::string& message);
    bool assertInRange(float value, float min, float max, const std::string& message);
    
    // Performance testing
    void startTimer() { timerStart_ = getCurrentTime(); }
    double stopTimer() { return getElapsedMs(timerStart_); }
    
    // Brain integration helpers
    bool setupBrainIntegration(nlm::Brain* brain);
    bool verifyBrainMemorySystems(nlm::Brain* brain);
    
    // Memory persistence tests
    bool testMemoryPersistence(const nlm::EpisodicMemoryItem& original);
    bool testMemoryAccuracy(const nlm::EpisodicMemoryItem& original, const nlm::EpisodicMemoryItem& restored);
    
    // Performance benchmarking
    double benchmarkMemoryOperation(std::function<void()> operation, size_t iterations = 1000);
    
    // Error handling
    bool testErrorHandling(std::function<void()> errorCondition, bool expectError);
    
    // Report generation
    void generateReport(const std::string& filename = "memory_system_test_report.txt");
    void printResults() const;
    
protected:
    virtual void runNeuralWorkingMemoryTests();
    virtual void runNeuralEpisodicMemoryTests();
    virtual void runNeuralAssociativeMemoryTests();
    virtual void runIntegrationTests();
    virtual void runPerformanceTests();
    virtual void runErrorHandlingTests();
    virtual void runMemoryConsistencyTests();
    
private:
    using TestFunction = std::function<bool()>;
    
    std::vector<std::pair<std::string, TestFunction>> tests_;
    std::vector<TestResult> results_;
    size_t passedTests_;
    size_t failedTests_;
    size_t totalTests_;
    double totalDurationMs_;
    double timerStart_;
    double startTime_;
    
    double getCurrentTime() {
        return std::chrono::duration<double, std::milli>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    double getElapsedMs(double start) {
        return getCurrentTime() - start;
    }
};