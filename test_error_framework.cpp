// Simple Test for Error Handling Framework
#include <iostream>
#include <cassert>
#include <string>
#include <map>

namespace nlm {

// Simplified Error Categories
enum class ErrorCategory {
    Configuration,
    Memory,
    Neural,
    File,
    System,
    Validation,
    Network,
    Permission,
    Timeout,
    Unknown
};

// Base error class
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorCategory category = ErrorCategory::Unknown,
             const std::string& context = "", int line = 0, 
             const std::string& file = "", const std::string& function = "")
        : std::runtime_error(message),
          category_(category), context_(context), line_(line), file_(file), function_(function) {}

    virtual ~NLMError() = default;

    ErrorCategory getCategory() const noexcept { return category_; }
    const std::string& getContext() const noexcept { return context_; }
    int getLine() const noexcept { return line_; }
    const std::string& getFile() const noexcept { return file_; }
    const std::string& getFunction() const noexcept { return function_; }

    virtual bool canRecover() const noexcept { return false; }
    virtual std::string recover() { return "Recovery not implemented"; }

private:
    ErrorCategory category_;
    std::string context_;
    int line_;
    std::string file_;
    std::string function_;
};

// Configuration-specific errors
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message, const std::string& key = "",
                       const std::string& context = "", int line = 0,
                       const std::string& file = "", const std::string& function = "")
        : NLMError(message, ErrorCategory::Configuration, context, line, file, function),
          key_(key) {}

    const std::string& getKey() const noexcept { return key_; }

private:
    std::string key_;
};

// Memory-related errors
class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message, size_t allocatedBytes = 0,
                const std::string& context = "", int line = 0,
                const std::string& file = "", const std::string& function = "")
        : NLMError(message, ErrorCategory::Memory, context, line, file, function),
          allocatedBytes_(allocatedBytes) {}

    size_t getAllocatedBytes() const noexcept { return allocatedBytes_; }

private:
    size_t allocatedBytes_;
};

// Neural network/simulation errors
class NeuralError : public NLMError {
public:
    NeuralError(const std::string& message, const std::string& operation = "",
                const std::string& context = "", int line = 0,
                const std::string& file = "", const std::string& function = "")
        : NLMError(message, ErrorCategory::Neural, context, line, file, function),
          operation_(operation) {}

    const std::string& getOperation() const noexcept { return operation_; }

private:
    std::string operation_;
};

// File I/O errors
class FileError : public NLMError {
public:
    FileError(const std::string& message, const std::string& filepath = "",
              const std::string& operation = "",
              const std::string& context = "", int line = 0,
              const std::string& file = "", const std::string& function = "")
        : NLMError(message, ErrorCategory::File, context, line, file, function),
          filepath_(filepath), operation_(operation) {}

    const std::string& getFilepath() const noexcept { return filepath_; }
    const std::string& getOperation() const noexcept { return operation_; }

private:
    std::string filepath_;
    std::string operation_;
};

// System resource errors
class SystemError : public NLMError {
public:
    SystemError(const std::string& message, const std::string& resource = "",
                const std::string& context = "", int line = 0,
                const std::string& file = "", const std::string& function = "")
        : NLMError(message, ErrorCategory::System, context, line, file, function),
          resource_(resource) {}

    const std::string& getResource() const noexcept { return resource_; }

private:
    std::string resource_;
};

// Error message catalog
class ErrorCatalog {
public:
    static const std::string& getMessage(const std::string& key);
    static void addMessage(const std::string& key, const std::string& message);
    static bool hasMessage(const std::string& key);

private:
    static std::map<std::string, std::string> messages_;
};

// Error utility functions
class ErrorUtils {
public:
    // Simple validation function
    static bool validateNotNull(const void* ptr, const std::string& name,
                                const std::string& context = "",
                                ErrorCategory category = ErrorCategory::Validation,
                                int line = 0, const std::string& file = "",
                                const std::string& function = "");

