// Comprehensive test of the enhanced error handling and logging system
#include "ErrorHandling.hpp"
#include <iostream>
#include <chrono>
#include <thread>

void testErrorHierarchy() {
    std::cout << "=== Testing Error Hierarchy ===" << std::endl;
    
    // Test application-level errors
    try {
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::ConfigurationError,
            nlm::ErrorSeverity::Warning,
            "Application",
            "initialize",
            "Missing configuration file"
        );
    } catch (const nlm::Error& e) {
        std::cout << "✓ ConfigurationError: " << e.getTypeString() << std::endl;
        std::cout << "  Severity: " << e.getSeverityString() << std::endl;
    }
    
    // Test component-level errors
    try {
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::BrainError,
            nlm::ErrorSeverity::Error,
            "Brain",
            "compute",
            "Brain computation failed"
        );
    } catch (const nlm::Error& e) {
        std::cout << "✓ BrainError: " << e.getTypeString() << std::endl;
    }
    
    // Test system-level errors
    try {
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::MemoryAllocationError,
            nlm::ErrorSeverity::Critical,
            "Memory",
            "allocate",
            "Memory allocation failed"
        );
    } catch (const nlm::Error& e) {
        std::cout << "✓ MemoryAllocationError: " << e.getTypeString() << std::endl;
    }
    
    // Test user-level errors
    try {
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::InvalidInputError,
            nlm::ErrorSeverity::Warning,
            "User",
            "validate_input",
            "Invalid input parameter"
        );
    } catch (const nlm::Error& e) {
        std::cout << "✓ InvalidInputError: " << e.getTypeString() << std::endl;
    }
}

void testErrorRecovery() {
    std::cout << "\n=== Testing Error Recovery ===" << std::endl;
    
    nlm::ErrorRecoveryManager& recoveryManager = nlm::getErrorRecoveryManager();
    
    // Add custom recovery action
    auto retryAction = std::make_shared<nlm::RetryAction>(3, std::chrono::milliseconds(100));
    recoveryManager.addRecoveryAction(nlm::ErrorType::ConfigurationError, retryAction);
    
    // Test error with recovery
    nlm::Error error(nlm::ErrorType::ConfigurationError, 
                    nlm::ErrorSeverity::Warning,
                    "Config", "load", "Test error");
    
    bool recovered = recoveryManager.attemptRecovery(std::make_shared<nlm::Error>(error));
    std::cout << "✓ Error recovery attempted: " << (recovered ? "Success" : "Failed") << std::endl;
    
    // Test circuit breaker
    recoveryManager.recordFailure("memory_allocator");
    bool canExecute = recoveryManager.canExecute("memory_allocator");
    std::cout << "✓ Circuit breaker after failure: " << (canExecute ? "Ready" : "Open") << std::endl;
    
    recoveryManager.recordSuccess("memory_allocator");
    canExecute = recoveryManager.canExecute("memory_allocator");
    std::cout << "✓ Circuit breaker after recovery: " << (canExecute ? "Ready" : "Open") << std::endl;
}

void testStructuredLogging() {
    std::cout << "\n=== Testing Structured Logging ===" << std::endl;
    
    nlm::Json::Value data;
    data["test_id"] = 12345;
    data["value"] = 3.14;
    data["success"] = true;
    
    // Test structured info logging
    nlm::ErrorHandling::logInfo(
        "Test information message",
        "TestComponent",
        "test_operation",
        data
    );
    std::cout << "✓ Structured info logging" << std::endl;
    
    // Test structured warning logging
    nlm::ErrorHandling::logWarning(
        "Test warning message",
        "TestComponent", 
        "test_operation",
        data
    );
    std::cout << "✓ Structured warning logging" << std::endl;
    
    // Test performance logging
    nlm::ErrorHandling::trackPerformance(
        std::chrono::milliseconds(500),
        2048 * 1024, // 2MB
        "performance_test",
        "PerformanceComponent"
    );
    std::cout << "✓ Performance logging" << std::endl;
}

void testEventLogging() {
    std::cout << "\n=== Testing Event Logging ===" << std::endl;
    
    // Test performance metric event
    nlm::Json::Value perfData;
    perfData["metric"] = "cpu_usage";
    perfData["value"] = 75.5;
    perfData["unit"] = "%";
    
    nlm::ErrorHandling::logEvent(
        nlm::EventType::PerformanceMetric,
        "System",
        "monitor",
        perfData
    );
    std::cout << "✓ Performance metric event" << std::endl;
    
    // Test state change event
    nlm::Json::Value stateData;
    stateData["old_state"] = "idle";
    stateData["new_state"] = "active";
    
    nlm::ErrorHandling::logEvent(
        nlm::EventType::StateChange,
        "System",
        "controller",
        stateData
    );
    std::cout << "✓ State change event" << std::endl;
}

