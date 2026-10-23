#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <stdexcept>
#include <string>
#include <cmath>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * Configuration constants for LIF neuron parameters
 * 
 * These define the biological constraints and operating ranges
 * for the Leaky Integrate-and-Fire neuron model.
 */
namespace NeuronConstants {
    // Membrane parameters (biological values in mV/ms/nS)
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;     // nF - typical neuronal capacitance
    static constexpr float TIME_CONSTANT = 20.0f;          // ms - membrane time constant
    static constexpr float RESTING_POTENTIAL = -70.0f;      // mV - typical resting potential
    static constexpr float THRESHOLD_DEFAULT = -55.0f;       // mV - typical firing threshold
    static constexpr float RESET_POTENTIAL = -70.0f;        // mV - post-spike reset potential
    static constexpr float LEAK_CONDUCTANCE_DEFAULT = 10.0f; // nS - default leak conductance
    
    // Physical bounds (biological constraints)
    static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;  // mV - absolute refractory limit
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;    // mV - maximum depolarization
    static constexpr float MIN_THRESHOLD = -60.0f;             // mV - minimum reasonable threshold
    static constexpr float MAX_THRESHOLD = -40.0f;             // mV - maximum reasonable threshold
    static constexpr float MIN_RESTING_POTENTIAL = -90.0f;     // mV - minimum resting potential
    static constexpr float MAX_RESTING_POTENTIAL = -50.0f;     // mV - maximum resting potential
    
    // Adaptation and plasticity parameters
    static constexpr float MAX_ADAPTATION_VARIABLE = 10.0f;   // Maximum spike-frequency adaptation
    static constexpr float ADAPTATION_DECAY_RATE = 0.95f;     // Decay rate for adaptation
    static constexpr size_t MAX_SPIKE_HISTORY = 100;          // Maximum spikes to record
    static constexpr float DEFAULT_REFRACTORY_PERIOD = 5;     // Default refractory period (ms)
}

/**
 * Neuron validation exceptions
 */
class NeuronValidationException : public std::runtime_error {
public:
    NeuronValidationException(const std::string& message) : std::runtime_error(message) {}
};

class NeuronStateException : public std::runtime_error {
public:
    NeuronStateException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * Neuron state structure for efficient storage
 * 
 * Contains all dynamic state variables for a single LIF neuron. Organized
 * for cache efficiency and memory locality.
 */
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation (mV)
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    NeuronState()
        : membranePotential(NeuronConstants::RESTING_POTENTIAL)
        , restingPotential(NeuronConstants::RESTING_POTENTIAL)
        , threshold(NeuronConstants::THRESHOLD_DEFAULT)
        , resetPotential(NeuronConstants::RESET_POTENTIAL)
        , leakConductance(NeuronConstants::LEAK_CONDUCTANCE_DEFAULT)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(NeuronConstants::DEFAULT_REFRACTORY_PERIOD)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
class Neuron {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    NeuronId getId() const;
    
    // Type
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access
    const NeuronState& getState() const;
    NeuronState& getState();
    
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
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
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
    void initializeRandom(RandomGenerator& rng);
    
/**
 * Validate neuron state
 * 
 * Performs comprehensive validation of neuron state and parameters.
 * 
 * @throws NeuronValidationException if state is invalid
 */
    void validate() const;
    
    /**
     * Get human-readable state summary
     * 
     * @return String containing neuron state information
     */
    std::string getStateSummary() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Internal validation helpers
    static bool validateMembranePotential(MembranePotential potential);
    static bool validateThreshold(MembranePotential threshold);
    static bool validateRefractoryPeriod(uint32_t steps);
    static bool validateInputAmplitude(MembranePotential amplitude);
    static bool validateTimestamp(Timestamp timestamp);
    static std::string getValidationErrorMessage(const std::string& field, 
                                                  const std::string& value,
                                                  const std::string& constraint);
};

} // namespace nlm
