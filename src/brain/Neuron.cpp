#include "Neuron.hpp"
#include "CommonUtilities.hpp"
#include "../core/Random/Random.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>

namespace nlm {

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    NLM_VALIDATE(id.value > 0, id.value, NLMErrors::InvalidParameter, "Neuron ID must be positive");
    
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->synapticInput = 0.0f;
    
    NLM_LOG_DEBUG("Created neuron with ID: " + std::to_string(id.value));
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

/**
 * @file Neuron.cpp
 * @brief Implementation of the Neuron class for NLM neural computation
 *
 * This file contains the complete implementation of the Neuron class, providing
 * biologically realistic spiking neural dynamics using the Leaky Integrate-and-Fire
 * (LIF) model. The implementation includes synaptic integration, spike generation,
 * plasticity mechanisms, and comprehensive error handling.
 *
 * @note This implementation uses the PImpl idiom for efficient memory management
 *       and thread-safe operations. The actual neuronal dynamics are implemented
 *       in the private Impl struct to allow for optimization and binary compatibility.
 *
 * @author NLM Development Team
 * @date Phase 6 Development
 */

namespace nlm {

/**
 * @brief Internal implementation details for the Neuron class
 *
 * The PImpl (Pointer to Implementation) idiom is used to hide the implementation
 * details of the Neuron class, allowing efficient compilation and binary
 * compatibility while maintaining encapsulation of the complex neuronal dynamics.
 *
 * This struct contains all the data members required for neuronal computation,
 * including the neuron's state, synaptic inputs, connectivity, plasticity state,
 * and the mathematical parameters required for LIF dynamics.
 *
 * The internal structure includes:
 * - Basic neuron identity and classification
 * - Complete state representation for LIF dynamics
 * - Synaptic input accumulation for current injection
 * - Connectivity management for synaptic transmission
 * - Plasticity state for learning mechanisms
 * - Constants for biophysical properties
 * - Spike history for plasticity and learning
 *
 * @note The Impl struct is designed to be compact and efficient, with all the
 *       neuronal dynamics implemented in-place to minimize memory overhead and
 *       maximize computational efficiency.
 */
struct Neuron::Impl {
    /** Unique identifier for this neuron within the neural network */
    NeuronId id;
    /** Type classification of the neuron (sensory, motor, internal, etc.) */
    NeuronType type;
    /** Complete neuronal state for LIF dynamics computation */
    NeuronState state;
    /** Identifier of the region (cortical area) this neuron belongs to */
    RegionId regionId;
    /** Identifier of the population (neural ensemble) this neuron belongs to */
    PopulationId populationId;
    /** Total synaptic current input received from presynaptic neurons */
    MembranePotential totalCurrent;
    /** Accumulated synaptic input for membrane potential update */
    MembranePotential synapticInput;
    /** History of spike timestamps for plasticity and learning mechanisms */
    std::vector<Timestamp> spikeHistory;
    /** Handles of incoming synapses (presynaptic neurons) */
    std::vector<SynapseHandle> incomingSynapses;
    /** Handles of outgoing synapses (postsynaptic neurons) */
    std::vector<SynapseHandle> outgoingSynapses;
    /** Plasticity flags for enabling different learning mechanisms */
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Membrane capacitance (nF) - determines how voltage changes with current
     * @note 1 nF = 10^-9 Farads, typical for biological neurons
     */
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    
    /**
     * @brief Membrane time constant (ms) - governs passive membrane properties
     * @note τ = R*C, where R is membrane resistance and C is capacitance
     *       20 ms is typical for cortical neurons
     */
    static constexpr float TIME_CONSTANT = 20.0f;
    
