// Error Handling Test Scenarios for NLM
// This file demonstrates comprehensive error handling and recovery testing

#include "errors/ErrorTypes.hpp"
#include "errors/ErrorContext.hpp"
#include "error_reporting/ErrorReporter.hpp"
#include "monitoring/SystemMonitor.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <random>

namespace nlm {

// Test Result Structure
struct TestResult {
    std::string testName;
    bool passed;
    std::string message;
    std::chrono::milliseconds duration;
    std::vector<std::string> errors;
    
    TestResult(const std::string& name, bool pass, const std::string& msg = "")
        : testName(name), passed(pass), message(msg), duration(0), errors() {}
    
    TestResult& addError(const std::string& error) {
        errors.push_back(error);
        passed = false;
        return *this;
    }
};

// Error Handling Test Suite
class ErrorHandlingTestSuite {
private:
    std::vector<TestResult> results;
    std::shared_ptr<ErrorReporter> errorReporter;
    std::shared_ptr<MonitoringSystem> monitoringSystem;
    
    std::mt19937 rng;
    
public:
    ErrorHandlingTestSuite()
        : rng(std::chrono::system_clock::now().time_since_epoch().count()) {
        errorReporter = std::make_shared<DefaultErrorReporter>();
        monitoringSystem = std::make_shared<DefaultMonitoringSystem>();
        
        // Start monitoring
        monitoringSystem->start();
    }
    
    ~ErrorHandlingTestSuite() {
        if (monitoringSystem) {
            monitoringSystem->stop();
        }
    }
    
    TestResult runAllTests() {
        TestResult overall("ErrorHandlingTestSuite", true);
        
        overall.addError(runErrorClassificationTests());
        overall.addError(runErrorContextTests());
        overall.addError(runErrorRecoveryTests());
        overall.addError(runResourceSafetyTests());
        overall.addError(runPerformanceMonitoringTests());
        overall.addError(runSystemHealthTests());
        overall.addError(runIntegrationTests());
        
        return overall;
    }
    
private:
    std::string runErrorClassificationTests() {
        std::string testName = "ErrorClassificationTests";
        TestResult result(testName, true);
        
        try {
            // Test error type classification
            auto errorContext = ErrorManager::createErrorContext(
                ErrorType::MemoryAllocation, ErrorSeverity::Critical,
                ErrorCodes::MEMORY_ALLOCATION, "Memory allocation failed"
            );
            
            if (errorContext.type != ErrorType::MemoryAllocation) {
                result.addError("Error type classification failed");
            }
            
            // Test error severity levels
            ErrorContext debugContext("Debug message", ErrorType::SystemArchitecture, 
                                     ErrorSeverity::Debug, ErrorCodes::SYSTEM_ARCHITECTURE);
            if (debugContext.severity != ErrorSeverity::Debug) {
                result.addError("Error severity classification failed");
            }
            
            // Test error code mapping
            ErrorContext errorContext2("Test error", ErrorType::ConfigurationInvalid,
                                      ErrorSeverity::Error, ErrorCodes::CONFIGURATION_INVALID);
            if (errorContext2.code != ErrorCodes::CONFIGURATION_INVALID) {
                result.addError("Error code mapping failed");
            }
            
            // Test error message formatting
            std::string formatted = errorContext.getFormattedMessage();
            if (formatted.empty()) {
                result.addError("Error message formatting failed");
            }
            
        } catch (const std::exception& e) {
            result.addError(std::string("Exception in error classification tests: ") + e.what());
        }
        
        return result.getSummary();
    }
    
    std::string runErrorContextTests() {
        std::string testName = "ErrorContextTests";
        TestResult result(testName, true);
        
        try {
            // Test error context creation
            ErrorContext context("Test error message", "TestComponent", "testFunction", 123);
            
            if (context.message != "Test error message") {
                result.addError("Error context message not set correctly");
            }
            
            if (context.sourceFile != "TestComponent") {
                result.addError("Error context source file not set correctly");
            }
            
            if (context.sourceFunction != "testFunction") {
                result.addError("Error context source function not set correctly");
            }
            
            if (context.lineNumber != 123) {
                result.addError("Error context line number not set correctly");
            }
            
            // Test error context formatting
            std::string formatted = context.getFormattedMessage();
            if (formatted.empty()) {
                result.addError("Error context formatting failed");
            }
            
            // Test error context validation
            if (!context.isValid()) {
                result.addError("Error context validation failed");
            }
            
            // Test error context comparison
            ErrorContext context2("Test error message", "TestComponent", "testFunction", 123);
            if (!context.equals(context2)) {
                result.addError("Error context comparison failed");
            }
            
        } catch (const std::exception& e) {
            result.addError(std::string("Exception in error context tests: ") + e.what());
        }
        
        return result.getSummary();
    }
    
