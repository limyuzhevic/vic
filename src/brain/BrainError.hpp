#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include "Brain.hpp"

namespace nlm {

// Forward declarations
class Config;
class ILogger;

/**
 * @brief Abstract base class for all Brain subsystems
 */
class BrainSystem {
public:
    virtual ~BrainSystem() = default;
    
    virtual bool initialize() = 0;
    virtual void step(TimestepDuration dt) = 0;
    virtual void reset() = 0;
    virtual bool isHealthy() const = 0;
};

/**
 * @brief Abstract base class for memory systems
 */
class MemorySystem : public BrainSystem {
public:
    virtual void storeToNeuron(NeuronId neuron, float activation) = 0;
    virtual void update(TimestepDuration dt) = 0;
    virtual std::vector<NeuronId> getMemoryNeurons() const = 0;
    virtual void clear() = 0;
};

/**
 * @brief Abstract base class for prediction systems
 */
class PredictionSystemBase : public BrainSystem {
public:
    virtual void updateWithInput(const std::vector<float>& input) = 0;
    virtual float getPredictionError() const = 0;
};

/**
 * @brief Abstract base class for cognition systems
 */
class CognitionSystem : public BrainSystem {
public:
    virtual void update(TimestepDuration dt) = 0;
    virtual void setAttentionWeight(float weight) = 0;
};

/**
 * @brief Abstract base class for neuromodulation systems
 */
class NeuromodulationSystem : public BrainSystem {
public:
    virtual void update(TimestepDuration dt) = 0;
    virtual float getLevel() const = 0;
    virtual float getPlasticityFactor() const = 0;
};

/**
 * @brief Abstract base class for development systems
 */
class DevelopmentSystemBase : public BrainSystem {
public:
    virtual void update(TimestepDuration dt) = 0;
    virtual void setStage(DevelopmentalStage stage) = 0;
    virtual DevelopmentalStage getStage() const = 0;
};

/**
 * @brief Base exception for Brain class errors
 */
class BrainError : public std::runtime_error {
public:
    explicit BrainError(const std::string& message) : std::runtime_error(message) {}
    virtual ~BrainError() = default;
};

/**
 * @brief Exception for initialization errors
 */
class BrainInitializationError : public BrainError {
public:
    explicit BrainInitializationError(const std::string& message) : BrainError("Brain initialization failed: " + message) {}
};

/**
 * @brief Exception for step errors
 */
class BrainStepError : public BrainError {
public:
    explicit BrainStepError(const std::string& message) : BrainError("Brain step failed: " + message) {}
};

/**
 * @brief Exception for configuration errors
 */
class BrainConfigurationError : public BrainError {
public:
    explicit BrainConfigurationError(const std::string& message) : BrainError("Brain configuration error: " + message) {}
};

/**
 * @brief Exception for memory errors
 */
class BrainMemoryError : public BrainError {
public:
    explicit BrainMemoryError(const std::string& message) : BrainError("Brain memory error: " + message) {}
};

/**
 * @brief Exception for I/O errors
 */
class BrainIOException : public BrainError {
public:
    explicit BrainIOException(const std::string& message) : BrainError("Brain I/O error: " + message) {}
};

/**
 * @brief Exception for validation errors
 */
class BrainValidationError : public BrainError {
public:
    explicit BrainValidationError(const std::string& message) : BrainError("Brain validation error: " + message) {}
};

/**
 * @brief Exception for resource management errors
 */
class BrainResourceError : public BrainError {
public:
    explicit BrainResourceError(const std::string& message) : BrainError("Brain resource error: " + message) {}
};

/**
 * @brief RAII resource manager for Brain operations
 */
class BrainResourceGuard {
public:
    virtual ~BrainResourceGuard() = default;
    
    virtual void cleanup() = 0;
    virtual bool isValid() const = 0;
};

/**
 * @brief Helper class for validating Brain preconditions
 */
class BrainValidator {
public:
    static bool validateConfig(std::shared_ptr<const Config> config);
    static bool validateRegions() const;
    static bool validateNeurons() const;
    static bool validateConnections() const;
    static bool validateSystems() const;
    
    static std::string getConfigError() { return "Invalid configuration"; }
    static std::string getRegionsError() { return "Invalid regions"; }
    static std::string getNeuronsError() { return "Invalid neurons"; }
    static std::string getConnectionsError() { return "Invalid connections"; }
    static std::string getSystemsError() { return "Invalid systems"; }
};

/**
 * @brief Utility class for common Brain operations
 */
class BrainUtils {
public:
    static void logError(ILogger& logger, const std::string& operation, const std::string& error);
    static void logWarning(ILogger& logger, const std::string& operation, const std::string& warning);
    static void logInfo(ILogger& logger, const std::string& operation, const std::string& info);
    
    // Iteration helpers
    template<typename RegionFunc>
    static void forEachRegion(const std::vector<std::unique_ptr<NeuralRegion>>& regions, RegionFunc func);
    
    template<typename PopulationFunc>
    static void forEachPopulation(const std::vector<NeuralPopulation*>& populations, PopulationFunc func);
    
    template<typename NeuronFunc>
    static void forEachNeuron(const std::vector<Neuron*>& neurons, NeuronFunc func);
    
    // Statistics helpers
    static size_t countTotalNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    static size_t countTotalSynapses(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    static size_t countActiveNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    static size_t countFiringNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Validation helpers
    static bool validateNeuronRange(const std::vector<Neuron*>& neurons) { return !neurons.empty(); }
    static bool validateSynapseCount(size_t count) { return count > 0; }
    static bool validateRegionCount(size_t count) { return count > 0; }
};

// Exception throwing helpers
void throwIfNot(bool condition, const std::string& operation, const std::string& message);
void throwIfNullptr(void* ptr, const std::string& operation, const std::string& message);
void throwIfError(bool error, const std::string& operation, const std::string& message);

} // namespace nlm

