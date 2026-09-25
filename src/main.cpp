// Enhanced Error Handling and Diagnostics Testing for NLM
// Comprehensive test suite for error handling and diagnostics system

#include "core/EnhancedErrorHandling.hpp"
#include "core/Diagnostics.hpp"
#include "core/PerformanceAnalysis.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>

using namespace nlm;

namespace test_enhanced_error_handling {

// Helper functions
static std::string errorTypeToString(NLMErrors type) {
    static const std::vector<std::string> names = {
        "ConfigParsing", "FileIO", "MemoryAllocation", "InvalidParameter",
        "SystemNotInitialized", "PlasticityFailed", "NeuronInvalid",
        "SynapseInvalid", "CallbackError", "IntegrationFailed",
        "DevelopmentError", "ValidationError", "UnknownError"
    };
    return type < names.size() ? names[static_cast<size_t>(type)] : "UNKNOWN";
}

static std::string errorSeverityToString(ErrorSeverity severity) {
    static const std::vector<std::string> names = {
        "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "FATAL"
    };
    return severity < names.size() ? names[static_cast<size_t>(severity)] : "UNKNOWN";
}

static std::string errorCategoryToString(ErrorCategory category) {
    static const std::vector<std::string> names = {
        "SYSTEM", "CONFIGURATION", "MEMORY", "NEURAL", "PLASTICITY",
        "SENSORY", "MOTOR", "INTEGRATION", "DEVELOPMENT", "EXTERNAL"
    };
    return category < names.size() ? names[static_cast<size_t>(category)] : "UNKNOWN";
}

// Test 1: Enhanced error handling framework
void testEnhancedErrorFramework() {
    std::cout << "Running Test 1: Enhanced Error Handling Framework" << std::endl;
    
    // Clear errors
    getEnhancedErrorManager().clearErrors();
    
    // Test basic error creation
    ErrorContext ctx("TestComponent", "testFunction", "test.cpp", 42, "testFunc");
    
    EnhancedErrorInfo error1(NLMErrors::InvalidParameter, "Test parameter validation error", 
                           ErrorSeverity::Error, ErrorCategory::System);
    error1.context = ctx;
    
    getEnhancedErrorManager().addError(error1);
    assert(getEnhancedErrorManager().getErrorCount() == 1);
    
    // Test error categorization
    auto errorsByType = getEnhancedErrorManager().getErrorsByType(NLMErrors::InvalidParameter);
    assert(!errorsByType.empty());
    assert(errorsByType[0].context.component == "TestComponent");
    
    // Test error statistics
    assert(getEnhancedErrorManager().getErrorCount(NLMErrors::InvalidParameter) == 1);
    assert(getEnhancedErrorManager().getErrorRatePerMinute() > 0.0);
    
    // Test error reporting
    std::string report = getEnhancedErrorManager().generateErrorReport();
    assert(report.find("Test parameter validation error") != std::string::npos);
    assert(report.find("InvalidParameter") != std::string::npos);
    
    // Clear for next test
    getEnhancedErrorManager().clearErrors();
    std::cout << "   Test 1 PASSED" << std::endl;
}

// Test 2: Error severity and category handling
void testErrorSeverityAndCategory() {
    std::cout << "Running Test 2: Error Severity and Category Handling" << std::endl;
    
    getEnhancedErrorManager().clearErrors();
    
    // Test different severity levels
    getEnhancedErrorManager().addError(NLMErrors::UnknownError, "Debug test message", 
                                      ErrorSeverity::Debug, ErrorCategory::System);
    getEnhancedErrorManager().addError(NLMErrors::UnknownError, "Info test message", 
                                      ErrorSeverity::Info, ErrorCategory::System);
    getEnhancedErrorManager().addError(NLMErrors::UnknownError, "Warning test message", 
                                      ErrorSeverity::Warning, ErrorCategory::System);
    getEnhancedErrorManager().addError(NLMErrors::UnknownError, "Error test message", 
                                      ErrorSeverity::Error, ErrorCategory::System);
    getEnhancedErrorManager().addError(NLMErrors::UnknownError, "Critical test message", 
                                      ErrorSeverity::Critical, ErrorCategory::System);
    getEnhancedErrorManager().addError(NLMErrors::UnknownError, "Fatal test message", 
                                      ErrorSeverity::Fatal, ErrorCategory::System);
    
    assert(getEnhancedErrorManager().getErrorCount() == 6);
    
    // Test error filtering by severity
    auto debugErrors = getEnhancedErrorManager().getErrorsBySeverity(ErrorSeverity::Debug);
    auto infoErrors = getEnhancedErrorManager().getErrorsBySeverity(ErrorSeverity::Info);
    auto warningErrors = getEnhancedErrorManager().getErrorsBySeverity(ErrorSeverity::Warning);
    auto errorErrors = getEnhancedErrorManager().getErrorsBySeverity(ErrorSeverity::Error);
    auto criticalErrors = getEnhancedErrorManager().getErrorsBySeverity(ErrorSeverity::Critical);
    auto fatalErrors = getEnhancedErrorManager().getErrorsBySeverity(ErrorSeverity::Fatal);
    
    assert(debugErrors.size() == 1);
    assert(infoErrors.size() == 1);
    assert(warningErrors.size() == 1);
    assert(errorErrors.size() == 1);
    assert(criticalErrors.size() == 1);
    assert(fatalErrors.size() == 1);
    
    // Test error filtering by category
    auto systemErrors = getEnhancedErrorManager().getErrorsByCategory(ErrorCategory::System);
    assert(systemErrors.size() == 6);
    
    // Clear for next test
    getEnhancedErrorManager().clearErrors();
    std::cout << "   Test 2 PASSED" << std::endl;
}

// Test 3: Error context and metadata
void testErrorContextAndMetadata() {
    std::cout << "Running Test 3: Error Context and Metadata" << std::endl;
    
    getEnhancedErrorManager().clearErrors();
    
    // Create error with rich context
    ErrorContext ctx;
    ctx.component = "NeuralNetwork";
    ctx.operation = "forwardPropagation";
    ctx.file = "neural_network.cpp";
    ctx.line = 123;
    ctx.function = "forward";
    ctx.metadata["layer"] = "hidden";
    ctx.metadata["neuron_count"] = "1000";
    ctx.metadata["activation"] = "ReLU";
    
    EnhancedErrorInfo error(NLMErrors::NeuronInvalid, 
                          "Neuron computation failed during forward propagation",
                          ErrorSeverity::Error, ErrorCategory::Neural);
    error.context = ctx;
    error.canAutoRecover = true;
    error.canRetry = true;
    error.retryCount = 0;
    error.retryDelay = std::chrono::milliseconds(500);
    error.recoveryAction = "Reset neuron weights and retry";
    error.suggestedSteps = {"Check neuron activation values", "Reset weights", "Retry computation"};
    error.errorData["layer_id"] = "hidden_1";
    error.errorData["neuron_index"] = "42";
    
    getEnhancedErrorManager().addError(error);
    
    // Verify context preservation
    auto errors = getEnhancedErrorManager().getErrors();
    assert(!errors.empty());
    assert(errors[0].context.component == "NeuralNetwork");
    assert(errors[0].context.operation == "forwardPropagation");
    assert(errors[0].context.file == "neural_network.cpp");
    assert(errors[0].context.line == 123);
    assert(errors[0].context.function == "forward");
    assert(errors[0].context.metadata["layer"] == "hidden");
    assert(errors[0].context.metadata["neuron_count"] == "1000");
    assert(errors[0].canAutoRecover == true);
    assert(errors[0].canRetry == true);
    assert(errors[0].recoveryAction.has_value());
    assert(errors[0].suggestedSteps.size() == 3);
    
    // Clear for next test
    getEnhancedErrorManager().clearErrors();
    std::cout << "   Test 3 PASSED" << std::endl;
}

// Test 4: Error recovery mechanisms
void testErrorRecoveryMechanisms() {
    std::cout << "Running Test 4: Error Recovery Mechanisms" << std::endl;
    
    getEnhancedErrorManager().clearErrors();
    
    // Add a recovery callback
    int recoveryAttemptCount = 0;
    getEnhancedErrorManager().addRecoveryCallback(
        [&recoveryAttemptCount](const EnhancedErrorInfo& error) -> bool {
            recoveryAttemptCount++;
            if (error.type == NLMErrors::SystemNotInitialized) {
                return true; // Successfully recovered
            }
            return false; // Cannot recover
        }
    );
    
    // Test recoverable error
    ErrorContext ctx("System", "initialize", "system.cpp", 999, "System::initialize");
    EnhancedErrorInfo recoverableError(NLMErrors::SystemNotInitialized,
                                      "System not initialized, attempting recovery",
                                      ErrorSeverity::Warning, ErrorCategory::System);
    recoverableError.context = ctx;
    recoverableError.canAutoRecover = true;
    
    getEnhancedErrorManager().addError(recoverableError);
    assert(recoveryAttemptCount == 1);
    
    // Test non-recoverable error
    EnhancedErrorInfo nonRecoverableError(NLMErrors::MemoryAllocation,
                                         "Memory allocation failed",
                                         ErrorSeverity::Critical, ErrorCategory::Memory);
    nonRecoverableError.context = ctx;
    nonRecoverableError.canAutoRecover = false;
    
    getEnhancedErrorManager().addError(nonRecoverableError);
    assert(recoveryAttemptCount == 1); // Should not increase for non-recoverable error
    
    // Verify errors were collected
    assert(getEnhancedErrorManager().getErrorCount() == 2);
    
    // Clear for next test
    getEnhancedErrorManager().clearErrors();
    std::cout << "   Test 4 PASSED" << std::endl;
}

// Test 5: Diagnostics system
void testDiagnosticsSystem() {
    std::cout << "Running Test 5: Diagnostics System" << std::endl;
    
    // Initialize system monitor
    getSystemMonitor().initialize();
    
    // Add health check
    getSystemMonitor().addHealthCheck(
        []() -> bool { return true; },
        "Test Health Check"
    );
    
    // Add performance callback
    int performanceCallCount = 0;
    getSystemMonitor().addPerformanceCallback(
        [&performanceCallCount](const PerformanceMetrics& metrics) {
            performanceCallCount++;
        }
    );
    
    // Add component callback
    std::string componentCallbackCount = "0";
    getSystemMonitor().addComponentCallback(
        [&componentCallbackCount](const std::string& name, const ComponentHealth& health) {
            componentCallbackCount = std::to_string(std::stoi(componentCallbackCount) + 1);
        }
    );
    
    // Perform health check
    HealthStatus status = getSystemMonitor().performHealthCheck();
    assert(status.isHealthy == true);
    assert(status.warnings.empty());
    
    // Update component health
    ComponentHealth testHealth("TestComponent", true, "Test component operational");
    getSystemMonitor().updateComponentHealth("TestComponent", testHealth);
    
    // Record performance metrics
    PerformanceMetrics testMetrics;
    testMetrics.cpuUsage = 50.0;
    testMetrics.memoryUsage = 1024.0;
    getSystemMonitor().recordPerformanceMetrics(testMetrics);
    
    // Get diagnostic info
    DiagnosticInfo diag = getSystemMonitor().getDiagnosticInfo();
    assert(diag.componentHealth.count("TestComponent") == 1);
    
    // Verify callbacks were called
    assert(performanceCallCount > 0);
    assert(std::stoi(componentCallbackCount) > 0);
    
    // Shutdown system monitor
    getSystemMonitor().shutdown();
    
    // Clear for next test
    getEnhancedErrorManager().clearErrors();
    std::cout << "   Test 5 PASSED" << std::endl;
}

// Test 6: Performance analysis
void testPerformanceAnalysis() {
    std::cout << "Running Test 6: Performance Analysis" << std::endl;
    
    // Clear existing profiles
    getPerformanceProfiler().clearProfiles();
    
    // Create and run a profile
    auto [result, profile] = getPerformanceProfiler().profileFunction<int>([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 42;
    }, "TestFunction");
    
    assert(result == 42);
    assert(profile.name == "TestFunction");
    assert(profile.duration.count() > 0.0);
    assert(profile.getSamplesPerSecond() > 0.0);
    
    // End profile
    getPerformanceProfiler().endProfile();
    
    // Analyze profile
    PerformanceAnalysis analysis = getPerformanceProfiler().analyzeProfile(profile);
    assert(analysis.profileName == "TestFunction");
    assert(analysis.totalTime > 0.0);
    assert(analysis.samplesPerSecond > 0.0);
    
    // Get all analyses
    auto analyses = getPerformanceProfiler().analyzeAllProfiles();
    assert(!analyses.empty());
    
    // Get specific analysis
    PerformanceAnalysis specificAnalysis = getPerformanceProfiler().getProfileAnalysis("TestFunction");
    assert(specificAnalysis.profileName == "TestFunction");
    
    // Clear for next test
    getEnhancedErrorManager().clearErrors();
    std::cout << "   Test 6 PASSED" << std::endl;
}

// Test 7: Error validation and filtering
void testErrorValidationAndFiltering() {
    std::cout << "Running Test 7: Error Validation and Filtering" << std::endl;
    
    getEnhancedErrorManager().clearErrors();
    
    // Add various errors
    ErrorContext ctx1("Component1", "func1", "file1.cpp", 10, "function1");
    getEnhancedErrorManager().addError(NLMErrors::InvalidParameter, "Invalid parameter in component 1", 
                                      ErrorSeverity::Error, ErrorCategory::System, ctx1);
    
    ErrorContext ctx2("Component2", "func2", "file2.cpp", 20, "function2");
    getEnhancedErrorManager().addError(NLMErrors::MemoryAllocation, "Memory allocation failed in component 2", 
                                      ErrorSeverity::Critical, ErrorCategory::Memory, ctx2);
    
    ErrorContext ctx3("Component1", "func3", "file1.cpp", 30, "function3");
    getEnhancedErrorManager().addError(NLMErrors::InvalidParameter, "Another parameter error in component 1", 
                                      ErrorSeverity::Warning, ErrorCategory::System, ctx3);
    
    // Test filtering by type
    auto paramErrors = getEnhancedErrorManager().getErrorsByType(NLMErrors::InvalidParameter);
    assert(paramErrors.size() == 2);
    
    // Test filtering by category
    auto systemErrors = getEnhancedErrorManager().getErrorsByCategory(ErrorCategory::System);
    assert(systemErrors.size() == 2);
    
    // Test filtering by severity
    auto errorSeverityErrors = getEnhancedErrorManager().getErrorsBySeverity(ErrorSeverity::Error);
    assert(errorSeverityErrors.size() == 1);
    
    // Test filtering with predicate
    auto component1Errors = getEnhancedErrorManager().filterErrors(
        [](const EnhancedErrorInfo& error) {
            return error.context.component == "Component1";
        }
    );
    assert(component1Errors.size() == 2);
    
    // Test time-based filtering
    auto now = std::chrono::system_clock::now();
    auto recentErrors = getEnhancedErrorManager().getErrorsInTimeRange(now - std::chrono::hours(1), now + std::chrono::hours(1));
    assert(recentErrors.size() == 3);
    
    // Clear for next test
    getEnhancedErrorManager().clearErrors();
    std::cout << "   Test 7 PASSED" << std::endl;
}

// Test 8: Error statistics and analysis
void testErrorStatisticsAndAnalysis() {
    std::cout << "Running Test 8: Error Statistics and Analysis" << std::endl;
    
    getEnhancedErrorManager().clearErrors();
    
    // Add test errors
    for (int i = 0; i < 5; ++i) {
        ErrorContext ctx("TestComponent", "testFunc", "test.cpp", i, "testFunc");
        getEnhancedErrorManager().addError(NLMErrors::InvalidParameter,
                                          "Test error " + std::to_string(i),
                                          ErrorSeverity::Error, ErrorCategory::System, ctx);
    }
    
    // Test basic statistics
    assert(getEnhancedErrorManager().getErrorCount() == 5);
    assert(getEnhancedErrorManager().getErrorCount(NLMErrors::InvalidParameter) == 5);
    assert(getEnhancedErrorManager().getErrorCount(ErrorSeverity::Error) == 5);
    
    // Test error rate calculation
    double errorRate = getEnhancedErrorManager().getErrorRatePerMinute();
    assert(errorRate > 0.0);
    
    // Test error report generation
    std::string report = getEnhancedErrorManager().generateErrorReport();
    assert(report.find("Total Errors: 5") != std::string::npos);
    assert(report.find("ErrorRate:") != std::string::npos);
    
    // Test error distribution
    auto errorsByType = getEnhancedErrorManager().getErrorsByType(NLMErrors::InvalidParameter);
    assert(errorsByType.size() == 5);
    
    // Clear for next test
    getEnhancedErrorManager().clearErrors();
    std::cout << "   Test 8 PASSED" << std::endl;
}

// Test 9: Configuration error handling
void testConfigurationErrorHandling() {
    std::cout << "Running Test 9: Configuration Error Handling" << std::endl;
    
    getEnhancedErrorManager().clearErrors();
    
    // Test configuration validation
    Config config;
    config.set("neuron_count", 100);
    config.set("region_count", 4);
    config.set("connection_probability", 0.1);
    
    // Test valid retrievals
    assert(config.get<int>("neuron_count").value_or(0) == 100);
    assert(config.get<int>("region_count").value_or(0) == 4);
    assert(config.get<double>("connection_probability").value_or(0.0) == 0.1);
    
    // Test type validation (should return nullopt for wrong type)
    assert(!config.get<std::string>("neuron_count").has_value());
    
    // Test parameter validation with error handling
    try {
        int invalidValue = -5;
        NLM_VALIDATE(invalidValue, invalidValue >= 0, NLMErrors::InvalidParameter,
                    "Configuration value must be non-negative");
        assert(false); // Should have thrown
    } catch (const NLMErrorManager::Exception& e) {
        assert(e.type == NLMErrors::InvalidParameter);
    }
    
    // Clear for next test
    getEnhancedErrorManager().clearErrors();
    std::cout << "   Test 9 PASSED" << std::endl;
}

// Test 10: Comprehensive integration test
void testComprehensiveIntegration() {
    std::cout << "Running Test 10: Comprehensive Integration Test" << std::endl;
    
    // Clear all systems
    getEnhancedErrorManager().clearErrors();
    getSystemMonitor().clearErrorCallbacks();
    getPerformanceProfiler().clearProfiles();
    
    // Initialize all systems
    getSystemMonitor().initialize();
    
    // Add comprehensive error handling
    getEnhancedErrorManager().addErrorCallback(
        [](const EnhancedErrorInfo& error) {
            // Error handling callback
        }
    );
    
    // Add performance monitoring
    getSystemMonitor().addPerformanceCallback(
        [](const PerformanceMetrics& metrics) {
            // Performance monitoring callback
        }
    );
    
    // Create and run a comprehensive test
    auto [result, profile] = getPerformanceProfiler().profileFunction<int>([]() {
        // Simulate some work
        Config config;
        config.set("test", 123);
        
        ErrorContext ctx("IntegrationTest", "main", "main.cpp", 1, "main");
        getEnhancedErrorManager().addError(NLMErrors::UnknownError,
                                          "Integration test error",
                                          ErrorSeverity::Info, ErrorCategory::System, ctx);
        
        return config.get<int>("test").value_or(0);
    }, "IntegrationTest");
    
    assert(result == 123);
    
    // Perform health check
    HealthStatus status = getSystemMonitor().performHealthCheck();
    assert(status.isHealthy);
    
    // Analyze profile
    PerformanceAnalysis analysis = getPerformanceProfiler().analyzeProfile(profile);
    assert(analysis.totalTime > 0.0);
    
    // Get comprehensive report
    std::string errorReport = getEnhancedErrorManager().generateErrorReport();
    std::string perfReport = getPerformanceReport();
    
    assert(!errorReport.empty());
    assert(!perfReport.empty());
    
    // Cleanup
    getSystemMonitor().shutdown();
    getEnhancedErrorManager().clearErrors();
    
    std::cout << "   Test 10 PASSED" << std::endl;
}

// Run all tests
void runAllTests() {
    std::cout << "=== NLM Enhanced Error Handling and Diagnostics Tests ===" << std::endl;
    std::cout << "Running comprehensive test suite for enhanced error handling..." << std::endl;
    
    testEnhancedErrorFramework();
    testErrorSeverityAndCategory();
    testErrorContextAndMetadata();
    testErrorRecoveryMechanisms();
    testDiagnosticsSystem();
    testPerformanceAnalysis();
    testErrorValidationAndFiltering();
    testErrorStatisticsAndAnalysis();
    testConfigurationErrorHandling();
    testComprehensiveIntegration();
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "✓ ALL TESTS PASSED" << std::endl;
    std::cout << "✓ Enhanced error handling framework validated" << std::endl;
    std::cout << "✓ Comprehensive error recovery and fallback mechanisms working" << std::endl;
    std::cout << "✓ Runtime diagnostics and monitoring operational" << std::endl;
    std::cout << "✓ Performance analysis and profiling functional" << std::endl;
    std::cout << "✓ Input validation and error handling integrated" << std::endl;
    std::cout << "✓ System health and performance monitoring active" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

} // namespace test_enhanced_error_handling

int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Error Handling Tests                            ║
    ║                                                               ║
    ║     Comprehensive test suite for enhanced error handling     ║
    ║     and diagnostics system                                    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    try {
        test_enhanced_error_handling::runAllTests();
    } catch (const std::exception& e) {
        std::cerr << "\n✗ TESTS FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
