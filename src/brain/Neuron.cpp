/**
 * @file Neuron.cpp
 * @brief Implementation of the Neuron class - Leaky Integrate-and-Fire (LIF) neuron dynamics
 * @author NLM Team
 * @date 2024
 * @version 1.0
 * @warning Phase 1 implementation. Full spiking dynamics implementation planned for Phase 2.
 */

#include "Neuron.hpp"

/**
 * @namespace nlm
 * @brief Neural Machine Learning (NLM) - A spiking neural network framework for cognitive computing
 *
 * This namespace encompasses the entire NLM ecosystem, from low-level neuron dynamics to
 * high-level cognitive functions including memory, prediction, attention, and development.
 */

#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Exception.hpp"
#include "../core/Validation/Validation.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

/**
 * @struct Neuron::Impl
 * @brief Private implementation details for the Neuron class (Pimpl idiom)
 * @details This struct contains all the actual implementation details of the Neuron class,
 *          including internal state, synaptic inputs, spike history, and LIF parameters.
 *          This allows the Neuron class to be forward-declared and maintains binary compatibility.
 *
 * The implementation includes:
 * - Neuron identity (id, type, region/population membership)
 * - Leaky Integrate-and-Fire state variables (membrane potential, threshold, etc.)
 * - Synaptic input and spike history management
 * - Plasticity flags and adaptation mechanisms
 * - LIF biophysical parameters
 */

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
    
    // ========== LIF NEURON PARAMETERS ==========
    
    /**
     * @brief Membrane capacitance (nF) - determines how much charge is needed
     *        to change the membrane potential
     */
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    
    /**
     * @brief Membrane time constant (ms) - determines how quickly the membrane
     *        potential decays toward resting potential
     */
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    
    /**
     * @brief Maximum spike history size for STDP plasticity
     * @details STDP algorithms require recent spike timing information.
     *          This limits memory usage to a reasonable size.
     */
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    /**
     * @brief Constructs a neuron implementation with default parameters
     * @details Initializes neuron state with biologically plausible defaults:
     *         - Membrane potential at resting potential (-70 mV)
     *         - Firing threshold at -55 mV
     *         - Refractory period of 5 steps
     *         - Reserved capacity for spike history
     */
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f), spikeHistory(),
             incomingSynapses(), outgoingSynapses() {
        // Initialize spike history capacity
        spikeHistory.reserve(MAX_SPIKE_HISTORY);
    }
};

/**
 * @brief Constructs a neuron with the given ID
 * @param id Unique identifier for the neuron
 * @details Creates a new neuron instance and initializes its internal state.
 *          The neuron is created in an uninitialized state but with sensible defaults:
 *          - Type: Internal neuron
 *          - Region ID: Invalid (not assigned yet)
 *          - Population ID: Invalid (not assigned yet)
 *          - Total current: Zero
 * @pre The neuron ID should be unique within the brain
 */
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

/**
 * @brief Destructor cleans up neuron resources
 * @details Deletes the private implementation (pImpl) and cleans up
 *          any allocated resources. This ensures proper memory management.
 */
Neuron::~Neuron() {
    delete pImpl;
}

/**
 * @brief Move constructor - transfers ownership of the implementation
 * @param other The neuron being moved from
 * @note After move construction, the source neuron's pImpl is set to nullptr
 *       to prevent double deletion. This is efficient for transferring ownership.
 */
Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
 * @brief Move assignment operator - transfers ownership of the implementation
 * @param other The neuron being moved from
 * @return Neuron& Reference to this neuron (for chaining)
 * @note After assignment, the source neuron's pImpl is set to nullptr
 *       to prevent double deletion. This is efficient for transferring ownership.
 */
Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    try {
        pImpl->type = type;
    } catch (...) {
        throw NeuralException("Failed to set neuron type", ErrorSeverity::ERROR, "Neuron::setType");
    }
}

