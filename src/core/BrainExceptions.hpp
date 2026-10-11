// BrainException hierarchy for comprehensive error handling
#pragma once

#include <stdexcept>
#include <string>

namespace nlm {

class BrainException : public std::runtime_error {
public:
    explicit BrainException(const std::string& message)
        : std::runtime_error("NLM Brain Error: " + message) {}
};

class BrainInitializationError : public BrainException {
public:
    explicit BrainInitializationError(const std::string& message)
        : BrainException("Brain initialization failed: " + message) {}
};

class BrainStepError : public BrainException {
public:
    explicit BrainStepError(const std::string& message)
        : BrainException("Brain step failed: " + message) {}
};

class MemoryError : public BrainException {
public:
    explicit MemoryError(const std::string& message)
        : BrainException("Memory system error: " + message) {}
};

class PlasticityError : public BrainException {
public:
    explicit PlasticityError(const std::string& message)
        : BrainException("Plasticity system error: " + message) {}
};

class NeuromodulationError : public BrainException {
public:
    explicit NeuromodulationError(const std::string& message)
        : BrainException("Neuromodulation system error: " + message) {}
};

class CheckpointError : public BrainException {
public:
    explicit CheckpointError(const std::string& message)
        : BrainException("Checkpoint error: " + message) {}
};

} // namespace nlm
