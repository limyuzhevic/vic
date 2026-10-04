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
// Implements Leaky Integrate-and-Fire (LIF) dynamics for real spiking neural computation
// 
// The Neuron class is a fundamental building block of the NLM brain architecture.
// Each neuron represents a simplified biological neuron model that can:
// - Integrate synaptic inputs over time
// - Generate action potentials (spikes) when membrane potential exceeds threshold
// - Participate in learning through synaptic plasticity rules
// - Maintain internal state across simulation steps
// 
// Biological inspiration:
// - Leaky Integrate-and-Fire (LIF) model: dV/dt = (V_rest - V)/tau + I/C
// - Membrane potential dynamics with exponential decay toward resting potential
// - Spike-frequency adaptation for realistic firing patterns
// - Refractory period after spiking to prevent immediate re-firing
// 
// Physical parameters:
// - Membrane capacitance: 1.0 nF (typical mammalian neuron)
// - Time constant: 20.0 ms (determines integration time)
// - Maximum membrane potential: 50.0 mV (prevents numerical instability)
// - Minimum membrane potential: -100.0 mV (hyperpolarization limit)
// 
// State variables:
// - Membrane potential (mV): V(t), current electrical potential
// - Resting potential (mV): V_rest, equilibrium potential
// - Threshold potential (mV): V_th, spike initiation level
// - Reset potential (mV): V_reset, post-spike value
// - Leak conductance (nS): g_L, determines decay rate
// - Refractory period (steps): prevents immediate re-firing
// - Adaptation variable: for spike-frequency adaptation
// 
// Simulation parameters:
// - Time step (dt): duration of each simulation step (typically 0.001-1.0 ms)
// - Current timestep: absolute simulation time (ms or seconds depending on scale)
// 
// The neuron operates according to the LIF equation:
// V(t+dt) = V(t) + dt * ( (V_rest - V(t))/tau + I(t)/C )
// 
// Where:
// - V(t) is membrane potential at time t
// - I(t) is total synaptic current input
// - tau = C/g_L is the membrane time constant
// - C = 1.0 nF is membrane capacitance
// 
// Neuron types:
// - Sensory: Receives external input, encodes sensory information
// - Internal: Processes internal representations, maintains working memory
// - Motor: Generates output actions for environmental interaction
// 
// Synaptic integration:
// - Excitatory inputs: positive current (depolarizes neuron)
// - Inhibitory inputs: negative current (hyperpolarizes neuron)
// - Modulatory inputs: affect plasticity but not immediate integration
// - Current injection: direct external input (e.g., from sensory receptors)
// 
// Plasticity support:
// - STDP (Spike-Timing-Dependent Plasticity): strengthens synapses when
//   pre-synaptic spikes precede post-synaptic spikes (LTP)
// - Hebbian learning: strengthens co-activated synapses
// - Reward-modulated plasticity: scales learning by neuromodulators
// 
// Usage in NLM brain:
// 1. Create neurons: Neuron neuron(NeuronId(1));
// 2. Set neuron type: neuron.setType(NeuronType::Sensory);
// 3. Initialize parameters: neuron.initializeRandom(rng);
// 4. Update each step: neuron.stepLIF(currentTime, dt);
// 5. Check for firing: if (neuron.checkThreshold()) { /* spike occurred */ }
// 6. Manage synaptic connections: neuron.addIncomingSynapse(handle);
// 7. Apply plasticity rules: neuron.enablePlasticity(true, true, false);
// 
// Performance considerations:
// - stepLIF method is computationally intensive (called for every neuron every step)
// - Implemented with numerical stability guards (clamping, NaN checks)
// - Supports vectorization through SIMD (in optimized builds)
// - Event-driven processing in SpikeSystem reduces unnecessary computations
// 
// Scientific foundations:
// - Brain models that bridge synaptic and systems neuroscience
// - Neural dynamics that emerge from biophysical principles
// - Compromises between biological realism and computational tractability
// - Based on experimental data from cortical and hippocampal neurons
// 
// See related documentation:
// - src/brain/Brain.hpp for brain-level integration
// - src/plasticity/STDP.hpp for plasticity rules
// - docs/ARCHITECTURE.md for neural system design
// - docs/SCIENCE.md for biophysical foundations
// 
// Note on limitations:
// - Simplified neuron model ignores detailed ion channel dynamics
// - No dendritic computation or compartmental modeling
// - Synaptic inputs treated as point inputs to soma
// - Does not claim to reproduce detailed biological neuron behavior
// - Suitable for computational neuroscience research and education
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
