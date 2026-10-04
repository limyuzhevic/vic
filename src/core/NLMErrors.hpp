# NLM Python Exception Classes

# NLM Python Exception Classes

import pybind11::pybind11.h
#include <stdexcept>
#include <string>

namespace py = pybind11;
namespace nlm {

/**
 * Base exception class for all NLM errors
 * Provides consistent error handling across Python bindings
 */
class NLMError : public std::exception {
public:
    explicit NLMError(const std::string& message) : message_("NLM Error: " + message) {}
    explicit NLMError(const char* message) : message_("NLM Error: " + std::string(message)) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    const std::string& message() const {
        return message_;
    }
    
protected:
    std::string message_;
};

/**
 * Exception thrown when configuration is invalid
 */
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) : NLMError("Config: " + message) {}
};

/**
 * Exception thrown when brain operations fail
 */
class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError("Brain: " + message) {}
};

/**
 * Exception thrown when neural operations fail
 */
class NeuronError : public NLMError {
public:
    explicit NeuronError(const std::string& message) : NLMError("Neuron: " + message) {}
};

/**
 * Exception thrown when memory operations fail
 */
class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError("Memory: " + message) {}
};

/**
 * Exception thrown when world operations fail
 */
class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message) : NLMError("World: " + message) {}
};

/**
 * Exception thrown when agent operations fail
 */
class AgentError : public NLMError {
public:
    explicit AgentError(const std::string& message) : NLMError("Agent: " + message) {}
};

} // namespace nlm
