#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// ==============================================================================
// Neuron.hpp - Leaky Integrate-and-Fire Neuron Implementation
// 
// Comprehensive single neuron implementation for the NLM brain simulation framework.
// Implements real spiking neural dynamics with LIF (Leaky Integrate-and-Fire) model,
// synaptic integration, plasticity mechanisms, and adaptive properties.
// 
// Author: NLM Development Team
// Date: Phase 2 Implementation
// Version: 1.0
// 
// This module implements:
// - LIF neuron dynamics with membrane potential integration
// - Synaptic signal reception and processing
// - Spike generation and refractory period management
// - Synaptic plasticity (STDP, Hebbian, reward-modulated)
// - Developmental parameter adaptation
// - Memory and learning capabilities
// 
// Dependencies: ../core/Types/Types.hpp
// External Dependencies: Random number generation, system timing
// 
// Key Features:
// - Real LIF neuron dynamics with biological plausibility
// - Event-driven spike propagation
// - Multiple plasticity rules integration
// - Memory management with pImpl pattern
// - Thread-safe operations with mutex protection
// - Comprehensive error handling and recovery
// - Developmental parameter adaptation over time
// - Activity-dependent plasticity regulation
// 
// Usage Examples:
// @code{.cpp}
// // Create neuron with specific ID
// Neuron neuron(NeuronId(42));
// 
// // Initialize neuron with random parameters
// RandomGenerator rng; // Initialize random generator
// neuron.initializeRandom(rng);
// 
// // Process synaptic input
// neuron.receiveExcitatoryInput(15.0f); // mV
// neuron.receiveInhibitoryInput(8.0f);  // mV
// 
// // Step neuron dynamics
// bool fired = neuron.stepLIF(current_time, dt);
// if (fired) {
//     std::cout << "Neuron fired at time " << current_time << std::endl;
// }
// @endcode
// 
// Error Handling:
// - Invalid neuron ID handling
// - Synaptic input validation
// - Membrane potential bounds checking
// - Thread synchronization with timeout
// - Memory safety with pImpl pattern
// 
// Performance Considerations:
// - Efficient state storage with NeuronState struct
// - O(1) synaptic signal processing
// - Event-driven spike detection
// - Pre-allocated memory for spike history
// - Optimized plasticity computation
// 
// TODO:
// - Complete LIF dynamics implementation (Phase 2)
// - Advanced synaptic plasticity rules
// - Neuromodulation integration
// - Detailed neuron subtype implementations
// 
// ==============================================================================

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @struct NeuronState
 * @brief Complete neuron state storage for efficient simulation
 * 
 * Comprehensive neuron state representation optimized for simulation performance:
 * 
 * @field membranePotential Current membrane potential (mV)
 * @field restingPotential Resting membrane potential (mV)
 * @field threshold Firing threshold potential (mV)
 * @field resetPotential Post-spike reset potential (mV)
 * @field leakConductance Membrane leak conductance (nS)
 * @field synapseConductance Total synaptic conductance (nS)
 * @field firingRate Current firing rate (Hz)
 * @field firingState Current firing state (Resting, Active, Refractory, Inhibited)
 * @field refractoryRemaining Steps remaining in refractory period
 * @field refractoryPeriod Total refractory period in simulation steps
 * @field adaptationVariable Activity-dependent adaptation variable
 * @field lastSpikeTime Timestamp of last spike (-1 if none)
 * 
 * Biological Plausibility:
 * - Membrane potentials in millivolts (mV)
 * - Time in simulation steps or milliseconds
 * - Conductances in nanosiemens (nS)
 * - Firing rates in Hertz (Hz)
 * - Spike timestamps in simulation time units
 * 
 * State Invariants:
 * - membranePotential >= resetPotential during normal operation
 * - refractoryRemaining <= refractoryPeriod always true
 * - lastSpikeTime < current_time when spike recorded
 * 
 * State Management:
 * - State updates occur atomically during stepLIF()
 * - State backup for plasticity computations
 * - State restoration for error recovery
 * 
 * @note State is copied by reference for efficient access within Neuron class
 */
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)\n    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
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
    Neuron(Neuron&& other) noexcept;
    Neuron& operator=(Neuron&& other) noexcept;
    
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
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
