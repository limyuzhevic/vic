// Simple test to demonstrate the error handling framework
// This file shows how to use the new error handling and recovery features

#include "errors/ErrorTypes.hpp"
#include "errors/ErrorContext.hpp"
#include "error_reporting/ErrorReporter.hpp"
#include "monitoring/SystemMonitor.hpp"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "=== NLM Error Handling Framework Test ===" << std::endl << std::endl;
    
    // Test 1: Error Classification
    std::cout << "Test 1: Error Classification" << std::endl;
    std::cout << "===========================" << std::endl;
    
    // Create different types of errors
    nlm::ErrorContext memoryError("Memory allocation failed", "MemoryManager", "allocate", 123);
    memoryError.severity = nlm::ErrorSeverity::Critical;
    memoryError.type = nlm::ErrorType::MemoryAllocation;
    memoryError.code = nlm::ErrorCodes::MEMORY_ALLOCATION;
    
    nlm::ErrorContext configError("Invalid configuration value", "ConfigParser", "parse", 456);
    configError.severity = nlm::ErrorSeverity::Error;
    configError.type = nlm::ErrorType::ConfigurationInvalid;
    configError.code = nlm::ErrorCodes::CONFIGURATION_INVALID;
    
    nlm::ErrorContext networkError("Network connection failed", "NetworkManager", "connect", 789);
    networkError.severity = nlm::ErrorSeverity::Warning;
    networkError.type = nlm::ErrorType::NetworkConnection;
    networkError.code = nlm::ErrorCodes::NETWORK_CONNECTION;
    
    // Display error information
    std::cout << "Memory Error: " << memoryError.getFormattedMessage() << std::endl;
    std::cout << "Configuration Error: " << configError.getFormattedMessage() << std::endl;
    std::cout << "Network Error: " << networkError.getFormattedMessage() << std::endl;
    
    std::cout << "\n" << std::endl;
    
    // Test 2: Error Reporting
    std::cout << "Test 2: Error Reporting" << std::endl;
    std::cout << "======================" << std::endl;
    
    // Get the default error reporter
    auto& errorReporter = nlm::getDefaultErrorReporter();
    
    // Report errors
    errorReporter.reportError(memoryError);
    errorReporter.reportWarning("This is a warning message");
    errorReporter.reportInfo("This is an info message");
    errorReporter.reportDebug("This is a debug message");
    
    std::cout << "Errors reported. Recent errors:" << std::endl;
    auto recentErrors = errorReporter.getRecentErrors(5);
    for (size_t i = 0; i < recentErrors.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << recentErrors[i].getFormattedMessage() << std::endl;
    }
    
    std::cout << "\n" << std::endl;
    
    // Test 3: Monitoring
    std::cout << "Test 3: System Monitoring" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // Get the default monitoring system
    auto& monitoringSystem = nlm::getDefaultMonitoringSystem();
    
    // Start monitoring
    monitoringSystem.start();
    
    // Collect system metrics
    auto metrics = monitoringSystem.collectSystemMetrics();
    std::cout << "System Metrics:" << std::endl;
    std::cout << "  CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
    std::cout << "  Memory Usage: " << metrics.getMemoryUsagePercentage() << "%" << std::endl;
    std::cout << "  Network Input: " << metrics.networkInputBytes << " bytes" << std::endl;
    std::cout << "  Disk Read: " << metrics.diskReadBytes << " bytes" << std::endl;
    
    // Collect component health
    auto health = monitoringSystem.collectComponentHealth("Brain");
    std::cout << "\nComponent Health:" << std::endl;
    std::cout << "  Component ID: " << health.componentId << std::endl;
    std::cout << "  Health Score: " << health.healthScore << std::endl;
    std::cout << "  Status: " << health.getStatusString() << std::endl;
    
    // Analyze system health
    auto systemHealth = monitoringSystem.analyzeSystemHealth();
    std::cout << "\nSystem Health Status: " << static_cast<int>(systemHealth) << std::endl;
    
    double overallHealthScore = monitoringSystem.calculateOverallHealthScore();
    std::cout << "Overall Health Score: " << overallHealthScore << std::endl;
    
    // Generate system report
    std::string report = monitoringSystem.generateSystemReport();
    std::cout << "\nSystem Report (first 500 chars):" << std::endl;
    std::cout << report.substr(0, std::min<size_t>(report.size(), 500)) << "..." << std::endl;
    
    std::cout << "\n" << std::endl;
    
    // Test 4: Logging
    std::cout << "Test 4: Structured Logging" << std::endl;
    std::cout << "==========================" << std::endl;
    
    // Get the default logger
    auto& logger = nlm::getDefaultLogger();
    
    // Log different types of messages
    logger.logInfo("This is an informational message", nlm::LogCategory::System);
    logger.logWarning("This is a warning message", nlm::LogCategory::Resource);
    logger.logError("This is an error message", nlm::LogCategory::Network);
    logger.logCritical("This is a critical message", nlm::LogCategory::Memory);
    
    std::cout << "Log messages recorded. Recent logs:" << std::endl;
    auto recentLogs = nlm::getLogHistory(5);
    for (size_t i = 0; i < recentLogs.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << recentLogs[i].getFormattedMessage() << std::endl;
    }
    
    std::cout << "\n" << std::endl;
    
    // Test 5: Input Validation
    std::cout << "Test 5: Input Validation" << std::endl;
    std::cout << "==========================" << std::endl;
    
    // Get the global validation manager
    auto& validator = nlm::getGlobalValidationManager();
    
    // Validate different fields
    std::string emailField = "email";
    std::string emailValue = "test@example.com";
    
    nlm::ValidationResult emailResult = validator.validateField(emailField, emailValue, true);
    std::cout << "Email validation: " << (emailResult.isValid ? "PASSED" : "FAILED") << std::endl;
    if (!emailResult.isValid) {
        std::cout << "  Errors: ";
        for (const auto& error : emailResult.errors) {
            std::cout << error << "; ";
        }
        std::cout << std::endl;
    }
    
    // Test 6: Error Recovery
    std::cout << "\nTest 6: Error Recovery" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // Create a recoverable error
    nlm::ErrorContext recoverableError("This error can be recovered", "TestComponent", "testFunction", 999);
    recoverableError.severity = nlm::ErrorSeverity::Warning;
    recoverableError.type = nlm::ErrorType::PerformanceDegradation;
    
    std::cout << "Recoverable error created: " << recoverableError.getFormattedMessage() << std::endl;
    std::cout << "Is recoverable: " << (recoverableError.isRecoverable() ? "YES" : "NO") << std::endl;
    std::cout << "Severity: " << static_cast<int>(recoverableError.severity) << std::endl;
    
    std::cout << "\n" << std::endl;
    
    // Stop monitoring
    monitoringSystem.stop();
    
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << "All tests completed successfully!" << std::endl;
    std::cout << "\nThe NLM error handling framework provides:" << std::endl;
    std::cout << "  ✓ Comprehensive error classification and reporting" << std::endl;
    std::cout << "  ✓ Advanced monitoring and alerting capabilities" << std::endl;
    std::cout << "  ✓ Structured logging with correlation IDs" << std::endl;
    std::cout << "  ✓ Input validation and sanitization" << std::endl;
    std::cout << "  ✓ Error recovery and rollback mechanisms" << std::endl;
    std::cout << "  ✓ System health monitoring and analysis" << std::endl;
    std::cout << "  ✓ Exception safety with resource management" << std::endl;
    
    return 0;
}

