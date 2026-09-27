#pragma once

/**
 * Integration Testing Framework for NLM
 * 
 * System integration testing framework providing comprehensive validation
 * of NLM component interactions, cross-component integration, and
 * real-time system behavior.
 * 
 * Key Features:
 * - System integration testing for all NLM components
 * - Cross-component integration validation
 * - Performance integration testing
 * - Real-time system integration tests
 * - Plugin integration testing
 * - Automated dependency validation
 * 
 * Design Principles:
 * - Component-agnostic testing architecture
 * - Dependency injection for test isolation
 * - State management for system-wide tests
 * - Performance-aware integration testing
 * - Biological system fidelity validation
 */

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <future>
#include <queue>
#include <condition_variable>
#include "TestFramework.hpp"

namespace nlm {
namespace testing {

/**
 * System component status enumeration
 */
enum class ComponentStatus {
    READY,
    INITIALIZING,
    RUNNING,
    PAUSED,
    STOPPED,
    ERROR,
    UNKNOWN
};

/**
 * Integration test scenario
 */
struct IntegrationTestScenario {
    std::string name;
    std::string description;
    std::vector<std::string> requiredComponents;
    std::vector<std::string> optionalComponents;
    std::function<bool()> setupFunction;
    std::function<bool()> testFunction;
    std::function<bool()> cleanupFunction;
    double timeoutDuration;
    bool isCritical;
    
    IntegrationTestScenario(const std::string& name, const std::string& description,
                           std::function<bool()> setup, std::function<bool()> test,
                           std::function<bool()> cleanup = nullptr)
        : name(name), description(description), setupFunction(setup),
          testFunction(test), cleanupFunction(cleanup), timeoutDuration(30.0),
          isCritical(false) {}
};

/**
 * Component dependency information
 */
struct ComponentDependency {
    std::string componentName;
    std::string dependencyName;
    std::string dependencyType; // "soft" or "hard"
    bool isInitialized;
    std::chrono::high_resolution_clock::time_point lastChecked;
    std::string errorMessage;
    
    ComponentDependency(const std::string& component, const std::string& dependency,
                       const std::string& type)
        : componentName(component), dependencyName(dependency),
          dependencyType(type), isInitialized(false), lastChecked(std::chrono::high_resolution_clock::now()) {}
};

/**
 * System integration test framework
 */
class IntegrationTestSuite {
public:
    IntegrationTestSuite();
    ~IntegrationTestSuite();
    
    // Test case registration
    void registerIntegrationTest(const std::string& name,
                                const std::string& description,
                                std::function<bool()> setup,
                                std::function<bool()> test,
                                std::function<bool()> cleanup = nullptr);
    
    // Component dependency management
    void registerComponent(const std::string& name, ComponentStatus status = ComponentStatus::UNKNOWN);
    void registerDependency(const std::string& component,
                           const std::string& dependency,
                           const std::string& type = "soft");
    bool checkComponentDependencies(const std::string& component) const;
    
    // Integration test execution
    void runIntegrationTests();
    void runSingleTest(const std::string& testName);
    
    // System state management
    void initializeSystem();
    void shutdownSystem();
    void pauseSystem();
    void resumeSystem();
    bool isSystemRunning() const { return systemRunning_.load(); }
    
    // Performance integration testing
    void runPerformanceIntegrationTests();
    double measureSystemPerformance(const std::string& operation) const;
    
    // Real-time integration testing
    void runRealTimeIntegrationTests();
    void simulateRealTimeBehavior(double duration); // Simulate real-time operation
    
    // Plugin integration testing
    void runPluginIntegrationTests();
    void registerPlugin(const std::string& pluginName, const std::string& pluginPath);
    
    // System state monitoring
    std::vector<std::string> getActiveComponents() const;
    ComponentStatus getComponentStatus(const std::string& componentName) const;
    double getSystemUtilization() const;
    
    // Test result reporting
    void generateIntegrationReport() const;
    
