#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

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

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Config/Config.hpp"
#include <cmath>
#include <algorithm>
#include <limits>
#include <random>

namespace nlm {

// Numerical integration helper functions
namespace {
    
    // LIF neuron dynamics function for numerical integration
    // dV/dt = (V_rest - V)/tau + I/C
    float computeDerivative(float V, float V_rest, float tau, float synapticContribution, float C) {
        if (tau <= 0.0f) return 0.0f;
        if (C <= 0.0f) return 0.0f;
        
        float leakTerm = (V_rest - V) / tau;
        float currentTerm = synapticContribution / C;
        return leakTerm + currentTerm;
    }
    
    // Runge-Kutta 4th order integration for LIF neuron
    bool stepRK4Impl(Neuron::Impl* impl, Timestamp currentTime, TimestepDuration dt) {
        bool fired = false;
        
        // Handle refractory period
        if (impl->state.refractoryRemaining > 0) {
            --impl->state.refractoryRemaining;
            impl->synapticInput = 0.0f;
            if (impl->state.refractoryRemaining == 0) {
                impl->state.firingState = FiringState::Resting;
            }
            return false;
        }
        
        float V_rest = impl->state.restingPotential;
        float V_reset = impl->state.resetPotential;
        float threshold = impl->state.threshold;
        float tau = Impl::TIME_CONSTANT;
        float C = Impl::MEMBRANE_CAPACITANCE;
        
        // RK4 integration
        float V = impl->state.membranePotential;
        
        // Calculate k1
        float k1 = computeDerivative(V, V_rest, tau, impl->synapticInput / C, C);
        
        // Calculate k2
        float V2 = V + 0.5f * dt * 1000.0f * k1;
        float k2 = computeDerivative(V2, V_rest, tau, impl->synapticInput / C, C);
        
        // Calculate k3
        float V3 = V + 0.5f * dt * 1000.0f * k2;
        float k3 = computeDerivative(V3, V_rest, tau, impl->synapticInput / C, C);
        
        // Calculate k4
        float V4 = V + dt * 1000.0f * k3;
        float k4 = computeDerivative(V4, V_rest, tau, impl->synapticInput / C, C);
        
        // Update membrane potential
        V = V + (dt * 1000.0f / 6.0f) * (k1 + 2.0f*k2 + 2.0f*k3 + k4);
        
        // Apply spike-frequency adaptation
        if (impl->state.adaptationVariable > 0.0f) {
            V -= impl->state.adaptationVariable * 0.01f;
            impl->state.adaptationVariable *= 0.95f;
        }
        
        // Clamp membrane potential
        V = std::clamp(V, -100.0f, 50.0f);
        impl->state.membranePotential = V;
        
        // Check for spike
        if (V >= threshold) {
            fired = true;
            impl->state.firingState = FiringState::Active;
            impl->state.lastSpikeTime = static_cast<float>(currentTime);
            
            impl->state.spikeHistory.push_back(currentTime);
            if (impl->state.spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
                impl->state.spikeHistory.erase(impl->state.spikeHistory.begin());
            }
            
            // Reset membrane potential
            impl->state.membranePotential = V_reset;
            
            // Enter refractory period
            impl->state.refractoryRemaining = impl->state.refractoryPeriod;
            impl->state.firingState = FiringState::Refractory;
            
            // Update adaptation for spike-frequency adaptation
            impl->state.adaptationVariable += 1.0f;
        } else {
            impl->state.firingState = FiringState::Active;
        }
        
        // Clear synaptic input
        impl->synapticInput = 0.0f;
        
        return fired;
    }
    
