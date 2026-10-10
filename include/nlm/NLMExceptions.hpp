#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

/**
 * Base exception for all NLM errors
 */
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * Exception for configuration-related errors
 */
class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message) : NLMException("ConfigurationError: " + message) {}
};

/**
 * Exception for simulation-related errors
 */
class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message) : NLMException("SimulationError: " + message) {}
};

/**
 * Exception for validation errors
 */
class ValidationException : public NLMException {
public:
    explicit ValidationException(const std::string& message) : NLMException("ValidationError: " + message) {}
};

/**
 * Exception for plasticity rule errors
 */
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message) : NLMException("PlasticityError: " + message) {}
};

/**
 * Exception for memory-related errors
 */
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) : NLMException("MemoryError: " + message) {}
};

/**
 * Exception for agent-related errors
 */
class AgentException : public NLMException {
public:
    explicit AgentException(const std::string& message) : NLMException("AgentError: " + message) {}
};
}