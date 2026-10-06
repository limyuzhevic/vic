#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// =============================================================================
// Neuron: Individual Leaky Integrate-and-Fire (LIF) Neuron Model
// =============================================================================
// 
// This class implements the core neuron model for NLM using the Leaky
// Integrate-and-Fire (LIF) dynamics. The model represents individual
// spiking neurons with realistic biophysical properties including
// membrane potential dynamics, ion channels, and spike generation.
// 
// Key Features:
// - Leaky Integrate-and-Fire dynamics with realistic membrane properties
// - Event-driven spike propagation with synaptic delays
// - Multiple ion channel models (excitation, inhibition, modulatory)
// - Refractory period handling
// - Stochastic noise for biological realism
// - Synaptic integration and transmission
// - Plasticity support (STDP, Hebbian, structural)
// 
// Biological Inspiration:
// - Similar to Hodgkin-Huxley but simplified for computational efficiency
// - Captures essential spiking dynamics of real neurons
// - Supports different neuron types with varying properties
// - Includes realistic firing patterns and adaptation mechanisms
// 
// Mathematical Model:
// dv/dt = (-v + I_syn + I_injected) / tau_m
// where:
// - v = membrane potential
// - I_syn = synaptic current
// - I_injected = external injected current
// - tau_m = membrane time constant
// 
// Spike generation when v >= threshold, then v is reset to v_reset
// and held at v_reset for refractory period tau_ref.
// 
// Usage Example:
// ```
// #include "Neuron.hpp"
// 
// // Create neuron with specific ID
// auto neuron = std::make_shared<Neuron>(NeuronId(42));
// 
// // Configure neuron properties
// neuron->setType(NeuronType::Excitatory);
// neuron->setThreshold(-50.0f);
// neuron->setRestingPotential(-70.0f);
// neuron->setRefractoryPeriod(10);  // 10ms refractory
// 
// // Add synaptic inputs
// neuron->receiveExcitatoryInput(5.0f);
// neuron->receiveInhibitoryInput(2.0f);
// 
// // Inject external current
// neuron->injectCurrent(10.0f);
// 
// // Simulate step
// neuron->step(0.0);  // Current time 0ms
// 
// // Check for firing
// if (neuron->isFiring()) {
//     double spikeTime = neuron->getLastSpikeTime();
//     std::cout << "Neuron " << neuron->getId() << " fired at " << spikeTime << " ms" << std::endl;
// }
// ```
// =============================================================================

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// =============================================================================
// Neuron State Structure
// =============================================================================

// Comprehensive neuron state structure storing all dynamic variables
// for a single neuron in the simulation.
struct NeuronState {
    // Membrane potential variables
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    
    // Firing state variables
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state (Resting, Active, Refractory, etc.)
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1.0 if never spiked)
    
    // =======================================================================
    // Constructor and Initialization
    // =======================================================================
    
    // Default constructor initializes neuron with biologically realistic parameters
    NeuronState()
        : membranePotential(-70.0f)           // Typical resting potential for mammalian neurons
        , restingPotential(-70.0f)
        , threshold(-55.0f)                   // Typical threshold for action potential initiation
        , resetPotential(-70.0f)              // Reset to resting potential after spike
        , leakConductance(10.0f)              // Controls membrane time constant (tau_m = 1/leakConductance)
        , synapseConductance(0.0f)            // Initially no synaptic input
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)              // Not in refractory period initially
        , refractoryPeriod(5)                 // 5ms refractory period typical for cortical neurons
        , adaptationVariable(0.0f)            // No adaptation initially
        , lastSpikeTime(-1.0f)                // Never spiked yet
    {
        // All other state variables are default-initialized to zero
    }
    
    // Reset method to restore initial state
    void reset() {
        membranePotential = -70.0f;
        restingPotential = -70.0f;
        threshold = -55.0f;
        resetPotential = -70.0f;
        leakConductance = 10.0f;
        synapseConductance = 0.0f;
        firingRate = 0.0f;
        firingState = FiringState::Resting;
        refractoryRemaining = 0;
        refractoryPeriod = 5;
        adaptationVariable = 0.0f;
        lastSpikeTime = -1.0f;
    }
};

// =============================================================================
// Neuron Class
// =============================================================================

// Individual neuron implementation with Leaky Integrate-and-Fire dynamics
// Supports multiple input types, output capabilities, and plasticity mechanisms
class Neuron {
public:
    // =======================================================================
    // Constructor and Destructor
    // =======================================================================
    
    // Create neuron with unique identifier
    // 
    // Parameters:
    //   id: Unique identifier for this neuron (helps with debugging and recording)
    // 
    // Notes:
    //   - Neuron ID is preserved throughout the neuron's lifetime
    //   - Used for spike recording and debugging
    //   - Does not affect simulation dynamics
    explicit Neuron(NeuronId id);
    
    // Destructor
    virtual ~Neuron();
    
    // =======================================================================
    // Copy and Move Semantics
    // =======================================================================
    
    // Disable copying to prevent multiple ownership of neuron resources
    // Neurons use reference counting internally, so copying is dangerous
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    // Enable move operations for efficient resource management
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // =======================================================================
    // Neuron Identity and Classification
    // =======================================================================
    
