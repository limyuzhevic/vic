#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticConductance;  // Total synaptic conductance input
    MembranePotential reversalPotential;     // Mean reversal potential for weighted integration
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // Hodgkin-Huxley parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float RESTING_POTENTIAL = -70.0f;  // mV
    static constexpr float SYNAPTIC_CONDUCTANCE = 0.0f;  // nS
    
    // Sodium channel parameters
    static constexpr float GNa = 120.0f;  // mS/cm^2 (conductance density)
    static constexpr float ENa = 55.0f;   // mV, reversal potential
    static constexpr float VNa = 55.0f;   // mV, sodium reversal potential
    
    // Potassium channel parameters
    static constexpr float GK = 36.0f;    // mS/cm^2 (conductance density)
    static constexpr float EK = -77.0f;   // mV, reversal potential
    static constexpr float VK = -77.0f;   // mV, potassium reversal potential
    
    // Leak channel parameters
    static constexpr float GL = 0.3f;     // mS/cm^2
    static constexpr float EL = -54.387f; // mV
    
    // Calcium dynamics parameters
    static constexpr float PCa = 0.0001f; // Calcium permeability
    static constexpr float ECa = 140.0f;  // mV, calcium reversal potential
    static constexpr float tauCa = 1000.0f; // ms, calcium time constant
    static constexpr float Ca0 = 0.0001f;  // mM, resting calcium concentration
    
    // Calcium-dependent potassium channel
    static constexpr float gKCaMax = 10.0f;  // nS, maximum conductance
    static constexpr float KdCa = 0.5f;      // mM, half-saturation constant
    static constexpr float tauKCa = 100.0f;  // ms, KCa time constant
    
    // Stochasticity parameters
    static constexpr float CHANNEL_NOISE_AMPLITUDE = 0.05f;  // dimensionless
    
    // Adaptive parameters
    static constexpr float ADAPTATION_CURRENT = 1.0f;  // nA
    static constexpr float ADAPTATION_TIME_CONST = 100.0f;  // ms
    
    // Time constants (for gating variables)
    static constexpr float TAU_M = 0.1f;   // ms, sodium activation
    static constexpr float TAU_H = 1.0f;   // ms, sodium inactivation
    static constexpr float TAU_N = 10.0f;  // ms, potassium activation
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticConductance(0.0f), reversalPotential(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
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

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

namespace nlm {

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticConductance;  // Total synaptic conductance input
    MembranePotential reversalPotential;     // Mean reversal potential for weighted integration
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // Hodgkin-Huxley parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float RESTING_POTENTIAL = -70.0f;  // mV
    static constexpr float SYNAPTIC_CONDUCTANCE = 0.0f;  // nS
    
    // Sodium channel parameters
    static constexpr float GNa = 120.0f;  // mS/cm^2 (conductance density)
    static constexpr float ENa = 55.0f;   // mV, reversal potential
    static constexpr float VNa = 55.0f;   // mV, sodium reversal potential
    
    // Potassium channel parameters
    static constexpr float GK = 36.0f;    // mS/cm^2 (conductance density)
    static constexpr float EK = -77.0f;   // mV, reversal potential
    static constexpr float VK = -77.0f;   // mV, potassium reversal potential
    
    // Leak channel parameters
    static constexpr float GL = 0.3f;     // mS/cm^2
    static constexpr float EL = -54.387f; // mV
    
    // Calcium dynamics parameters
    static constexpr float PCa = 0.0001f; // Calcium permeability
    static constexpr float ECa = 140.0f;  // mV, calcium reversal potential
    static constexpr float tauCa = 1000.0f; // ms, calcium time constant
    static constexpr float Ca0 = 0.0001f;  // mM, resting calcium concentration
    
    // Calcium-dependent potassium channel
    static constexpr float gKCaMax = 10.0f;  // nS, maximum conductance
    static constexpr float KdCa = 0.5f;      // mM, half-saturation constant
    static constexpr float tauKCa = 100.0f;  // ms, KCa time constant
    
    // Stochasticity parameters
    static constexpr float CHANNEL_NOISE_AMPLITUDE = 0.05f;  // dimensionless
    
    // Adaptive parameters
    static constexpr float ADAPTATION_CURRENT = 1.0f;  // nA
    static constexpr float ADAPTATION_TIME_CONST = 100.0f;  // ms
    
    // Time constants (for gating variables)
    static constexpr float TAU_M = 0.1f;   // ms, sodium activation
    static constexpr float TAU_H = 1.0f;   // ms, sodium inactivation
    static constexpr float TAU_N = 10.0f;  // ms, potassium activation
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticConductance(0.0f), reversalPotential(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
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
    pImpl->synapticConductance += amplitude;
    // Also update reversal potential for proper weighted integration
    pImpl->reversalPotential = (pImpl->reversalPotential * (pImpl->synapticConductance - amplitude) + ENa * amplitude) / pImpl->synapticConductance;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total conductance
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticConductance -= amplitude;
    // Update reversal potential
    pImpl->reversalPotential = (pImpl->reversalPotential * (pImpl->synapticConductance + amplitude) - EK * amplitude) / pImpl->synapticConductance;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Convert current to equivalent conductance at resting potential
    // I = g * (V - E), so g = I / (V - E)
    MembranePotential V = pImpl->state.membranePotential;
    MembranePotential E = pImpl->state.restingPotential;
    if (std::abs(V - E) > 1e-6) {
        pImpl->synapticConductance += current / (V - E);
        // Update reversal potential contribution
        pImpl->reversalPotential = (pImpl->reversalPotential * (pImpl->synapticConductance - current / (V - E)) + current) / pImpl->synapticConductance;
    }
}

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->totalCurrent;
}

void Neuron::clearTotalCurrent() {
    pImpl->totalCurrent = 0.0f;
    pImpl->synapticConductance = 0.0f;
    pImpl->reversalPotential = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
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

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
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

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
}

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

// Private HH dynamics implementation
namespace {
    // Alpha and beta functions for Hodgkin-Huxley gating variables
    float alpha_m(float V) {
        return (0.1f * (V + 40.0f)) / (1.0f - std::exp(-(V + 40.0f) / 10.0f));
    }
    
    float beta_m(float V) {
        return 4.0f * std::exp(-(V + 65.0f) / 18.0f);
    }
    
    float alpha_h(float V) {
        return 0.07f * std::exp(-(V + 65.0f) / 20.0f);
    }
    
    float beta_h(float V) {
        return 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f));
    }
    
    float alpha_n(float V) {
        return (0.01f * (V + 55.0f)) / (1.0f - std::exp(-(V + 55.0f) / 10.0f));
    }
    
    float beta_n(float V) {
        return 0.125f * std::exp(-(V + 65.0f) / 80.0f);
    }
    
    float alpha_ca(float V) {
        return 0.01f * (10.0f - V) / (1.0f - std::exp(-(10.0f - V) / 10.0f));
    }
    
    float beta_ca(float V) {
        return 0.001f * std::exp((30.0f - V) / 10.0f);
    }
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Check for critical errors before processing
    if (nlm::ErrorHandler::getInstance().hasCriticalErrors()) {
        NLM_LOG_WARNING("Skipping neuron step due to critical errors");
        return false;
    }
    
    // Validate input parameters
    if (dt <= 0.0) {
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::InvalidParameter,
            "Invalid timestep duration: " + std::to_string(dt),
            __FILE__, __LINE__, "Neuron::stepLIF",
            nlm::LogLevel::Warning,
            pImpl->id.index(), -1, -1,
            static_cast<SimulationStep>(currentTime), currentTime
        );
    }
    
    if (currentTime < 0.0) {
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::InvalidParameter,
            "Negative current time: " + std::to_string(currentTime),
            __FILE__, __LINE__, "Neuron::stepLIF",
            nlm::LogLevel::Warning,
            pImpl->id.index(), -1, -1,
            static_cast<SimulationStep>(currentTime), currentTime
        );
    }
    
    // Handle refractory period with error checking
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticConductance = 0.0f;
        pImpl->reversalPotential = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    try {
        // Hodgkin-Huxley Dynamics
        MembranePotential& V = pImpl->state.membranePotential;
        float dt_ms = static_cast<float>(dt) * 1000.0f;  // Convert to ms
        
        if (dt_ms <= 0.0) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::InvalidParameter,
                "Invalid timestep in ms: " + std::to_string(dt_ms),
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Warning,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            dt_ms = 1.0f; // Safe fallback
        }
        
        // Check for NaN or infinite values in membrane potential
        if (std::isnan(V) || std::isinf(V)) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ComputationOverflow,
                "Invalid membrane potential: " + std::to_string(V),
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Critical,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            // Reset to safe value
            V = pImpl->state.restingPotential;
        }
        
        // 1. Update gating variables with stochasticity
        float V_mV = V;  // Already in mV
        
        // Calculate alpha and beta functions with error checking
        float alpha_m = 0.0f, beta_m = 0.0f, alpha_h = 0.0f, beta_h = 0.0f, 
              alpha_n = 0.0f, beta_n = 0.0f, alpha_ca = 0.0f, beta_ca = 0.0f;
        
        try {
            alpha_m = alpha_m(V_mV);
            beta_m = beta_m(V_mV);
            alpha_h = alpha_h(V_mV);
            beta_h = beta_h(V_mV);
            alpha_n = alpha_n(V_mV);
            beta_n = beta_n(V_mV);
            alpha_ca = alpha_ca(V_mV);
            beta_ca = beta_ca(V_mV);
        } catch (...) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ComputationOverflow,
                "Exception calculating gating variable functions",
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Error,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            // Use safe defaults
            alpha_m = 0.1f; beta_m = 4.0f; alpha_h = 0.07f; beta_h = 1.0f;
            alpha_n = 0.01f; beta_n = 0.125f; alpha_ca = 0.01f; beta_ca = 0.001f;
        }
        
        // Avoid division by zero
        float m_sum = alpha_m + beta_m;
        float h_sum = alpha_h + beta_h;
        float n_sum = alpha_n + beta_n;
        float ca_sum = alpha_ca + beta_ca;
        
        if (std::abs(m_sum) < 1e-6f) m_sum = 1e-6f;
        if (std::abs(h_sum) < 1e-6f) h_sum = 1e-6f;
        if (std::abs(n_sum) < 1e-6f) n_sum = 1e-6f;
        if (std::abs(ca_sum) < 1e-6f) ca_sum = 1e-6f;
        
        // Sodium activation (m)
        float m_inf = alpha_m / m_sum;
        float tau_m = 1.0f / m_sum;
        
        // Validate tau_m to prevent instability
        if (std::isnan(tau_m) || std::isinf(tau_m) || tau_m > 100.0f) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ComputationOverflow,
                "Invalid sodium time constant: " + std::to_string(tau_m),
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Warning,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            tau_m = 1.0f; // Safe fallback
        }
        
        pImpl->state.m += dt_ms * ((m_inf - pImpl->state.m) / tau_m) + 
                        std::normal_distribution<float>(0.0f, Impl::CHANNEL_NOISE_AMPLITUDE)();
        pImpl->state.m = std::clamp(pImpl->state.m, 0.0f, 1.0f);
        
        // Sodium inactivation (h)
        float h_inf = alpha_h / h_sum;
        float tau_h = 1.0f / h_sum;
        
        if (std::isnan(tau_h) || std::isinf(tau_h) || tau_h > 100.0f) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ComputationOverflow,
                "Invalid sodium inactivation time constant: " + std::to_string(tau_h),
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Warning,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            tau_h = 1.0f;
        }
        
        pImpl->state.h += dt_ms * ((h_inf - pImpl->state.h) / tau_h) +
                        std::normal_distribution<float>(0.0f, Impl::CHANNEL_NOISE_AMPLITUDE)();
        pImpl->state.h = std::clamp(pImpl->state.h, 0.0f, 1.0f);
        
        // Potassium activation (n)
        float n_inf = alpha_n / n_sum;
        float tau_n = 1.0f / n_sum;
        
        if (std::isnan(tau_n) || std::isinf(tau_n) || tau_n > 100.0f) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ComputationOverflow,
                "Invalid potassium time constant: " + std::to_string(tau_n),
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Warning,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            tau_n = 1.0f;
        }
        
        pImpl->state.n += dt_ms * ((n_inf - pImpl->state.n) / tau_n) +
                        std::normal_distribution<float>(0.0f, Impl::CHANNEL_NOISE_AMPLITUDE)();
        pImpl->state.n = std::clamp(pImpl->state.n, 0.0f, 1.0f);
        
        // Calcium dynamics
        float Ca_inf = alpha_ca / ca_sum;
        float tau_ca = 1.0f / ca_sum;
        
        if (std::isnan(tau_ca) || std::isinf(tau_ca) || tau_ca > 1000.0f) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ComputationOverflow,
                "Invalid calcium time constant: " + std::to_string(tau_ca),
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Warning,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            tau_ca = 1000.0f;
        }
        
        pImpl->state.caConcentration += dt_ms * ((Ca_inf - pImpl->state.caConcentration) / tau_ca);
        pImpl->state.caConcentration = std::clamp(pImpl->state.caConcentration, 0.0f, 1.0f);
        
        // Calcium-dependent potassium channel conductance
        float gkCa_inf = Impl::gKCaMax * (pImpl->state.caConcentration * pImpl->state.caConcentration) / 
                         ((Impl::KdCa * Impl::KdCa) + pImpl->state.caConcentration * pImpl->state.caConcentration);
        pImpl->state.gkCa += dt_ms * ((gkCa_inf - pImpl->state.gkCa) / Impl::tauKCa);
        
        // Check for numerical instability
        if (std::isnan(pImpl->state.gkCa) || std::isinf(pImpl->state.gkCa)) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ComputationOverflow,
                "Invalid calcium-dependent potassium conductance",
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Warning,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            pImpl->state.gkCa = 0.0f;
        }
        
        // 2. Calculate ion currents
        float I_Na = pImpl->state.m * pImpl->state.m * pImpl->state.m * pImpl->state.h * 
                     Impl::GNa * (V - Impl::ENa);  // Sodium current
        
        float I_K = pImpl->state.n * pImpl->state.n * pImpl->state.n * 
                     Impl::GK * (V - Impl::EK);  // Potassium current
        
        float I_leak = Impl::GL * (V - Impl::EL);  // Leak current
        
        float I_Ca = Impl::PCa * (V - Impl::ECa);  // Calcium current
        
        float I_KCa = pImpl->state.gkCa * (V - Impl::EK);  // Calcium-dependent potassium current
        
        // Check for NaN/inf in currents
        if (std::isnan(I_Na) || std::isinf(I_Na)) I_Na = 0.0f;
        if (std::isnan(I_K) || std::isinf(I_K)) I_K = 0.0f;
        if (std::isnan(I_leak) || std::isinf(I_leak)) I_leak = 0.0f;
        if (std::isnan(I_Ca) || std::isinf(I_Ca)) I_Ca = 0.0f;
        if (std::isnan(I_KCa) || std::isinf(I_KCa)) I_KCa = 0.0f;
        
        // 3. Synaptic currents (conductance-based)
        float I_syn = 0.0f;
        if (pImpl->synapticConductance > 0.0f && std::abs(pImpl->reversalPotential) > 1e-6f) {
            I_syn = pImpl->synapticConductance * (V - pImpl->reversalPotential);
        }
        
        // Check for NaN/inf in synaptic current
        if (std::isnan(I_syn) || std::isinf(I_syn)) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ComputationOverflow,
                "Invalid synaptic current",
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Warning,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            I_syn = 0.0f;
        }
        
        // 4. Adaptation current (calcium-dependent)
        float I_adapt = pImpl->state.adaptationVariable * Impl::ADAPTATION_CURRENT;
        
        // Check for NaN/inf in adaptation current
        if (std::isnan(I_adapt) || std::isinf(I_adapt)) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ComputationOverflow,
                "Invalid adaptation current",
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Warning,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            I_adapt = 0.0f;
        }
        
        // 5. Total membrane current
        float I_total = I_Na + I_K + I_leak + I_Ca + I_KCa + I_syn - I_adapt;
        
        // Check for NaN/inf in total current
        if (std::isnan(I_total) || std::isinf(I_total)) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ComputationOverflow,
                "Invalid total membrane current",
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Warning,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            I_total = 0.0f;
        }
        
        // 6. Update membrane potential (Ohm's law: I = C * dV/dt)
        pImpl->totalCurrent = I_total;
        V += dt_ms * (I_total / Impl::MEMBRANE_CAPACITANCE);
        
        // Check for numerical instability in potential update
        if (std::isnan(V) || std::isinf(V)) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ComputationOverflow,
                "NaN or infinite membrane potential after update",
                __FILE__, __LINE__, "Neuron::stepLIF",
                nlm::LogLevel::Critical,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            V = pImpl->state.restingPotential; // Emergency reset
        }
        
        // Clamp membrane potential to prevent instability
        V = std::clamp(V, -100.0f, 50.0f);
        
        // 7. Check for spike initiation
        if (V >= pImpl->state.threshold) {
            fired = true;
            pImpl->state.firingState = FiringState::Active;
            pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
            
            // Record spike
            recordSpike(currentTime);
            
            // Realistic spike reset: integrate-and-fire with adaptation
            V = pImpl->state.resetPotential;
            
            // Enter refractory period
            pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
            pImpl->state.firingState = FiringState::Refractory;
            
            // Update adaptation for spike-frequency adaptation
            pImpl->state.adaptationVariable += Impl::ADAPTATION_CURRENT;
            
            // Calcium buildup during spike
            pImpl->state.caConcentration += 0.1f;
            
            // Check for spike-related errors
            if (pImpl->state.caConcentration > 10.0f) {
                nlm::ErrorHandler::getInstance().logWarning(
                    nlm::ErrorCode::ComputationOverflow,
                    "High calcium concentration during spike: " + std::to_string(pImpl->state.caConcentration),
                    __FILE__, __LINE__, "Neuron::stepLIF",
                    nlm::LogLevel::Warning,
                    pImpl->id.index(), -1, -1,
                    static_cast<SimulationStep>(currentTime), currentTime
                );
                pImpl->state.caConcentration = 10.0f; // Clamp to prevent runaway
            }
        } else {
            pImpl->state.firingState = FiringState::Active;
        }
        
        // 8. Update firing rate (exponential decay of spike history)
        if (pImpl->state.lastSpikeTime > 0.0f) {
            float timeSinceLastSpike = static_cast<float>(currentTime) - pImpl->state.lastSpikeTime;
            if (timeSinceLastSpike > 0.0f) {
                pImpl->state.firingRate = 1000.0f / timeSinceLastSpike;  // Convert to Hz
                
                if (std::isnan(pImpl->state.firingRate) || std::isinf(pImpl->state.firingRate)) {
                    pImpl->state.firingRate = 0.0f;
                }
            }
        }
        
        // 9. Decay adaptation variable
        pImpl->state.adaptationVariable *= std::exp(-dt_ms / Impl::ADAPTATION_TIME_CONST);
        
        // Check for negative adaptation
        if (pImpl->state.adaptationVariable < 0.0f) {
            pImpl->state.adaptationVariable = 0.0f;
        }
        
        // 10. Clear synaptic state for next step
        pImpl->synapticConductance = 0.0f;
        pImpl->reversalPotential = 0.0f;
        
        return fired;
        
    } catch (const std::exception& e) {
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::ComputationOverflow,
            "Exception during LIF neuron dynamics: " + std::string(e.what()),
            __FILE__, __LINE__, "Neuron::stepLIF",
            nlm::LogLevel::Error,
            pImpl->id.index(), -1, -1,
            static_cast<SimulationStep>(currentTime), currentTime
        );
        
        // Reset neuron to safe state
        pImpl->state = NeuronState();
        pImpl->synapticConductance = 0.0f;
        pImpl->reversalPotential = 0.0f;
        
        return false;
    } catch (...) {
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::Unknown,
            "Unknown exception during LIF neuron dynamics",
            __FILE__, __LINE__, "Neuron::stepLIF",
            nlm::LogLevel::Critical,
            pImpl->id.index(), -1, -1,
            static_cast<SimulationStep>(currentTime), currentTime
        );
        
        // Emergency reset
        pImpl->state = NeuronState();
        pImpl->synapticConductance = 0.0f;
        pImpl->reversalPotential = 0.0f;
        
        return false;
    }
}

