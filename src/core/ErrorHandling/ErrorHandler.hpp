// Enhanced error handling and validation system
// Provides comprehensive error reporting and validation for neural computations

#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>

namespace nlm {

// Enhanced exception hierarchy for neural computation errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
    virtual ~NLMError() = default;
};

class NeuralComputationError : public NLMError {
public:
    explicit NeuralComputationError(const std::string& message) 
        : NLMError("Neural Computation Error: " + message) {}
};

class PlasticityError : public NLMError {
public:
    explicit PlasticityError(const std::string& message) 
        : NLMError("Plasticity Error: " + message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) 
        : NLMError("Configuration Error: " + message) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message) 
        : NLMError("Validation Error: " + message) {}
};

// Comprehensive error reporting and logging system
class ErrorHandler {
public:
    enum class ErrorLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    struct ErrorInfo {
        ErrorLevel level;
        std::string category;
        std::string message;
        std::string file;
        int line;
        std::string context;
        std::chrono::system_clock::time_point timestamp;
        
        ErrorInfo(ErrorLevel lvl, const std::string& cat, const std::string& msg,
                 const std::string& file = "", int line_num = 0,
                 const std::string& ctx = "")
            : level(lvl), category(cat), message(msg), file(file), line(line_num),
              context(ctx), timestamp(std::chrono::system_clock::now()) {}
    };
    
    ErrorHandler();
    ~ErrorHandler();
    
    // Disable copying, enable moving
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    ErrorHandler(ErrorHandler&&) noexcept;
    ErrorHandler& operator=(ErrorHandler&&) noexcept;
    
    // Error reporting
    void report(ErrorLevel level, const std::string& category, const std::string& message,
                const std::string& file = "", int line = 0, const std::string& context = "");
    
    void debug(const std::string& category, const std::string& message,
               const std::string& file = "", int line = 0, const std::string& context = "");
    
    void info(const std::string& category, const std::string& message,
              const std::string& file = "", int line = 0, const std::string& context = "");
    
    void warning(const std::string& category, const std::string& message,
                 const std::string& file = "", int line = 0, const std::string& context = "");
    
    void error(const std::string& category, const std::string& message,
               const std::string& file = "", int line = 0, const std::string& context = "");
    
    void critical(const std::string& category, const std::string& message,
                  const std::string& file = "", int line = 0, const std::string& context = "");
    
    // Exception throwing with context
    void throwException(ErrorLevel level, const std::string& category, const std::string& message,
                       const std::string& file = "", int line = 0, const std::string& context = "");
    
    // Validation
    bool validate(bool condition, const std::string& category, const std::string& message,
                  const std::string& file = "", int line = 0, const std::string& context = "");
    
    void validate(const std::string& category, const std::string& message,
                  const std::string& file = "", int line = 0, const std::string& context = "");
    
    // Bounds checking
    template<typename T>
    void checkBounds(const T& value, const T& min, const T& max,
                    const std::string& category, const std::string& parameter,
                    const std::string& file = "", int line = 0);
    
    // Null pointer checking
    template<typename T>
    void checkNull(const T* ptr, const std::string& category, const std::string& parameter,
                   const std::string& file = "", int line = 0);
    
    // Configuration validation
    void validateConfig(const Config& config, const std::vector<ConfigSchema>& schema);
    
    // Get error statistics
    struct ErrorStats {
        size_t debugCount;
        size_t infoCount;
        size_t warningCount;
        size_t errorCount;
        size_t criticalCount;
        size_t totalCount;
    };
    
    ErrorStats getErrorStats() const;
    
    // Get recent errors
    std::vector<ErrorInfo> getRecentErrors(ErrorLevel minLevel = ErrorLevel::ERROR,
                                           const std::string& category = "") const;
    
    // Clear error history
    void clearErrorHistory();
    
    // Set error level threshold
    void setErrorLevelThreshold(ErrorLevel level);
    
    // Get current error level threshold
    ErrorLevel getErrorLevelThreshold() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for error reporting
#define NLM_THROW_ERROR(category, message) \
    nlm::ErrorHandler::instance().throwException( \
        nlm::ErrorHandler::ErrorLevel::ERROR, category, message, __FILE__, __LINE__)

#define NLM_THROW_VALIDATION(category, message) \
    nlm::ErrorHandler::instance().throwException( \
        nlm::ErrorHandler::ErrorLevel::ERROR, "Validation", message, __FILE__, __LINE__)

#define NLM_VALIDATE(condition, category, message) \
    if (!(condition)) { \
        nlm::ErrorHandler::instance().throwException( \
            nlm::ErrorHandler::ErrorLevel::ERROR, category, message, __FILE__, __LINE__); \
    }

#define NLM_CHECK_BOUNDS(value, min, max, category, parameter) \
    nlm::ErrorHandler::instance().checkBounds(value, min, max, category, parameter, __FILE__, __LINE__)

#define NLM_CHECK_NULL(ptr, category, parameter) \
    nlm::ErrorHandler::instance().checkNull(ptr, category, parameter, __FILE__, __LINE__)

// Global error handler instance
ErrorHandler& errorHandler();

// Convenience functions for common error scenarios
void reportNeuronError(const Neuron& neuron, const std::string& operation, const std::string& context = "");
void reportSynapseError(const Synapse& synapse, const std::string& operation, const std::string& context = "");
void reportConfigurationError(const std::string& key, const std::string& operation, const std::string& context = "");

} // namespace nlm
