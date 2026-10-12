#pragma once

#include "../core/Types/Types.hpp"
#include <array>
#include <vector>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Forward declarations
class Synapse;
class RandomGenerator;

// Custom exceptions for Neuron validation
class NeuronValidationError : public ValidationError {
public:
    explicit NeuronValidationError(const std::string &message) : ValidationError(message) {}
    virtual const char *category() const noexcept override { return "NeuronValidationError"; }
};

class NeuronStateError : public NeuronValidationError {
public:
    explicit NeuronStateError(const std::string &message) : NeuronValidationError(message) {}
    virtual const char *category() const noexcept override { return "NeuronStateError"; }
};

class NeuronParameterError : public NeuronValidationError {
public:
    NeuronParameterError(const std::string &message, const std::string &parameterName)
        : NeuronValidationError(message), parameterName_(parameterName) {}
    const std::string &getParameterName() const { return parameterName_; }
    virtual const char *category() const noexcept override { return "NeuronParameterError"; }
private:
    std::string parameterName_;
};

// Validation macros for Neuron
#define NLM_VALIDATE_NEURON_PARAM(param, condition, message) \
    do { \
        if (!(condition)) { \
            throw nlm::NeuronParameterError(message, #param); \
        } \
    } while(0)

#define NLM_VALIDATE_NEURON_STATE(condition, message) \
    do { \
        if (!(condition)) { \
            throw nlm::NeuronStateError(message); \
        } \
    } while(0)

// Neuron state structure for efficient storage
struct NeuronState {
  MembranePotential membranePotential;  // Current membrane potential (mV)
  MembranePotential restingPotential;   // Resting potential (mV)
  MembranePotential threshold;          // Firing threshold (mV)
  MembranePotential resetPotential;     // Post-spike reset value (mV)
  MembranePotential leakConductance;    // Leak conductance (nS)
  MembranePotential synapseConductance; // Synaptic conductance (nS)
  FiringRate firingRate;                // Current firing rate (Hz)
  FiringState firingState;              // Current firing state
  uint32_t refractoryRemaining;         // Steps remaining in refractory period
  uint32_t refractoryPeriod;            // Total refractory period (steps)
  float adaptationVariable;             // For spike-frequency adaptation
  float lastSpikeTime;                  // Timestamp of last spike (-1 if none)

  NeuronState()
      : membranePotential(-70.0f), restingPotential(-70.0f), threshold(-55.0f),
        resetPotential(-70.0f), leakConductance(10.0f),
        synapseConductance(0.0f), firingRate(0.0f),
        firingState(FiringState::Resting), refractoryRemaining(0),
        refractoryPeriod(5), adaptationVariable(0.0f), lastSpikeTime(-1.0f) {}

  // Validate neuron state
  ValidationResult validate() const {
    ValidationResult result = ValidationResult::success("NeuronState");

    // Validate membrane potential range
    if (!NLM_VALIDATE_NEURON_PARAM(membranePotential, 
                                   membranePotential >= -100.0f && membranePotential <= 50.0f,
                                   "Membrane potential out of valid range (-100mV to 50mV)")) {
      return ValidationResult::failure("Invalid membrane potential state", "NeuronState");
    }

    // Validate resting potential
    if (!NLM_VALIDATE_NEURON_PARAM(restingPotential, 
                                   restingPotential >= -100.0f && restingPotential <= 50.0f,
                                   "Resting potential out of valid range (-100mV to 50mV)")) {
      return ValidationResult::failure("Invalid resting potential state", "NeuronState");
    }

    // Validate threshold
    if (!NLM_VALIDATE_NEURON_PARAM(threshold, 
                                   threshold >= -100.0f && threshold <= 50.0f,
                                   "Threshold out of valid range (-100mV to 50mV)")) {
      return ValidationResult::failure("Invalid threshold state", "NeuronState");
    }

    // Validate reset potential
    if (!NLM_VALIDATE_NEURON_PARAM(resetPotential, 
                                   resetPotential >= -100.0f && resetPotential <= 50.0f,
                                   "Reset potential out of valid range (-100mV to 50mV)")) {
      return ValidationResult::failure("Invalid reset potential state", "NeuronState");
    }

    // Validate leak conductance
    if (!NLM_VALIDATE_NEURON_PARAM(leakConductance, 
                                   leakConductance > 0.0f && leakConductance <= 1000.0f,
                                   "Leak conductance must be positive and reasonable (0-1000 nS)")) {
      return ValidationResult::failure("Invalid leak conductance state", "NeuronState");
    }

    // Validate firing rate
    if (!NLM_VALIDATE_NEURON_PARAM(firingRate, 
                                   firingRate >= 0.0f && firingRate <= 1000.0f,
                                   "Firing rate must be positive and reasonable (0-1000 Hz)")) {
      return ValidationResult::failure("Invalid firing rate state", "NeuronState");
    }

    // Validate refractory period
    if (!NLM_VALIDATE_NEURON_PARAM(refractoryPeriod, 
                                   refractoryPeriod > 0 && refractoryPeriod <= 10000,
                                   "Refractory period must be positive and reasonable (1-10000 steps)")) {
      return ValidationResult::failure("Invalid refractory period state", "NeuronState");
    }

    // Validate refractory remaining
    if (!NLM_VALIDATE_NEURON_PARAM(refractoryRemaining, 
                                   refractoryRemaining <= refractoryPeriod,
                                   "Refractory remaining cannot exceed total refractory period")) {
      return ValidationResult::failure("Invalid refractory remaining state", "NeuronState");
    }

    // Validate adaptation variable
    if (!NLM_VALIDATE_NEURON_PARAM(adaptationVariable, 
                                   adaptationVariable >= -10.0f && adaptationVariable <= 10.0f,
                                   "Adaptation variable must be reasonable (-10 to 10)")) {
      return ValidationResult::failure("Invalid adaptation variable state", "NeuronState");
    }

    // Validate last spike time
    if (!NLM_VALIDATE_NEURON_PARAM(lastSpikeTime, 
                                   lastSpikeTime >= -1.0f && lastSpikeTime <= 1e9f,
                                   "Last spike time must be valid or -1 (no spike)")) {
      return ValidationResult::failure("Invalid last spike time state", "NeuronState");
    }

    return result;
  }

  // Check if state is consistent
  ValidationResult validateConsistency() const {
    ValidationResult result = ValidationResult::success("NeuronStateConsistency");

    // Membrane potential should not be above threshold when not firing
    if (firingState != FiringState::Active && 
        membranePotential > threshold) {
      return ValidationResult::failure("Membrane potential above threshold when not firing", "NeuronStateConsistency");
    }

    // Neuron should be in refractory period when refractory remaining > 0
    if (refractoryRemaining > 0 && firingState != FiringState::Refractory) {
      return ValidationResult::failure("Refractory remaining > 0 but not in refractory state", "NeuronStateConsistency");
    }

    // Neuron should not be in refractory state when refractory remaining is 0
    if (refractoryRemaining == 0 && firingState == FiringState::Refractory) {
      return ValidationResult::failure("Refractory remaining is 0 but still in refractory state", "NeuronStateConsistency");
    }

    return result;
  }
};

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
class Neuron {
public:
  // Create neuron with ID
  explicit Neuron(NeuronId id);

  ~Neuron();

  // Disable copying, enable moving
  Neuron(const Neuron &) = delete;
  Neuron &operator=(const Neuron &) = delete;
  Neuron(Neuron &&) noexcept;
  Neuron &operator=(Neuron &&) noexcept;

  // Identity
  NeuronId getId() const;

  // Type
  NeuronType getType() const;
  void setType(NeuronType type);

  // State access
  const NeuronState &getState() const;
  NeuronState &getState();

  // Membrane potential
  MembranePotential getMembranePotential() const;
  void setMembranePotential(MembranePotential potential);
  void addToMembranePotential(MembranePotential delta);

  // Threshold
  MembranePotential getThreshold() const;
  void setThreshold(MembranePotential threshold);

  // Firing state
  bool isFiring() const;
  bool isRefractory() const;
  void setFiringState(FiringState state);
  void setRefractoryPeriod(uint32_t steps);
  void decrementRefractory();

  // Firing rate (for rate-based computation)
  FiringRate getFiringRate() const;
  void setFiringRate(FiringRate rate);

  // LIF neuron parameters
  void setLeakConductance(MembranePotential conductance);
  MembranePotential getLeakConductance() const;
  uint32_t getRefractoryPeriod() const;
  void setRestingPotential(MembranePotential potential);
  MembranePotential getRestingPotential() const;
  void setResetPotential(MembranePotential potential);

  // Spike detection
  bool checkThreshold() const;
  float getLastSpikeTime() const;

  // LIF step function - returns true if neuron fired
  bool stepLIF(Timestamp currentTime, TimestepDuration dt);

  // Incoming signals (post-synaptic potentials)
  void receiveExcitatoryInput(MembranePotential amplitude);
  void receiveInhibitoryInput(MembranePotential amplitude);
  void receiveModulatoryInput(MembranePotential amplitude);

  // Current injection (from external sources)
  void injectCurrent(MembranePotential current);
  MembranePotential getTotalCurrent() const;
  void clearTotalCurrent();

  // Spike history (recent spikes for STDP)
  void recordSpike(Timestamp timestamp);
  const std::vector<Timestamp> &getSpikeHistory() const;
  void clearSpikeHistory();

  // Synapse management (incoming and outgoing)
  void addIncomingSynapse(SynapseHandle handle);
  void addOutgoingSynapse(SynapseHandle handle);
  const std::vector<SynapseHandle> &getIncomingSynapses() const;
  const std::vector<SynapseHandle> &getOutgoingSynapses() const;

  // Plasticity state
  const PlasticityFlags &getPlasticityFlags() const;
  PlasticityFlags &getPlasticityFlags();
  void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);

  // Region/population membership
  void setRegionId(RegionId region);
  RegionId getRegionId() const;
  void setPopulationId(PopulationId population);
  PopulationId getPopulationId() const;

  // Update neuron for one simulation step
  // TODO PHASE 2: Implement real integrate-and-fire dynamics
  void step(Timestamp currentTime);

  // Reset to initial state
  void reset();

  // Initialize with random parameters
  void initializeRandom(RandomGenerator &rng);

  // Validation methods
  ValidationResult validate() const;
  ValidationResult validateState() const;
  ValidationResult validateParameters() const;
  void validateOrThrow() const;

  // Recovery methods
  bool attemptRecovery();
  void setAutoRecovery(bool enable);
  bool isAutoRecoveryEnabled() const;

  // Performance monitoring
  void enablePerformanceMonitoring();
  void disablePerformanceMonitoring();
  bool isPerformanceMonitoringEnabled() const;
  struct NeuronStats {
    uint64_t stepCount = 0;
    uint64_t spikeCount = 0;
    uint64_t validationCount = 0;
    uint64_t validationFailureCount = 0;
    double averageValidationTime = 0.0;
    double averageStepTime = 0.0;
  } stats_;

private:
  struct Impl;
  Impl *pImpl;

  // Validation helpers
  ValidationResult validateMembranePotential() const;
  ValidationResult validateFiringState() const;
  ValidationResult validateRefractoryState() const;
  ValidationResult validateSynapticState() const;
  ValidationResult validatePlasticityState() const;
};

} // namespace nlm