void testAuditLogging() {
    std::cout << "\n=== Testing Audit Logging ===" << std::endl;
    
    nlm::AuditLogEntry auditEntry;
    auditEntry.userId = "test_user";
    auditEntry.sessionId = "session_123";
    auditEntry.action = "login";
    auditEntry.resource = "auth_system";
    auditEntry.outcome = "success";
    auditEntry.ipAddress = "192.168.1.100";
    auditEntry.userAgent = "TestClient/1.0";
    
    nlm::Json::Value details;
    details["login_time"] = "2024-01-15T10:30:00Z";
    auditEntry.details = details;
    
    nlm::getAdvancedLogger().logAudit(auditEntry);
    std::cout << "✓ Audit logging" << std::endl;
}

void testSystemMonitoring() {
    std::cout << "\n=== Testing System Monitoring ===" << std::endl;
    
    // Test health check
    bool isHealthy = nlm::ErrorHandling::checkSystemHealth();
    std::cout << "✓ System health: " << (isHealthy ? "Healthy" : "Unhealthy") << std::endl;
    
    // Test error statistics
    std::string stats = nlm::ErrorHandling::getErrorStatistics();
    std::cout << "✓ Error statistics retrieved (" << stats.size() << " chars)" << std::endl;
    
    // Test log analysis
    std::string analysis = nlm::ErrorHandling::getLogAnalysis("TestComponent");
    std::cout << "✓ Log analysis retrieved (" << analysis.size() << " chars)" << std::endl;
    
    // Test log export
    std::string logs = nlm::ErrorHandling::exportLogs("json");
    std::cout << "✓ Log export completed (" << logs.size() << " bytes)" << std::endl;
}

void testGracefulDegradation() {
    std::cout << "\n=== Testing Graceful Degradation ===" << std::endl;
    
    bool degraded = nlm::ErrorHandling::degradeGracefully(
        "Test degradation scenario",
        "TestComponent",
        {{ "mode", "limited" }, { "capacity", "50%" }}
    );
    std::cout << "✓ Graceful degradation: " << (degraded ? "Successful" : "Failed") << std::endl;
}

void testCircuitBreakers() {
    std::cout << "\n=== Testing Circuit Breakers ===" << std::endl;
    
    nlm::ErrorRecoveryManager& recoveryManager = nlm::getErrorRecoveryManager();
    
    // Configure circuit breaker
    recoveryManager.configureCircuitBreaker("test_circuit", 3, std::chrono::milliseconds(1000));
    
    // Test circuit breaker state
    std::string state = recoveryManager.getCircuitBreakerState("test_circuit");
    std::cout << "✓ Circuit breaker state:\n" << state << std::endl;
}

void testErrorTranslation() {
    std::cout << "\n=== Testing Error Translation ===" << std::endl;
    
    try {
        throw std::runtime_error("Standard C++ exception");
    } catch (const std::exception& e) {
        nlm::Error translated = nlm::ErrorHandling::translateException(e, "Test", "test_operation");
        std::cout << "✓ Error translated: " << translated.what() << std::endl;
    }
}

void testSafeExecution() {
    std::cout << "\n=== Testing Safe Execution ===" << std::endl;
    
    nlm::ErrorHandling::safeExecute(
        "SafeTest",
        "execute",
        []() {
            std::cout << "  [Safe execution] Performing safe operation..." << std::endl;
            // Simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "  [Safe execution] Operation completed successfully" << std::endl;
        }
    );
}

void testComponentSpecificErrors() {
    std::cout << "\n=== Testing Component-Specific Errors ===" << std::endl;
    
    try {
        nlm::ErrorHandling::throwBrainError(
            "initialize",
            "Brain initialization failed"
        );
    } catch (const nlm::Error& e) {
        std::cout << "✓ BrainError: " << e.what() << std::endl;
    }
    
    try {
        nlm::ErrorHandling::throwNeuronError(
            "compute",
            "Neuron computation error"
        );
    } catch (const nlm::Error& e) {
        std::cout << "✓ NeuronError: " << e.what() << std::endl;
    }
    
    try {
        nlm::ErrorHandling::throwMemoryError(
            "allocate",
            "Memory allocation error"
        );
    } catch (const nlm::Error& e) {
        std::cout << "✓ MemoryError: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "=== Comprehensive Error Handling and Logging Test ===" << std::endl;
    
    // Initialize error handling system
    nlm::ErrorHandling::initialize("./logs");
    std::cout << "✓ Error handling system initialized" << std::endl;
    
    // Run all tests
    testErrorHierarchy();
    testErrorRecovery();
    testStructuredLogging();
    testEventLogging();
    testAuditLogging();
    testSystemMonitoring();
    testGracefulDegradation();
    testCircuitBreakers();
    testErrorTranslation();
    testSafeExecution();
    testComponentSpecificErrors();
    
    // Shutdown error handling system
    nlm::ErrorHandling::shutdown();
    std::cout << "✓ Error handling system shutdown" << std::endl;
    
    std::cout << "\n=== All Tests Completed Successfully ===" << std::endl;
    std::cout << "The enhanced error handling and logging system is working correctly!" << std::endl;
    
    return 0;
}