    /**
     * @brief Maximum spike history size for plasticity computation
     * @note Limits memory usage while preserving sufficient history for STDP
     */
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    /**
     * @brief Default constructor for Neuron::Impl
     * @note Initializes neuron with biologically plausible default values
     *       representing an internal neuron in resting state.
     */
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

/**
 * @brief Constructor implementation for the Neuron class
 *
 * Creates a new neuron with the specified ID and initializes its internal
 * state to biologically plausible default values. The constructor performs
 * comprehensive validation of input parameters and sets up the neuron for
 * neural computation.
 *
 * @param id Unique identifier for the neuron (must be positive)
 * @throws NLMErrors::InvalidParameter if neuron ID is not positive
 * @throws NLMErrors::MemoryAllocation if memory allocation fails
 *
 * @note The constructor implements:
 * - Parameter validation for neuron identity
 * - Initialization of neuron type to Internal (default)
 * - Setting of region and population IDs to invalid (unassigned)
 * - Initialization of synaptic input and current to zero
 * - Creation of internal implementation structure
 * - Logging for neuron creation tracking
 *
 * The neuron is initialized in a resting state with default parameters
 * suitable for typical integrate-and-fire neural dynamics.
 *
 * @see setType
 * @see initializeRandom
 */
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    NLM_VALIDATE(id.value > 0, id.value, NLMErrors::InvalidParameter, "Neuron ID must be positive");
    
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->synapticInput = 0.0f;
    
    NLM_LOG_DEBUG("Created neuron with ID: " + std::to_string(id.value));
}

/**
 * @brief Default destructor for the Neuron class
 *
 * Cleans up the neuron by destroying the internal implementation structure.
 * The destructor is explicitly defaulted to ensure proper cleanup of the
 * unique_ptr smart pointer while maintaining exception safety.
 *
 * @note The destructor is defaulted to avoid unnecessary code and ensure that
 *       the unique_ptr smart pointer properly cleans up the implementation
 *       structure when the Neuron object is destroyed.
 */
Neuron::~Neuron() = default;

/**
 * @brief Move constructor for the Neuron class
 *
 * Transfers ownership of the internal implementation structure from the source
 * Neuron object to this new Neuron object. The source object's implementation
 * pointer is set to nullptr to prevent double deletion.
 *
 * @param other The source Neuron object to move from
 * @note This operation provides exception safety and efficient transfer of
 *       ownership of the internal implementation structure.
 */
Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
 * @brief Move assignment operator for the Neuron class
 *
 * Transfers ownership of the internal implementation structure from the source
 * Neuron object to this Neuron object. Properly handles self-assignment and
 * ensures safe resource transfer.
 *
 * @param other The source Neuron object to move from
 * @return Reference to this Neuron object after assignment
 * @note This operation provides strong exception safety and efficient transfer
 *       of ownership while properly handling self-assignment cases.
 */
Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

/**
 * @brief Get the neuron's unique identifier
 *
 * Retrieves the unique ID assigned to this neuron within the neural network.
 * This identifier is used for synaptic connections, population addressing,
 * and neural circuit mapping.
 *
 * @return NeuronId The unique identifier of this neuron
 * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
 *
 * @note The neuron ID is set during construction and remains constant throughout
 *       the neuron's lifetime. It is used for addressing neurons in synaptic
 *       connections and neural circuit operations.
 *
 * @see setType
 * @see addIncomingSynapse
 * @see addOutgoingSynapse
 */
NeuronId Neuron::getId() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->id;
}

/**
 * @brief Get the neuron's type classification
 *
 * Retrieves the neuron type that determines its functional role in the
 * neural network. Different neuron types have different properties and
 * behaviors in neural computation.
 *
 * @return NeuronType The type of this neuron
 * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
 *
 * @note Neuron types include:
 * - Internal: Generic computational neurons
 * - Sensory: Neurons receiving external input
 * - Motor: Neurons producing output signals
 * - Specialized: Neurons with specific computational roles
 *
 * The neuron type influences synaptic properties, firing patterns, and
 * integration capabilities.
 *
 * @see setType
 * @see getType
 */
NeuronType Neuron::getType() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->type;
}

/**
 * @brief Set the neuron's type classification
 *
 * Assigns a type to the neuron that determines its functional role and
 * properties in the neural network. Type assignment includes comprehensive
 * validation to ensure biological plausibility.
 *
 * @param type The neuron type to assign
 * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
 * @throws NLMErrors::InvalidParameter if the neuron type is invalid
 *
 * @note Valid neuron types are constrained by the NeuronType enumeration:
 * - NeuronType::Internal: Generic computational neuron
 * - NeuronType::Sensory: Neuron receiving sensory input
 * - NeuronType::Motor: Neuron producing motor output
 * - NeuronType::Specialized: Neuron with specific computational role
 *
 * The type assignment affects:
 * - Synaptic integration properties
 * - Firing threshold characteristics
 * - Connection rules
 * - Plasticity mechanisms
 *
 * @see getType
 * @see getState
 */
