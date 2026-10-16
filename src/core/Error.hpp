#pragma once

#include <string>
#include <stdexcept>
#include <sstream>
#include <system_error>
#include <iostream>

namespace nlm {

/**
 * @enum ErrorCategory
 * @brief Categories of errors for NLM framework
 */
enum class ErrorCategory {
    Configuration = 0,    ///< Configuration loading/validation errors
    Memory = 1,           ///< Memory management errors
    InputValidation = 2,  ///< Input parameter validation errors
    System = 3,           ///< System state errors
    Resource = 4,         ///< Resource access errors
    Python = 5,           ///< Python binding errors
    Unknown = 6           ///< Unknown/unclassified errors
};

class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message) 
        : std::runtime_error(message), category(ErrorCategory::Unknown), 
          code(0), file_(), line_(0), function_(), suggestion_() {}
    
    NLMException(const std::string& message, ErrorCategory cat, int errCode = 0)
        : std::runtime_error(message), category(cat), code(errCode),
          file_(), line_(0), function_(), suggestion_() {}
    
    NLMException(const std::string& message, ErrorCategory cat, 
                 const std::string& file, int line, 
                 const std::string& func, int errCode = 0)
        : std::runtime_error(message), category(cat), code(errCode),
          file_(file), line_(line), function_(func), suggestion_() {}
    
    // Add contextual information to error
    NLMException& withFile(const std::string& file, int line, const std::string& func) {
        file_ = file;
        line_ = line;
        function_ = func;
        return *this;
    }
    
    NLMException& withContext(const std::string& context) {
        context_ = context;
        return *this;
    }
    
    NLMException& withSuggestion(const std::string& suggestion) {
        suggestion_ = suggestion;
        return *this;
    }
    
    // Enhanced error reporting
    std::string what() const override {
        std::ostringstream oss;
        oss << "[NLM Error] ";
        
        if (!file_.empty()) {
            oss << file_;
            if (line_ > 0) oss << ":" << line_;
            if (!function_.empty()) oss << " in " << function_;
            oss << " - ";
        }
        
        oss << std::runtime_error::what();
        
        if (!context_.empty()) {
            oss << " | Context: " << context_;
        }
        
        if (!suggestion_.empty()) {
            oss << " | Suggestion: " << suggestion_;
        }
        
        if (code != 0) {
            oss << " | Error code: " << code;
        }
        
        oss << " | Category: " << static_cast<int>(category);
        
        return oss.str();
    }
    
    // Accessors
    ErrorCategory getCategory() const { return category; }
    int getCode() const { return code; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    const std::string& getContext() const { return context_; }
    const std::string& getSuggestion() const { return suggestion_; }
    
    // Static convenience constructors
    static NLMException config(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::Configuration, code);
    }
    
    static NLMException memory(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::Memory, code);
    }
    
    static NLMException validation(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::InputValidation, code);
    }
    
    static NLMException system(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::System, code);
    }
    
    static NLMException resource(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::Resource, code);
    }
    
    static NLMException python(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::Python, code);
    }
    
private:
    ErrorCategory category;
    int code;
    std::string file_;
    int line_;
    std::string function_;
    std::string context_;
    std::string suggestion_;
};

/**
 * @class InvalidArgumentException
 * @brief Exception for invalid function arguments
 */
#pragma once

#include <string>
#include <stdexcept>
#include <sstream>
#include <system_error>
#include <iostream>

namespace nlm {

/**
 * @enum ErrorCategory
 * @brief Categories of errors for NLM framework
 */
enum class ErrorCategory {
    Configuration = 0,    ///< Configuration loading/validation errors
    Memory = 1,           ///< Memory management errors
    InputValidation = 2,  ///< Input parameter validation errors
    System = 3,           ///< System state errors
    Resource = 4,         ///< Resource access errors
    Python = 5,           ///< Python binding errors
    Unknown = 6           ///< Unknown/unclassified errors
};

/**
 * @class NLMException
 * @brief Base exception class for all NLM errors
 * 
 * Provides enhanced error reporting with context, suggestions, and
 * integration with the logging system.
 */
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message) 
        : std::runtime_error(message), category(ErrorCategory::Unknown), 
          code(0), file_(), line_(0), function_(), suggestion_() {}
    
    NLMException(const std::string& message, ErrorCategory cat, int errCode = 0)
        : std::runtime_error(message), category(cat), code(errCode),
          file_(), line_(0), function_(), suggestion_() {}
    
    NLMException(const std::string& message, ErrorCategory cat, 
                 const std::string& file, int line, 
                 const std::string& func, int errCode = 0)
        : std::runtime_error(message), category(cat), code(errCode),
          file_(file), line_(line), function_(func), suggestion_() {}
    
    // Add contextual information to error
    NLMException& withFile(const std::string& file, int line, const std::string& func) {
        file_ = file;
        line_ = line;
        function_ = func;
        return *this;
    }
    
    NLMException& withContext(const std::string& context) {
        context_ = context;
        return *this;
    }
    
    NLMException& withSuggestion(const std::string& suggestion) {
        suggestion_ = suggestion;
        return *this;
    }
    
    // Enhanced error reporting
    std::string what() const override {
        std::ostringstream oss;
        oss << "[NLM Error] ";
        
        if (!file_.empty()) {
            oss << file_;
            if (line_ > 0) oss << ":" << line_;
            if (!function_.empty()) oss << " in " << function_;
            oss << " - ";
        }
        
        oss << std::runtime_error::what();
        
        if (!context_.empty()) {
            oss << " | Context: " << context_;
        }
        
        if (!suggestion_.empty()) {
            oss << " | Suggestion: " << suggestion_;
        }
        
        if (code != 0) {
            oss << " | Error code: " << code;
        }
        
        oss << " | Category: " << static_cast<int>(category);
        
        return oss.str();
    }
    
    // Accessors
    ErrorCategory getCategory() const { return category; }
    int getCode() const { return code; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    const std::string& getContext() const { return context_; }
    const std::string& getSuggestion() const { return suggestion_; }
    
    // Static convenience constructors
    static NLMException config(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::Configuration, code);
    }
    
    static NLMException memory(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::Memory, code);
    }
    
    static NLMException validation(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::InputValidation, code);
    }
    
    static NLMException system(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::System, code);
    }
    
    static NLMException resource(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::Resource, code);
    }
    
    static NLMException python(const std::string& msg, int code = 0) {
        return NLMException(msg, ErrorCategory::Python, code);
    }
    