void Neuron::step(Timestamp currentTime) {
    // Default HH step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    
    try {
        // Validate timestep
        if (dt <= 0.0) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::InvalidParameter,
                "Invalid timestep in Neuron::step: " + std::to_string(dt),
                __FILE__, __LINE__, "Neuron::step",
                nlm::LogLevel::Warning,
                pImpl->id.index(), -1, -1,
                static_cast<SimulationStep>(currentTime), currentTime
            );
            dt = 0.001; // Safe fallback
        }
        
        stepLIF(currentTime, dt);
    } catch (const std::exception& e) {
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::ComputationOverflow,
            "Exception during neuron step: " + std::string(e.what()),
            __FILE__, __LINE__, "Neuron::step",
            nlm::LogLevel::Error,
            pImpl->id.index(), -1, -1,
            static_cast<SimulationStep>(currentTime), currentTime
        );
        
        // Reset neuron to safe state
        reset();
    } catch (...) {
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::Unknown,
            "Unknown exception during neuron step",
            __FILE__, __LINE__, "Neuron::step",
            nlm::LogLevel::Critical,
            pImpl->id.index(), -1, -1,
            static_cast<SimulationStep>(currentTime), currentTime
        );
        
        // Emergency reset
        reset();
    }
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticConductance = 0.0f;
    pImpl->reversalPotential = 0.0f;
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
    
    // Initialize HH gating variables to steady-state values
    pImpl->state.m = alpha_m(pImpl->state.restingPotential) / (alpha_m(pImpl->state.restingPotential) + beta_m(pImpl->state.restingPotential));
    pImpl->state.h = alpha_h(pImpl->state.restingPotential) / (alpha_h(pImpl->state.restingPotential) + beta_h(pImpl->state.restingPotential));
    pImpl->state.n = alpha_n(pImpl->state.restingPotential) / (alpha_n(pImpl->state.restingPotential) + beta_n(pImpl->state.restingPotential));
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    pImpl->state.caConcentration = Impl::Ca0;
    pImpl->state.gkCa = 0.0f;
    
    // Clear any residual state
    pImpl->synapticConductance = 0.0f;
    pImpl->reversalPotential = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
