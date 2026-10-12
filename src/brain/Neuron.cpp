#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/ErrorHandling.hpp"
#include <cmath>
#include <algorithm>
#include <cstring>

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
    // Validate input parameters
    if (!Validation::isValidNeuronId(id)) {
        ExceptionManager::throwError<NLMException>(
            "Invalid neuron ID for neuron creation");
    }
    
    try {
        pImpl->id = id;
        pImpl->type = NeuronType::Internal;
        pImpl->regionId = INVALID_REGION_ID;
        pImpl->populationId = INVALID_POPULATION_ID;
        pImpl->totalCurrent = 0.0f;
    } catch (const std::exception& e) {
        // If initialization fails, clean up and throw
        delete pImpl;
        pImpl = nullptr;
        ExceptionManager::throwError<MemoryError>(
            "Failed to initialize neuron: " + std::string(e.what()),
            MemoryError::Type::MemoryError);
    }
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        try {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        } catch (const std::exception& e) {
            // If move assignment fails, attempt to recover by setting self to valid state
            ExceptionManager::getInstance().logError(NLMException(
                "Failed to move-assign neuron: " + std::string(e.what())));
        }
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    try {
        // Validate that we have a proper neuron type
        if (static_cast<uint8_t>(type) > static_cast<uint8_t>(NeuronType::Internal)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid neuron type: " + std::to_string(static_cast<int>(type)));
        }
        
        pImpl->type = type;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set neuron type: " + std::string(e.what()));
    }
}

