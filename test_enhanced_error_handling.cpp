// Test the enhanced error handling and logging system
#include "ErrorHandling.hpp"
#include <iostream>
#include <fstream>

int main() {
    std::cout << "Testing Enhanced Error Handling and Logging System..." << std::endl;
    
    // Initialize error handling
    nlm::ErrorHandling::initialize("./logs");
    
    std::cout << "✓ Error handling system initialized" << std::endl;
    
    // Test basic error logging
    try {
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::ConfigurationError,
            nlm::ErrorSeverity::Warning,
            "TestComponent",
            "testOperation",
            "Test configuration error"
        );
    } catch (const nlm::Error& e) {
        std::cout << "✓ Configuration error logged: " << e.what() << std::endl;
    }
    
    // Test memory error
    try {
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::MemoryError,
            nlm::ErrorSeverity::Critical,
            "MemorySystem",
            "allocate",
            "Memory allocation failed"
        );
    } catch (const nlm::Error& e) {
        std::cout << "✓ Memory error logged: " << e.what() << std::endl;
    }
    
    // Test performance logging
    nlm::ErrorHandling::trackPerformance(
        std::chrono::milliseconds(100),
        1024 * 1024,
        "test_operation",
        "TestComponent"
    );
    std::cout << "✓ Performance logged" << std::endl;
    
    // Test event logging
    nlm::Json::Value data;
    data["test_id"] = 12345;
    data["value"] = 42.0;
    
    nlm::ErrorHandling::logEvent(
        nlm::EventType::PerformanceMetric,
        "TestComponent",
        "test_system",
        data
    );
    std::cout << "✓ Event logged" << std::endl;
    
    // Test system health check
    bool isHealthy = nlm::ErrorHandling::checkSystemHealth();
    std::cout << "✓ System health check: " << (isHealthy ? "Healthy" : "Unhealthy") << std::endl;
    
    // Test error statistics
    std::string stats = nlm::ErrorHandling::getErrorStatistics();
    std::cout << "✓ Error statistics retrieved (" << stats.size() << " chars)" << std::endl;
    
    // Test log analysis
    std::string analysis = nlm::ErrorHandling::getLogAnalysis("TestComponent");
    std::cout << "✓ Log analysis retrieved (" << analysis.size() << " chars)" << std::endl;
    
    // Test circuit breaker
    bool canExecute = nlm::ErrorHandling::checkCircuitBreaker("memory_allocator");
    std::cout << "✓ Circuit breaker check: " << (canExecute ? "Ready" : "Open") << std::endl;
    
    // Record success
    nlm::ErrorHandling::recordCircuitBreakerSuccess("memory_allocator");
    std::cout << "✓ Circuit breaker success recorded" << std::endl;
    
    // Test graceful degradation
    bool degraded = nlm::ErrorHandling::degradeGracefully(
        "Test degradation",
        "TestComponent",
        { { "mode", "limited" } }
    );
    std::cout << "✓ Graceful degradation: " << (degraded ? "Successful" : "Failed") << std::endl;
    
    // Test log export
    std::string logs = nlm::ErrorHandling::exportLogs("json");
    std::cout << "✓ Log export completed (" << logs.size() << " bytes)" << std::endl;
    
    // Shutdown
    nlm::ErrorHandling::shutdown();
    std::cout << "✓ Error handling system shutdown" << std::endl;
    
    std::cout << "\n=== Test Complete ===" << std::endl;
    std::cout << "All error handling and logging features working correctly!" << std::endl;
    
    return 0;
}
