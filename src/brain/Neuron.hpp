#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

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

/**
 * @class Neuron
 * @brief Represents a single artificial neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
 * @details This class models a biologically-inspired neuron that integrates synaptic inputs over time
 *          and emits spikes when the membrane potential crosses a threshold. The neuron implements
 *          the classic LIF model used in computational neuroscience to balance biological plausibility
 *          with computational efficiency.
 *          
 *          The neuron follows the differential equation:
 *          τ dV/dt = -(V - V_rest) + R·I_syn + I_ext
 *          where τ = membrane time constant, V = membrane potential, R = membrane resistance
 *          
 *          Biological inspiration comes from real cortical neurons with:
 *          - Membrane potential dynamics with exponential decay
 *          - Spike threshold and reset mechanism
 *          - Refractory period preventing excessive firing
 *          - Synaptic integration of excitatory and inhibitory inputs
 *          - Short-term plasticity and adaptation mechanisms
 *          
 *          Parameters typically range:
 *          - Resting potential: -70 mV
 *          - Threshold: -55 mV  
 *          - Refractory period: 1-5 ms
 *          - Membrane time constant: 10-20 ms
 *          
 * @note This class uses the PImpl idiom for performance and forward compatibility
 * @see Synapse, NeuralPopulation, NeuralRegion, PlasticityRule, Neuromodulator
 * @example
 * @code
 * Neuron neuron(1);  // Create neuron with ID 1
 * neuron.setThreshold(-50.0f);
 * neuron.setLeakConductance(20.0f);
 * float input = 100.0f;
 * neuron.injectCurrent(input);
 * bool fired = neuron.stepLIF(currentTime, dt);
 * if (fired) {
 *     // Handle spike: update connected synapses
 * }
 * @endcode
 */
class Neuron {
public:
    /**
     * @brief Construct a new Neuron object with the given ID
     * @param id Unique identifier for this neuron (typically 0 to neuron_count-1)
     * 
     * Neuron IDs are used for:
     * - References in synaptic connections
     * - Identifying neurons in population management
     * - Spike event tracking
     * - Memory of neuron identity across simulation steps
     * 
     * @note IDs must be unique within the brain simulation
     * @see NeuronId, Synapse
     */
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
    
    // Update neuron for one simulation step
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
    // Step LIF simulation (kept for backward compatibility)
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
