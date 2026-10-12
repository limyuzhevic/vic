// Test file for the new error handling system
#include "core/ErrorHandling/include/nlm/NLMError.hpp"
#include "core/ErrorHandling/include/nlm/SpecificErrors.hpp"
#include "core/ErrorHandling/include/nlm/ErrorHandling.hpp"
#include <iostream>
#include <cassert>

void testNLMErrorCreation() {
    std::cout << "Testing NLMError creation..." << std::endl;
    
    // Test ConfigurationError
    auto configError = std::make_unique<nlm::ConfigurationError>(
        "Test configuration error", 
        nlm::ConfigurationError::ConfigErrorType::InvalidFormat,
        "test_field",
        "Use default configuration"
    );
    assert(configError->getMessage() == "Configuration Error: Test configuration error");
    assert(configError->getCategory() == nlm::ErrorCategory::Configuration);
    assert(configError->getSeverity() == nlm::ErrorSeverity::Error);
    std::cout << "  ✓ ConfigurationError created successfully" << std::endl;

    // Test ValidationError
    auto validationError = std::make_unique<nlm::ValidationError>(
        "Test validation error",
        nlm::ValidationError::ValidationErrorType::TypeMismatch,
        "test_field",
        "Convert value to correct type"
    );
    assert(validationError->getMessage() == "Validation Error: Test validation error");
    assert(validationError->getCategory() == nlm::ErrorCategory::Validation);
    assert(validationError->getSeverity() == nlm::ErrorSeverity::Warning);
    std::cout << "  ✓ ValidationError created successfully" << std::endl;

    // Test SerializationError
    auto serializationError = std::make_unique<nlm::SerializationError>(
        "Test serialization error",
        nlm::SerializationError::SerializationErrorType::VersionMismatch,
        "brain",
        "Use compatible serialization format"
    );
    assert(serializationError->getMessage() == "Serialization Error: Test serialization error");
    assert(serializationError->getCategory() == nlm::ErrorCategory::Serialization);
    assert(serializationError->getSeverity() == nlm::ErrorSeverity::Error);
    std::cout << "  ✓ SerializationError created successfully" << std::endl;

    // Test NeuralError
    auto neuralError = std::make_unique<nlm::NeuralError>(
        "Test neural error",
        nlm::NeuralError::NeuralErrorType::NeuronInitializationFailed,
        "initialize_neuron",
        "Check neuron parameters"
    );
    assert(neuralError->getMessage() == "Neural Error: Test neural error");
    assert(neuralError->getCategory() == nlm::ErrorCategory::Neural);
    assert(neuralError->getSeverity() == nlm::ErrorSeverity::Critical);
    std::cout << "  ✓ NeuralError created successfully" << std::endl;

    // Test MemoryError
    auto memoryError = std::make_unique<nlm::MemoryError>(
        "Test memory error",
        nlm::MemoryError::MemoryErrorType::AllocationFailed,
        0x12345678,
        "Check memory allocation"
    );
    assert(memoryError->getMessage() == "Memory Error: Test memory error");
    assert(memoryError->getCategory() == nlm::ErrorCategory::Memory);
    assert(memoryError->getSeverity() == nlm::ErrorSeverity::Critical);
    std::cout << "  ✓ MemoryError created successfully" << std::endl;

    // Test SystemError
    auto systemError = std::make_unique<nlm::SystemError>(
        "Test system error",
        nlm::SystemError::SystemErrorType::FileSystemError,
        "file_system",
        "Check file system"
    );
    assert(systemError->getMessage() == "System Error: Test system error");
    assert(systemError->getCategory() == nlm::ErrorCategory::System);
    assert(systemError->getSeverity() == nlm::ErrorSeverity::Critical);
    std::cout << "  ✓ SystemError created successfully" << std::endl;

    std::cout << "All NLMError tests passed!" << std::endl;
}

void testErrorHandlingSystem() {
    std::cout << "Testing ErrorHandlingSystem..." << std::endl;

    // Initialize error handling system
    std::unordered_map<std::string, std::string> config;
    config["min_handled_severity"] = "WARNING";
    config["error_retention_hours"] = "24";
    config["enable_structured_reporting"] = "true";
    config["enable_error_correlation"] = "true";

    nlm::initializeErrorHandling(config);

    // Test error handling
    auto error = std::make_unique<nlm::ConfigurationError>(
        "Test error handling",
        nlm::ConfigurationError::ConfigErrorType::InvalidFormat
    );

    std::cout << "  Before error handling: total errors = " << nlm::getErrorHandlingStats().totalErrors << std::endl;

    // Handle error
    nlm::handleError(std::move(error));

    std::cout << "  After error handling: total errors = " << nlm::getErrorHandlingStats().totalErrors << std::endl;

    // Add error to group
    auto error2 = std::make_unique<nlm::ValidationError>(
        "Test validation error",
        nlm::ValidationError::ValidationErrorType::TypeMismatch
    );

    nlm::addErrorToGroup("validation_tests", std::move(error2));

    // Get error group
    auto errorGroup = nlm::getErrorGroup("validation_tests");
    if (errorGroup) {
        std::cout << "  Error group created: " << errorGroup->getSummary() << std::endl;
    }

    // Test error statistics
    auto stats = nlm::getErrorHandlingStats();
    std::cout << "  Error statistics: " << stats.totalErrors << " total errors, " 
              << stats.totalRecoveries << " total recoveries" << std::endl;

    std::cout << "ErrorHandlingSystem tests completed!" << std::endl;
}

void testErrorFormatting() {
    std::cout << "Testing error formatting..." << std::endl;

    auto error = std::make_unique<nlm::ConfigurationError>(
        "Test formatting error",
        nlm::ConfigurationError::ConfigErrorType::MissingKey,
        "missing_key"
    );

    // Test toString
    std::string strFormat = error->toString();
    assert(strFormat.find("Configuration Error: Test formatting error") != std::string::npos);
    std::cout << "  ✓ toString format works" << std::endl;

    // Test toJSON
    std::string jsonFormat = error->toJSON();
    assert(jsonFormat.find("Configuration Error: Test formatting error") != std::string::npos);
    assert(jsonFormat.find("category") != std::string::npos);
    std::cout << "  ✓ toJSON format works" << std::endl;

    // Test toYAML
    std::string yamlFormat = error->toYAML();
    assert(yamlFormat.find("Configuration Error: Test formatting error") != std::string::npos);
    std::cout << "  ✓ toYAML format works" << std::endl;

    // Test toXML
    std::string xmlFormat = error->toXML();
    assert(xmlFormat.find("Configuration Error: Test formatting error") != std::string::npos);
    assert(xmlFormat.find("<error>") != std::string::npos);
    std::cout << "  ✓ toXML format works" << std::endl;

    std::cout << "All error formatting tests passed!" << std::endl;
}

int main() {
    std::cout << "=== NLM Error Handling System Tests ===" << std::endl << std::endl;

    try {
        testNLMErrorCreation();
        std::cout << std::endl;

        testErrorHandlingSystem();
        std::cout << std::endl;

        testErrorFormatting();
        std::cout << std::endl;

        std::cout << "=== All Tests Passed! ===" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}