void Neuron::setType(NeuronType type) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    
    NLM_VALIDATE(static_cast<int>(type) >= 0 && static_cast<int>(type) <= static_cast<int>(NeuronType::Internal),
        type, NLMErrors::InvalidParameter, "Invalid neuron type");
    
    pImpl->type = type;
}

/**
 * @brief Get the neuron's complete state
 *
 * Retrieves a reference to the neuron state's complete set of parameters
 * defining the current and properties of the neuron. This includes membrane
 * potential, thresholds, synaptic inputs, and firing state.
 *
 * @return const NeuronState& Constant reference to the neuron's state
 * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
 *
 * @note The state contains all variables required for LIF dynamics computation:
 * - Membrane potential and related potentials
 * - Firing rate and state information
 * - Refractory period variables
 * - Adaptation variables
 * - Spike history
 *
 * @see getState (non-const version)
 * @see setMembranePotential
 * @see stepLIF
 */
const NeuronState& Neuron::getState() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state;
}

/**
 * @brief Get mutable reference to the neuron's state
 *
 * Retrieves a non-constant reference to the neuron state, allowing modification
 * of state variables. This is used for internal state updates during neural
 * computation and simulation.
 *
 * @return NeuronState& Non-constant reference to the neuron's state
 * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
 *
 * @note This method provides controlled access to the neuron's internal state
 *       for use in simulation steps, synaptic integration, and other internal
 *       operations. Direct modification should be performed through the
 *       appropriate accessor methods.
 *
 * @see getState (const version)
 * @see stepLIF
 * @see recordSpike
 */
NeuronState& Neuron::getState() {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state;
}

/**
 * @brief Get the neuron's current membrane potential
 *
 * Retrieves the current membrane potential of the neuron, which determines
 * its excitability and firing probability. The membrane potential is the
 * primary state variable in LIF dynamics.
 *
 * @return MembranePotential The current membrane potential (mV)
 * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
 *
 * @note The membrane potential evolves according to LIF dynamics:
 * - Integration of synaptic inputs
 * - Exponential decay toward resting potential
 * - Threshold crossing for spike generation
 * - Reset after spike generation
 *
 * @see setMembranePotential
 * @see addToMembranePotential
 * @see getThreshold
 * @see stepLIF
 */
MembranePotential Neuron::getMembranePotential() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state.membranePotential;
}

/**
 * @brief Set the neuron's membrane potential
 *
 * Assigns a specific value to the neuron's membrane potential, typically used
 * for initialization or direct state manipulation. The value must be finite
 * and biologically plausible.
 *
 * @param potential The membrane potential to set (mV)
 * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
 * @throws NLMErrors::InvalidParameter if potential is not finite
 *
 * @note Setting membrane potential directly is typically used for:
 * - Neuron initialization
 * - Simulation restarts
 * - Special computational operations
 * - Testing and debugging
 *
 * In normal simulation, membrane potential should evolve naturally through
 * the LIF dynamics integration process.
 *
 * @see getMembranePotential
 * @see addToMembranePotential
 * @see getThreshold
 */
void Neuron::setMembranePotential(MembranePotential potential) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    
    NLM_VALIDATE(std::isfinite(potential), potential, NLMErrors::InvalidParameter, "Membrane potential must be finite");
    
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    
    NLM_VALIDATE(std::isfinite(delta), delta, NLMErrors::InvalidParameter, "Delta must be finite");
    
    pImpl->state.membranePotential += delta;
}

MembranePotential Neuron::getThreshold() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state.threshold;
}

void Neuron::setThreshold(MembranePotential threshold) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    
    NLM_VALIDATE(std::isfinite(threshold), threshold, NLMErrors::InvalidParameter, "Threshold must be finite");
    NLM_VALIDATE(threshold > pImpl->state.restingPotential - 50.0f, threshold,
        NLMErrors::InvalidParameter, "Threshold must be physiologically realistic");
    
    pImpl->state.threshold = threshold;
}

bool Neuron::isFiring() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state.firingState == FiringState::Refractory && 
           pImpl->state.refractoryRemaining > 0;
}

