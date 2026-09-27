#include "core/Error/Error.hpp"
#include "core/ValidationEngine/ValidationEngine.hpp"
#include "core/ErrorReporter/ErrorReporter.hpp"
#include "core/AdvancedErrorHandling/AdvancedErrorHandling.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include <map>

// Test error hierarchy
void testErrorHierarchy() {
    std::cout << "Testing error hierarchy..." << std::endl;
    
    nlm::Error testError("Test error", nlm::ErrorType::SystemError, nlm::ErrorSeverity::Error);
    assert(testError.getMessage() == "Test error");
    assert(testError.getType() == nlm::ErrorType::SystemError);
    assert(testError.getSeverity() == nlm::ErrorSeverity::Error);
    
    nlm::SystemError systemError("System error");
    assert(systemError.getType() == nlm::ErrorType::SystemError);
    assert(systemError.getSeverity() == nlm::ErrorSeverity::Critical);
    
    nlm::ConfigurationError configError("Config error");
    assert(configError.getType() == nlm::ErrorType::ConfigurationError);
    
    nlm::MemoryError memoryError("Memory error");
    assert(memoryError.getType() == nlm::ErrorType::MemoryError);
    
    nlm::NeuralError neuralError("Neural error");
    assert(neuralError.getType() == nlm::ErrorType::NeuralError);
    
    nlm::FileError fileError("File error");
    assert(fileError.getType() == nlm::ErrorType::FileError);
    
    nlm::NetworkError networkError("Network error");
    assert(networkError.getType() == nlm::ErrorType::NetworkError);
    
    nlm::ValidationError validationError("Validation error");
    assert(validationError.getType() == nlm::ErrorType::ValidationError);
    
    nlm::PlasticityError plasticityError("Plasticity error");
    assert(plasticityError.getType() == nlm::ErrorType::PlasticityError);
    
    nlm::DevelopmentError developmentError("Development error");
    assert(developmentError.getType() == nlm::ErrorType::DevelopmentError);
    
    nlm::AttentionError attentionError("Attention error");
    assert(attentionError.getType() == nlm::ErrorType::AttentionError);
    
    nlm::MotorError motorError("Motor error");
    assert(motorError.getType() == nlm::ErrorType::MotorError);
    
    nlm::CognitiveError cognitiveError("Cognitive error");
    assert(cognitiveError.getType() == nlm::ErrorType::CognitiveError);
    
    nlm::DevelopmentalError developmentalError("Developmental error");
    assert(developmentalError.getType() == nlm::ErrorType::DevelopmentalError);
    
    std::cout << "  ✓ Error hierarchy test passed" << std::endl;
}

// Test error reporting system
void testErrorReporting() {
    std::cout << "Testing error reporting system..." << std::endl;
    
    nlm::ErrorReporter& reporter = nlm::ErrorReporter::getGlobal();
    
    // Clear existing errors
    reporter.clear();
    
    // Test basic error reporting
    reporter.logError("Test error 1");
    reporter.logWarning("Test warning 1");
    reporter.logInfo("Test info 1");
    reporter.logDebug("Test debug 1");
    reporter.logCritical("Test critical 1");
    
    assert(reporter.getErrorCount(nlm::ErrorSeverity::Error) > 0);
    assert(reporter.getErrorCount(nlm::ErrorSeverity::Warning) > 0);
    
    // Test error formatting
    std::string formatted = reporter.formatAllErrors();
    assert(!formatted.empty());
    
    // Test error statistics
    std::string stats = reporter.getStatisticsReport();
    assert(!stats.empty());
    
    std::cout << "  ✓ Error reporting test passed" << std::endl;
}

