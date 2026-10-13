// Simplified Error Handling Framework
#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <map>
#include <chrono>
#include <stdexcept>
#include <system_error>

namespace nlm {

// Error categories for different types of failures
enum class ErrorCategory {
    Configuration,  // Invalid config, missing keys
    Memory,         // Allocation failures, out of memory
    Neural,         // Network errors, simulation errors
    File,           // Read/write failures, corrupted files
    System,         // Resource limits, permissions
    Validation,     // Invalid input parameters
    Network,        // Connection issues, timeouts
    Permission,     // Access denied, insufficient privileges
    Timeout,        // Operations timed out
    Unknown         // Unknown/unexpected errors
};

// Base error class
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorCategory category = ErrorCategory::Unknown,
             const std::string& context = "", int line = 0, 
             const std::string& file = "", const std::string& function = "")
        : std::runtime_error(formatMessage(message, context, line, file, function)),
          category_(category), context_(context), line_(line), file_(file), function_(function),
          timestamp_(std::chrono::system_clock::now()) {}

    virtual ~NLMError() = default;

    // Error category accessor
    ErrorCategory getCategory() const noexcept { return category_; }

    // Context information
    const std::string& getContext() const noexcept { return context_; }
    int getLine() const noexcept { return line_; }
    const std::string& getFile() const noexcept { return file_; }
    const std::string& getFunction() const noexcept { return function_; }
    const std::chrono::system_clock::time_point& getTimestamp() const noexcept { return timestamp_; }

    // Stack trace if available
    virtual const std::string& getStackTrace() const noexcept { return emptyString_; }

    // Error recovery strategy
    virtual bool canRecover() const noexcept { return false; }
    virtual std::string recover() { return "Recovery not implemented"; }

protected:
    static std::string formatMessage(const std::string& message, const std::string& context,
                                     int line, const std::string& file, const std::string& function);

private:
    static const std::string emptyString_;

    ErrorCategory category_;
    std::string context_;
    int line_;
    std::string file_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
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

// Error message catalog for consistent error messages
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
    static void logError(const NLMError& error);
    static std::string formatError(const NLMError& error);
    static bool shouldRetry(const NLMError& error);
    static std::string suggestRecovery(const NLMError& error);

    // Exception translation utilities
    static NLMError translate(const std::exception& e, ErrorCategory category = ErrorCategory::Unknown);
    static NLMError translateSystemError(int errorCode, const std::string& context = "");
    static NLMError translateFileError(const std::string& filepath, std::error_code ec);
    
private:
    static std::string emptyString_;
};

// Convenience macros for error handling
#define NLM_THROW_ERROR(category, message) \
    nlm::NLMError(__FILE__, __LINE__, __func__, category, message)

#define NLM_THROW_CONFIG_ERROR(key, message) \
    nlm::ConfigurationError(__FILE__, __LINE__, __func__, key, message)

#define NLM_THROW_MEMORY_ERROR(bytes, message) \
    nlm::MemoryError(__FILE__, __LINE__, __func__, bytes, message)

#define NLM_THROW_NEURAL_ERROR(op, message) \
    nlm::NeuralError(__FILE__, __LINE__, __func__, op, message)

#define NLM_THROW_FILE_ERROR(filepath, op, message) \
    nlm::FileError(__FILE__, __LINE__, __func__, filepath, op, message)

#define NLM_THROW_SYSTEM_ERROR(resource, message) \
    nlm::SystemError(__FILE__, __LINE__, __func__, resource, message)

// Exception translation namespace
namespace exception {
    // Convert standard exceptions to NLMError
    NLMError translate(const std::exception& e, ErrorCategory category = ErrorCategory::Unknown);
    
    // Convert system errors
    NLMError translateSystemError(int errorCode, const std::string& context = "");
    
    // Convert file system errors
    NLMError translateFileError(const std::string& filepath, std::error_code ec);
}

} // namespace nlm