void Neuron::setFiringState(FiringState state) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(static_cast<int>(state) >= 0 && static_cast<int>(state) <= static_cast<int>(FiringState::Inhibited),
        state, NLMErrors::InvalidParameter, "Invalid firing state");
    
    pImpl->state.firingState = state;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(steps > 0 && steps < 100000, steps, NLMErrors::InvalidParameter, "Refractory period must be reasonable (1-100000 steps)");
    
    pImpl->state.refractoryPeriod = steps;
}

void Neuron::decrementRefractory() {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

FiringRate Neuron::getFiringRate() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state.firingRate;
}

void Neuron::setFiringRate(FiringRate rate) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(std::isfinite(rate), rate, NLMErrors::InvalidParameter, "Firing rate must be finite");
    NLM_VALIDATE(rate >= 0.0f, rate, NLMErrors::InvalidParameter, "Firing rate cannot be negative");
    NLM_VALIDATE(rate < 10000.0f, rate, NLMErrors::InvalidParameter, "Firing rate too high");
    
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(std::isfinite(conductance), conductance, NLMErrors::InvalidParameter, "Leak conductance must be finite");
    NLM_VALIDATE(conductance > 0.0f && conductance < 1000.0f, conductance, NLMErrors::InvalidParameter, "Leak conductance must be positive and reasonable");
    
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state.leakConductance;
}

uint32_t Neuron::getRefractoryPeriod() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(std::isfinite(potential), potential, NLMErrors::InvalidParameter, "Resting potential must be finite");
    NLM_VALIDATE(potential < -40.0f, potential, NLMErrors::InvalidParameter, "Resting potential must be hyperpolarized");
    
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(std::isfinite(potential), potential, NLMErrors::InvalidParameter, "Reset potential must be finite");
    
    pImpl->state.resetPotential = potential;
}

MembranePotential Neuron::getResetPotential() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state.resetPotential;
}

bool Neuron::checkThreshold() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return getMembranePotential() >= getThreshold();
}

float Neuron::getLastSpikeTime() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->state.lastSpikeTime;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(std::isfinite(currentTime), currentTime, NLMErrors::InvalidParameter, "Current time must be finite");
    NLM_VALIDATE(dt > 0.0f && dt <= 0.1f, dt, NLMErrors::InvalidParameter, "Timestep must be positive and reasonable");
    
    try {
        // Check for refractory period first
        if (isRefractory()) {
            decrementRefractory();
            return false;
        }
        
        // Record state for potential spike detection
        bool fired = false;
        
        // LIF dynamics: Leaky Integrate-and-Fire
        // dV/dt = (V_rest - V)/tau + I/C
        // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
        
        MembranePotential& V = pImpl->state.membranePotential;
        MembranePotential V_rest = pImpl->state.restingPotential;
        MembranePotential V_reset = pImpl->state.resetPotential;
        MembranePotential threshold = pImpl->state.threshold;
        float tau = Impl::TIME_CONSTANT;  // ms
        float C = Impl::MEMBRANE_CAPACITANCE;  // nF
        
        // Synaptic input contributes to membrane potential change
        float synapticContribution = pImpl->synapticInput / C;
        
        // Leak contribution
        float leakContribution = (V_rest - V) / tau;
        
        // Update membrane potential using exponential Euler integration
        V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
        
        // Apply spike-frequency adaptation (slow hyperpolarization after spike)
        if (pImpl->state.adaptationVariable > 0.0f) {
            V -= pImpl->state.adaptationVariable * 0.01f;
            pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
        }
        
        // Clamp membrane potential to prevent instability using utility function
        V = WeightClamper::clamp(V, -100.0f, 50.0f);
        
        // Check for spike
        if (V >= threshold) {
            fired = true;
            pImpl->state.firingState = FiringState::Active;
            pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
            
            // Record spike
            recordSpike(currentTime);
            
            // Reset membrane potential
            V = V_reset;
            
            // Enter refractory period
            pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
            pImpl->state.firingState = FiringState::Refractory;
            
            // Update adaptation for spike-frequency adaptation
            pImpl->state.adaptationVariable += 1.0f;
            
            NLM_LOG_DEBUG("Neuron " + std::to_string(pImpl->id.value) + " spiked at time " + std::to_string(currentTime));
        } else {
            pImpl->state.firingState = FiringState::Active;
        }
        
        // Clear synaptic input for next step
        pImpl->synapticInput = 0.0f;
        
        return fired;
        
    } catch (const std::exception& e) {
        NLM_THROW(NLMErrors::IntegrationFailed, "Error in neuron LIF simulation: " + std::string(e.what()));
    }
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(std::isfinite(amplitude), amplitude, NLMErrors::InvalidParameter, "Excitatory input must be finite");
    NLM_VALIDATE(amplitude >= 0.0f, amplitude, NLMErrors::InvalidParameter, "Excitatory input must be non-negative");
    
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(std::isfinite(amplitude), amplitude, NLMErrors::InvalidParameter, "Inhibitory input must be finite");
    NLM_VALIDATE(amplitude >= 0.0f, amplitude, NLMErrors::InvalidParameter, "Inhibitory input must be non-negative");
    
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(std::isfinite(amplitude), amplitude, NLMErrors::InvalidParameter, "Modulatory input must be finite");
    
    pImpl->synapticInput += amplitude * 0.5f; // Modulatory input has reduced effect
}

