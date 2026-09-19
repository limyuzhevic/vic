#include "NeuralDynamics.hpp"
#include "../core/Config/Config.hpp"
#include <cmath>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <unordered_map>

namespace nlm {

/**
 * @class IntegrateAndFireDynamics
 * @brief Implements a biologically realistic Integrate-and-Fire (LIF) neuron model with spike-frequency adaptation and short-term plasticity.
 * 
 * This class provides a comprehensive neural dynamics model based on the Leaky Integrate-and-Fire framework,
 * enhanced with biological features:
 * - Exponential Euler integration for stable membrane potential dynamics
 * - Spike detection with refractory period and reset
 * - Conductance-based synaptic integration with reversal potentials
 * - Spike-frequency adaptation via adaptive conductance
 * - Short-term plasticity (Tsodyks-Markram model) for synaptic dynamics
 * - Delay-based synaptic transmission
 * - Configuration-driven parameters with validation
 * 
 * @tparam TimestepDuration Type representing timestep duration in seconds
 * 
 * @see https://en.wikipedia.org/wiki/Leaky_integrate-and-fire_model
 * @see Tsodyks, A. V., Markram, H., & Tsodyks, E. (1998). The neural code. Biased nets.
 */

/**
 * @brief Internal implementation structure for IntegrateAndFireDynamics
 * 
 * Contains all parameters and state variables for the LIF neuron model.
 * The pImpl idiom is used to separate interface from implementation details.
 */
struct IntegrateAndFireDynamics::Impl {
    // =============================================================================
    // NEURON MEMBRANE PARAMETERS
    // =============================================================================
    
    /**
     * @brief Membrane time constant (tau)
     * @details Time constant for passive membrane properties
     * @units ms
     * @range [1.0, 100.0]
     * @default 20.0
     * @biological_rationale Typical for cortical pyramidal neurons
     */
    float membraneTimeConstant;     // ms
    
    /**
     * @brief Membrane resistance (Rm)
     * @details Input resistance of the neuron membrane
     * @units MOhm
     * @range [1.0, 1000.0]
     * @default 10.0
     * @biological_rationale Determines input resistance and excitability
     */
    float membraneResistance;        // MOhm
    
    /**
     * @brief Resting membrane potential (Vrest)
     * @details Steady-state membrane potential in absence of input
     * @units mV
     * @range [-90.0, -50.0]
     * @default -70.0
     * @biological_rationale Established resting potential for cortical neurons
     */
    float restingPotential;         // mV
    
    /**
     * @brief Firing threshold (Vth)
     * @details Membrane potential at which neuron fires an action potential
     * @units mV
     * @range [-55.0, -30.0]
     * @default -55.0
     * @biological_rationale Typical action potential threshold
     */
    float thresholdPotential;       // mV
    
    /**
     * @brief Reset potential (Vreset)
     * @details Membrane potential after spike and refractory period
     * @units mV
     * @range [-80.0, -65.0]
     * @default -70.0
     * @biological_rationale Reset to resting potential, may include hyperpolarization
     */
    float resetPotential;            // mV
    
    /**
     * @brief Membrane capacitance (Cm)
     * @details Membrane's ability to store charge
     * @units nF
     * @range [0.1, 5.0]
     * @default 0.5
     * @biological_rationale Typical for cortical neuron dendrites and soma
     */
    float capacitance;               // nF
    
    // =============================================================================
    // SYNAPTIC REVERSAL POTENTIALS
    // =============================================================================
    
    /**
     * @brief Excitatory synaptic reversal potential
     * @details Equilibrium potential for AMPA/NMDA receptors
     * @units mV
     * @range [0.0, 60.0]
     * @default 0.0
     * @biological_rationale Sodium equilibrium potential for excitatory synapses
     */
    float reversalPotentialExc;     // mV
    
    /**
     * @brief Inhibitory synaptic reversal potential
     * @details Equilibrium potential for GABA_A receptors
     * @units mV
     * @range [-90.0, -60.0]
     * @default -70.0
     * @biological_rationale Chloride equilibrium potential for inhibitory synapses
     */
    float reversalPotentialInh;     // mV
    
    // =============================================================================
    // SPIKE-FREQUENCY ADAPTATION PARAMETERS
    // =============================================================================
    
    /**
     * @brief Adaptation conductance (gK)
     * @details Conductance of potassium channels responsible for AHP
     * @units nS
     * @range [0.0, 10.0]
     * @default 0.5
     * @biological_rationale Mediates spike-frequency adaptation
     */
    float adaptationConductance;     // nS
    
    /**
     * @brief Adaptation time constant (tau_adapt)
     * @details Time constant for adaptation decay
     * @units ms
     * @range [10.0, 1000.0]
     * @default 100.0
     * @biological_rationale Slow recovery of adaptation after spiking
     */
    float adaptationTimeConstant;   // ms
    
