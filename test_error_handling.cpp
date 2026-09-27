#include "core/Error/Error.hpp"
#include "core/ValidationEngine/ValidationEngine.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>

void testErrorClassification() {
    std::cout << "Testing error classification..." << std::endl;
    
    // Test basic error creation
    auto configError = std::make_shared<nlm::ConfigurationError>("Invalid configuration value");
    assert(configError->getType() == nlm::ErrorType::ConfigurationError);
    assert(configError->getSeverity() == nlm::ErrorSeverity::Error);
    
    auto memoryError = std::make_shared<nlm::MemoryError>("Out of memory");
    assert(memoryError->getType() == nlm::ErrorType::MemoryError);
    assert(memoryError->getSeverity() == nlm::ErrorSeverity::Critical);
    
    auto neuralError = std::make_shared<nlm::NeuralError>("Neural network error");
    assert(neuralError->getType() == nlm::ErrorType::NeuralError);
    
    auto fileError = std::make_shared<nlm::FileError>("File not found");
    assert(fileError->getType() == nlm::ErrorType::FileError);
    
    auto validationError = std::make_shared<nlm::ValidationError>("Validation failed", nlm::ErrorSeverity::Warning);
    assert(validationError->getType() == nlm::ErrorType::ValidationError);
    assert(validationError->getSeverity() == nlm::ErrorSeverity::Warning);
    
    std::cout << "  ✓ Error classification test passed" << std::endl;
}

void testErrorReporting() {
    std::cout << "Testing error reporting..." << std::endl;
    
    // Test error reporter
    nlm::ErrorReporter& reporter = nlm::ErrorReporter::getGlobal();
    
    reporter.logError("Test error message");
    reporter.logWarning("Test warning message");
    reporter.logInfo("Test info message");
    reporter.logDebug("Test debug message");
    
    size_t errorCount = reporter.getErrorCount();
    assert(errorCount > 0);
    
    std::cout << "  ✓ Error reporting test passed" << std::endl;
}

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
    
    // Test validation
    nlm::ValidationResult result1 = engine.validate({"5.0"}, "test_context");
    assert(result1.valid);
    
    nlm::ValidationResult result2 = engine.validate({"-2.0"}, "test_context");
    assert(!result2.valid);
    assert(result2.errors.size() > 0);
    
    std::cout << "  ✓ Validation engine test passed" << std::endl;
}

void testEnhancedError() {
    std::cout << "Testing enhanced error..." << std::endl;
    
    nlm::ErrorContext context;
    context.operation = "test_operation";
    context.component = "test_component";
    context.metadata["key"] = "value";
    context.tags["tag1"] = "value1";
    
    auto enhancedError = std::make_shared<nlm::EnhancedError>(
        "Enhanced error with context", 
        nlm::ErrorType::NeuralError,
        nlm::ErrorSeverity::Error,
        "test.cpp", 42, "test_function",
        "TestCategory", context
    );
    
    assert(enhancedError->hasContext("key"));
    assert(enhancedError->getContextValue("key") == "value");
    assert(enhancedError->hasTag("tag1"));
    
    std::string formatted = enhancedError->formatWithContext();
    assert(!formatted.empty());
    
    std::cout << "  ✓ Enhanced error test passed" << std::endl;
}

void testErrorRecovery() {
    std::cout << "Testing error recovery..." << std::endl;
    
    nlm::ValidationEngine engine;
    
    // Test that validation engine has recovery strategies
    bool hasRecoveryStrategies = engine.getRecoveryStrategy("Default") != nullptr;
    assert(hasRecoveryStrategies);
    
    std::cout << "  ✓ Error recovery test passed" << std::endl;
}

void testErrorFormatting() {
    std::cout << "Testing error formatting..." << std::endl;
    
    nlm::ErrorReporter& reporter = nlm::ErrorReporter::getGlobal();
    
    // Add different formatters
    reporter.addFormatter(std::make_shared<nlm::SimpleErrorFormatter>());
    reporter.addFormatter(std::make_shared<nlm::DetailedErrorFormatter>());
    reporter.addFormatter(std::make_shared<nlm::JSONErrorFormatter>());
    reporter.addFormatter(std::make_shared<nlm::XMLErrorFormatter>());
    reporter.addFormatter(std::make_shared<nlm::CSVErrorFormatter>());
    
    // Add some errors
    reporter.logError("Test error for formatting");
    reporter.logWarning("Test warning for formatting");
    
    // Test different output formats
    std::string formatted = reporter.formatAllErrors();
    assert(!formatted.empty());
    
    std::cout << "  ✓ Error formatting test passed" << std::endl;
}

void testErrorStatistics() {
    std::cout << "Testing error statistics..." << std::endl;
    
    nlm::ErrorReporter& reporter = nlm::ErrorReporter::getGlobal();
    
    // Clear existing errors
    reporter.clear();
    
    // Add some errors
    reporter.logError("Error 1");
    reporter.logError("Error 2");
    reporter.logWarning("Warning 1");
    reporter.logDebug("Debug 1");
    
    // Get statistics
    std::string statsReport = reporter.getStatisticsReport();
    assert(!statsReport.empty());
    
    // Test error distribution
    std::string typeReport = reporter.getErrorsByTypeReport();
    assert(!typeReport.empty());
    
    std::string severityReport = reporter.getErrorsBySeverityReport();
    assert(!severityReport.empty());
    
    std::cout << "  ✓ Error statistics test passed" << std::endl;
}

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

int main() {
    std::cout << "=== NLM Error Handling System Test ===" << std::endl << std::endl;
    
    try {
        testErrorClassification();
        testErrorReporting();
        testValidationEngine();
        testEnhancedError();
        testErrorRecovery();
        testErrorFormatting();
        testErrorStatistics();
        testErrorContextBuilder();
        
        std::cout << std::endl;
        std::cout << "=== All tests passed! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}