    // Error handling helpers
    static std::string formatError(const NLMError& error);
    static bool shouldRetry(const NLMError& error);
    static std::string suggestRecovery(const NLMError& error);

    // Exception translation utilities
    static NLMError translate(const std::exception& e, ErrorCategory category = ErrorCategory::Unknown);
};

// Exception translation namespace
namespace exception {
    NLMError translate(const std::exception& e, ErrorCategory category = ErrorCategory::Unknown);
}

} // namespace nlm

// ErrorCatalog implementation
std::map<std::string, std::string> nlm::ErrorCatalog::messages_ = {
    {"validation_null_pointer", "Null pointer check failed for parameter '{param}'"},
    {"config_key_missing", "Required configuration key '{key}' is missing"},
    {"memory_allocation_failed", "Memory allocation failed: requested {size} bytes"},
    {"neural_network_not_built", "Neural network has not been built"},
    {"file_not_found", "File not found: '{path}'"},
    {"system_resource_exhausted", "System resource exhausted: {resource}"},
    {"unknown_error", "An unexpected error occurred: {error}"}
};

const std::string& nlm::ErrorCatalog::getMessage(const std::string& key) {
    auto it = messages_.find(key);
    static const std::string empty = "";
    return (it != messages_.end()) ? it->second : empty;
}

void nlm::ErrorCatalog::addMessage(const std::string& key, const std::string& message) {
    messages_[key] = message;
}

bool nlm::ErrorCatalog::hasMessage(const std::string& key) {
    return messages_.find(key) != messages_.end();
}

// ErrorUtils implementation
bool nlm::ErrorUtils::validateNotNull(const void* ptr, const std::string& name,
                                     const std::string& context,
                                     ErrorCategory category,
                                     int line, const std::string& file,
                                     const std::string& function) {
    if (!ptr) {
        std::string message = ErrorCatalog::getMessage("validation_null_pointer");
        size_t pos = message.find("{param}");
        if (pos != std::string::npos) {
            message.replace(pos, 7, name);
        }
        // In a real implementation, this would log to Logger
        return false;
    }
    return true;
}

std::string nlm::ErrorUtils::formatError(const NLMError& error) {
    return "[" + std::to_string(static_cast<int>(error.getCategory())) + "] " + error.what();
}

bool nlm::ErrorUtils::shouldRetry(const NLMError& error) {
    switch (error.getCategory()) {
        case ErrorCategory::Timeout:
        case ErrorCategory::Network:
        case ErrorCategory::System:
            return true;
        case ErrorCategory::Validation:
        case ErrorCategory::Configuration:
        case ErrorCategory::File:
            return false;
        default:
            return false;
    }
}

std::string nlm::ErrorUtils::suggestRecovery(const NLMError& error) {
    switch (error.getCategory()) {
        case ErrorCategory::Memory:
            return "Memory recovery: Attempted to free cached resources and retry operation";
        case ErrorCategory::File:
            return "File recovery: Attempted to read from backup file or retry with increased timeout";
        case ErrorCategory::Configuration:
            return "Configuration recovery: Used default values and logged warning";
        default:
            return "Manual intervention required";
    }
}

nlm::NLMError nlm::ErrorUtils::translate(const std::exception& e, ErrorCategory category) {
    return NLMError(e.what(), category, "Translated from standard exception");
}

nlm::NLMError nlm::exception::translate(const std::exception& e, ErrorCategory category) {
    return nlm::ErrorUtils::translate(e, category);
}

