#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <source_location>
#include <chrono>
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * Base exception class for NLM errors
 * Provides common interface for all NLM-specific exceptions
 */
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message, 
                     const std::string& file = "",
                     int line = 0,
                     const std::string& function = "")
        : std::runtime_error(formatMessage(message, file, line, function)) {
        logError();
    }
    
    virtual ~NLMError() = default;
    
    const char* what() const noexcept override {
        return std::runtime_error::what();
    }
    
protected:
    virtual void logError() const {
        std::string errorMsg = "NLM Error: ";
        errorMsg += std::runtime_error::what();
        Logger::getGlobal().error(errorMsg, "", 0, "");
    }
    
private:
    static std::string formatMessage(const std::string& message,
                                    const std::string& file,
                                    int line,
                                    const std::string& function) {
        std::string result = message;
        if (!file.empty()) {
            result += " (at " + file;
            if (line > 0) result += ":" + std::to_string(line);
            result += ")";
        }
        return result;
    }
};

/**
 * Exception for configuration-related errors
 */
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message,
                       const std::string& key = "",
                       const std::string& file = "",
                       int line = 0,
                       const std::string& function = "")
        : NLMError(formatMessage(message, file, line, function), file, line, function),
          key_(key) {
        logError();
    }
    
    const std::string& getKey() const noexcept { return key_; }
    
protected:
    void logError() const override {
        std::string errorMsg = "Config Error: ";
        errorMsg += std::runtime_error::what();
        if (!key_.empty()) {
            errorMsg += " [key: " + key_ + "]";
        }
        Logger::getGlobal().error(errorMsg, "", 0, "");
    }
    
private:
    static std::string formatMessage(const std::string& message,
                                    const std::string& file,
                                    int line,
                                    const std::string& function) {
        std::string result = message;
        if (!file.empty()) {
            result += " (at " + file;
            if (line > 0) result += ":" + std::to_string(line);
            result += ")";
        }
        return result;
    }
    
    std::string key_;
};

/**
 * Exception for initialization-related errors
 */
class InitializationError : public NLMError {
public:
    explicit InitializationError(const std::string& message,
                                const std::string& component = "",
                                const std::string& file = "",
                                int line = 0,
                                const std::string& function = "")
        : NLMError(formatMessage(message, file, line, function), file, line, function),
          component_(component) {
        logError();
    }
    
    const std::string& getComponent() const noexcept { return component_; }
    
protected:
    void logError() const override {
        std::string errorMsg = "Initialization Error: ";
        errorMsg += std::runtime_error::what();
        if (!component_.empty()) {
            errorMsg += " [component: " + component_ + "]";
        }
        Logger::getGlobal().error(errorMsg, "", 0, "");
    }
    
private:
    static std::string formatMessage(const std::string& message,
                                    const std::string& file,
                                    int line,
                                    const std::string& function) {
        std::string result = message;
        if (!file.empty()) {
            result += " (at " + file;
            if (line > 0) result += ":" + std::to_string(line);
            result += ")";
        }
        return result;
    }
    
    std::string component_;
};

/**
 * Exception for file-related errors
 */
class FileError : public NLMError {
public:
    enum class ErrorType {
        NotFound,
        PermissionDenied,
        AlreadyExists,
        IsDirectory,
        NotADirectory,
        ReadError,
        WriteError,
        InvalidFormat,
        DiskFull,
        Unknown
    };
    
    explicit FileError(const std::string& message,
                      const std::string& filepath = "",
                      ErrorType type = ErrorType::Unknown,
                      const std::string& file = "",
                      int line = 0,
                      const std::string& function = "")
        : NLMError(formatMessage(message, file, line, function), file, line, function),
          filepath_(filepath),
          type_(type) {
        logError();
    }
    
    const std::string& getFilePath() const noexcept { return filepath_; }
    ErrorType getType() const noexcept { return type_; }
    
protected:
    void logError() const override {
        std::string errorMsg = "File Error: ";
        errorMsg += std::runtime_error::what();
        errorMsg += " [type: " + errorTypeToString(type_) + "]";
        if (!filepath_.empty()) {
            errorMsg += " [file: " + filepath_ + "]";
        }
        Logger::getGlobal().error(errorMsg, "", 0, "");
    }
    
private:
    static std::string formatMessage(const std::string& message,
                                    const std::string& file,
                                    int line,
                                    const std::string& function) {
        std::string result = message;
        if (!file.empty()) {
            result += " (at " + file;
            if (line > 0) result += ":" + std::to_string(line);
            result += ")";
        }
        return result;
    }
    
