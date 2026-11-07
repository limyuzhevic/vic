// NLM Error Handling System
// Centralized error handling and validation for NLM

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

namespace nlm {

// Custom exception types for NLM
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error(message) {}
    
    NLMException(const std::string& component, const std::string& operation, const std::string& details)
        : std::runtime_error(formatMessage(component, operation, details)) {}
    
protected:
    static std::string formatMessage(const std::string& component, 
                                    const std::string& operation, 
                                    const std::string& details) {
        std::stringstream ss;
        ss << "NLM Error[" << component << "] " << operation;
        if (!details.empty()) {
            ss << ": " << details;
        }
        return ss.str();
    }
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& details) 
        : NLMException("Memory", "Allocation/Access", details) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& details) 
        : NLMException("Config", "Validation", details) {}
};

class NeuralError : public NLMException {
public:
    explicit NeuralError(const std::string& details) 
        : NLMException("Neural", "Operation", details) {}
};

// Error context and validation utilities
class ErrorContext {
private:
    std::vector<std::string> errors_;
    
public:
    void addError(const std::string& error) {
        errors_.push_back(error);
    }
    
    bool hasErrors() const {
        return !errors_.empty();
    }
    
    std::string getErrors() const {
        if (errors_.empty()) return "No errors";
        std::stringstream ss;
        for (size_t i = 0; i < errors_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << errors_[i];
        }
        return ss.str();
    }
    
    void clear() {
        errors_.clear();
    }
};

// Validation helpers
namespace Validation {
    
    template<typename T>
    void validateNotNull(const T* ptr, const std::string& name) {
        if (ptr == nullptr) {
            throw NLMException("Validation", "NotNull", name + " cannot be null");
        }
    }
    
    template<typename T>
    void validateRange(const T& value, const T& min, const T& max, const std::string& name) {
        if (value < min || value > max) {
            std::stringstream ss;
            ss << name << " must be between " << min << " and " << max << ", got " << value;
            throw ConfigurationError(ss.str());
        }
    }
    
    void validatePositive(int value, const std::string& name) {
        if (value <= 0) {
            throw ConfigurationError(name + " must be positive, got " + std::to_string(value));
        }
    }
    
    void validateProbability(float prob, const std::string& name) {
        if (prob < 0.0f || prob > 1.0f) {
            throw ConfigurationError(name + " must be between 0.0 and 1.0, got " + std::to_string(prob));
        }
    }
}

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP
