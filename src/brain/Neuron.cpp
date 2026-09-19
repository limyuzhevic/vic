#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

/**
 * @class Neuron::Impl
 * @brief Private implementation details for the Neuron class
 * 
 * Encapsulates all internal state and configuration for neuron computation.
 * Uses the pImpl pattern to hide implementation details from the header file,
 * reducing compilation dependencies and improving modularity.
 * 
 * The implementation contains neuron state, synaptic management, and
 * biophysical parameters for Leaky Integrate-and-Fire dynamics.
 */
struct Neuron::Impl {
    NeuronId id;                         /**< Unique identifier for this neuron */
    NeuronType type;                     /**< Neuron type (excitatory, inhibitory, modulatory) */
    NeuronState state;                   /**< Complete neuron state for computation */
    RegionId regionId;                   /**< Brain region identifier for connectivity */
    PopulationId populationId;           /**< Neural population identifier for organization */
    MembranePotential totalCurrent;      /**< Total synaptic current input this integration step */
    MembranePotential synapticInput;     /**< Accumulated synaptic and injected current for current step */
    std::vector<Timestamp> spikeHistory; /**< History of recent spike times for plasticity */
    std::vector<SynapseHandle> incomingSynapses;  /**< Handles of synapses projecting to this neuron */
    std::vector<SynapseHandle> outgoingSynapses; /**< Handles of synapses originating from this neuron */
    PlasticityFlags plasticityFlags;     /**< Flags controlling which plasticity rules apply */
    
    // LIF parameters - constants for Leaky Integrate-and-Fire dynamics
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  /**< Membrane capacitance in nF (typical neuron) */
    static constexpr float TIME_CONSTANT = 20.0f;      /**< Membrane time constant in ms (tau = RC) */
    static constexpr size_t MAX_SPIKE_HISTORY = 100;   /**< Maximum spike history entries for plasticity */
    
    /**
     * @brief Default constructor initializes neuron implementation
     * 
     * Sets all member variables to default values. The neuron is initialized
     * as an internal neuron type with default biophysical parameters.
     */
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

/**
 * @brief Constructs a new Neuron with specified identifier
 * 
 * Creates a new neuron instance with the given ID. The neuron is initialized
 * as an internal neuron type with default biophysical parameters. Use setters
 * to configure neuron type, region, population, and other properties.
 * 
 * @param id Unique identifier for this neuron (should be globally unique)
 * 
 * @note Memory management uses the pImpl pattern for encapsulation and
 *       exception safety. The implementation is allocated on the heap.
 */
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

/**
 * @brief Destroys the Neuron object and cleans up resources
 * 
 * Automatically called when Neuron object goes out of scope.
 * The unique_ptr pImpl ensures proper cleanup of the implementation.
 */
Neuron::~Neuron() = default;

/**
 * @brief Move constructor - transfers ownership of neuron resources
 * 
 * Enables efficient transfer of neuron data without copying the
 * internal implementation. The source object's pImpl is reset to nullptr
 * to prevent double deletion.
 * 
 * @param other The Neuron object to move from
 * 
 * @note This constructor is part of the Rule of Five for resource management.
 *       Self-assignment is handled by the move assignment operator.
 */
Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
 * @brief Move assignment operator - transfers ownership of neuron resources
 * 
 * Enables efficient assignment by transferring ownership of the internal
 * neuron implementation without copying. Follows the Rule of Five.
 * 
 * @param other The Neuron object to move from
 * @return Neuron& Reference to this Neuron object for method chaining
 * 
 * @note Handles self-assignment safely and efficiently. Implements proper
 *       resource management with delete on the target and transfer on the source.
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
 * @brief Gets the neuron's unique identifier
 * 
 * @return NeuronId The unique identifier of this neuron
 * 
 * @note This is a read-only accessor for the neuron's identity.
 *       The ID is set at construction and remains constant.
 */
NeuronId Neuron::getId() const {
    return pImpl->id;
}

/**
 * @brief Gets the neuron type
 * 
 * @return NeuronType The type of this neuron (excitatory, inhibitory, modulatory)
 * 
 * @note Neuron type affects synaptic interaction rules and plasticity mechanisms.
 *       Used to classify neuron behavior in the network.
 */
NeuronType Neuron::getType() const {
    return pImpl->type;
}

/**
 * @brief Sets the neuron type
 * 
 * Changes the neuron type which affects synaptic interaction rules and plasticity.
 * Common types include excitatory, inhibitory, and modulatory neurons.
 * 
 * @param type The neuron type to set
 * 
 * @note Modifying neuron type after initialization can change learning dynamics
 *       and synaptic behavior in the network.
 */
void Neuron::setType(NeuronType type) {
    // Set the neuron type (excitatory, inhibitory, modulatory, etc.)
    // This affects synaptic interaction rules and plasticity
    pImpl->type = type;
}

/**
 * @brief Gets the current membrane potential
 * 
 * @return MembranePotential Current membrane potential in millivolts
 * 
 * @note The membrane potential represents the neuron's electrical state.
 *       Negative values indicate hyperpolarization, positive values
 *       indicate depolarization toward threshold.
 */
MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

/**
 * @brief Sets the membrane potential directly
 * 
 * Sets the membrane potential to an exact value. This is a direct setting
 * operation and differs from addToMembranePotential() which adds to the
 * current value. Use with caution as it bypasses normal integration dynamics.
 * 
 * @param potential Target membrane potential in millivolts
 * 
 * @note This method is typically used for initialization or testing.
 *       In normal simulation, membrane potential changes through synaptic
 *       integration in the stepLIF() method.
 */
void Neuron::setMembranePotential(MembranePotential potential) {
    // Set the membrane potential of the neuron
    // Clamping is handled in stepLIF to prevent instability
    pImpl->state.membranePotential = potential;
}

/**
 * @brief Adds a delta to the membrane potential
 * 
 * Increments the membrane potential by the specified amount. This is the
 * primary method for synaptic integration, where incoming synaptic currents
 * are converted to voltage changes.
 * 
 * @param delta The membrane potential change to add (can be positive or negative)
 * 
 * @note This is the main method for synaptic input integration.
 *       Positive delta = excitatory input, negative delta = inhibitory input.
 */
void Neuron::addToMembranePotential(MembranePotential delta) {
    // Add a delta to the membrane potential
    // Used for synaptic input integration
    pImpl->state.membranePotential += delta;
}

/**
 * @brief Gets the firing threshold of the neuron
 * 
 * @return MembranePotential The firing threshold potential in millivolts
 * 
 * @note The threshold determines when the neuron will fire an action potential.
 *       Typical values range from -55mV (excitatory) to -40mV (inhibitory).
 */
MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

/**
 * @brief Sets the firing threshold of the neuron
 * 
 * Establishes the membrane potential level at which the neuron will fire.
 * The threshold is a critical parameter determining neuronal excitability.
 * 
 * @param threshold The firing threshold potential in millivolts
 * 
 * @note This parameter is crucial for neuronal dynamics. Lower thresholds
 *       make neurons more excitable (fire more easily).
 */
void Neuron::setThreshold(MembranePotential threshold) {
    // Set the firing threshold for the LIF neuron
    // When membrane potential reaches this value, neuron fires
    pImpl->state.threshold = threshold;
}

/**
 * @brief Checks if the neuron is currently firing
 * 
 * @return bool true if the neuron's firing state is FiringState::Active
 * 
 * @note This method checks the current firing state, not whether the neuron
 *       has just spiked. Use stepLIF() to detect actual spiking events.
 */
bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active;
}