void Neuron::setMembranePotential(MembranePotential potential) {
    try {
        if (!Validation::isValidMembranePotential(potential)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid membrane potential: " + std::to_string(potential));
        }
        
        pImpl->state.membranePotential = potential;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set membrane potential: " + std::string(e.what()));
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    try {
        if (!Validation::isValidMembranePotential(delta)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid membrane potential delta: " + std::to_string(delta));
        }
        
        pImpl->state.membranePotential += delta;
        
        // Clamp to prevent instability
        if (!Validation::isValidMembranePotential(pImpl->state.membranePotential)) {
            pImpl->state.membranePotential = std::clamp(pImpl->state.membranePotential, -100.0f, 50.0f);
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to add to membrane potential: " + std::string(e.what()));
    }
}

void Neuron::setThreshold(MembranePotential threshold) {
    try {
        if (!Validation::isValidMembranePotential(threshold)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid threshold: " + std::to_string(threshold));
        }
        
        pImpl->state.threshold = threshold;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set threshold: " + std::string(e.what()));
    }
}

void Neuron::setFiringState(FiringState state) {
    try {
        if (static_cast<uint8_t>(state) > static_cast<uint8_t>(FiringState::Inhibited)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid firing state: " + std::to_string(static_cast<int>(state)));
        }
        
        pImpl->state.firingState = state;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set firing state: " + std::string(e.what()));
    }
}

void Neuron::decrementRefractory() {
    try {
        if (pImpl->state.refractoryRemaining > 0) {
            --pImpl->state.refractoryRemaining;
            if (pImpl->state.refractoryRemaining == 0) {
                pImpl->state.firingState = FiringState::Resting;
            }
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to decrement refractory period: " + std::string(e.what()));
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    try {
        if (!Validation::isValidMembranePotential(rate)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid firing rate: " + std::to_string(rate));
        }
        
        pImpl->state.firingRate = rate;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set firing rate: " + std::string(e.what()));
    }
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    try {
        if (!Validation::isValidMembranePotential(conductance)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid leak conductance: " + std::to_string(conductance));
        }
        
        pImpl->state.leakConductance = conductance;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set leak conductance: " + std::string(e.what()));
    }
}

MembranePotential Neuron::getLeakConductance() const {
    try {
        return pImpl->state.leakConductance;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get leak conductance: " + std::string(e.what()));
    }
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    try {
        if (steps == 0) {
            ExceptionManager::throwError<NLMException>(
                "Refractory period cannot be zero");
        }
        if (steps > 1000) {  // Reasonable upper limit
            ExceptionManager::throwError<NLMException>(
                "Refractory period too large: " + std::to_string(steps));
        }
        
        pImpl->state.refractoryPeriod = steps;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set refractory period: " + std::string(e.what()));
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    try {
        return pImpl->state.refractoryPeriod;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get refractory period: " + std::string(e.what()));
    }
}

void Neuron::setRestingPotential(MembranePotential potential) {
    try {
        if (!Validation::isValidMembranePotential(potential)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid resting potential: " + std::to_string(potential));
        }
        
        pImpl->state.restingPotential = potential;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set resting potential: " + std::string(e.what()));
    }
}

MembranePotential Neuron::getRestingPotential() const {
    try {
        return pImpl->state.restingPotential;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get resting potential: " + std::string(e.what()));
    }
}

void Neuron::setResetPotential(MembranePotential potential) {
    try {
        if (!Validation::isValidMembranePotential(potential)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid reset potential: " + std::to_string(potential));
        }
        
        pImpl->state.resetPotential = potential;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set reset potential: " + std::string(e.what()));
    }
}

bool Neuron::checkThreshold() const {
    try {
        return pImpl->state.membranePotential >= pImpl->state.threshold;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to check threshold: " + std::string(e.what()));
    }
}

float Neuron::getLastSpikeTime() const {
    try {
        return pImpl->state.lastSpikeTime;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get last spike time: " + std::string(e.what()));
    }
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    try {
        if (!Validation::isValidMembranePotential(amplitude)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid excitatory input amplitude: " + std::to_string(amplitude));
        }
        
        pImpl->synapticInput += amplitude;
        
        // Clamp to prevent instability
        if (pImpl->synapticInput > 100.0f) {
            pImpl->synapticInput = 100.0f;
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to receive excitatory input: " + std::string(e.what()));
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    try {
        if (!Validation::isValidMembranePotential(amplitude)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid inhibitory input amplitude: " + std::to_string(amplitude));
        }
        
        pImpl->synapticInput -= amplitude;
        
        // Clamp to prevent instability
        if (pImpl->synapticInput < -100.0f) {
            pImpl->synapticInput = -100.0f;
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to receive inhibitory input: " + std::string(e.what()));
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    try {
        if (!Validation::isValidMembranePotential(amplitude)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid modulatory input amplitude: " + std::to_string(amplitude));
        }
        
        pImpl->state.adaptationVariable += amplitude * 0.1f;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to receive modulatory input: " + std::string(e.what()));
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    try {
        if (!Validation::isValidMembranePotential(current)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid injected current: " + std::to_string(current));
        }
        
        pImpl->synapticInput += current;
        
        // Clamp to prevent instability
        if (pImpl->synapticInput > 100.0f) {
            pImpl->synapticInput = 100.0f;
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to inject current: " + std::string(e.what()));
    }
}

void Neuron::clearTotalCurrent() {
    try {
        pImpl->synapticInput = 0.0f;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to clear total current: " + std::string(e.what()));
    }
}

void Neuron::recordSpike(Timestamp timestamp) {
    try {
        if (!Validation::isValidTimestamp(timestamp)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid spike timestamp: " + std::to_string(timestamp));
        }
        
        pImpl->spikeHistory.push_back(timestamp);
        if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to record spike: " + std::string(e.what()));
    }
}

void Neuron::clearSpikeHistory() {
    try {
        pImpl->spikeHistory.clear();
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to clear spike history: " + std::string(e.what()));
    }
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    try {
        pImpl->incomingSynapses.push_back(handle);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to add incoming synapse: " + std::string(e.what()));
    }
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    try {
        pImpl->outgoingSynapses.push_back(handle);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to add outgoing synapse: " + std::string(e.what()));
    }
}

NeuronState& Neuron::getState() {
    try {
        return pImpl->state;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get neuron state: " + std::string(e.what()));
    }
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    try {
        return pImpl->plasticityFlags;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get plasticity flags: " + std::string(e.what()));
    }
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    try {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to enable plasticity: " + std::string(e.what()));
    }
}

void Neuron::setRegionId(RegionId region) {
    try {
        pImpl->regionId = region;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set region ID: " + std::string(e.what()));
    }
}

void Neuron::setPopulationId(PopulationId population) {
    try {
        pImpl->populationId = population;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set population ID: " + std::string(e.what()));
    }
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    try {
        // Validate input parameters
        if (!Validation::isValidTimestamp(currentTime)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid current time: " + std::to_string(currentTime));
        }
        if (!Validation::isValidTimestepDuration(dt)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid timestep duration: " + std::to_string(dt));
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
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to step LIF neuron: " + std::string(e.what()));
    }
}

void Neuron::step(Timestamp currentTime) {
    try {
        // Default LIF step with standard timestep (1ms)
        TimestepDuration dt = 0.001;  // 1ms default
        stepLIF(currentTime, dt);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to step neuron: " + std::string(e.what()));
    }
}

void Neuron::reset() {
    try {
        pImpl->state = NeuronState();
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to reset neuron: " + std::string(e.what()));
    }
}

void Neuron::initializeRandom(RandomGenerator& rng) {
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
        
        // Initial state
        pImpl->state.firingState = FiringState::Resting;
        pImpl->state.refractoryRemaining = 0;
        pImpl->state.adaptationVariable = 0.0f;
        pImpl->state.lastSpikeTime = -1.0f;
        
        // Clear any residual state
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to initialize neuron randomly: " + std::string(e.what()));
    }
}

} // namespace nlm
