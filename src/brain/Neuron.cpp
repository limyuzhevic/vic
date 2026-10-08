#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

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
             totalCurrent(0.0f), synapticInput(0.0f) {
        // Initialize state with biologically plausible defaults
        state.membranePotential = -70.0f;  // Resting potential
        state.restingPotential = -70.0f;
        state.threshold = -55.0f;
        state.resetPotential = -70.0f;
        state.leakConductance = 10.0f;
        state.synapseConductance = 0.0f;
        state.firingRate = 0.0f;
        state.firingState = FiringState::Resting;
        state.refractoryRemaining = 0;
        state.refractoryPeriod = 5;
        state.adaptationVariable = 0.0f;
        state.lastSpikeTime = -1.0f;
    }
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl()) {
    if (!pImpl) {
        NLM_LOG_ERROR("Failed to allocate memory for neuron");
        return;
    }
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
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

void Neuron::setType(NeuronType type) {
    if (pImpl) {
        pImpl->type = type;
    }
}

void Neuron::setMembranePotential(MembranePotential potential) {
    if (pImpl) {
        // Validate membrane potential is within reasonable bounds
        if (potential < -100.0f || potential > 50.0f) {
            NLM_LOG_WARNING("Unusual membrane potential: " + std::to_string(potential));
        }
        pImpl->state.membranePotential = potential;
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    if (pImpl) {
        pImpl->state.membranePotential += delta;
    }
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (pImpl) {
        // Validate threshold is reasonable
        if (threshold < -100.0f || threshold > 0.0f) {
            NLM_LOG_WARNING("Unusual threshold: " + std::to_string(threshold));
        }
        pImpl->state.threshold = threshold;
    }
}

void Neuron::setFiringState(FiringState state) {
    if (pImpl) {
        pImpl->state.firingState = state;
    }
}

void Neuron::decrementRefractory() {
    if (pImpl) {
        if (pImpl->state.refractoryRemaining > 0) {
            --pImpl->state.refractoryRemaining;
            if (pImpl->state.refractoryRemaining == 0) {
                pImpl->state.firingState = FiringState::Resting;
            }
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    if (pImpl) {
        if (rate < 0.0f || rate > 1000.0f) {
            NLM_LOG_WARNING("Unusual firing rate: " + std::to_string(rate));
        }
        pImpl->state.firingRate = rate;
    }
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (pImpl) {
        if (conductance < 0.0f || conductance > 100.0f) {
            NLM_LOG_WARNING("Unusual leak conductance: " + std::to_string(conductance));
        }
        pImpl->state.leakConductance = conductance;
    }
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl ? pImpl->state.leakConductance : 0.0f;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (pImpl) {
        if (steps > 1000) {
            NLM_LOG_WARNING("Unusually long refractory period: " + std::to_string(steps) + " steps");
        }
        pImpl->state.refractoryPeriod = steps;
        // Cap remaining refractory period if it's currently set
        if (pImpl->state.refractoryRemaining > steps) {
            pImpl->state.refractoryRemaining = steps;
        }
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl ? pImpl->state.refractoryPeriod : 0;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (pImpl) {
        if (potential < -100.0f || potential > 0.0f) {
            NLM_LOG_WARNING("Unusual resting potential: " + std::to_string(potential));
        }
        pImpl->state.restingPotential = potential;
    }
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl ? pImpl->state.restingPotential : 0.0f;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (pImpl) {
        pImpl->state.resetPotential = potential;
    }
}

bool Neuron::checkThreshold() const {
    return pImpl && pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl ? pImpl->state.lastSpikeTime : -1.0f;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (pImpl) {
        if (amplitude < 0.0f) {
            NLM_LOG_WARNING("Negative excitatory input amplitude: " + std::to_string(amplitude));
        }
        pImpl->synapticInput += amplitude;
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (pImpl) {
        if (amplitude > 0.0f) {
            NLM_LOG_WARNING("Positive inhibitory input amplitude: " + std::to_string(amplitude));
        }
        pImpl->synapticInput -= amplitude;
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    if (pImpl) {
        pImpl->state.adaptationVariable += amplitude * 0.1f;
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    if (pImpl) {
        if (std::abs(current) > 1000.0f) {
            NLM_LOG_WARNING("Large current injection: " + std::to_string(current));
        }
        pImpl->synapticInput += current;
    }
}

void Neuron::clearTotalCurrent() {
    if (pImpl) {
        pImpl->synapticInput = 0.0f;
    }
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (pImpl) {
        pImpl->spikeHistory.push_back(timestamp);
        if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
    }
}

void Neuron::clearSpikeHistory() {
    if (pImpl) {
        pImpl->spikeHistory.clear();
    }
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (pImpl) {
        pImpl->incomingSynapses.push_back(handle);
    }
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (pImpl) {
        pImpl->outgoingSynapses.push_back(handle);
    }
}

NeuronState& Neuron::getState() {
    static NeuronState emptyState;
    return pImpl ? pImpl->state : emptyState;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    static PlasticityFlags emptyFlags;
    return pImpl ? pImpl->plasticityFlags : emptyFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (pImpl) {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
    }
}

void Neuron::setRegionId(RegionId region) {
    if (pImpl) {
        pImpl->regionId = region;
    }
}

void Neuron::setPopulationId(PopulationId population) {
    if (pImpl) {
        pImpl->populationId = population;
    }
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    if (!pImpl) return false;
    
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
    if (pImpl) {
        pImpl->state = NeuronState();
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
    }
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) return;
    
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
