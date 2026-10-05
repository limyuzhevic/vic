#pragma once

#include <string>
#include <stdexcept>
#include <format>

namespace nlm {

// Base exception for all NLM Brain errors
class BrainError : public std::runtime_error {
public:
    explicit BrainError(const std::string& message) : std::runtime_error(message) {}
    virtual ~BrainError() = default;
};

// Configuration-related errors
class BrainConfigError : public BrainError {
public:
    explicit BrainConfigError(const std::string& message) : BrainError("Configuration error: " + message) {}
    virtual ~BrainConfigError() = default;
};

class MissingConfigValueError : public BrainConfigError {
public:
    MissingConfigValueError(const std::string& key, const std::string& type) 
        : BrainConfigError(std::format("Missing required configuration value '{}' of type {}", key, type)) {}
    virtual ~MissingConfigValueError() = default;
};

class InvalidConfigValueError : public BrainConfigError {
public:
    InvalidConfigValueError(const std::string& key, const std::string& value, const std::string& reason)
        : BrainConfigError(std::format("Invalid configuration value '{}'='{}': {}", key, value, reason)) {}
    virtual ~InvalidConfigValueError() = default;
};

// Initialization errors
class BrainInitError : public BrainError {
public:
    explicit BrainInitError(const std::string& message) : BrainError("Initialization error: " + message) {}
    virtual ~BrainInitError() = default;
};

class NeuronInitializationError : public BrainInitError {
public:
    NeuronInitializationError(size_t neuronId, const std::string& reason)
        : BrainInitError(std::format("Failed to initialize neuron {}: {}", neuronId, reason)) {}
    virtual ~NeuronInitializationError() = default;
};

// Memory system errors
class MemorySystemError : public BrainError {
public:
    explicit MemorySystemError(const std::string& message) : BrainError("Memory system error: " + message) {}
    virtual ~MemorySystemError() = default;
};

class WorkingMemoryError : public MemorySystemError {
public:
    explicit WorkingMemoryError(const std::string& message) : MemorySystemError("Working memory: " + message) {}
    virtual ~WorkingMemoryError() = default;
};

// State management errors
class StateError : public BrainError {
public:
    explicit StateError(const std::string& message) : BrainError("State error: " + message) {}
    virtual ~StateError() = default;
};

class InvalidStateError : public StateError {
public:
    InvalidStateError(const std::string& operation, const std::string& reason)
        : StateError(std::format("Invalid state for operation '{}': {}", operation, reason)) {}
    virtual ~InvalidStateError() = default;
};

// Checkpoint/save/load errors
class CheckpointError : public BrainError {
public:
    explicit CheckpointError(const std::string& message) : BrainError("Checkpoint error: " + message) {}
    virtual ~CheckpointError() = default;
};

class SaveError : public CheckpointError {
public:
    SaveError(const std::string& filepath, const std::string& reason)
        : CheckpointError(std::format("Failed to save brain state to '{}': {}", filepath, reason)) {}
    virtual ~SaveError() = default;
};

class LoadError : public CheckpointError {
public:
    LoadError(const std::string& filepath, const std::string& reason)
        : CheckpointError(std::format("Failed to load brain state from '{}': {}", filepath, reason)) {}
    virtual ~LoadError() = default;
};

// Plasticity errors
class PlasticityError : public BrainError {
public:
    explicit PlasticityError(const std::string& message) : BrainError("Plasticity error: " + message) {}
    virtual ~PlasticityError() = default;
};

// Neuropathy errors
class NeuromodulationError : public BrainError {
public:
    explicit NeuromodulationError(const std::string& message) : BrainError("Neuromodulation error: " + message) {}
    virtual ~NeuromodulationError() = default;
};

// Development errors
class DevelopmentError : public BrainError {
public:
    explicit DevelopmentError(const std::string& message) : BrainError("Development error: " + message) {}
    virtual ~DevelopmentError() = default;
};

} // namespace nlm