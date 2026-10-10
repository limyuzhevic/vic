#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

/**
 * Base exception for NLM errors
 */
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error("NLM: " + message) {}
    
    explicit NLMException(const char* message)
        : std::runtime_error(std::string("NLM: ") + message) {}
};

/**
 * Exception for configuration errors
 */
class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message) 
        : NLMException("Configuration error: " + message) {}
    
    explicit ConfigurationException(const char* message)
        : NLMException(std::string("Configuration error: ") + message) {}
};

/**
 * Exception for brain operation errors
 */
class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message) 
        : NLMException("Brain error: " + message) {}
    
    explicit BrainException(const char* message)
        : NLMException(std::string("Brain error: ") + message) {}
};

/**
 * Exception for memory system errors
 */
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) 
        : NLMException("Memory error: " + message) {}
    
    explicit MemoryException(const char* message)
        : NLMException(std::string("Memory error: ") + message) {}
};

/**
 * Exception for plasticity rule errors
 */
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message) 
        : NLMException("Plasticity error: " + message) {}
    
    explicit PlasticityException(const char* message)
        : NLMException(std::string("Plasticity error: ") + message) {}
};

/**
 * Exception for neuromodulation errors
 */
class NeuromodulationException : public NLMException {
public:
    explicit NeuromodulationException(const std::string& message) 
        : NLMException("Neuromodulation error: " + message) {}
    
    explicit NeuromodulationException(const char* message)
        : NLMException(std::string("Neuromodulation error: ") + message) {}
};

/**
 * Result type for operations that can succeed or fail
 */
template<typename T>
class Result {
public:
    Result() : success_(false), has_value_(false) {}
    
    Result(const T& value) : success_(true), value_(value), has_value_(true) {}
    
    Result(const std::string& error) : success_(false), error_(error), has_value_(false) {}
    
    bool success() const { return success_; }
    bool has_value() const { return has_value_; }
    
    const T& value() const {
        if (!has_value_) {
            throw NLMException("No value available - operation failed");
        }
        return value_;
    }
    
    const std::string& error() const {
        if (success_) {
            throw NLMException("No error available - operation succeeded");
        }
        return error_;
    }
    
private:
    bool success_;
    bool has_value_;
    T value_;
    std::string error_;
};

} // namespace nlm
