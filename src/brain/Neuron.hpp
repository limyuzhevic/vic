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
// Core computational unit of the neural system
// Enables realistic neural computation with biologically plausible dynamics
// 
// LIF Dynamics Overview:
// -------------------
// 1. Integration: Membrane potential integrates incoming currents
// 2. Threshold Detection: Spike when potential reaches threshold
// 3. Reset: Membrane potential resets after spike
// 4. Refractory Period: Temporary unresponsiveness after spike
// 5. Recovery: Gradual return to resting potential

// LIF Mathematical Formulation:
// ---------------------------
// τ_m × dV/dt = -(V - V_rest) + R × I_total
// Where:
// - V: membrane potential
// - τ_m: membrane time constant
// - V_rest: resting potential
// - R: membrane resistance
// - I_total: total input current
// - dV/dt: rate of change of membrane potential

class Neuron {
public:
    // Basic neuron creation
    // Biological inspiration: Individual neuron characteristics
    // Computational rationale: Neuronal identity and properties
    // Performance: O(1) initialization with predictable parameters
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Memory management
    // Biological inspiration: Neuronal integrity and isolation
    // Computational rationale: Prevent unintended neuron mixing
    // Performance: Move semantics for efficient neuron transfer
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity management
    // Biological inspiration: Neuronal identification and labeling
    // Computational rationale: Neuron-based addressing and routing
    // Performance: O(1) identity access with cached values
    NeuronId getId() const;
    
    // Functional classification
    // Biological inspiration: Neuronal functional specialization
    // Computational rationale: Cell-type-based processing
    // Performance: O(1) type access with cached values
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State management
    // Biological inspiration: Neuronal state dynamics and plasticity
    // Computational rationale: State-based processing and adaptation
    // Performance: O(1) state access with cached values
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane potential management
    // Biological inspiration: Ionic flux and membrane potential dynamics
    // Computational rationale: Direct control of neuronal integration
    // Performance: O(1) potential access and updates
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold management
    // Biological inspiration: Voltage-gated ion channel activation
    // Computational rationale: Spike initiation control
    // Performance: O(1) threshold access and updates
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state management
    // Biological inspiration: Neuronal firing state and refractory periods
    // Computational rationale: Spike generation control
    // Performance: O(1) state updates and checks
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate management
    // Biological inspiration: Neuronal firing frequency control
    // Computational rationale: Rate-based neural computation
    // Performance: O(1) rate access and updates
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    // Biological inspiration: Neuronal biophysical properties
    // Computational rationale: LIF model parameter control
    // Performance: O(1) parameter access and updates
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection and management
    // Biological inspiration: Spike generation and timing
    // Computational rationale: Spike detection and recording
    // Performance: O(1) spike detection and management
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function
    // Core neural computation engine
    // Biological inspiration: Membrane potential integration and spike generation
    // Mathematical: τ_m × dV/dt = -(V - V_rest) + R × I_total
    // Computational complexity: O(1) for direct integration
    // Implementation: Explicit Euler method for numerical stability
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Synaptic input processing
    // Biological inspiration: Synaptic transmission and integration
    // Computational rationale: Synaptic current processing
    // Performance: O(1) per synaptic event
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // External current injection
    // Biological inspiration: External neural stimulation and modulation
    // Computational rationale: Current-based neuron control
    // Performance: O(1) for external input processing
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history management
    // Biological inspiration: Short-term synaptic plasticity and memory
    // Computational rationale: STDP and spike timing dependent processes
    // Performance: O(1) for spike history management
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management
    // Biological inspiration: Neural connectivity and wiring
    // Computational rationale: Synaptic connectivity management
    // Performance: O(1) for synaptic connection management
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state management
    // Biological inspiration: Synaptic plasticity and learning
    // Computational rationale: Plasticity control and modulation
    // Performance: O(1) for plasticity state management
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Regional and population membership
    // Biological inspiration: Hierarchical neural organization
    // Computational rationale: Multi-level neural architecture
    // Performance: O(1) for regional and population management
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Main neural computation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    // Placeholder for advanced neural dynamics
    // Biological inspiration: Complex neuronal dynamics and computations
    // Computational rationale: Future enhancement for advanced capabilities
    // Performance: O(1) placeholder for future implementation
    void step(Timestamp currentTime);
    
    // State reset
    // Biological inspiration: Neural state reset and recovery
    // Computational rationale: State initialization for new computations
    // Performance: O(1) for state reset
    void reset();
    
    // Random initialization
    // Biological inspiration: Variability in neural development
    // Computational rationale: Diverse initial conditions
    // Performance: O(n) for neuron parameter initialization
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
