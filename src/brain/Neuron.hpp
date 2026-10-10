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

namespace nlm {

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
//
// Neuron Architecture:
// ===================
// Individual neurons in NLM follow biologically-inspired integrate-and-fire models
// with adaptations for computational efficiency and scalability. Each neuron maintains
// its own state and responds to inputs from synaptic connections.
//
// Core Computational Model:
// 1. **Membrane Potential Integration**: Inputs accumulate according to LIF dynamics
// 2. **Threshold Detection**: When membrane potential reaches threshold, neuron fires
// 3. **Refractory Period**: Neuron enters recovery period after firing
// 4. **Synaptic Transmission**: Neuron sends signals to post-synaptic neurons
// 5. **Plasticity**: Synaptic weights change based on spiking history
//
// Biological Plausibility:
// ----------------------
// - **Membrane Potential**: Continuous variable that integrates inputs
// - **Spike Generation**: Binary output (firing or not)
// - **Refractory Period**: Recovery from spike generation
// - **Synaptic Integration**: Summation of post-synaptic potentials
// - **Adaptation**: Spike-frequency adaptation for realistic firing patterns
//
// Neuron State Variables:
// ---------------------+
// - **membranePotential**: Current membrane potential (mV) (~ -70mV resting)
// - **restingPotential**: Baseline membrane potential (-70mV typical)
// - **threshold**: Firing threshold (-55mV typical)
// - **resetPotential**: Post-spike reset value (typically close to resting)
// - **leakConductance**: Membrane leak conductance (determines decay rate)
// - **synapseConductance**: Current synaptic input (accumulated)
// - **firingRate**: Current firing rate (Hz)
// - **firingState**: Discrete firing state (Resting, Active, Refractory)
// - **refractoryRemaining**: Steps remaining in refractory period
// - **refractoryPeriod**: Total refractory period duration (2-10ms typical)
// - **adaptationVariable**: Spike-frequency adaptation variable
// - **lastSpikeTime**: Timestamp of last spike (for STDP calculations)
//
// Input Processing:
// ---------------
// Neurons receive three types of inputs:
//
// 1. **Excitatory Input**: Depolarizes neuron (makes firing more likely)
//    - Represents AMPA receptor-mediated currents
//    - Inhibits threshold crossing when negative
//
// 2. **Inhibitory Input**: Hyperpolarizes neuron (makes firing less likely)
//    - Represents GABA receptor-mediated currents
//    - Can override excitatory inputs
//
// 3. **Modulatory Input**: Affects plasticity and adaptation
//    - Represents neuromodulators (dopamine, acetylcholine, etc.)
//    - Does not directly cause firing but affects learning
//
// LIF Dynamics Implementation:
// ---------------------------
// The neuron implements the Leaky Integrate-and-Fire model:
//
// dV/dt = (V_rest - V)/τ + I/C
//
// Where:
// - V: Membrane potential
// - V_rest: Resting potential
// - τ: Membrane time constant (20ms typical)
// - I: Total input current
// - C: Membrane capacitance (1nF typical)
//
// Discrete approximation:
// V(t+1) = V(t) + dt * ( (V_rest - V(t))/τ + I(t)/C )
//
// Where dt is the simulation timestep (typically 1ms).
//
// Spike Generation:
// ----------------
// When membrane potential reaches threshold, the neuron:
//
// 1. **Fires**: Generates a spike (output event)
// 2. **Resets**: Membrane potential is set to reset potential
// 3. **Refractors**: Enters refractory period
// 4. **Adapts**: Increases adaptation variable
// 5. **Records**: Timestamp for STDP calculations
//
// Neuron Types:
// -------------
// - **Sensory**: Respond to external inputs
// - **Interneuron**: Process and relay information between neurons
// - **Motor**: Generate output signals for action selection
// - **Modulatory**: Regulate plasticity and learning
// - **Internally-Generated**: Produce autonomous activity (oscillators)
//
// Synaptic Connectivity:
// --------------------
// Each neuron maintains lists of:
//
// - **Incoming Synapses**: Postsynaptic connections (receive signals)
// - **Outgoing Synapses**: Presynaptic connections (send signals)
// - **Plasticity Flags**: Whether synapse is plastic (Hebbian, STDP, reward-modulated)
//
// Neuron Algorithms:
// -----------------
// Key computational methods:
//
// 1. **stepLIF()**: Main LIF integration and spike detection
// 2. **recordSpike()**: Maintain spike history for STDP
// 3. **initializeRandom()**: Biological initialization with variability
// 4. **reset()**: Return to initial state
// 5. **plasticityUpdate()**: Integrate with plasticity mechanisms
//
// Performance Optimizations:
// ---------------------------
// - **SoA (Structure of Arrays)**: Neuron states stored contiguously
// - **Pre-allocated State**: No dynamic allocation during simulation
// - **Vectorized Operations**: SIMD-friendly for performance-critical paths
// - **Event-Driven Processing**: Only update active neurons
// - **Cache-Friendly**: Memory layout optimized for CPU cache
//
// Neuron Pooling:
// --------------
// Neurons are organized into pools for efficiency:
//
// - **Population**: Neurons with shared properties (type, parameters)
// - **Region**: Logical grouping of neurons (sensory, cognitive, motor)
// - **Cluster**: Spatial or functional grouping within regions
//
// Neuron Initialization:
// ---------------------+
// Biological initialization ensures realistic behavior:
//
// - Membrane potential starts near resting with small noise
// - Threshold typically -55mV (±2mV)
// - Refractory period 2-10ms (typically 5ms)
// - Leak conductance 10nS (sets time constant)
// - Adaptation variable starts at 0 (increases after each spike)
// - Type assigned based on regional function
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
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
