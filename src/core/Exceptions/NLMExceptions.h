#ifndef NLM_EXCEPTIONS_H
#define NLM_EXCEPTIONS_H

#include <string>
#include <stdexcept>

namespace nlm {
    
    /**
     * Base exception class for all NLM errors
     */
    class NLMException : public std::runtime_error {
    public:
        explicit NLMException(const std::string& message) : std::runtime_error(message) {}
    };
    
    /**
     * Configuration-related exceptions
     */
    class ConfigException : public NLMException {
    public:
        explicit ConfigException(const std::string& message) : NLMException("Config Error: " + message) {}
    };
    
    class ConfigValidationException : public ConfigException {
    public:
        explicit ConfigValidationException(const std::string& message) : ConfigException("Validation Error: " + message) {}
    };
    
    /**
     * Brain/simulation-related exceptions
     */
    class BrainException : public NLMException {
    public:
        explicit BrainException(const std::string& message) : NLMException("Brain Error: " + message) {}
    };
    
    class InitializationException : public BrainException {
    public:
        explicit InitializationException(const std::string& message) : BrainException("Initialization Error: " + message) {}
    };
    
    /**
     * Memory-related exceptions
     */
    class MemoryException : public NLMException {
    public:
        explicit MemoryException(const std::string& message) : NLMException("Memory Error: " + message) {}
    };
    
    /**
     * Invalid state exceptions
     */
    class InvalidStateException : public NLMException {
    public:
        explicit InvalidStateException(const std::string& message) : NLMException("Invalid State: " + message) {}
    };
    
    /**
     * Performance-related exceptions
     */
    class PerformanceException : public NLMException {
    public:
        explicit PerformanceException(const std::string& message) : NLMException("Performance Error: " + message) {}
    };
    
} // namespace nlm

#endif // NLM_EXCEPTIONS_H