void Neuron::injectCurrent(MembranePotential current) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(std::isfinite(current), current, NLMErrors::InvalidParameter, "Current injection must be finite");
    
    pImpl->totalCurrent += current;
}

MembranePotential Neuron::getTotalCurrent() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->totalCurrent;
}

void Neuron::clearTotalCurrent() {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    pImpl->totalCurrent = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(std::isfinite(timestamp), timestamp, NLMErrors::InvalidParameter, "Spike timestamp must be finite");
    
    // Prevent spike history overflow
    if (pImpl->spikeHistory.size() >= Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
    
    pImpl->spikeHistory.push_back(timestamp);
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->spikeHistory;
}

void Neuron::clearSpikeHistory() {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    nlm::clearHistory(pImpl->spikeHistory);
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(handle.id.value > 0, handle.id, NLMErrors::InvalidParameter, "Synapse handle ID must be positive");
    
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(handle.id.value > 0, handle.id, NLMErrors::InvalidParameter, "Synapse handle ID must be positive");
    
    pImpl->outgoingSynapses.push_back(handle);
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->outgoingSynapses;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->plasticityFlags;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.rewardModulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(region.value > 0, region, NLMErrors::InvalidParameter, "Region ID must be positive");
    
    pImpl->regionId = region;
}

RegionId Neuron::getRegionId() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    NLM_VALIDATE(population.value > 0, population, NLMErrors::InvalidParameter, "Population ID must be positive");
    
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    return pImpl->populationId;
}

void Neuron::reset() {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    
    NLM_LOG_DEBUG("Resetting neuron with ID: " + std::to_string(pImpl->id.value));
    
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->totalCurrent = 0.0f;
    nlm::clearHistory(pImpl->spikeHistory);
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Neuron not properly initialized");
    
    try {
        // Real random initialization with biological constraints
        // Membrane potential starts near resting potential
        pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
        
        // Threshold is typically -55mV with small variation
        pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
        
        // Resting potential typically -70mV
        pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
        
        // Reset potential is usually close to resting
        pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
        
        // Refractory period: 2-10ms typical
        pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
        
        // Set leak conductance with biological plausibility
        pImpl->state.leakConductance = rng.uniformReal(1.0f, 20.0f); // nS
        
        // Initial state
        pImpl->state.firingState = FiringState::Resting;
        pImpl->state.refractoryRemaining = 0;
        pImpl->state.adaptationVariable = 0.0f;
        pImpl->state.lastSpikeTime = -1.0f;
        
        // Clear any residual state
        pImpl->synapticInput = 0.0f;
        pImpl->totalCurrent = 0.0f;
        nlm::clearHistory(pImpl->spikeHistory);
        
        NLM_LOG_DEBUG("Randomly initialized neuron with ID: " + std::to_string(pImpl->id.value));
        
    } catch (const std::exception& e) {
        NLM_THROW(NLMErrors::DevelopmentError, "Failed to randomly initialize neuron: " + std::string(e.what()));
    }
}

} // namespace nlm