    /**
     * @brief Adaptation slope factor (slope)
     * @details Conversion factor from adaptation conductance to voltage shift
     * @units mV/nS
     * @range [0.0, 1.0]
     * @default 0.1
     * @biological_rationale Scales adaptation effect on membrane potential
     */
    float adaptationSlope;           // mV/nS
    
    // =============================================================================
    // REFRACTORY PERIOD PARAMETERS
    // =============================================================================
    
    /**
     * @brief Refractory period duration
     * @details Number of timesteps neuron cannot fire after a spike
     * @units simulation steps
     * @range [1, 100]
     * @default 5
     * @biological_rationale Absolute refractory period for action potential generation
     */
    uint32_t refractoryPeriod;       // simulation steps
    
    /**
     * @brief Post-spike adaptation hyperpolarization
     * @details Additional hyperpolarization after spike beyond reset
     * @units mV
     * @range [0.0, 20.0]
     * @default 5.0
     * @biological_rationale Additional AHP following spike train
     */
    float postSpikeAdaptation;      // mV
    
    // =============================================================================
    // SHORT-TERM PLASTICITY PARAMETERS (Tsodyks-Markram Model)
    // =============================================================================
    
    /**
     * @brief Facilitation time constant (tau_fac)
     * @details Time constant for short-term facilitation
     * @units ms
     * @range [10.0, 500.0]
     * @default 100.0
     * @biological_rationale Rate of facilitation decay
     */
    float stpFacilitationTime;       // ms
    
    /**
     * @brief Depression time constant (tau_dep)
     * @details Time constant for short-term depression
     * @units ms
     * @range [10.0, 500.0]
     * @default 200.0
     * @biological_rationale Rate of depression recovery
     */
    float stpDepressionTime;        // ms
    
    /**
     * @brief Maximum utilization (U)
     * @details Maximum probability of vesicle release
     * @units dimensionless
     * @range [0.0, 1.0]
     * @default 1.0
     * @biological_rationale Release probability parameter in STM model
     */
    float stpUmax;                   // dimensionless (0-1)
    
    // =============================================================================
    // STATE VARIABLES (Updated during simulation)
    // =============================================================================
    
    /**
     * @brief Current adaptation variable value
     * @details Tracks cumulative adaptation from spike activity
     * @units nS
     * @range [0.0, 10.0]
     */
    float currentAdaptation;         // nS
    
    /**
     * @brief Last spike timestamp
     * @details Time of most recent action potential
     * @units seconds
     * @range [-inf, inf] (-1 if none)
     */
    float lastSpikeTime;             // seconds
    
    // =============================================================================
    // VALIDATION AND INITIALIZATION
    // =============================================================================
    
    /**
     * @brief Validates all parameters for numerical stability and biological plausibility
     * @throws Assertion failures if parameters are invalid
     */
    void validateParameters() const {
        assert(membraneTimeConstant > 0.0f && "Membrane time constant must be positive");
        assert(membraneResistance > 0.0f && "Membrane resistance must be positive");
        assert(std::abs(restingPotential - thresholdPotential) > 5.0f && 
               "Threshold too close to rest - potential instability");
        assert(std::abs(thresholdPotential - resetPotential) > 1.0f && 
               "Reset too close to threshold - spike generation failure");
        assert(capacitance > 0.0f && "Membrane capacitance must be positive");
        assert(adaptationConductance >= 0.0f && "Adaptation conductance must be non-negative");
        assert(refractoryPeriod > 0 && "Refractory period must be positive");
        assert(stpUmax >= 0.0f && stpUmax <= 1.0f && "STP Umax must be in [0,1]");
        assert(reversalPotentialExc > reversalPotentialInh && 
               "Excitatory reversal must be greater than inhibitory reversal");
    }
    
