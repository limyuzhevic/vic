#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Forward declaration
class Neuron;

// Custom exceptions for Synapse validation
class SynapseValidationError : public ValidationError {
public:
    explicit SynapseValidationError(const std::string &message) : ValidationError(message) {}
    virtual const char *category() const noexcept override { return "SynapseValidationError"; }
};

class SynapseConnectionError : public SynapseValidationError {
public:
    explicit SynapseConnectionError(const std::string &message) : SynapseValidationError(message) {}
    virtual const char *category() const noexcept override { return "SynapseConnectionError"; }
};

class SynapseParameterError : public SynapseValidationError {
public:
    SynapseParameterError(const std::string &message, const std::string &parameterName)
        : SynapseValidationError(message), parameterName_(parameterName) {}
    const std::string &getParameterName() const { return parameterName_; }
    virtual const char *category() const noexcept override { return "SynapseParameterError"; }
private:
    std::string parameterName_;
};

// Validation macros for Synapse
#define NLM_VALIDATE_SYNAPSE_PARAM(param, condition, message) \
    do { \
        if (!(condition)) { \
            throw nlm::SynapseParameterError(message, #param); \
        } \
    } while(0)

#define NLM_VALIDATE_SYNAPSE_STATE(condition, message) \
    do { \
        if (!(condition)) { \
            throw nlm::SynapseValidationError(message); \
        } \
    } while(0)

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
class Synapse {
public:
  // Create synapse with IDs
  Synapse(SynapseId id, NeuronId source, NeuronId destination);

  ~Synapse();

  // Disable copying, enable moving
  Synapse(const Synapse &) = delete;
  Synapse &operator=(const Synapse &) = delete;
  Synapse(Synapse &&) noexcept;
  Synapse &operator=(Synapse &&) noexcept;

  // Identity
  SynapseId getId() const;

  // Connection
  NeuronId getSourceNeuron() const;
  NeuronId getDestinationNeuron() const;

  // Connection validation
  ValidationResult validateConnection() const;
  void validateConnectionOrThrow() const;

  // Weight
  SynapticWeight getWeight() const;
  void setWeight(SynapticWeight weight);
  void addToWeight(SynapticWeight delta);

  // Delay (in simulation steps)
  Delay getDelay() const;
  void setDelay(Delay delay);

  // Type
  SynapseType getType() const;
  void setType(SynapseType type);

  // Excitatory/Inhibitory
  bool isExcitatory() const;
  bool isInhibitory() const;

  // Activity history for plasticity
  void recordPreSpike(Timestamp timestamp);
  void recordPostSpike(Timestamp timestamp);
  const std::vector<Timestamp> &getPreSpikeHistory() const;
  const std::vector<Timestamp> &getPostSpikeHistory() const;
  void clearHistory();

  // Plasticity flags
  const PlasticityFlags &getPlasticityFlags() const;
  PlasticityFlags &getPlasticityFlags();

  // Enable/disable plasticity rules
  void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);

  // Eligibility trace (for reward-modulated learning)
  float getEligibilityTrace() const;
  void setEligibilityTrace(float trace);
  void decayEligibilityTrace(float decayRate);

  // Synaptic efficacy (use-dependent modulation)
  float getEfficacy() const;
  void setEfficacy(float efficacy);

  // Update synapse for one simulation step
  // TODO PHASE 2: Implement real synaptic dynamics
  void step(Timestamp currentTime);

  // Reset to initial state
  void reset();

  // Initialize with random parameters
  void initializeRandom(class RandomGenerator &rng);

  // Validation methods
  ValidationResult validate() const;
  ValidationResult validateState() const;
  ValidationResult validateParameters() const;
  ValidationResult validatePlasticityState() const;
  void validateOrThrow() const;

  // Recovery methods
  bool attemptRecovery();
  void setAutoRecovery(bool enable);
  bool isAutoRecoveryEnabled() const;

  // Performance monitoring
  void enablePerformanceMonitoring();
  void disablePerformanceMonitoring();
  bool isPerformanceMonitoringEnabled() const;
  struct SynapseStats {
    uint64_t stepCount = 0;
    uint64_t preSpikeCount = 0;
    uint64_t postSpikeCount = 0;
    uint64_t validationCount = 0;
    uint64_t validationFailureCount = 0;
    double averageValidationTime = 0.0;
    double averageStepTime = 0.0;
  } stats_;

private:
  struct Impl;
  Impl *pImpl;

  // Validation helpers
  ValidationResult validateConnectionIds() const;
  ValidationResult validateWeight() const;
  ValidationResult validateDelay() const;
  ValidationResult validateType() const;
  ValidationResult validateSpikeHistory() const;
};

// Validation functions for synapse connections
ValidationResult validateSynapseConnection(const Synapse &synapse,
                                          const std::unordered_map<NeuronId, bool> &validNeurons);

ValidationResult validateSynapseNetwork(const std::vector<Synapse> &synapses);

} // namespace nlm