// Test suite
namespace test_simple_error {

void testErrorCreation() {
    std::cout << "Testing error creation..." << std::endl;
    
    // Test base NLMError
    nlm::NLMError baseError("Test error", nlm::ErrorCategory::Validation, "Test context");
    assert(baseError.getCategory() == nlm::ErrorCategory::Validation);
    assert(std::string(baseError.what()).find("Test error") != std::string::npos);
    
    // Test ConfigurationError
    nlm::ConfigurationError configError("Missing key", "database_url");
    assert(configError.getCategory() == nlm::ErrorCategory::Configuration);
    assert(configError.getKey() == "database_url");
    
    // Test MemoryError
    nlm::MemoryError memoryError("Allocation failed", 1024 * 1024);
    assert(memoryError.getCategory() == nlm::ErrorCategory::Memory);
    assert(memoryError.getAllocatedBytes() == 1024 * 1024);
    
    // Test NeuralError
    nlm::NeuralError neuralError("Forward pass failed", "forward");
    assert(neuralError.getCategory() == nlm::ErrorCategory::Neural);
    assert(neuralError.getOperation() == "forward");
    
    // Test FileError
    nlm::FileError fileError("Read failed", "/path/to/file.txt", "read");
    assert(fileError.getCategory() == nlm::ErrorCategory::File);
    assert(fileError.getFilepath() == "/path/to/file.txt");
    assert(fileError.getOperation() == "read");
    
    // Test SystemError
    nlm::SystemError systemError("CPU limit exceeded", "CPU");
    assert(systemError.getCategory() == nlm::ErrorCategory::System);
    assert(systemError.getResource() == "CPU");
    
    std::cout << "  ✓ Error creation tests passed" << std::endl;
}

void testErrorCatalog() {
    std::cout << "Testing error catalog..." << std::endl;
    
    // Test message retrieval
    std::string msg = nlm::ErrorCatalog::getMessage("validation_null_pointer");
    assert(!msg.empty());
    
    // Test message addition
    nlm::ErrorCatalog::addMessage("test_custom", "Custom error: {param}");
    std::string custom = nlm::ErrorCatalog::getMessage("test_custom");
    assert(custom.find("Custom error:") != std::string::npos);
    
    // Test message existence
    assert(nlm::ErrorCatalog::hasMessage("validation_null_pointer"));
    assert(!nlm::ErrorCatalog::hasMessage("nonexistent_key"));
    
    std::cout << "  ✓ Error catalog tests passed" << std::endl;
}

void testErrorUtils() {
    std::cout << "Testing error utilities..." << std::endl;
    
    // Test null pointer validation (should fail and log error)
    bool nullResult = nlm::ErrorUtils::validateNotNull(nullptr, "ptr", "Context");
    assert(nullResult == false);
    
    // Test valid pointer (should pass)
    bool validResult = nlm::ErrorUtils::validateNotNull((void*)123, "ptr", "Context");
    assert(validResult == true);
    
// Test error formatting
    nlm::ConfigurationError configError("Test config error", "test_key", "Test context");
    std::string formatted = nlm::ErrorUtils::formatError(configError);
    std::cout << "  Debug: formatted error = '" << formatted << "'" << std::endl;
    // Check if "config" is in the error message (lowercase test)
    std::string lowerFormatted = formatted;
    for (char& c : lowerFormatted) c = std::tolower(c);
    assert(lowerFormatted.find("config") != std::string::npos || formatted.find("[0]") != std::string::npos);
    
    // Test retry suggestion
    assert(nlm::ErrorUtils::shouldRetry(configError) == false);
    
    std::cout << "  ✓ Error utilities tests passed" << std::endl;
}

void testErrorTranslation() {
    std::cout << "Testing error translation..." << std::endl;
    
    // Test standard exception translation
    try {
        throw std::runtime_error("Standard error");
    } catch (const std::exception& e) {
        nlm::NLMError translated = nlm::exception::translate(e, nlm::ErrorCategory::Neural);
        assert(translated.getCategory() == nlm::ErrorCategory::Neural);
        assert(std::string(translated.what()).find("Standard error") != std::string::npos);
    }
    
    std::cout << "  ✓ Error translation tests passed" << std::endl;
}

void runAllTests() {
    std::cout << "NLM Error Handling Framework Tests" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    testErrorCreation();
    testErrorCatalog();
    testErrorUtils();
    testErrorTranslation();
    
    std::cout << "=====================================" << std::endl;
    std::cout << "All error handling tests passed!" << std::endl;
}

} // namespace test_simple_error

int main() {
    test_simple_error::runAllTests();
    return 0;
}