    /**
     * @brief Constructs LIF parameters with biologically realistic defaults
     */
    Impl() 
        : membraneTimeConstant(20.0f)     // Typical cortical neuron tau
        , membraneResistance(10.0f)        // Typical specific resistance
        , restingPotential(-70.0f)        // Standard resting potential
        , thresholdPotential(-55.0f)      // Typical threshold
        , resetPotential(-70.0f)           // Reset to rest
        , capacitance(0.5f)               // Typical membrane capacitance
        , reversalPotentialExc(0.0f)      // Excitatory reversal (Na+ equilibrium)
        , reversalPotentialInh(-70.0f)    // Inhibitory reversal (Cl- equilibrium)
        , adaptationConductance(0.5f)     // AHP conductance
        , adaptationTimeConstant(100.0f)  // Slow adaptation decay
        , adaptationSlope(0.1f)           // Slope factor
        , refractoryPeriod(5)             // 5 steps refractory period
        , postSpikeAdaptation(5.0f)      // 5mV hyperpolarization after spike
        , stpFacilitationTime(100.0f)    // 100ms facilitation time constant
        , stpDepressionTime(200.0f)      // 200ms depression time constant
        , stpUmax(1.0f)                   // Maximum utilization
        , currentAdaptation(0.0f)        // Initial adaptation state
        , lastSpikeTime(-1.0f)            // No previous spike
    {
        validateParameters();
    }
};

/**
 * @brief Default constructor for IntegrateAndFireDynamics
 */
IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

/**
 * @brief Destructor for IntegrateAndFireDynamics
 */
IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

/**
 * @brief Updates neuron membrane potential and handles spike generation for one timestep
 * 
 * Implements exponential Euler integration of the LIF equation:
 * C * dV/dt = -g_leak * (V - E_leak) + I_syn - g_K(t) * (V - E_K)
 * 
 * Where g_K(t) is the time-varying adaptation conductance that increases with spike rate.
 * 
 * @param neuron Pointer to neuron to update
 * @param dt Timestep duration in seconds
 * 
 * @pre neuron != nullptr
 * @post neuron membrane potential updated, spike state may be Active
 */
void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    assert(neuron != nullptr && "Neuron pointer cannot be null");
    assert(dt > 0.0 && "Timestep duration must be positive");
    
    const auto& state = neuron->getState();
    float& V = neuron->getMembranePotential();
    
    // Extract neuron state for convenience
    float V_rest = state.restingPotential;
    float I_syn = neuron->getTotalCurrent();
    
    // Get dynamics parameters
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float Cm = pImpl->capacitance;
    float gK = pImpl->adaptationConductance;
    float Ek = pImpl->reversalPotentialInh;  // Potassium reversal potential
    
    // Calculate adaptation conductance based on spike history
    // Adapts according to: dgK/dt = (gK_inf - gK) / tau_adapt
    float gK_inf = pImpl->currentAdaptation;
    float tau_adapt = pImpl->adaptationTimeConstant;
    
    // Exponential Euler integration for adaptation variable
    if (pImpl->lastSpikeTime >= 0.0f) {
        float timeSinceLastSpike = static_cast<float>(neuron->getState().lastSpikeTime);
        // Adaptation saturates at current spike rate dependent level
        pImpl->currentAdaptation = gK_inf * (1.0f - std::exp(-timeSinceLastSpike / tau_adapt));
    }
    
    // Exponential Euler integration for membrane potential
    // dV/dt = (V_rest - V)/tau + I_syn/C - gK*(V - Ek)/Cm
    // V_new = V + dt * (V_rest - V)/tau + I_syn/C - gK*(V - Ek)/Cm
    
    float dV_leak = (V_rest - V) / tau;
    float I_syn_contrib = I_syn / Cm;
    float I_adapt = pImpl->currentAdaptation * (V - Ek) / Cm;
    
    // Update membrane potential using exponential Euler (more stable than explicit Euler)
    V += static_cast<float>(dt) * 1000.0f * (dV_leak + I_syn_contrib - I_adapt);
    
    // Apply spike-frequency adaptation effect
    // Adaptation reduces membrane potential proportionally to adaptation conductance
    float V_shift = pImpl->adaptationSlope * pImpl->currentAdaptation;
    V -= V_shift;
    
    // Clamp membrane potential to prevent numerical instability
    // Below -100mV (hyperpolarized) and above +50mV (unphysiological)
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        // During refractory period, neuron cannot fire and potential is clamped
        // to reset potential plus additional hyperpolarization
        float refractoryVoltage = pImpl->resetPotential - pImpl->postSpikeAdaptation;
        V = refractoryVoltage;
        neuron->decrementRefractory();
    }
    
    // Check for spike threshold crossing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        pImpl->lastSpikeTime = neuron->getState().lastSpikeTime;
        
        // Record spike with accurate timing
        neuron->recordSpike(pImpl->lastSpikeTime);
        
        // Apply spike reset
        V = pImpl->resetPotential - pImpl->postSpikeAdaptation;
        
        // Update adaptation variable on spike
        // Each spike increases adaptation conductance
        pImpl->currentAdaptation += pImpl->adaptationConductance * 0.1f;  // 10% increase per spike
        
        // Enter refractory period
        neuron->setRefractoryPeriod(pImpl->refractoryPeriod);
        neuron->setFiringState(FiringState::Refractory);
    }
    
    // Clear synaptic current for next timestep
    neuron->clearTotalCurrent();
}

