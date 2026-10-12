#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Validation/ValidationSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <unordered_map>

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

// Custom exceptions for Brain validation
class BrainValidationError : public ValidationError {
public:
    explicit BrainValidationError(const std::string &message) : ValidationError(message) {}
    virtual const char *category() const noexcept override { return "BrainValidationError"; }
};

class BrainInitializationError : public BrainValidationError {
public:
    explicit BrainInitializationError(const std::string &message) : BrainValidationError(message) {}
    virtual const char *category() const noexcept override { return "BrainInitializationError"; }
};

class BrainParameterError : public BrainValidationError {
public:
    BrainParameterError(const std::string &message, const std::string &parameterName)
        : BrainValidationError(message), parameterName_(parameterName) {}
    const std::string &getParameterName() const { return parameterName_; }
    virtual const char *category() const noexcept override { return "BrainParameterError"; }
private:
    std::string parameterName_;
};

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
  RegionId sourceRegion;
  RegionId targetRegion;
  float weight;
  Delay delay;
  PlasticityFlags plasticityFlags;

  InterRegionConnection()
      : sourceRegion(), targetRegion(), weight(0.0f), delay(1),
        plasticityFlags() {}

  InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
      : sourceRegion(src), targetRegion(tgt), weight(w), delay(d),
        plasticityFlags() {}
};

