#ifndef NLM_AGENT_BRAIN_ERROR_HPP
#define NLM_AGENT_BRAIN_ERROR_HPP

#include <string>
#include <stdexcept>
#include <memory>
#include <sstream>

namespace nlm {

/**
 * AgentBrain error codes for standardized error handling
 */
enum class AgentBrainErrorCode {
    // Invalid input errors
    INVALID_INPUT = 1,      // null pointers, invalid indices
    CONFIGURATION_ERROR = 2, // invalid parameter values
    MEMORY_ERROR = 3,        // allocation failures
    NEURAL_ERROR = 4,        // neural system errors
    STATE_ERROR = 5          // inconsistent state
};

/**
 * Base exception for all AgentBrain errors
 */
class AgentBrainError : public std::runtime_error {
protected:
    AgentBrainErrorCode code_;
    std::string component_;
    std::string context_;
    int line_;
    
public:
    AgentBrainError(AgentBrainErrorCode code, const std::string& component, 
                   const std::string& message, int line = 0)
        : std::runtime_error(formatMessage(message)),
          code_(code), component_(component), context_(message), line_(line) {}
    
    AgentBrainErrorCode getCode() const { return code_; }
    const std::string& getComponent() const { return component_; }
    const std::string& getContext() const { return context_; }
    int getLine() const { return line_; }
    
private:
    static std::string formatMessage(const std::string& message) {
        return "[AgentBrain Error] " + message;
    }
};

/**
 * Exception for invalid input errors (null pointers, invalid indices)
 */
class AgentBrainInvalidInputError : public AgentBrainError {
public:
    AgentBrainInvalidInputError(const std::string& component,
                               const std::string& description,
                               int line = 0)
        : AgentBrainError(AgentBrainErrorCode::INVALID_INPUT,
                         component,
                         "Invalid input: " + description,
                         line) {}
};

/**
 * Exception for configuration errors
 */
class AgentBrainConfigurationError : public AgentBrainError {
public:
    AgentBrainConfigurationError(const std::string& component,
                                const std::string& description,
                                int line = 0)
        : AgentBrainError(AgentBrainErrorCode::CONFIGURATION_ERROR,
                         component,
                         "Configuration error: " + description,
                         line) {}
};

/**
 * Exception for memory allocation errors
 */
class AgentBrainMemoryError : public AgentBrainError {
public:
    AgentBrainMemoryError(const std::string& component,
                         const std::string& description,
                         int line = 0)
        : AgentBrainError(AgentBrainErrorCode::MEMORY_ERROR,
                         component,
                         "Memory error: " + description,
                         line) {}
};

/**
 * Exception for neural system errors
 */
class AgentBrainNeuralError : public AgentBrainError {
public:
    AgentBrainNeuralError(const std::string& component,
                         const std::string& description,
                         int line = 0)
        : AgentBrainError(AgentBrainErrorCode::NEURAL_ERROR,
                         component,
                         "Neural error: " + description,
                         line) {}
};

/**
 * Exception for state consistency errors
 */
class AgentBrainStateError : public AgentBrainError {
public:
    AgentBrainStateError(const std::string& component,
                        const std::string& description,
                        int line = 0)
        : AgentBrainError(AgentBrainErrorCode::STATE_ERROR,
                         component,
                         "State error: " + description,
                         line) {}
};

/**
 * Error code to string conversion for reporting
 */
inline const char* AgentBrainErrorCodeToString(AgentBrainErrorCode code) {
    switch (code) {
        case AgentBrainErrorCode::INVALID_INPUT:
            return "INVALID_INPUT";
        case AgentBrainErrorCode::CONFIGURATION_ERROR:
            return "CONFIGURATION_ERROR";
        case AgentBrainErrorCode::MEMORY_ERROR:
            return "MEMORY_ERROR";
        case AgentBrainErrorCode::NEURAL_ERROR:
            return "NEURAL_ERROR";
        case AgentBrainErrorCode::STATE_ERROR:
            return "STATE_ERROR";
        default:
            return "UNKNOWN_ERROR";
    }
}

/**
 * Result type for error handling - similar to std::expected from C++23
 * but available in current C++ version
 */
template<typename T>
class AgentBrainResult {
private:
    T value_;
    std::shared_ptr<AgentBrainError> error_;
    bool hasValue_ = false;
    
public:
    // Constructors
    AgentBrainResult() = default;
    
    explicit AgentBrainResult(const T& value) : value_(value), hasValue_(true) {}
    
    explicit AgentBrainResult(T&& value) : value_(std::move(value)), hasValue_(true) {}
    
    explicit AgentBrainResult(std::shared_ptr<AgentBrainError> error) 
        : error_(std::move(error)), hasValue_(false) {}
    
    // Check if result contains a value
    explicit operator bool() const { return hasValue_; }
    
    // Check if result contains an error
    bool hasError() const { return !hasValue_; }
    
    // Access the value (throw if error)
    T& value() {
        if (!hasValue_) {
            throw *error_;
        }
        return value_;
    }
    
    const T& value() const {
        if (!hasValue_) {
            throw *error_;
        }
        return value_;
    }
    
    // Access the error (if any)
    const std::shared_ptr<AgentBrainError>& error() const { return error_; }
    
    // Get value or default (no throw)
    T getValueOr(const T& defaultValue) const {
        if (hasValue_) {
            return value_;
        }
        return defaultValue;
    }
    
    // Get value or throw with custom message
    T getValueOrThrow(const std::string& customMessage) const {
        if (hasValue_) {
            return value_;
        }
        std::ostringstream oss;
        oss << customMessage << ". Error: " << error_->what();
        throw std::runtime_error(oss.str());
    }
    
    // Helper for converting to bool
    bool operator!() const { return !hasValue_; }
};

} // namespace nlm

#endif // NLM_AGENT_BRAIN_ERROR_HPP