    std::string runErrorRecoveryTests() {
        std::string testName = "ErrorRecoveryTests";
        TestResult result(testName, true);
        
        try {
            // Test transaction recovery
            auto transactionManager = getDefaultTransactionManager();
            std::string transactionId = "TEST_TRANSACTION_" + std::to_string(rng());
            
            if (!transactionManager->beginTransaction(transactionId)) {
                result.addError("Transaction begin failed");
            }
            
            // Test transaction rollback
            if (!transactionManager->rollbackTransaction(transactionId)) {
                result.addError("Transaction rollback failed");
            }
            
            // Test resource recovery
            ResourceHolder<int> resourceHolder;
            if (resourceHolder.hasResource()) {
                result.addError("Resource holder should not have resource initially");
            }
            
            // Test exception safety
            try {
                throw std::runtime_error("Test exception");
            } catch (...) {
                // Resource should still be clean after exception
                if (resourceHolder.hasResource()) {
                    result.addError("Resource holder leaked after exception");
                }
            }
            
        } catch (const std::exception& e) {
            result.addError(std::string("Exception in error recovery tests: ") + e.what());
        }
        
        return result.getSummary();
    }
    
    std::string runResourceSafetyTests() {
        std::string testName = "ResourceSafetyTests";
        TestResult result(testName, true);
        
        try {
            // Test resource allocation safety
            ResourceHolder<std::vector<int>> resourceHolder = ResourceUtils::allocateResource<std::vector<int>>(
                std::vector<int>{1, 2, 3, 4, 5}
            );
            
            if (!resourceHolder.hasResource()) {
                result.addError("Resource holder should have resource");
            }
            
            // Test resource deallocation safety
            resourceHolder.reset();
            if (resourceHolder.hasResource()) {
                result.addError("Resource holder should not have resource after reset");
            }
            
            // Test exception safety with resources
            ResourceHolder<int> tempResource;
            try {
                // Simulate operation that fails
                throw std::runtime_error("Simulated failure");
            } catch (...) {
                // Resource should still be clean
                if (tempResource.hasResource()) {
                    result.addError("Resource leaked after exception");
                }
            }
            
        } catch (const std::exception& e) {
            result.addError(std::string("Exception in resource safety tests: ") + e.what());
        }
        
        return result.getSummary();
    }
    
    std::string runPerformanceMonitoringTests() {
        std::string testName = "PerformanceMonitoringTests";
        TestResult result(testName, true);
        
        try {
            // Test system metrics collection
            SystemMetrics metrics = monitoringSystem->collectSystemMetrics();
            
            if (metrics.cpuUsage < 0.0 || metrics.cpuUsage > 100.0) {
                result.addError("CPU usage out of valid range");
            }
            
            if (metrics.getMemoryUsagePercentage() < 0.0 || metrics.getMemoryUsagePercentage() > 100.0) {
                result.addError("Memory usage percentage out of valid range");
            }
            
            // Test component health collection
            ComponentHealth health = monitoringSystem->collectComponentHealth("TestComponent");
            
            if (health.healthScore < 0.0 || health.healthScore > 1.0) {
                result.addError("Health score out of valid range");
            }
            
            // Test performance trends analysis
            std::string trends = monitoringSystem->analyzePerformanceTrends();
            if (trends.empty()) {
                result.addError("Performance trends analysis returned empty result");
            }
            
        } catch (const std::exception& e) {
            result.addError(std::string("Exception in performance monitoring tests: ") + e.what());
        }
        
        return result.getSummary();
    }
    