// Validation macros for Brain
#define NLM_VALIDATE_BRAIN_PARAM(param, condition, message) \
    do { \
        if (!(condition)) { \
            throw nlm::BrainParameterError(message, #param); \
        } \
    } while(0)

#define NLM_VALIDATE_BRAIN_STATE(condition, message) \
    do { \
        if (!(condition)) { \
            throw nlm::BrainValidationError(message); \
        } \
    } while(0)

// Validation and error handling methods
class ValidationContext {
public:
    ValidationContext(const std::string &component) : component_(component) {}
    virtual ~ValidationContext() = default;
    virtual ValidationResult validate() const = 0;
    const std::string &getComponent() const { return component_; }
protected:
    std::string component_;
};

// Brain parameter validation
class BrainParameterValidator : public ValidationContext {
public:
    explicit BrainParameterValidator(const std::shared_ptr<Config> &config) 
        : ValidationContext("BrainParameterValidator"), config_(config) {}

    ValidationResult validate() const override {
        ValidationResult result = ValidationResult::success("BrainParameterValidator");

        // Validate brain-specific parameters
        if (config_->has("brain.max_neurons")) {
            auto maxNeurons = config_->getOr<size_t>("brain.max_neurons", 0);
            if (!NLM_VALIDATE_PARAM("brain.max_neurons", maxNeurons > 0 && maxNeurons <= 1000000, "Maximum neurons must be positive and reasonable")) {
                return ValidationResult::failure("Invalid max_neurons parameter", "BrainParameterValidator", "brain.max_neurons");
            }
        }

        if (config_->has("brain.simulation.dt")) {
            auto dt = config_->getOr<double>("brain.simulation.dt", 0.0);
            if (!NLM_VALIDATE_PARAM("brain.simulation.dt", dt > 0.0 && dt < 1.0, "Simulation timestep must be positive and less than 1.0")) {
                return ValidationResult::failure("Invalid simulation.dt parameter", "BrainParameterValidator", "brain.simulation.dt");
            }
        }

        if (config_->has("brain.simulation.max_steps")) {
            auto maxSteps = config_->getOr<SimulationStep>("brain.simulation.max_steps", 0);
            if (!NLM_VALIDATE_PARAM("brain.simulation.max_steps", maxSteps > 0 && maxSteps <= 100000000, "Maximum simulation steps must be positive and reasonable")) {
                return ValidationResult::failure("Invalid max_steps parameter", "BrainParameterValidator", "brain.simulation.max_steps");
            }
        }

        return result;
    }

    ValidationResult validateMemoryParameters() const {
        ValidationResult result = ValidationResult::success("BrainMemoryValidator");

        if (config_->has("brain.memory.working_memory.capacity")) {
            auto capacity = config_->getOr<size_t>("brain.memory.working_memory.capacity", 0);
            if (!NLM_VALIDATE_PARAM("brain.memory.working_memory.capacity", capacity > 0 && capacity <= 1000000, "Working memory capacity must be positive and reasonable")) {
                return ValidationResult::failure("Invalid working memory capacity parameter", "BrainMemoryValidator", "brain.memory.working_memory.capacity");
            }
        }

        return result;
    }

    ValidationResult validateLearningParameters() const {
        ValidationResult result = ValidationResult::success("BrainLearningValidator");

        if (config_->has("brain.learning.learning_rate")) {
            auto lr = config_->getOr<double>("brain.learning.learning_rate", 0.0);
            if (!NLM_VALIDATE_PARAM("brain.learning.learning_rate", lr >= 0.0 && lr <= 1.0, "Learning rate must be between 0.0 and 1.0")) {
                return ValidationResult::failure("Invalid learning_rate parameter", "BrainLearningValidator", "brain.learning.learning_rate");
            }
        }

        return result;
    }

private:
    std::shared_ptr<Config> config_;
};

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
  RegionId sourceRegion;
  RegionId targetRegion;
  float weight;
  Delay delay;
  PlasticityFlags plasticityFlags;

  InterRegionConnection()
      : sourceRegion(), targetRegion(), weight(0.0f), delay(1),
        plasticityFlags() {}

  InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
      : sourceRegion(src), targetRegion(tgt), weight(w), delay(d),
        plasticityFlags() {}

  // Validate connection
  ValidationResult validate() const {
    ValidationResult result = ValidationResult::success("InterRegionConnection");

    if (!NLM_VALIDATE_BRAIN_PARAM(sourceRegion, sourceRegion.value != UINT64_MAX, "Invalid source region")) {
        return ValidationResult::failure("Invalid source region", "InterRegionConnection");
    }

    if (!NLM_VALIDATE_BRAIN_PARAM(targetRegion, targetRegion.value != UINT64_MAX, "Invalid target region")) {
        return ValidationResult::failure("Invalid target region", "InterRegionConnection");
    }

    if (!NLM_VALIDATE_BRAIN_PARAM(weight, weight >= 0.0f && weight <= 10.0f, "Connection weight must be between 0.0 and 10.0")) {
        return ValidationResult::failure("Invalid connection weight", "InterRegionConnection");
    }

    if (!NLM_VALIDATE_BRAIN_PARAM(delay, delay > 0 && delay <= 1000, "Delay must be positive and reasonable")) {
        return ValidationResult::failure("Invalid connection delay", "InterRegionConnection");
    }

    return result;
  }
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
  // Create brain with configuration
  explicit Brain(std::shared_ptr<Config> config);

  ~Brain();

  // Disable copying, enable moving
  Brain(const Brain &) = delete;
  Brain &operator=(const Brain &) = delete;
  Brain(Brain &&) noexcept;
  Brain &operator=(Brain &&) noexcept;

  // Initialize brain with configuration and validation
  bool initialize();
  
  // Initialize brain with validation (throws on failure)
  void initializeWithValidation();

  // Main simulation step
  void step(SimulationStep currentStep);

  // Simulation time step
  void step(SimulationStep currentStep, Timestamp currentTime);

  // Receive sensory input from environment
  // Injects current into sensory neurons based on input pattern
  void receiveSensoryInput(const class SensoryInput &input);

  // Inject current directly into a specific neuron
  void injectCurrent(NeuronId neuron, MembranePotential current);

  // Inject current into all neurons of a specific type
  void injectCurrentToNeurons(NeuronType type, MembranePotential current);

  // Spike system access
  SpikeSystem *getSpikeSystem();
  const SpikeSystem *getSpikeSystem() const;

  // Plasticity system access
  STDP *getSTDP();
  Hebbian *getHebbian();
  StructuralPlasticity *getStructuralPlasticity();

  // Statistics
  float getExcitationInhibitionRatio() const;
  size_t getTotalSpikeCount() const;
  size_t getPendingSpikeEventCount() const;

  // Produce motor/action output based on motor neuron activity
  std::unique_ptr<class Action> produceAction();

  // Apply neuromodulatory signals
  void applyNeuromodulation(const class Neuromodulator &signal);

  // Update plasticity rules (called automatically in step)
  void updatePlasticity();

  // Apply developmental changes (called automatically in step)
  void develop();

  // Reset brain state
  void reset();

  // Save brain state to file (checkpointing)
  bool save(const std::string &filepath) const;

  // Load brain state from file
  bool load(const std::string &filepath);

  // Region management
  RegionId addRegion(const std::string &name = "");
  NeuralRegion *getRegion(RegionId id);
  const NeuralRegion *getRegion(RegionId id) const;
  size_t getRegionCount() const;
  std::vector<RegionId> getRegionIds() const;

  // Get all regions
  const std::vector<std::unique_ptr<NeuralRegion>> &getRegions() const;

  // Inter-region connection management with validation
  void addInterRegionConnection(RegionId source, RegionId target,
                                float weight = 0.0f, Delay delay = 1);
  void removeInterRegionConnection(RegionId source, RegionId target);

  // Global statistics
  size_t getTotalNeuronCount() const;
  size_t getTotalSynapseCount() const;
  size_t getActiveNeuronCount() const;
  size_t getFiringNeuronCount() const;
  float getAverageFiringRate() const;

  // ========== MEMORY SYSTEMS ==========

  // Working memory - transient active information
  NeuralWorkingMemory *getWorkingMemory();

  // Episodic memory - experience storage
  NeuralEpisodicMemory *getEpisodicMemory();

  // Associative memory - pattern associations
  NeuralAssociativeMemory *getAssociativeMemory();

  // ========== PREDICTION SYSTEM ==========

  // Prediction system for sensory prediction and error computation
  PredictionSystem *getPredictionSystem();

  // ========== COGNITION SYSTEMS ==========

  // Neural planner for action planning
  NeuralPlanner *getPlanner();

  // Concept formation for pattern discovery
  ConceptFormation *getConceptFormation();

  // Attentional selection for focus
  AttentionalSelection *getAttention();

  // ========== DEVELOPMENT SYSTEM ==========

  DevelopmentSystem *getDevelopmentSystem();
  DevelopmentalStage getDevelopmentalStage() const;
  void setDevelopmentalStage(DevelopmentalStage stage);

  // ========== NEUROMODULATION SYSTEMS ==========

  // Dopamine - reward and reinforcement
  Dopamine *getDopamine();

  // Curiosity - exploration motivation
  Curiosity *getCuriosity();

  // Novelty - novelty detection
  Novelty *getNovelty();

  // Prediction error signal
  PredictionError *getPredictionErrorSignal();

  // Get current configuration
  std::shared_ptr<const Config> getConfig() const;

  // Get random generator
  RandomGenerator *getRandomGenerator();

  // Logging
  void logStatus() const;

  // Validation methods
  bool validate() const;
  void validateOrThrow() const;
  std::vector<std::string> getValidationErrors() const;

  // Performance monitoring
  void enablePerformanceMonitoring();
  void disablePerformanceMonitoring();
  bool isPerformanceMonitoringEnabled() const;
  struct PerformanceStats {
    uint64_t validationCount = 0;
    uint64_t validationFailureCount = 0;
    uint64_t stepCount = 0;
    double averageValidationTime = 0.0;
  };
  PerformanceStats getPerformanceStats() const;
  void resetPerformanceStats();

  // Recovery strategies
  bool attemptRecovery();
  void enableAutoRecovery(bool enable);
  bool isAutoRecoveryEnabled() const;

private:
  struct Impl;
  Impl *pImpl;

  // Validation and error handling helpers
  ValidationResult validateInitialization() const;
  ValidationResult validateState() const;
  ValidationResult validateParameters() const;
  void logValidationResult(const ValidationResult &result) const;
  bool attemptRecoveryStrategy(const std::string &failureType);
};;

} // namespace nlm