/**
 * @brief Updates synaptic state for one timestep including short-term plasticity
 * 
 * Implements Tsodyks-Markram model for short-term plasticity (STP):
 * du/dt = (U - u) / tau_u + x * (1 - u) / tau_x
 * dx/dt = -u * x / tau_x
 * dy/dt = (-y + x) / tau_y
 * 
 * Where:
 * - u is utilization factor (0-1)
 * - x is available resource fraction (0-1)  
 * - y is accumulated resources (0-1)
 * - U is maximum utilization probability
 * - tau_u, tau_x, tau_y are time constants for different dynamics
 * 
 * @param synapse Pointer to synapse to update
 * @param dt Timestep duration in seconds
 * 
 * @pre synapse != nullptr
 * @post Synapse efficacy and state updated
 */
void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    assert(synapse != nullptr && "Synapse pointer cannot be null");
    assert(dt > 0.0 && "Timestep duration must be positive");
    
    // Delegate to synapse's step function which contains STP dynamics
    // The Synapse class implements the full Tsodyks-Markram model
    synapse->step(static_cast<Timestamp>(dt));
}

/**
 * @brief Applies synaptic input to neuron based on spike transmission
 * 
 * Implements conductance-based synaptic transmission:
 * I_syn = g_syn * (V - E_rev)
 * 
 * Where:
 * - g_syn is synaptic conductance
 * - V is postsynaptic membrane potential
 * - E_rev is reversal potential (0 for excitatory, -70 for inhibitory)
 * 
 * Includes delay-based spike delivery and synaptic weight scaling.
 * 
 * @param neuron Pointer to postsynaptic neuron
 * @param synapse Pointer to synapse delivering input
 * 
 * @pre neuron != nullptr and synapse != nullptr
 * @post Neuron receives synaptic input based on synapse properties
 */
void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    assert(neuron != nullptr && "Neuron pointer cannot be null");
    assert(synapse != nullptr && "Synapse pointer cannot be null");
    
    // Get synaptic properties
    SynapticWeight weight = synapse->getWeight();
    SynapseType type = synapse->getType();
    
    // Convert weight to conductance based on synapse type
    // Excitatory synapses contribute positive current
    // Inhibitory synapses contribute negative current
    MembranePotential synapticConductance = 0.0f;
    
    switch (type) {
        case SynapseType::Excitatory:
            // AMPA/NMDA mediated excitation
            // Reversal potential ~0mV (Na+ equilibrium)
            synapticConductance = std::max(weight, 0.0f) * 10.0f;  // Scale factor
            neuron->receiveExcitatoryInput(synapticConductance);
            break;
            
        case SynapseType::Inhibitory:
            // GABA_A mediated inhibition
            // Reversal potential ~-70mV (Cl- equilibrium)
            synapticConductance = -std::min(weight, 0.0f) * 10.0f;  // Scale and negate
            neuron->receiveInhibitoryInput(-synapticConductance);
            break;
            
        case SynapseType::Modulatory:
            // Neuromodulatory input (e.g., acetylcholine, dopamine)
            // Acts on plasticity rather than direct voltage change
            synapticConductance = weight * 0.1f;  // Smaller effect
            neuron->receiveModulatoryInput(synapticConductance);
            break;
            
        case SynapseType::Electrical:
            // Electrical coupling (gap junctions)
            // Direct voltage sharing
            float V_pre = neuron->getMembranePotential();  // Would need pre-synaptic voltage
            // Simplified: add small fraction of difference
            neuron->addToMembranePotential(V_pre * 0.01f);
            break;
            
        case SynapseType::GapJunction:
            // Similar to electrical coupling
            neuron->addToMembranePotential(neuron->getMembranePotential() * 0.01f);
            break;
    }
}

/**
 * @brief Checks if neuron has crossed firing threshold
 * 
 * Determines if neuron should fire based on membrane potential crossing
 * threshold. Includes refractory period consideration.
 * 
 * @param neuron Pointer to neuron to check
 * @return true if neuron should fire, false otherwise
 * 
 * @pre neuron != nullptr
 */
bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    assert(neuron != nullptr && "Neuron pointer cannot be null");
    
    // Check if membrane potential has crossed threshold
    bool shouldFire = neuron->getMembranePotential() >= neuron->getThreshold();
    
    // If neuron is in refractory period, it cannot fire even if threshold crossed
    if (shouldFire && neuron->isRefractory()) {
        return false;
    }
    
    return shouldFire;
}

/**
 * @brief Resets dynamics state (no internal state maintained)
 * 
 * The IntegrateAndFireDynamics has no persistent internal state that needs
 * resetting between simulations. All state is stored in neuron and synapse objects.
 */
void IntegrateAndFireDynamics::reset() {
    // No internal state to reset
}

/**
 * @brief Sets membrane time constant parameter
 * 
 * @param tau Membrane time constant in milliseconds
 * 
 * @pre tau > 0.0
 */
void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    assert(tau > 0.0f && "Membrane time constant must be positive");
    pImpl->membraneTimeConstant = tau;
}

/**
 * @brief Gets membrane time constant parameter
 * 
 * @return Membrane time constant in milliseconds
 */
float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
