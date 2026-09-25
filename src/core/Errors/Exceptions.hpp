#pragma once

#include "Config.hpp"
#include <stdexcept>
#include <string>
#include <sstream>

namespace nlm {

// Configuration-specific exceptions
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message)
        : std::runtime_error("Configuration Error: " + message) {}
};

class ConfigFileNotFoundException : public ConfigException {
public:
    explicit ConfigFileNotFoundException(const std::string& filepath)
        : ConfigException("Configuration file not found: " + filepath) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& key, const std::string& value, const std::string& reason)
        : ConfigException("Failed to parse configuration key \"" + key + "\" with value \"" + value + "\": " + reason) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& key, const std::string& reason)
        : ConfigException("Configuration validation failed for key \"" + key + "\": " + reason) {}
};

class ConfigSchemaException : public ConfigException {
public:
    explicit ConfigSchemaException(const std::string& message)
        : ConfigException("Configuration schema error: " + message) {}
};

// Configuration file format exceptions
class ConfigJSONParseException : public ConfigException {
public:
    explicit ConfigJSONParseException(const std::string& filepath, const std::string& details)
        : ConfigException("JSON parsing failed for file \"" + filepath + "\": " + details) {}
};

class ConfigYAMLParseException : public ConfigException {
public:
    explicit ConfigYAMLParseException(const std::string& filepath, const std::string& details)
        : ConfigException("YAML parsing failed for file \"" + filepath + "\": " + details) {}
};

class ConfigINIParseException : public ConfigException {
public:
    explicit ConfigINIParseException(const std::string& filepath, const std::string& details)
        : ConfigException("INI parsing failed for file \"" + filepath + "\": " + details) {}
};

// Brain simulation exceptions
class SimulationException : public std::runtime_error {
public:
    explicit SimulationException(const std::string& message)
        : std::runtime_error("Simulation Error: " + message) {}
};

class BrainInitializationException : public SimulationException {
public:
    explicit BrainInitializationException(const std::string& reason)
        : SimulationException("Brain initialization failed: " + reason) {}
};

class SimulationStepException : public SimulationException {
public:
    explicit SimulationStepException(SimulationStep step, const std::string& reason)
        : SimulationException("Simulation step " + std::to_string(step) + " failed: " + reason) {}
};

class MemoryError : public SimulationException {
public:
    explicit MemoryError(const std::string& message)
        : SimulationException("Memory error: " + message) {}
};

// Agent simulation exceptions
class AgentException : public std::runtime_error {
public:
    explicit AgentException(const std::string& message)
        : std::runtime_error("Agent Error: " + message) {}
};

class AgentInitializationException : public AgentException {
public:
    explicit AgentInitializationException(const std::string& reason)
        : AgentException("Agent initialization failed: " + reason) {}
};

class ActionException : public AgentException {
public:
    explicit ActionException(const std::string& reason)
        : AgentException("Action failed: " + reason) {}
};

// Python binding exceptions
class PythonBindingException : public std::runtime_error {
public:
    explicit PythonBindingException(const std::string& message)
        : std::runtime_error("Python Binding Error: " + message) {}
};

class TypeConversionException : public PythonBindingException {
public:
    explicit TypeConversionException(const std::string& fromType, const std::string& toType, const std::string& value)
        : PythonBindingException("Cannot convert " + fromType + " to " + toType + ": \"" + value + "\") {}
};

class ArgumentError : public PythonBindingException {
public:
    explicit ArgumentError(const std::string& message)
        : PythonBindingException("Argument error: " + message) {}
};

} // namespace nlm
