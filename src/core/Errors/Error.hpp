// Error hierarchy for NLM
#pragma once

#include <stdexcept>
#include <string>
#include <sstream>
#include <system_error>

namespace nlm {

// Base error class for all NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
    virtual ~NLMError() = default;
};

// Configuration-related errors
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) : NLMError("Config error: " + message) {}
};

class ConfigParseError : public ConfigError {
public:
    explicit ConfigParseError(const std::string& message) : ConfigError("Parse error: " + message) {}
};

class ConfigLoadError : public ConfigError {
public:
    explicit ConfigLoadError(const std::string& message) : ConfigError("Load error: " + message) {}
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& message) : ConfigError("Validation error: " + message) {}
};

// World/environment related errors
class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message) : NLMError("World error: " + message) {}
};

class InvalidPositionError : public WorldError {
public:
    explicit InvalidPositionError(float x, float y) : 
        WorldError("Invalid position: (" + std::to_string(x) + ", " + std::to_string(y) + ")") {}
};

class CollisionError : public WorldError {
public:
    explicit CollisionError(const std::string& message) : WorldError("Collision: " + message) {}
};

// Agent-related errors
class AgentError : public NLMError {
public:
    explicit AgentError(const std::string& message) : NLMError("Agent error: " + message) {}
};

// Brain/neural network errors
class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError("Brain error: " + message) {}
};

// Memory-related errors
class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError("Memory error: " + message) {}
};

// Exception helper for better error messages
class ExceptionHelper {
public:
    template<typename T, typename... Args>
    static T makeError(Args&&... args) {
        std::ostringstream oss;
        (oss << ... << args);
        return T(oss.str());
    }
};

} // namespace nlm