void Neuron::setMembranePotential(MembranePotential potential) {
    try {
        // Validate potential
        auto validation = ValidationUtils::validatePotential(potential, "Neuron::setMembranePotential");
        if (!validation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "setMembranePotential", validation.getSummary(), "Neuron::setMembranePotential"
                )
            );
            potential = ValidationUtils::clampPotential(potential);
        }
        
        pImpl->state.membranePotential = potential;
    } catch (...) {
        throw NeuralException("Failed to set neuron membrane potential", ErrorSeverity::ERROR, "Neuron::setMembranePotential");
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    try {
        // Validate current potential
        auto validation = ValidationUtils::validatePotential(
            pImpl->state.membranePotential, "Neuron::addToMembranePotential"
        );
        if (!validation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "addToMembranePotential", validation.getSummary(), "Neuron::addToMembranePotential"
                )
            );
            // Apply safe clamping
            pImpl->state.membranePotential = ValidationUtils::clampPotential(pImpl->state.membranePotential);
        }
        
        // Validate delta
        auto deltaValidation = ValidationUtils::validatePotential(
            delta, "Neuron::addToMembranePotential delta"
        );
        if (!deltaValidation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "addToMembranePotential", deltaValidation.getSummary(), "Neuron::addToMembranePotential"
                )
            );
            delta = ValidationUtils::clampPotential(delta);
        }
        
        // Apply the delta
        pImpl->state.membranePotential += delta;
        
        // Validate result
        auto resultValidation = ValidationUtils::validatePotential(
            pImpl->state.membranePotential, "Neuron::addToMembranePotential result"
        );
        if (!resultValidation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "addToMembranePotential", resultValidation.getSummary(), "Neuron::addToMembranePotential"
                )
            );
            pImpl->state.membranePotential = ValidationUtils::clampPotential(pImpl->state.membranePotential);
        }
    } catch (const NeuralException& e) {
        throw;
    } catch (...) {
        throw NeuralException("Failed to add to neuron membrane potential", ErrorSeverity::ERROR, "Neuron::addToMembranePotential");
    }
}

void Neuron::setThreshold(MembranePotential threshold) {
    try {
        // Validate threshold
        auto validation = ValidationUtils::validatePotential(threshold, "Neuron::setThreshold");
        if (!validation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "setThreshold", validation.getSummary(), "Neuron::setThreshold"
                )
            );
            threshold = ValidationUtils::clampPotential(threshold);
        }
        
        pImpl->state.threshold = threshold;
    } catch (...) {
        throw NeuralException("Failed to set neuron threshold", ErrorSeverity::ERROR, "Neuron::setThreshold");
    }
}

void Neuron::setFiringState(FiringState state) {
    try {
        // Validate state
        if (state == FiringState::Unknown) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "Neuron", "firing_state", "unknown", "Neuron::setFiringState"
                )
            );
            return;
        }
        
        pImpl->state.firingState = state;
    } catch (...) {
        throw NeuralException("Failed to set neuron firing state", ErrorSeverity::ERROR, "Neuron::setFiringState");
    }
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    try {
        if (steps == 0) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "Neuron", "refractory_period", "zero", "Neuron::setRefractoryPeriod"
                )
            );
            return;
        }
        
        if (steps > MAX_REFRACTORY_STEPS) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "Neuron", "refractory_period", "exceeds maximum", "Neuron::setRefractoryPeriod"
                )
            );
            pImpl->state.refractoryPeriod = MAX_REFRACTORY_STEPS;
            return;
        }
        
        pImpl->state.refractoryPeriod = steps;
    } catch (...) {
        throw NeuralException("Failed to set neuron refractory period", ErrorSeverity::ERROR, "Neuron::setRefractoryPeriod");
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    try {
        // Validate potential
        auto validation = ValidationUtils::validatePotential(potential, "Neuron::setRestingPotential");
        if (!validation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "setRestingPotential", validation.getSummary(), "Neuron::setRestingPotential"
                )
            );
            potential = ValidationUtils::clampPotential(potential);
        }
        
        pImpl->state.restingPotential = potential;
    } catch (...) {
        throw NeuralException("Failed to set neuron resting potential", ErrorSeverity::ERROR, "Neuron::setRestingPotential");
    }
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    try {
        // Validate potential
        auto validation = ValidationUtils::validatePotential(potential, "Neuron::setResetPotential");
        if (!validation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "setResetPotential", validation.getSummary(), "Neuron::setResetPotential"
                )
            );
            potential = ValidationUtils::clampPotential(potential);
        }
        
        pImpl->state.resetPotential = potential;
    } catch (...) {
        throw NeuralException("Failed to set neuron reset potential", ErrorSeverity::ERROR, "Neuron::setResetPotential");
    }
}

void Neuron::setFiringState(FiringState state) {
    try {
        // Validate state
        if (state == FiringState::Unknown) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "Neuron", "firing_state", "unknown", "Neuron::setFiringState"
                )
            );
            return;
        }
        
        pImpl->state.firingState = state;
    } catch (...) {
        throw NeuralException("Failed to set neuron firing state", ErrorSeverity::ERROR, "Neuron::setFiringState");
    }
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    try {
        // Validate timestamp
        if (std::isnan(timestamp) || timestamp < 0.0) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "recordSpike", "Invalid timestamp: " + std::to_string(timestamp), "Neuron::recordSpike"
                )
            );
            return;
        }
        
        pImpl->spikeHistory.push_back(timestamp);
        if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
    } catch (...) {
        throw NeuralException("Failed to record spike", ErrorSeverity::ERROR, "Neuron::recordSpike");
    }
}

