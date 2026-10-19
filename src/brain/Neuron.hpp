#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
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
// Real spiking neuron simulation with refractory period and adaptation

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
    
    // ========== IDENTITY AND TYPE ==========
    
    // Get neuron identifier
    // @return NeuronId Unique identifier for this neuron
    NeuronId getId() const;
    
    // Get neuron type (Sensory, Internal, or Motor)
    // @return NeuronType Type of neuron based on function
    NeuronType getType() const;
    
    // Set neuron type
    // @param type NeuronType New type for this neuron
    void setType(NeuronType type);
    
    // ========== STATE ACCESS ==========
    
    // Get complete neuron state (const access)
    // @return const NeuronState& Complete neuron state for read-only access
    const NeuronState& getState() const;
    
    // Get complete neuron state (read-write access)
    // @return NeuronState& Complete neuron state for modification
    NeuronState& getState();
    
    // ========== MEMBRANE POTENTIAL ==========
    
    // Get current membrane potential
    // @return MembranePotential Current membrane potential in mV
    MembranePotential getMembranePotential() const;
    
    // Set membrane potential directly
    // @param potential MembranePotential New membrane potential in mV
    void setMembranePotential(MembranePotential potential);
    
    // Add to membrane potential (e.g., from synaptic input)
    // @param delta MembranePotential Change in potential in mV
    void addToMembranePotential(MembranePotential delta);
    
    // ========== THRESHOLD ==========
    
    // Get firing threshold
    // @return MembranePotential Voltage threshold for spike initiation in mV
    MembranePotential getThreshold() const;
    
    // Set firing threshold
    // @param threshold MembranePotential New threshold in mV
    void setThreshold(MembranePotential threshold);
    
    // ========== FIRING STATE ==========
    
    // Check if neuron is currently firing
    // @return bool true if neuron is in active firing state
    bool isFiring() const;
    
    // Check if neuron is in refractory period
    // @return bool true if neuron cannot fire due to refractory period
    bool isRefractory() const;
    
    // Set firing state directly
    // @param state FiringState New firing state
    void setFiringState(FiringState state);
    
    // Set refractory period duration
    // @param steps uint32_t Number of simulation steps to remain refractory
    void setRefractoryPeriod(uint32_t steps);
    
    // Decrement refractory period counter (called by simulation)
    void decrementRefractory();
    
    // ========== FIRING RATE ==========
    
    // Get current firing rate
    // @return FiringRate Current firing rate in Hz
    FiringRate getFiringRate() const;
    
    // Set firing rate (for rate-based computation)
    // @param rate FiringRate New firing rate in Hz
    void setFiringRate(FiringRate rate);
    
    // ========== LIF NEURON PARAMETERS ==========
    
    // Set leak conductance (controls membrane time constant)
    // @param conductance MembranePotential Leak conductance in nS
    void setLeakConductance(MembranePotential conductance);
    
    // Get leak conductance
    // @return MembranePotential Leak conductance in nS
    MembranePotential getLeakConductance() const;
    
    // Get refractory period duration
    // @return uint32_t Number of simulation steps in refractory period
    uint32_t getRefractoryPeriod() const;
    
    // Set resting membrane potential
    // @param potential MembranePotential Resting potential in mV
    void setRestingPotential(MembranePotential potential);
    
    // Get resting membrane potential
    // @return MembranePotential Resting potential in mV
    MembranePotential getRestingPotential() const;
    
    // Set reset potential (post-spike value)
    // @param potential MembranePotential Reset potential in mV
    void setResetPotential(MembranePotential potential);
    
    // ========== SPIKE DETECTION ==========
    
    // Check if membrane potential exceeds firing threshold
    // @return bool true if neuron is ready to fire
    bool checkThreshold() const;
    
    // Get timestamp of last spike
    // @return float Timestamp of last spike (-1.0f if never spiked)
    float getLastSpikeTime() const;
    
    // ========== LIF STEP FUNCTION ==========
    
    // Execute Leaky Integrate-and-Fire simulation step
    // Updates membrane potential based on LIF differential equation,
    // handles refractory period, spike detection, and reset.
    // 
    // @param currentTime Current simulation time in ms
    // @param dt Timestep duration in ms
    // @return bool true if neuron fired during this step
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // ========== INTERNAL HELPER FUNCTIONS ==========
    
    // Handle refractory period logic
    // Decrements refractory counter and manages state transitions
    // 
    // @return bool true if neuron is still in refractory period
    bool handleRefractoryPeriod();
    
    // Execute LIF integration computation
    // Core neuron dynamics: membrane potential change due to leak
    // and synaptic inputs, spike detection, and adaptation
    // 
    // @param currentTime Current simulation time in ms
    // @param dt Timestep duration in ms
    // @return bool true if neuron fired
    bool integrateLIF(Timestamp currentTime, TimestepDuration dt);
    
    // ========== INCOMING SIGNALS ==========
    
    // Receive excitatory synaptic input
    // @param amplitude MembranePotential Synaptic current in mV (positive)
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    // Receive inhibitory synaptic input
    // @param amplitude MembranePotential Synaptic current in mV (positive magnitude, subtracted)
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    // Receive modulatory input (e.g., dopamine, acetylcholine)
    // @param amplitude MembranePotential Modulatory signal in mV
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // ========== CURRENT INJECTION ==========
    
    // Inject external current (e.g., from sensory input)
    // @param current MembranePotential External current in mV
    void injectCurrent(MembranePotential current);
    
    // Get total current accumulated for this step
    // @return MembranePotential Total current input in mV
    MembranePotential getTotalCurrent() const;
    
    // Clear accumulated current for next step
    void clearTotalCurrent();
    
    // ========== SPIKE HISTORY ==========
    
    // Record spike occurrence
    // @param timestamp Timestamp when spike occurred
    void recordSpike(Timestamp timestamp);
    
    // Get recent spike history (for STDP)
    // @return const std::vector<Timestamp>& Recent spike timestamps
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    // Clear spike history
    void clearSpikeHistory();
    
    // ========== SYNAPSE MANAGEMENT ==========
    
    // Add incoming synapse connection
    // @param handle SynapseHandle Handle to incoming synapse
    void addIncomingSynapse(SynapseHandle handle);
    
    // Add outgoing synapse connection
    // @param handle SynapseHandle Handle to outgoing synapse
    void addOutgoingSynapse(SynapseHandle handle);
    
    // Get list of incoming synapses
    // @return const std::vector<SynapseHandle>& Handles of incoming connections
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    // Get list of outgoing synapses
    // @return const std::vector<SynapseHandle>& Handles of outgoing connections
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // ========== PLASTICITY STATE ==========
    
    // Get plasticity flags (const access)
    // @return const PlasticityFlags& Flags controlling plasticity mechanisms
    const PlasticityFlags& getPlasticityFlags() const;
    
    // Get plasticity flags (read-write access)
    // @return PlasticityFlags& Flags controlling plasticity mechanisms
    PlasticityFlags& getPlasticityFlags();
    
    // Configure plasticity mechanisms
    // @param hebbian Enable Hebbian learning
    // @param stdp Enable Spike-Timing-Dependent Plasticity
    // @param rewardModulated Enable reward-modulated plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // ========== REGION/POPULATION MEMBERSHIP ==========
    
    // Set neural region membership
    // @param region RegionId Identifier of containing region
    void setRegionId(RegionId region);
    
    // Get neural region membership
    // @return RegionId Identifier of containing region
    RegionId getRegionId() const;
    
    // Set population membership
    // @param population PopulationId Identifier of containing population
    void setPopulationId(PopulationId population);
    
    // Get population membership
    // @return PopulationId Identifier of containing population
    PopulationId getPopulationId() const;
    
    // ========== MAIN UPDATE FUNCTION ==========
    
    // Update neuron for one simulation step
    // Default implementation delegates to stepLIF with standard timestep
    // Can be overridden for custom simulation dynamics
    // 
    // @param currentTime Current simulation time in ms
    void step(Timestamp currentTime);
    
    // ========== STATE MANAGEMENT ==========
    
    // Reset neuron to initial state
    // Restores default parameters and clears all activity
    void reset();
    
    // Initialize with random biological parameters
    // Sets membrane potentials, thresholds, and timing parameters
    // based on biologically plausible ranges
    // 
    // @param rng RandomGenerator Random number generator for initialization
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