    // Get neuron identifier
    // 
    // Returns:
    //   Unique neuron ID for this neuron
    NeuronId getId() const;
    
    // Get current neuron type
    // 
    // Returns:
    //   Neuron type (excitatory, inhibitory, sensory, motor, etc.)
    NeuronType getType() const;
    
    // Set neuron type and update internal parameters
    // 
    // Parameters:
    //   type: New neuron type to assign
    // 
    // Notes:
    //   - Changes ion channel composition and default parameters
    //   - May affect firing patterns and plasticity capabilities
    void setType(NeuronType type);
    
    // =======================================================================
    // State Access and Modification
    // =======================================================================
    
    // Get complete neuron state
    // 
    // Returns:
    //   Const reference to neuron state structure
    const NeuronState& getState() const;
    
    // Get mutable neuron state
    // 
    // Returns:
    //   Mutable reference to neuron state (use with caution)
    NeuronState& getState();
    
    // =======================================================================
    // Membrane Potential Dynamics
    // =======================================================================
    
    // Get current membrane potential
    // 
    // Returns:
    //   Current membrane potential in mV (-80 to +50 typical range)
    MembranePotential getMembranePotential() const;
    
    // Set membrane potential directly
    // 
    // Parameters:
    //   potential: New membrane potential value (mV)
    // 
    // Notes:
    //   - Useful for injecting known initial conditions
    //   - Should generally be avoided during normal simulation
    void setMembranePotential(MembranePotential potential);
    
    // Add delta to membrane potential
    // 
    // Parameters:
    //   delta: Change in membrane potential (mV)
    // 
    // Notes:
    //   - Convenience method for synaptic input accumulation
    void addToMembranePotential(MembranePotential delta);
    
    // =======================================================================
    // Firing Threshold and State
    // =======================================================================
    
    // Get firing threshold
    // 
    // Returns:
    //   Membrane potential threshold for spike generation (mV)
    MembranePotential getThreshold() const;
    
    // Set firing threshold
    // 
    // Parameters:
    //   threshold: New threshold value (mV)
    void setThreshold(MembranePotential threshold);
    
    // Check if neuron is currently firing (in active state)
    // 
    // Returns:
    //   true if neuron is in active firing state, false otherwise
    bool isFiring() const;
    
    // Check if neuron is in refractory period
    // 
    // Returns:
    //   true if neuron cannot fire due to refractory period
    bool isRefractory() const;
    
    // Set firing state explicitly
    // 
    // Parameters:
    //   state: New firing state (Resting, Active, Refractory, etc.)
    void setFiringState(FiringState state);
    
    // Set refractory period duration
    // 
    // Parameters:
    //   steps: Number of simulation steps to remain refractory
    void setRefractoryPeriod(uint32_t steps);
    
    // Decrement refractory counter
    // 
    // Notes:
    //   - Called automatically during simulation steps
    //   - When refractoryRemaining reaches 0, neuron can fire again
    void decrementRefractory();
    
    // =======================================================================
    // LIF Neuron Parameters
    // =======================================================================
    
    // Get leak conductance (controls membrane time constant)
    // 
    // Returns:
    //   Leak conductance (nS)
    MembranePotential getLeakConductance() const;
    
    // Set leak conductance
    // 
    // Parameters:
    //   conductance: Leak conductance value (nS)
    void setLeakConductance(MembranePotential conductance);
    
    // Get refractory period
    // 
    // Returns:
    //   Refractory period in simulation steps
    uint32_t getRefractoryPeriod() const;
    
    // Set resting potential
    // 
    // Parameters:
    //   potential: Resting potential (mV)
    void setRestingPotential(MembranePotential potential);
    
    // Get resting potential
    // 
    // Returns:
    //   Resting potential (mV)
    MembranePotential getRestingPotential() const;
    
    // Set reset potential (after spike)
    // 
    // Parameters:
    //   potential: Reset potential (mV)
    void setResetPotential(MembranePotential potential);
    
    // Get reset potential
    // 
    // Returns:
    //   Reset potential (mV)
    MembranePotential getResetPotential() const;
    
    // =======================================================================
    // Spike Detection and Timing
    // =======================================================================
    
    // Check if neuron has reached firing threshold
    // 
    // Returns:
    //   true if membrane potential >= threshold (would fire)
    bool checkThreshold() const;
    
    // Get timestamp of last spike
    // 
    // Returns:
    //   Simulation time of last spike (ms), -1.0 if never spiked
    float getLastSpikeTime() const;
    
    // =======================================================================
    // LIF Integration Step
    // =======================================================================
    
    // Perform one LIF integration step
    // 
    // Parameters:
    //   currentTime: Current simulation time (ms)
    // 
    // Returns:
    //   true if neuron fired during this step, false otherwise
    // 
    // Notes:
    //   - Implements standard LIF equation: dv/dt = (-v + I_total) / tau_m
    //   - Handles spike generation, refractory period, and reset
    //   - Updates membrane potential and other state variables
    bool stepLIF(Timestamp currentTime);
    