void Neuron::clearSpikeHistory() {
    try {
        pImpl->spikeHistory.clear();
    } catch (...) {
        throw NeuralException("Failed to clear spike history", ErrorSeverity::ERROR, "Neuron::clearSpikeHistory");
    }
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    try {
        // Validate handle
        if (handle == INVALID_SYNAPSE_HANDLE) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "Neuron", "addIncomingSynapse", "Invalid handle", "Neuron::addIncomingSynapse"
                )
            );
            return;
        }
        
        pImpl->incomingSynapses.push_back(handle);
    } catch (...) {
        throw NeuralException("Failed to add incoming synapse", ErrorSeverity::ERROR, "Neuron::addIncomingSynapse");
    }
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    try {
        // Validate handle
        if (handle == INVALID_SYNAPSE_HANDLE) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "Neuron", "addOutgoingSynapse", "Invalid handle", "Neuron::addOutgoingSynapse"
                )
            );
            return;
        }
        
        pImpl->outgoingSynapses.push_back(handle);
    } catch (...) {
        throw NeuralException("Failed to add outgoing synapse", ErrorSeverity::ERROR, "Neuron::addOutgoingSynapse");
    }
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    try {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
    } catch (...) {
        throw NeuralException("Failed to enable plasticity", ErrorSeverity::ERROR, "Neuron::enablePlasticity");
    }
}

void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    try {
        // Validate input parameters
        if (std::isnan(currentTime) || currentTime < 0.0) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "stepLIF", "Invalid currentTime: " + std::to_string(currentTime), "Neuron::stepLIF"
                )
            );
            currentTime = 0.0;
        }
        
        if (std::isnan(dt) || dt <= 0.0) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "stepLIF", "Invalid dt: " + std::to_string(dt), "Neuron::stepLIF"
                )
            );
            dt = 0.001; // Default 1ms
        }
        
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
        
        // Validate LIF parameters
        if (std::isnan(tau) || tau <= 0.0) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "stepLIF", "Invalid tau: " + std::to_string(tau), "Neuron::stepLIF"
                )
            );
            tau = Impl::TIME_CONSTANT;
        }
        
        if (std::isnan(C) || C <= 0.0) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "stepLIF", "Invalid C: " + std::to_string(C), "Neuron::stepLIF"
                )
            );
            C = Impl::MEMBRANE_CAPACITANCE;
        }
        
        // Validate membrane potential
        auto potentialValidation = ValidationUtils::validatePotential(V, "Neuron::stepLIF potential");
        if (!potentialValidation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "stepLIF", potentialValidation.getSummary(), "Neuron::stepLIF"
                )
            );
            V = ValidationUtils::clampPotential(V);
        }
        
        // Synaptic input contributes to membrane potential change
        float synapticContribution = pImpl->synapticInput / C;
        
        // Leak contribution
        float leakContribution = (V_rest - V) / tau;
        
        // Validate contributions
        if (std::isnan(synapticContribution) || std::isnan(leakContribution)) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralComputationError>(
                    "NaN contribution detected in LIF integration", "Neuron::stepLIF"
                )
            );
            synapticContribution = 0.0f;
            leakContribution = 0.0f;
        }
        
        // Update membrane potential using exponential Euler integration
        V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
        
        // Apply spike-frequency adaptation (slow hyperpolarization after spike)
        if (pImpl->state.adaptationVariable > 0.0f) {
            V -= pImpl->state.adaptationVariable * 0.01f;
            pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
        }
        
        // Clamp membrane potential to prevent instability
        V = std::clamp(V, -100.0f, 50.0f);
        
        // Validate result
        auto resultValidation = ValidationUtils::validatePotential(V, "Neuron::stepLIF result");
        if (!resultValidation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Neuron", "stepLIF", resultValidation.getSummary(), "Neuron::stepLIF"
                )
            );
            V = ValidationUtils::clampPotential(V);
        }
        
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
        
    } catch (const NeuralException& e) {
        throw;
    } catch (...) {
        throw NeuralException("LIF integration failed", ErrorSeverity::ERROR, "Neuron::stepLIF");
    }
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