    static const char* errorTypeToString(ErrorType type) {
        switch (type) {
            case ErrorType::NotFound: return "NotFound";
            case ErrorType::PermissionDenied: return "PermissionDenied";
            case ErrorType::AlreadyExists: return "AlreadyExists";
            case ErrorType::IsDirectory: return "IsDirectory";
            case ErrorType::NotADirectory: return "NotADirectory";
            case ErrorType::ReadError: return "ReadError";
            case ErrorType::WriteError: return "WriteError";
            case ErrorType::InvalidFormat: return "InvalidFormat";
            case ErrorType::DiskFull: return "DiskFull";
            case ErrorType::Unknown: return "Unknown";
            default: return "Unknown";
        }
    }
    
    std::string filepath_;
    ErrorType type_;
};

/**
 * Exception for validation-related errors
 */
class ValidationError : public NLMError {
public:
    enum class ErrorType {
        NullPointer,
        InvalidValue,
        OutOfRange,
        Empty,
        InvalidFormat,
        ConstraintViolation,
        Unknown
    };
    
    explicit ValidationError(const std::string& message,
                            const std::string& field = "",
                            ErrorType type = ErrorType::Unknown,
                            const std::string& file = "",
                            int line = 0,
                            const std::string& function = "")
        : NLMError(formatMessage(message, file, line, function), file, line, function),
          field_(field),
          type_(type) {
        logError();
    }
    
    const std::string& getField() const noexcept { return field_; }
    ErrorType getType() const noexcept { return type_; }
    
protected:
    void logError() const override {
        std::string errorMsg = "Validation Error: ";
        errorMsg += std::runtime_error::what();
        errorMsg += " [type: " + validationErrorTypeToString(type_) + "]";
        if (!field_.empty()) {
            errorMsg += " [field: " + field_ + "]";
        }
        Logger::getGlobal().error(errorMsg, "", 0, "");
    }
    
private:
    static std::string formatMessage(const std::string& message,
                                    const std::string& file,
                                    int line,
                                    const std::string& function) {
        std::string result = message;
        if (!file.empty()) {
            result += " (at " + file;
            if (line > 0) result += ":" + std::to_string(line);
            result += ")";
        }
        return result;
    }
    
    static const char* validationErrorTypeToString(ErrorType type) {
        switch (type) {
            case ErrorType::NullPointer: return "NullPointer";
            case ErrorType::InvalidValue: return "InvalidValue";
            case ErrorType::OutOfRange: return "OutOfRange";
            case ErrorType::Empty: return "Empty";
            case ErrorType::InvalidFormat: return "InvalidFormat";
            case ErrorType::ConstraintViolation: return "ConstraintViolation";
            case ErrorType::Unknown: return "Unknown";
            default: return "Unknown";
        }
    }
    
    std::string field_;
    ErrorType type_;
};

/**
 * Result type for functions that can fail
 * Provides both success and error cases without exceptions
 */
template<typename T>
class Result {
public:
    Result(const T& value) : value_(value), has_value_(true) {}
    Result(T&& value) : value_(std::move(value)), has_value_(true) {}
    Result(const NLMError& error) : error_(error), has_value_(false) {}
    
    bool has_value() const { return has_value_; }
    
    const T& value() const {
        if (!has_value_) {
            throw std::runtime_error("Cannot access value of failed Result");
        }
        return value_;
    }
    
    T& value() {
        if (!has_value_) {
            throw std::runtime_error("Cannot access value of failed Result");
        }
        return value_;
    }
    
    const NLMError& error() const {
        if (has_value_) {
            throw std::runtime_error("Cannot access error of successful Result");
        }
        return error_;
    }
    
private:
    T value_;
    NLMError error_;
    bool has_value_;
};

/**
 * Error handling utilities
 */
namespace ErrorHandling {
    
    /**
     * Check if a pointer is null and throw ValidationError if it is
     */
    template<typename T>
    T* checkNotNull(T* ptr, const std::string& field,
                   const std::string& file = "",
                   int line = 0,
                   const std::string& function = "") {
        if (!ptr) {
            throw ValidationError("Null pointer dereference",
                                 field,
                                 ValidationError::ErrorType::NullPointer,
                                 file, line, function);
        }
        return ptr;
    }
    