private:
    ErrorCategory category;
    int code;
    std::string file_;
    int line_;
    std::string function_;
    std::string context_;
    std::string suggestion_;
};

/**
 * @class InvalidArgumentException
 * @brief Exception for invalid function arguments
 */
class InvalidArgumentException : public NLMException {
public:
    InvalidArgumentException(const std::string& message)
        : NLMException(message, ErrorCategory::InputValidation, 100) {
        withSuggestion("Check input parameters for valid ranges and types");
    }
    
    InvalidArgumentException(const std::string& message, const std::string& paramName)
        : NLMException("Parameter '" + paramName + "': " + message, 
                      ErrorCategory::InputValidation, 101) {
        withSuggestion("Verify parameter '" + paramName + "' meets requirements");
    }
};

/**
 * @class ConfigurationException
 * @brief Exception for configuration errors
 */
class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message)
        : NLMException(message, ErrorCategory::Configuration, 200) {
        withSuggestion("Check configuration files for syntax errors and required parameters");
    }
    
    ConfigurationException(const std::string& message, const std::string& configKey)
        : NLMException("Config key '" + configKey + "': " + message,
                      ErrorCategory::Configuration, 201) {
        withSuggestion("Add or fix configuration for '" + configKey + "'");
    }
};

/**
 * @class MemoryManagementException
 * @brief Exception for memory management errors
 */
class MemoryManagementException : public NLMException {
public:
    MemoryManagementException(const std::string& message)
        : NLMException(message, ErrorCategory::Memory, 300) {
        withSuggestion("Check system resources and memory availability");
    }
    
    MemoryManagementException(const std::string& message, size_t size)
        : NLMException("Requested size " + std::to_string(size) + ": " + message,
                      ErrorCategory::Memory, 301) {
        withSuggestion("Try reducing memory requirements or increase system memory");
    }
};

/**
 * @class ResourceAccessException
 * @brief Exception for resource access errors
 */
class ResourceAccessException : public NLMException {
public:
    ResourceAccessException(const std::string& message)
        : NLMException(message, ErrorCategory::Resource, 400) {
        withSuggestion("Check file permissions and resource availability");
    }
    
    ResourceAccessException(const std::string& message, const std::string& resource)
        : NLMException("Resource '" + resource + "': " + message,
                      ErrorCategory::Resource, 401) {
        withSuggestion("Verify resource '" + resource + "' exists and is accessible");
    }
};

/**
 * @class SystemStateException
 * @brief Exception for system state errors
 */
class SystemStateException : public NLMException {
public:
    SystemStateException(const std::string& message)
        : NLMException(message, ErrorCategory::System, 500) {
        withSuggestion("Check system state and try to reinitialize or reset");
    }
    
    SystemStateException(const std::string& message, const std::string& operation)
        : NLMException("Operation '" + operation + "' failed: " + message,
                      ErrorCategory::System, 501) {
        withSuggestion("Verify system is in proper state for operation '" + operation + "'");
    }
};

/**
 * @class PythonBindingException
 * @brief Exception for Python binding errors
 */
class PythonBindingException : public NLMException {
public:
    PythonBindingException(const std::string& message)
        : NLMException(message, ErrorCategory::Python, 600) {
        withSuggestion("Check Python API usage and data type compatibility");
    }
    
    PythonBindingException(const std::string& message, const std::string& pyType)
        : NLMException("Python type '" + pyType + "': " + message,
                      ErrorCategory::Python, 601) {
        withSuggestion("Convert data to correct Python type or check type compatibility");
    }
};

} // namespace nlm


} // namespace nlm