    // Basic Euler integration for LIF neuron
    bool stepEulerImpl(Neuron::Impl* impl, Timestamp currentTime, TimestepDuration dt) {
        bool fired = false;
        
        // Handle refractory period
        if (impl->state.refractoryRemaining > 0) {
            --impl->state.refractoryRemaining;
            impl->synapticInput = 0.0f;
            if (impl->state.refractoryRemaining == 0) {
                impl->state.firingState = FiringState::Resting;
            }
            return false;
        }
        
        float V_rest = impl->state.restingPotential;
        float V_reset = impl->state.resetPotential;
        float threshold = impl->state.threshold;
        float tau = Impl::TIME_CONSTANT;
        float C = Impl::MEMBRANE_CAPACITANCE;
        
        // Simple Euler integration
        float V = impl->state.membranePotential;
        
        float synapticContribution = impl->synapticInput / C;
        float leakContribution = (V_rest - V) / tau;
        
        V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
        
        if (impl->state.adaptationVariable > 0.0f) {
            V -= impl->state.adaptationVariable * 0.01f;
            impl->state.adaptationVariable *= 0.95f;
        }
        
        V = std::clamp(V, -100.0f, 50.0f);
        impl->state.membranePotential = V;
        
        // Check for spike
        if (V >= threshold) {
            fired = true;
            impl->state.firingState = FiringState::Active;
            impl->state.lastSpikeTime = static_cast<float>(currentTime);
            
            impl->state.spikeHistory.push_back(currentTime);
            if (impl->state.spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
                impl->state.spikeHistory.erase(impl->state.spikeHistory.begin());
            }
            
            impl->state.membranePotential = V_reset;
            impl->state.refractoryRemaining = impl->state.refractoryPeriod;
            impl->state.firingState = FiringState::Refractory;
            impl->state.adaptationVariable += 1.0f;
        } else {
            impl->state.firingState = FiringState::Active;
        }
        
        impl->synapticInput = 0.0f;
        
        return fired;
    }
    
    // Exponential Euler integration for LIF neuron (current implementation)
    bool stepExponentialEulerImpl(Neuron::Impl* impl, Timestamp currentTime, TimestepDuration dt) {
        bool fired = false;
        
        // Handle refractory period
        if (impl->state.refractoryRemaining > 0) {
            --impl->state.refractoryRemaining;
            impl->synapticInput = 0.0f;
            if (impl->state.refractoryRemaining == 0) {
                impl->state.firingState = FiringState::Resting;
            }
            return false;
        }
        
        float V_rest = impl->state.restingPotential;
        float V_reset = impl->state.resetPotential;
        float threshold = impl->state.threshold;
        float tau = Impl::TIME_CONSTANT;
        float C = Impl::MEMBRANE_CAPACITANCE;
        
        // Synaptic input contributes to membrane potential change
        float synapticContribution = impl->synapticInput / C;
        
        // Leak contribution
        float leakContribution = (V_rest - impl->state.membranePotential) / tau;
        
        // Update membrane potential using exponential Euler integration
        impl->state.membranePotential = 
            impl->state.membranePotential + 
            static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
        
        // Apply spike-frequency adaptation (slow hyperpolarization after spike)
        if (impl->state.adaptationVariable > 0.0f) {
            impl->state.membranePotential -= impl->state.adaptationVariable * 0.01f;
            impl->state.adaptationVariable *= 0.95f;  // Decay adaptation
        }
        
        // Clamp membrane potential to prevent instability
        impl->state.membranePotential = std::clamp(
            impl->state.membranePotential, -100.0f, 50.0f
        );
        
        // Check for spike
        if (impl->state.membranePotential >= threshold) {
            fired = true;
            impl->state.firingState = FiringState::Active;
            impl->state.lastSpikeTime = static_cast<float>(currentTime);
            
            // Record spike
            impl->state.spikeHistory.push_back(currentTime);
            if (impl->state.spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
                impl->state.spikeHistory.erase(impl->state.spikeHistory.begin());
            }
            
            // Reset membrane potential
            impl->state.membranePotential = V_reset;
            
            // Enter refractory period
            impl->state.refractoryRemaining = impl->state.refractoryPeriod;
            impl->state.firingState = FiringState::Refractory;
            
            // Update adaptation for spike-frequency adaptation
            impl->state.adaptationVariable += 1.0f;
        } else {
            impl->state.firingState = FiringState::Active;
        }
        
        // Clear synaptic input for next step
        impl->synapticInput = 0.0f;
        
        return fired;
    }
    
}} // namespace nlm

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->synapticInput = 0.0f;
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
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
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
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
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
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    return stepExponentialEulerImpl(pImpl.get(), currentTime, dt);
}

// Runge-Kutta 4th order integration
bool Neuron::stepRK4(Timestamp currentTime, TimestepDuration dt) {
    return stepRK4Impl(pImpl.get(), currentTime, dt);
}

// Basic Euler integration
bool Neuron::stepEuler(Timestamp currentTime, TimestepDuration dt) {
    return stepEulerImpl(pImpl.get(), currentTime, dt);
}

// Exponential Euler integration (current implementation)
bool Neuron::stepExponentialEuler(Timestamp currentTime, TimestepDuration dt) {
    return stepExponentialEulerImpl(pImpl.get(), currentTime, dt);
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