/**
 * @brief Checks if the neuron is in refractory period
 * 
 * @return bool true if the neuron is refractory (cannot fire)
 * 
 * @note Neurons enter refractory period after firing to prevent
 *       immediate re-firing and allow for recovery.
 */
bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
}

/**
 * @brief Sets the neuron's firing state
 * 
 * Changes the neuron's current state (resting, active, or refractory).
 * Typically, state changes are managed internally by stepLIF() rather
 * than directly by user code.
 * 
 * @param state The firing state to set
 * 
 * @note Direct manipulation of firing state is usually not needed for
 *       normal simulation. Use isFiring() and isRefractory() for queries.
 */
void Neuron::setFiringState(FiringState state) {
    // Set the firing state of the neuron
    // Used to track whether neuron is resting, active, or refractory
    pImpl->state.firingState = state;
}

/**
 * @brief Sets the refractory period duration
 * 
 * Establishes how long the neuron will be refractory after firing.
 * During refractory period, the neuron cannot fire regardless of input.
 * 
 * @param steps Number of simulation steps the neuron should remain refractory
 * 
 * @note Typical refractory periods range from 2-10 ms in biological neurons.
 *       In simulation terms, this is measured in integration steps.
 */
void Neuron::setRefractoryPeriod(uint32_t steps) {
    // Set the total refractory period length in time steps
    // Neurons cannot fire during this period after spiking
    pImpl->state.refractoryPeriod = steps;
}

/**
 * @brief Gets the refractory period duration
 * 
 * @return uint32_t Number of steps the neuron remains refractory after firing
 * 
 * @note This is the total refractory period length, not the remaining time.
 *       Use isRefractory() to check current state.
 */
uint32_t Neuron::getRefractoryPeriod() const {
    // Get the refractory period length
    return pImpl->state.refractoryPeriod;
}

/**
 * @brief Decrements the remaining refractory period
 * 
 * Should be called once per simulation step to track refractory progress.
 * Automatically resets firing state to resting when period ends.
 * 
 * @return void
 * 
 * @note This method is typically called by the simulation framework
 *       during the stepLIF() method. Users generally don't need to call it
 *       directly unless implementing custom stepping logic.
 */
void Neuron::decrementRefractory() {
    // Decrease the remaining refractory period by one time step
    // Automatically resets firing state to resting when complete
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
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
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
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
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
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
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
