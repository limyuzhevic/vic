#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

/**
 * @brief Base exception class for all NLM brain errors
 */
class BrainException : public std::runtime_error {
public:
    explicit BrainException(const std::string& message) 
        : std::runtime_error("NLM Brain Error: " + message) {}
    
    explicit BrainException(const std::string& context, const std::string& message)
        : std::runtime_error("NLM Brain Error: " + context + ": " + message) {}
};

/**
 * @brief Exception thrown when brain initialization fails
 */
class InitializationException : public BrainException {
public:
    explicit InitializationException(const std::string& message)
        : BrainException("Initialization", message) {}
    
    explicit InitializationException(const std::string& component, const std::string& message)
        : BrainException("Initialization: " + component, message) {}
};

/**
 * @brief Exception thrown when configuration operations fail
 */
class ConfigurationException : public BrainException {
public:
    explicit ConfigurationException(const std::string& message)
        : BrainException("Configuration", message) {}
    
    explicit ConfigurationException(const std::string& key, const std::string& message)
        : BrainException("Configuration: key=" + key, message) {}
};

/**
 * @brief Exception thrown when neuron operations fail
 */
class NeuronException : public BrainException {
public:
    explicit NeuronException(const std::string& message)
        : BrainException("Neuron", message) {}
    
    explicit NeuronException(const NeuronId& id, const std::string& message)
        : BrainException("Neuron: id=" + std::to_string(id.value), message) {}
};

/**
 * @brief Exception thrown when synapse operations fail
 */
class SynapseException : public BrainException {
public:
    explicit SynapseException(const std::string& message)
        : BrainException("Synapse", message) {}
    
    explicit SynapseException(const SynapseId& id, const std::string& message)
        : BrainException("Synapse: id=" + std::to_string(id.value), message) {}
};

/**
 * @brief Exception thrown when memory operations fail
 */
class MemoryException : public BrainException {
public:
    explicit MemoryException(const std::string& message)
        : BrainException("Memory", message) {}
    
    explicit MemoryException(const std::string& system, const std::string& message)
        : BrainException("Memory: " + system, message) {}
};

/**
 * @brief Exception thrown when plasticity operations fail
 */
class PlasticityException : public BrainException {
public:
    explicit PlasticityException(const std::string& message)
        : BrainException("Plasticity", message) {}
    
    explicit PlasticityException(const std::string& rule, const std::string& message)
        : BrainException("Plasticity: " + rule, message) {}
};

/**
 * @brief Exception thrown when neuromodulation operations fail
 */
class NeuromodulationException : public BrainException {
public:
    explicit NeuromodulationException(const std::string& message)
        : BrainException("Neuromodulation", message) {}
    
    explicit NeuromodulationException(const std::string& modulator, const std::string& message)
        : BrainException("Neuromodulation: " + modulator, message) {}
};

/**
 * @brief Exception thrown when world interaction fails
 */
class WorldException : public BrainException {
public:
    explicit WorldException(const std::string& message)
        : BrainException("World", message) {}
    
    explicit WorldException(const std::string& operation, const std::string& message)
        : BrainException("World: " + operation, message) {}
};

/**
 * @brief Exception thrown when file I/O operations fail
 */
class IOError : public BrainException {
public:
    explicit IOError(const std::string& message)
        : BrainException("File I/O", message) {}
    
    explicit IOError(const std::string& filepath, const std::string& message)
        : BrainException("File I/O: " + filepath, message) {}
};

// Helper function for creating exception messages with context
inline std::string formatError(const std::string& context, const std::string& operation, const std::string& details = "") {
    std::ostringstream oss;
    oss << context;
    if (!operation.empty()) {
        oss << ". " << operation;
    }
    if (!details.empty()) {
        oss << ". " << details;
    }
    return oss.str();
}

// Helper function for validating preconditions
inline void validatePrecondition(bool condition, const std::string& context, const std::string& operation) {
    if (!condition) {
        throw InitializationException(formatError(context, operation, "Precondition not met"));
    }
}

// Helper function for validating configuration
inline void validateConfig(const std::string& key, const std::string& value, const std::string& constraint) {
    if (value.empty() && key.find("name") != std::string::npos) {
        throw ConfigurationException(key, "Name cannot be empty");
    }
}

// Helper function for validating neuron operations
inline void validateNeuron(const NeuronId& id, const std::string& operation) {
    if (id.value == 0) {
        throw NeuronException(id, "Invalid neuron ID for " + operation);
    }
}

// Helper function for validating synapse operations
inline void validateSynapse(const SynapseId& id, const std::string& operation) {
    if (id.value == 0) {
        throw SynapseException(id, "Invalid synapse ID for " + operation);
    }
}

// Helper function for validating file operations
inline void validateFileOperation(const std::string& filepath, bool success, const std::string& operation) {
    if (!success && !filepath.empty()) {
        throw IOError(filepath, operation + " failed");
    }
}

// Helper function for memory validation
inline void validateMemoryAllocation(void* ptr, const std::string& context) {
    if (!ptr) {
        throw MemoryException(context, "Memory allocation failed");
    }
}

// Helper function for plasticity validation
inline void validatePlasticity(float weightChange, const std::string& rule) {
    if (std::isnan(weightChange)) {
        throw PlasticityException(rule, "NaN weight change detected");
    }
    if (std::isinf(weightChange)) {
        throw PlasticityException(rule, "Infinite weight change detected");
    }
}

// Helper function for neuromodulation validation
inline void validateNeuromodulation(float level, const std::string& modulator) {
    if (std::isnan(level)) {
        throw NeuromodulationException(modulator, "NaN neuromodulation level");
    }
    if (level < 0.0f) {
        throw NeuromodulationException(modulator, "Negative neuromodulation level: " + std::to_string(level));
    }
}

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