    /**
     * Check if a shared_ptr is null and throw ValidationError if it is
     */
    template<typename T>
    const T* checkNotNull(const std::shared_ptr<T>& ptr, const std::string& field,
                         const std::string& file = "",
                         int line = 0,
                         const std::string& function = "") {
        if (!ptr) {
            throw ValidationError("Null pointer dereference",
                                 field,
                                 ValidationError::ErrorType::NullPointer,
                                 file, line, function);
        }
        return ptr.get();
    }
    
    /**
     * Check if a unique_ptr is null and throw ValidationError if it is
     */
    template<typename T>
    T* checkNotNull(const std::unique_ptr<T>& ptr, const std::string& field,
                   const std::string& file = "",
                   int line = 0,
                   const std::string& function = "") {
        if (!ptr) {
            throw ValidationError("Null pointer dereference",
                                 field,
                                 ValidationError::ErrorType::NullPointer,
                                 file, line, function);
        }
        return ptr.get();
    }
    
    /**
     * Check validation condition and throw ValidationError if false
     */
    void checkCondition(bool condition, const std::string& message,
                       const std::string& field = "",
                       ValidationError::ErrorType type = ValidationError::ErrorType::InvalidValue,
                       const std::string& file = "",
                       int line = 0,
                       const std::string& function = "");
    
    /**
     * Convert file operation error to FileError
     */
    FileError convertFileError(const std::string& operation,
                              const std::string& filepath,
                              const std::string& message,
                              FileError::ErrorType type = FileError::ErrorType::Unknown,
                              const std::string& file = "",
                              int line = 0,
                              const std::string& function = "");
    
    /**
     * Convert configuration error to ConfigError
     */
    ConfigError convertConfigError(const std::string& key,
                                  const std::string& message,
                                  const std::string& file = "",
                                  int line = 0,
                                  const std::string& function = "");
    
    /**
     * Convert initialization error to InitializationError
     */
    InitializationError convertInitializationError(const std::string& component,
                                                    const std::string& message,
                                                    const std::string& file = "",
                                                    int line = 0,
                                                    const std::string& function = "");
} // namespace ErrorHandling

/**
 * Error handling macros for common patterns
 */
#define NLM_CHECK_NULL(ptr, field) \
    nlm::ErrorHandling::checkNotNull(ptr, field, __FILE__, __LINE__, __func__)

#define NLM_CHECK_CONDITION(condition, message, field, type) \
    nlm::ErrorHandling::checkCondition(condition, message, field, type, __FILE__, __LINE__, __func__)

#define NLM_THROW_VALIDATION(message, field, type) \
    throw nlm::ValidationError(message, field, type, __FILE__, __LINE__, __func__)

#define NLM_THROW_CONFIG(message, key) \
    throw nlm::ConfigError(message, key, __FILE__, __LINE__, __func__)

#define NLM_THROW_INITIALIZATION(message, component) \
    throw nlm::InitializationError(message, component, __FILE__, __LINE__, __func__)

#define NLM_THROW_FILE(operation, filepath, message, type) \
    throw nlm::FileError(message, filepath, type, __FILE__, __LINE__, __func__)

/**
 * Safe execution macros for functions that return bool
 */
#define NLM_RETURN_ON_ERROR(expr) \
    { bool result = (expr); if (!result) return false; }

#define NLM_RETURN_ON_THROW(expr) \
    try { (expr); } catch (const nlm::NLMError& e) { return false; }

/**
 * Exception-safe wrapper for resource cleanup
 */
template<typename T>
class SafeResource {
public:
    SafeResource(std::unique_ptr<T> resource) : resource_(std::move(resource)) {}
    
    ~SafeResource() {
        try {
            if (resource_) {
                resource_->cleanup();
            }
        } catch (...) {
            Logger::getGlobal().warning("Exception during resource cleanup", __FILE__, __LINE__, __func__);
        }
    }
    
    T* get() const { return resource_.get(); }
    T* operator->() const { return get(); }
    T& operator*() const { return *get(); }
    
private:
    std::unique_ptr<T> resource_;
};

} // namespace nlm