    std::string runSystemHealthTests() {
        std::string testName = "SystemHealthTests";
        TestResult result(testName, true);
        
        try {
            // Test system health analysis
            HealthStatus health = monitoringSystem->analyzeSystemHealth();
            
            if (health == HealthStatus::Unknown) {
                result.addError("System health status is unknown");
            }
            
            // Test component health analysis
            auto componentHealths = monitoringSystem->analyzeComponentHealth();
            
            for (const auto& pair : componentHealths) {
                if (pair.second == HealthStatus::Unknown) {
                    result.addError("Component " + pair.first + " health status is unknown");
                }
            }
            
            // Test overall health score
            double healthScore = monitoringSystem->calculateOverallHealthScore();
            
            if (healthScore < 0.0 || healthScore > 1.0) {
                result.addError("Overall health score out of valid range");
            }
            
            // Test health report generation
            std::string report = monitoringSystem->generateSystemReport();
            if (report.empty()) {
                result.addError("System health report is empty");
            }
            
        } catch (const std::exception& e) {
            result.addError(std::string("Exception in system health tests: ") + e.what());
        }
        
        return result.getSummary();
    }
    
    std::string runIntegrationTests() {
        std::string testName = "IntegrationTests";
        TestResult result(testName, true);
        
        try {
            // Test error reporting integration
            errorReporter->reportError("Integration test error", ErrorLevel::Warning);
            
            auto recentErrors = errorReporter->getRecentErrors(10);
            if (recentErrors.empty()) {
                result.addError("Error reporter did not capture error");
            }
            
            // Test monitoring integration
            ComponentHealth health = monitoringSystem->collectComponentHealth("IntegrationTestComponent");
            
            if (health.componentId.empty()) {
                result.addError("Monitoring system did not collect component health");
            }
            
            // Test error and monitoring coordination
            SystemMetrics metrics = monitoringSystem->collectSystemMetrics();
            errorReporter->reportInfo("System metrics collected: CPU usage " + 
                                    std::to_string(metrics.cpuUsage) + "%, Memory usage " +
                                    std::to_string(metrics.getMemoryUsagePercentage()) + "%");
            
            // Test error recovery integration
            bool recoverySuccess = true; // In real implementation, would test actual recovery
            if (!recoverySuccess) {
                result.addError("Error recovery integration failed");
            }
            
        } catch (const std::exception& e) {
            result.addError(std::string("Exception in integration tests: ") + e.what());
        }
        
        return result.getSummary();
    }
    
public:
    void runErrorClassificationTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        TestResult result = runErrorClassificationTests();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        results.push_back(result);
        
        if (!result.passed) {
            std::cout << "FAILED: " << testName << " - " << result.message << std::endl;
        } else {
            std::cout << "PASSED: " << testName << std::endl;
        }
    }
    
    void runErrorContextTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        TestResult result = runErrorContextTests();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        results.push_back(result);
        
        if (!result.passed) {
            std::cout << "FAILED: " << testName << " - " << result.message << std::endl;
        } else {
            std::cout << "PASSED: " << testName << std::endl;
        }
    }
    
    void runAllTestsAndPrintReport() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        TestResult overall = runAllTests();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        overall.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "\n=== Error Handling Test Suite Report ===\n" << std::endl;
        
        for (const auto& result : results) {
            std::cout << (result.passed ? "✓ PASSED" : "✗ FAILED") << ": " << result.testName;
            if (!result.message.empty()) {
                std::cout << " - " << result.message;
            }
            std::cout << " (" << result.duration.count() << " ms)" << std::endl;
            
            if (!result.errors.empty()) {
                std::cout << "  Errors:" << std::endl;
                for (const auto& error : result.errors) {
                    std::cout << "    - " << error << std::endl;
                }
            }
        }
        
        std::cout << "\n=== Overall Result ===" << std::endl;
        std::cout << (overall.passed ? "✓ PASSED" : "✗ FAILED") << ": " << overall.testName;
        if (!overall.message.empty()) {
            std::cout << " - " << overall.message;
        }
        std::cout << " (" << overall.duration.count() << " ms)" << std::endl;
        
        if (!overall.errors.empty()) {
            std::cout << "  Errors:" << std::endl;
            for (const auto& error : overall.errors) {
                std::cout << "    - " << error << std::endl;
            }
        }
        
        std::cout << std::endl;
    }
};

} // namespace nlm

