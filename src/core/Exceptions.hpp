#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

// Base NLM error class
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) 
        : std::runtime_error("NLM Error: " + message) {}
    
    virtual ~NLMError() = default;
};

// Configuration-specific errors
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) 
        : NLMError("Config Error: " + message) {}
    
    virtual ~ConfigError() = default;
};

class ConfigLoadError : public ConfigError {
public:
    explicit ConfigLoadError(const std::string& filepath, const std::string& reason = "")
        : ConfigError("Failed to load config from " + filepath + ": " + reason) {}
    
    virtual ~ConfigLoadError() = default;
};

class ConfigParseError : public ConfigError {
public:
    explicit ConfigParseError(const std::string& message)
        : ConfigError("Parse error: " + message) {}
    
    virtual ~ConfigParseError() = default;
};

// Brain-specific errors
class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message)
        : NLMError("Brain Error: " + message) {}
    
    virtual ~BrainError() = default;
};

class BrainInitializationError : public BrainError {
public:
    explicit BrainInitializationError(const std::string& reason)
        : BrainError("Initialization failed: " + reason) {}
    
    virtual ~BrainInitializationError() = default;
};

class MemorySystemError : public BrainError {
public:
    explicit MemorySystemError(const std::string& message)
        : BrainError("Memory system error: " + message) {}
    
    virtual ~MemorySystemError() = default;
};

class PredictionSystemError : public BrainError {
public:
    explicit PredictionSystemError(const std::string& message)
        : BrainError("Prediction system error: " + message) {}
    
    virtual ~PredictionSystemError() = default;
};

class CognitionSystemError : public BrainError {
public:
    explicit CognitionSystemError(const std::string& message)
        : BrainError("Cognition system error: " + message) {}
    
    virtual ~CognitionSystemError() = default;
};

class NeuromodulationError : public BrainError {
public:
    explicit NeuromodulationError(const std::string& message)
        : BrainError("Neuromodulation error: " + message) {}
    
    virtual ~NeuromodulationError() = default;
};

class DevelopmentError : public BrainError {
public:
    explicit DevelopmentError(const std::string& message)
        : BrainError("Development error: " + message) {}
    
    virtual ~DevelopmentError() = default;
};

// General system errors
class MemoryAccessError : public NLMError {
public:
    explicit MemoryAccessError(const std::string& message)
        : NLMError("Memory access error: " + message) {}
    
    virtual ~MemoryAccessError() = default;
};

class SystemAvailabilityError : public NLMError {
public:
    explicit SystemAvailabilityError(const std::string& message)
        : NLMError("System unavailable: " + message) {}
    
    virtual ~SystemAvailabilityError() = default;
};

class PlasticityError : public NLMError {
public:
    explicit PlasticityError(const std::string& message)
        : NLMError("Plasticity error: " + message) {}
    
    virtual ~PlasticityError() = default;
};

class StateError : public NLMError {
public:
    explicit StateError(const std::string& message)
        : NLMError("State error: " + message) {}
    
    virtual ~StateError() = default;
};

// Exception factory functions for easier usage
[[nodiscard]] inline ConfigLoadError makeConfigLoadError(const std::string& filepath, const std::string& reason) {
    return ConfigLoadError(filepath, reason);
}

[[nodiscard]] inline BrainInitializationError makeBrainInitializationError(const std::string& reason) {
    return BrainInitializationError(reason);
}

[[nodiscard]] inline MemoryAccessError makeMemoryAccessError(const std::string& message) {
    return MemoryAccessError(message);
}

[[nodiscard]] inline SystemAvailabilityError makeSystemAvailabilityError(const std::string& message) {
    return SystemAvailabilityError(message);
}

} // namespace nlm