    // =======================================================================
    // Input Processing
    // =======================================================================
    
    // Receive excitatory post-synaptic potential (PSP)
    // 
    // Parameters:
    //   amplitude: PSP amplitude (mV)
    // 
    // Notes:
    //   - Typically from glutamatergic synapses
    //   - Increases membrane potential towards threshold
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    // Receive inhibitory post-synaptic potential (PSP)
    // 
    // Parameters:
    //   amplitude: PSP amplitude (mV) - typically negative
    // 
    // Notes:
    //   - Typically from GABAergic synapses
    //   - Decreases membrane potential (hyperpolarization)
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    // Receive modulatory post-synaptic potential (PSP)
    // 
    // Parameters:
    //   amplitude: PSP amplitude (mV)
    // 
    // Notes:
    //   - Typically from neuromodulatory synapses (dopamine, etc.)
    //   - Can have longer time constants and different effects
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // =======================================================================
    // External Current Injection
    // =======================================================================
    
    // Inject external current from biological or experimental sources
    // 
    // Parameters:
    //   current: External current to inject (pA)
    // 
    // Notes:
    //   - Represents biological input or experimental stimulation
    //   - Positive current depolarizes, negative hyperpolarizes
    void injectCurrent(MembranePotential current);
    
    // Get total injected current
    // 
    // Returns:
    //   Total external current (pA)
    MembranePotential getTotalCurrent() const;
    
    // Clear total injected current
    // 
    // Notes:
    //   - Useful for resetting experimental stimulation
    void clearTotalCurrent();
    
    // =======================================================================
    // Spike Recording
    // =======================================================================
    
    // Record a spike timestamp
    // 
    // Parameters:
    //   timestamp: Time when spike occurred (ms)
    // 
    // Notes:
    //   - Used for STDP calculations and spike history analysis
    //   - Maintains spike history for plasticity rules
    void recordSpike(Timestamp timestamp);
    
    // Get spike history
    // 
    // Returns:
    //   Vector of spike timestamps (most recent last)
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    // Clear spike history
    // 
    // Notes:
    //   - Used for experiments or testing
    //   - Resets STDP eligibility traces
    void clearSpikeHistory();
    
    // =======================================================================
    // Synapse Management
    // =======================================================================
    
    // Add incoming synaptic connection
    // 
    // Parameters:
    //   handle: Synapse handle for incoming connection
    void addIncomingSynapse(SynapseHandle handle);
    
    // Add outgoing synaptic connection  
    // 
    // Parameters:
    //   handle: Synapse handle for outgoing connection
    void addOutgoingSynapse(SynapseHandle handle);
    
    // Get all incoming synapses
    // 
    // Returns:
    //   Vector of incoming synapse handles
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    // Get all outgoing synapses
    // 
    // Returns:
    //   Vector of outgoing synapse handles
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // =======================================================================
    // Plasticity State and Management
    // =======================================================================
    
    // Get plasticity flags
    // 
    // Returns:
    //   Const reference to plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    
    // Get mutable plasticity flags
    // 
    // Returns:
    //   Mutable reference to plasticity flags
    PlasticityFlags& getPlasticityFlags();
    
    // Enable plasticity mechanisms
    // 
    // Parameters:
    //   hebbian: Enable Hebbian (rate-based) plasticity
    //   stdp: Enable Spike-Timing-Dependent Plasticity
    //   rewardModulated: Enable reward-modulated plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // =======================================================================
    // Regional and Population Membership
    // =======================================================================
    
    // Set region ID for brain region membership
    // 
    // Parameters:
    //   region: Region ID to assign
    void setRegionId(RegionId region);
    
    // Get region ID
    // 
    // Returns:
    //   Current region ID
    RegionId getRegionId() const;
    
    // Set population ID for neural population membership
    // 
    // Parameters:
    //   population: Population ID to assign
    void setPopulationId(PopulationId population);
    
    // Get population ID
    // 
    // Returns:
    //   Current population ID
    PopulationId getPopulationId() const;
    
    // =======================================================================
    // Simulation Step
    // =======================================================================
    
    // Update neuron for one simulation step (main interface)
    // 
    // Parameters:
    //   currentTime: Current simulation time (ms)
    // 
    // Notes:
    //   - Calls stepLIF() to implement LIF dynamics
    //   - Handles all input processing and spike generation
    //   - Main simulation entry point for neurons
    void step(Timestamp currentTime);
    
    // =======================================================================
    // State Reset and Initialization
    // =======================================================================
    
    // Reset neuron to initial state
    // 
    // Resets all state variables to initial conditions while
    // preserving neuron ID and type.
    void reset();
    
    // Initialize neuron with random parameters
    // 
    // Parameters:
    //   rng: Random number generator for stochastic initialization
    // 
    // Notes:
    //   - Used for generating diverse neural populations
    //   - Sets random membrane properties and initial state
    void initializeRandom(RandomGenerator& rng);
    
protected:
    // Internal implementation details (pImpl pattern)
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal constructor for implementation
    explicit Neuron(Impl* impl);
    
private:
    // Additional private implementation details
    class Logger* getLogger() const;
};

} // namespace nlm