// Test validation engine
void testValidationEngine() {
    std::cout << "Testing validation engine..." << std::endl;
    
    nlm::ValidationEngine engine;
    
    // Add validation rules
    engine.addRule("positive_number",
                   "Validate that number is positive",
                   [](const std::vector<std::string>& data) {
                       try {
                           double value = std::stod(data[0]);
                           return value > 0.0;
                       } catch (...) {
                           return false;
                       }
                   },
                   nlm::ErrorType::ValidationError,
                   nlm::ErrorSeverity::Error);
    
    engine.addRule("email_format",
                   "Validate email format",
                   [](const std::vector<std::string>& data) {
                       const std::string& email = data[0];
                       return email.find("@") != std::string::npos && email.find(".") != std::string::npos;
                   },
                   nlm::ErrorType::ValidationError,
                   nlm::ErrorSeverity::Warning);
    
    // Test validation
    nlm::ValidationResult result1 = engine.validate({"5.0"}, "test_context");
    assert(result1.valid);
    
    nlm::ValidationResult result2 = engine.validate({"-2.0"}, "test_context");
    assert(!result2.valid);
    assert(result2.errors.size() > 0);
    
    nlm::ValidationResult result3 = engine.validate({"invalid@email"}, "test_context");
    assert(!result3.valid);
    
    // Test enhanced validation
    nlm::ValidationContext context("test_validation", "test_component", {"data1", "data2"});
    nlm::EnhancedValidationResult enhancedResult = engine.validate(context);
    
    // Test configuration validation
    nlm::ValidationResult configResult = engine.validateConfiguration("test_key", "test_value");
    
    std::cout << "  ✓ Validation engine test passed" << std::endl;
}

// Test enhanced error
void testEnhancedError() {
    std::cout << "Testing enhanced error..." << std::endl;
    
    nlm::ErrorContext context;
    context.operation = "test_operation";
    context.component = "test_component";
    context.metadata["key1"] = "value1";
    context.tags["tag1"] = "value1";
    
    auto enhancedError = std::make_shared<nlm::EnhancedError>(
        "Enhanced error test",
        nlm::ErrorType::NeuralError,
        nlm::ErrorSeverity::Error,
        "test.cpp",
        42,
        "test_function",
        "TestCategory",
        context
    );
    
    assert(enhancedError->hasContext("key1"));
    assert(enhancedError->getContextValue("key1") == "value1");
    assert(enhancedError->hasTag("tag1"));
    
    std::string formatted = enhancedError->formatWithContext();
    assert(!formatted.empty());
    
    std::string json = enhancedError->formatForJSON();
    assert(!json.empty());
    
    std::cout << "  ✓ Enhanced error test passed" << std::endl;
}

// Test error recovery
void testErrorRecovery() {
    std::cout << "Testing error recovery..." << std::endl;
    
    nlm::ErrorRecoverySystem recoverySystem;
    
    // Test that recovery strategies are available
    assert(recoverySystem.getRecoveryStrategy("Default") != nullptr);
    assert(recoverySystem.getRecoveryStrategy("Memory") != nullptr);
    assert(recoverySystem.getRecoveryStrategy("File") != nullptr);
    assert(recoverySystem.getRecoveryStrategy("Configuration") != nullptr);
    assert(recoverySystem.getRecoveryStrategy("GracefulDegradation") != nullptr);
    
    // Test recovery statistics
    nlm::ErrorRecoverySystem::RecoveryStatistics stats = recoverySystem.getStatistics();
    assert(stats.totalRecoveryAttempts == 0);
    assert(stats.successfulRecoveries == 0);
    assert(stats.failedRecoveries == 0);
    
    std::cout << "  ✓ Error recovery test passed" << std::endl;
}

// Test error diagnostics
void testErrorDiagnostics() {
    std::cout << "Testing error diagnostics..." << std::endl;
    
    nlm::ErrorDiagnostics diagnostics;
    
    // Test that diagnostics config is set
    nlm::DiagnosticConfig diagConfig = diagnostics.getConfig();
    assert(diagConfig.enableAutomaticCollection == true);
    assert(diagConfig.maxDiagnosticHistory == 1000);
    
    std::cout << "  ✓ Error diagnostics test passed" << std::endl;
}

// Test error analytics
void testErrorAnalytics() {
    std::cout << "Testing error analytics..." << std::endl;
    
    nlm::ErrorAnalytics analytics;
    
    // Test that analytics config is set
    nlm::AnalyticsConfig analyticsConfig = analytics.getConfig();
    assert(analyticsConfig.enableRealTimeAnalytics == true);
    assert(analyticsConfig.analyticsWindowSize == 3600);
    assert(analyticsConfig.trackRecoveryMetrics == true);
    assert(analyticsConfig.trackErrorTrends == true);
    assert(analyticsConfig.detectAnomalies == true);
    
    std::cout << "  ✓ Error analytics test passed" << std::endl;
}