    // Cross-component validation
    bool validateCrossComponentCommunication() const;
    bool validateDataConsistencyAcrossComponents() const;
    
private:
    // Test execution utilities
    bool executeTestWithTimeout(const std::string& testName, std::function<bool()> testFunction,
                               double timeoutDuration);
    
    // System management
    void systemThreadFunc();
    
    // Component state management
    void updateComponentStatus(const std::string& component, ComponentStatus status);
    
    // Test utilities
    void cleanupCompletedTests();
    void resetSystemState();
    
    // Integration test scenarios
    std::vector<IntegrationTestScenario> integrationTests_;
    
    // Component management
    std::unordered_map<std::string, ComponentStatus> componentStatus_;
    std::vector<ComponentDependency> componentDependencies_;
    mutable std::shared_mutex componentsMutex_;
    
    // System state
    std::atomic<bool> systemRunning_{false};
    std::atomic<bool> systemPaused_{false};
    std::chrono::high_resolution_clock::time_point systemStartTime_;
    std::thread systemThread_;
    mutable std::mutex systemMutex_;
    std::condition_variable systemStopCondition_;
    
    // Plugin management
    std::vector<std::string> loadedPlugins_;
    
    // Performance metrics
    mutable std::unordered_map<std::string, double> operationDurations_;
    mutable std::mutex performanceMutex_;
    
    // Test results
    std::unordered_map<std::string, bool> testResults_;
    std::unordered_map<std::string, std::string> testLogs_;
};

/**
 * Real-time system behavior simulator
 */
class RealTimeSystemSimulator {
public:
    RealTimeSystemSimulator() = default;
    ~RealTimeSystemSimulator() = default;
    
    // Real-time test scenarios
    void simulateRealTimeSpikingActivity(size_t neuronCount, double duration);
    void simulateRealTimeSynapticPlasticity(double duration);
    void simulateRealTimeNetworkDynamics(double duration);
    void simulateRealTimeInputOutput(double duration);
    
    // Performance under real-time constraints
    bool testRealTimePerformance(double maxLatency, size_t operationsPerSecond);
    
    // Behavioral fidelity tests
    bool testSpikingPatternFidelity(const std::vector<double>& expectedPattern,
                                   const std::vector<double>& actualPattern);
    bool testTemporalCodingFidelity(double expectedPeriod, double actualPeriod);
    bool testRateCodingFidelity(double expectedRate, double actualRate);
    
private:
    // Real-time simulation utilities
    void generateSpikingActivity(std::vector<double>& spikes, size_t count, double rate);
    void generateSynapticChanges(std::vector<double>& changes, size_t count);
    
    // Performance monitoring
    bool measureLatency(const std::function<void()>& operation, double maxLatency) const;
};

/**
 * Component integration validator
 */
class ComponentIntegrationValidator {
public:
    ComponentIntegrationValidator() = default;
    ~ComponentIntegrationValidator() = default;
    
    // Integration validation tests
    bool validateBrainComponentIntegration() const;
    bool validateMemoryComponentIntegration() const;
    bool validateDynamicsComponentIntegration() const;
    bool validateEnvironmentComponentIntegration() const;
    bool validateNeuromodulationComponentIntegration() const;
    bool validateDevelopmentComponentIntegration() const;
    
    // Cross-component communication tests
    bool testBrainToMemoryCommunication() const;
    bool testMemoryToBrainCommunication() const;
    bool testBrainToDynamicsCommunication() const;
    bool testDynamicsToBrainCommunication() const;
    
    // Data consistency validation
    bool validateMemoryStateConsistency() const;
    bool validateBrainStateConsistency() const;
    bool validateEnvironmentStateConsistency() const;
    
private:
    // Validation utilities
    template<typename T>
    bool compareStates(const T& state1, const T& state2, double tolerance) const {
        return std::abs(state1 - state2) <= tolerance;
    }
    
    bool deepMemoryValidation() const;
    bool deepBrainValidation() const;
    bool deepEnvironmentValidation() const;
};

} // namespace testing
} // namespace nlm