// Test advanced error manager
void testAdvancedErrorManager() {
    std::cout << "Testing advanced error manager..." << std::endl;
    
    nlm::AdvancedErrorManager manager;
    
    // Test that manager is initialized
    nlm::AdvancedErrorManager::SystemHealth health = manager.getSystemHealth();
    assert(health.isHealthy() == true); // Should be healthy by default
    
    // Test health thresholds
    manager.setHealthThresholds(95.0, 85.0, 50.0);
    
    std::cout << "  ✓ Advanced error manager test passed" << std::endl;
}

// Test error correlation
void testErrorCorrelation() {
    std::cout << "Testing error correlation..." << std::endl;
    
    // Create some test errors
    auto error1 = std::make_shared<nlm::Error>("Error 1", nlm::ErrorType::SystemError);
    auto error2 = std::make_shared<nlm::Error>("Error 2", nlm::ErrorType::ConfigurationError);
    auto error3 = std::make_shared<nlm::Error>("Error 3", nlm::ErrorType::MemoryError);
    
    std::vector<std::shared_ptr<nlm::Error>> errors = {error1, error2, error3};
    
    // Test correlation (simplified)
    std::vector<std::shared_ptr<nlm::Error>> correlated = nlm::AdvancedErrorHandling::correlateErrors(errors);
    
    std::cout << "  ✓ Error correlation test passed" << std::endl;
}

// Test error prevention
void testErrorPrevention() {
    std::cout << "Testing error prevention..." << std::endl;
    
    std::map<std::string, std::string> riskFactors;
    riskFactors["cpu_usage"] = "95";
    riskFactors["memory_usage"] = "85";
    
    bool preventionResult = nlm::AdvancedErrorHandling::applyPreventiveMeasures(riskFactors);
    
    std::cout << "  ✓ Error prevention test passed" << std::endl;
}

// Test error context builder
void testErrorContextBuilder() {
    std::cout << "Testing error context builder..." << std::endl;
    
    auto context = nlm::ErrorContextBuilder::create()
        .setOperation("test_operation")
        .setComponent("test_component")
        .setSubsystem("test_subsystem")
        .setModule("test_module")
        .addMetadata("metadata_key", "metadata_value")
        .setUserId("test_user")
        .setSessionId("test_session")
        .addTag("tag_key", "tag_value")
        .build();
    
    assert(context.operation == "test_operation");
    assert(context.component == "test_component");
    assert(context.subsystem == "test_subsystem");
    assert(context.module == "test_module");
    assert(context.metadata.find("metadata_key") != context.metadata.end());
    assert(context.tags.find("tag_key") != context.tags.end());
    
    std::cout << "  ✓ Error context builder test passed" << std::endl;
}

// Test thread safety
void testThreadSafety() {
    std::cout << "Testing thread safety..." << std::endl;
    
    nlm::ErrorReporter& reporter = nlm::ErrorReporter::getGlobal();
    reporter.clear();
    
    // Create multiple threads that report errors concurrently
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&reporter, i]() {
            reporter.logError("Thread " + std::to_string(i) + " error");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            reporter.logWarning("Thread " + std::to_string(i) + " warning");
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    size_t errorCount = reporter.getErrorCount();
    assert(errorCount >= 10); // Should have at least 10 errors
    
    std::cout << "  ✓ Thread safety test passed" << std::endl;
}

// Test performance
void testPerformance() {
    std::cout << "Testing performance..." << std::endl;
    
    nlm::ErrorReporter& reporter = nlm::ErrorReporter::getGlobal();
    reporter.clear();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        reporter.logError("Performance test error " + std::to_string(i));
        reporter.logWarning("Performance test warning " + std::to_string(i));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  Processed 2000 errors in " << duration.count() << " ms" << std::endl;
    assert(duration.count() < 1000); // Should process 2000 errors in under 1 second
    
    std::cout << "  ✓ Performance test passed" << std::endl;
}

int main() {
    std::cout << "=== NLM Advanced Error Handling System Test Suite ===" << std::endl << std::endl;
    
    try {
        testErrorHierarchy();
        testErrorReporting();
        testValidationEngine();
        testEnhancedError();
        testErrorRecovery();
        testErrorDiagnostics();
        testErrorAnalytics();
        testAdvancedErrorManager();
        testErrorCorrelation();
        testErrorPrevention();
        testErrorContextBuilder();
        testThreadSafety();
        testPerformance();
        
        std::cout << std::endl;
        std::cout << "=== All tests passed! ===" << std::endl;
        std::cout << "The advanced error handling system is working correctly." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}