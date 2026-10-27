#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <xmmintrin.h>  // SSE (32-bit float)
#include <smmintrin.h>   // SSSE3 (additional SSE operations)
#include <pmmintrin.h>   // SSE4.1 (additional SIMD instructions)

// SSE intrinsics for neuron processing
namespace SIMD {
    
    // Process 4 neurons in parallel using SSE
    class NeuronVectorProcessor {
    public:
        struct NeuronData {
            __m128 membranePotential;      // 4 floats
            __m128 threshold;             // 4 floats  
            __m128 restingPotential;      // 4 floats
            __m128 resetPotential;        // 4 floats
            __m128 leakyConductance;      // 4 floats
            __m128 adaptationVariable;    // 4 floats
            __m128 refractoryRemaining;   // 4 uint32_t (as float)
            __m128 lastSpikeTime;         // 4 timestamps (as float)
            __m128 synapticInput;         // 4 floats
            __m128 firingState;           // 4 floats (0.0=resting, 1.0=firing, 2.0=refractory)
            __m128 spikeMask;             // 4 flags for spike detection
        };
        
        static void process4Neurons(const NeuronData& neurons, NeuronData& results, 
                                   float timestep, float tau, float C) {
            // LIF dynamics for 4 neurons in parallel
            // V_new = V + dt * ((V_rest - V)/tau + I/C)
            
            // Load synaptic input
            __m128 I = neurons.synapticInput;
            
            // Calculate synaptic contribution: I/C
            __m128 synapticContribution = _mm_div_ps(I, _mm_set1_ps(C));
            
            // Calculate leak contribution: (V_rest - V)/tau
            __m128 V = neurons.membranePotential;
            __m128 V_rest = neurons.restingPotential;
            __m128 leakContribution = _mm_div_ps(_mm_sub_ps(V_rest, V), _mm_set1_ps(tau));
            
            // Total change: dt * (leakContribution + synapticContribution)
            __m128 dt = _mm_set1_ps(timestep * 1000.0f);  // Convert to ms
            __m128 totalChange = _mm_mul_ps(dt, _mm_add_ps(leakContribution, synapticContribution));
            
            // Calculate new membrane potential
            results.membranePotential = _mm_add_ps(V, totalChange);
            
            // Apply spike-frequency adaptation (slow hyperpolarization after spike)
            __m128 adaptationFactor = neurons.adaptationVariable;
            adaptationFactor = _mm_mul_ps(adaptationFactor, _mm_set1_ps(0.01f));
            results.membranePotential = _mm_sub_ps(results.membranePotential, adaptationFactor);
            
            // Decay adaptation variable
            results.adaptationVariable = _mm_mul_ps(adaptationFactor, _mm_set1_ps(0.95f));
            
            // Clamp membrane potential to prevent instability
            results.membranePotential = clamp4(results.membranePotential, -100.0f, 50.0f);
            
            // Check for spikes: V >= threshold
            __m128 threshold = neurons.threshold;
            results.spikeMask = _mm_cmpge_ps(results.membranePotential, threshold);
            
            // Store spike flags in firing state (1.0 = spike, 0.0 = no spike)
            results.firingState = _mm_and_ps(results.spikeMask, _mm_set1_ps(1.0f));
        }
        
        static __m128 clamp4(__m128 value, float minVal, float maxVal) {
            __m128 min = _mm_set1_ps(minVal);
            __m128 max = _mm_set1_ps(maxVal);
            value = _mm_max_ps(value, min);
            value = _mm_min_ps(value, max);
            return value;
        }
        
        static void applySpikeReset(NeuronData& neurons, const NeuronData& spikes, 
                                   Timestamp currentTime, float resetPotential) {
            // Apply spike reset: V = V_reset, enter refractory period, record spike
            __m128 mask = spikes.spikeMask;
            
            if (_mm_movemask_ps(mask) == 0) return;  // No spikes
            
            // Reset membrane potential to reset potential
            neurons.membranePotential = _mm_andnot_ps(mask, neurons.membranePotential);
            neurons.membranePotential = _mm_or_ps(
                neurons.membranePotential,
                _mm_and_ps(mask, _mm_set1_ps(resetPotential))
            );
            
            // Set firing state to refractory (2.0)
            neurons.firingState = _mm_andnot_ps(mask, neurons.firingState);
            neurons.firingState = _mm_or_ps(
                neurons.firingState,
                _mm_and_ps(mask, _mm_set1_ps(2.0f))  // Refractory
            );
            
            // Set refractory period
            neurons.refractoryRemaining = _mm_or_ps(
                neurons.refractoryRemaining,
                _mm_and_ps(mask, _mm_set1_ps(10.0f))  // 10ms refractory
            );
            
            // Update spike time
            __m128 spikeTime = _mm_set1_ps(static_cast<float>(currentTime));
            neurons.lastSpikeTime = _mm_or_ps(
                neurons.lastSpikeTime,
                _mm_and_ps(mask, spikeTime)
            );
            
            // Record adaptation for spike-frequency adaptation
            neurons.adaptationVariable = _mm_add_ps(
                neurons.adaptationVariable,
                _mm_and_ps(mask, _mm_set1_ps(1.0f))
            );
        }
        
        static void decayRefractory(NeuronData& neurons, float dt) {
            // Decay refractory counter and enter resting state when done
            __m128 mask = _mm_cmpgt_ps(neurons.refractoryRemaining, _mm_set1_ps(0.0f));
            
            // Decrement refractory remaining
            neurons.refractoryRemaining = _mm_max_ps(
                _mm_sub_ps(neurons.refractoryRemaining, _mm_set1_ps(dt)),
                _mm_set1_ps(0.0f)
            );
            
            // Enter resting state when refractory expires
            neurons.firingState = _mm_andnot_ps(mask, neurons.firingState);
            neurons.firingState = _mm_or_ps(
                neurons.firingState,
                _mm_and_ps(mask, _mm_set1_ps(0.0f))  // Resting
            );
            
            // Clear synaptic input when in refractory
            neurons.synapticInput = _mm_and_ps(neurons.synapticInput, _mm_set1_ps(0.0f));
        }
    };
}

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

void Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // SIMD-optimized version (falls back to scalar for small numbers)
    if (sizeof(NeuronId) % 4 == 0) {  // Use SIMD for 4+ neurons
        SIMD::NeuronVectorProcessor::process4NeuronsOptimized(this, currentTime, dt);
    } else {
        // Fallback to scalar implementation for compatibility
        stepLIFScalar(currentTime, dt);
    }
}

void Neuron::stepLIFScalar(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return;
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
}

void Neuron::stepLIFOptimized(SIMD::NeuronVectorProcessor::NeuronData& neuronData, 
                             Timestamp currentTime, TimestepDuration dt) {
    // Process 4 neurons in SIMD for maximum performance
    float timestep_ms = static_cast<float>(dt) * 1000.0f;
    float tau = Impl::TIME_CONSTANT;
    float C = Impl::MEMBRANE_CAPACITANCE;
    
    // Prepare SIMD registers
    __m128 dt_register = _mm_set1_ps(timestep_ms);
    __m128 tau_register = _mm_set1_ps(tau);
    __m128 C_register = _mm_set1_ps(C);
    
    // Extract neuron data into SIMD registers
    __m128 V = _mm_loadu_ps(&neuronData.membranePotential);
    __m128 V_rest = _mm_loadu_ps(&neuronData.restingPotential);
    __m128 threshold = _mm_loadu_ps(&neuronData.threshold);
    __m128 synapticInput = _mm_loadu_ps(&neuronData.synapticInput);
    
    // Calculate synaptic contribution: I/C
    __m128 synapticContribution = _mm_div_ps(synapticInput, C_register);
    
    // Calculate leak contribution: (V_rest - V)/tau
    __m128 leakContribution = _mm_div_ps(_mm_sub_ps(V_rest, V), tau_register);
    
    // Total change: dt * (leakContribution + synapticContribution)
    __m128 totalChange = _mm_mul_ps(dt_register, _mm_add_ps(leakContribution, synapticContribution));
    
    // Calculate new membrane potential
    __m128 V_new = _mm_add_ps(V, totalChange);
    
    // Apply spike-frequency adaptation
    __m128 adaptation = _mm_loadu_ps(&neuronData.adaptationVariable);
    __m128 adaptationFactor = _mm_mul_ps(adaptation, _mm_set1_ps(0.01f));
    V_new = _mm_sub_ps(V_new, adaptationFactor);
    
    // Decay adaptation variable
    adaptation = _mm_mul_ps(adaptationFactor, _mm_set1_ps(0.95f));
    _mm_storeu_ps(&neuronData.adaptationVariable, adaptation);
    
    // Clamp membrane potential
    V_new = clampVector(V_new, -100.0f, 50.0f);
    _mm_storeu_ps(&neuronData.membranePotential, V_new);
    
    // Check for spikes
    __m128 spikeMask = _mm_cmpge_ps(V_new, threshold);
    _mm_storeu_ps(&neuronData.spikeMask, spikeMask);
    
    // Apply spike reset and record spikes
    applySpikeResetOptimized(neuronData, spikeMask, currentTime);
    
    // Clear synaptic input for next step
    neuronData.synapticInput = _mm_set1_ps(0.0f);
}

void Neuron::applySpikeResetOptimized(__m128& spikeMask, Timestamp currentTime, float resetPotential) {
    // Apply spike reset logic using SIMD
    if (_mm_movemask_ps(spikeMask) == 0) return;  // No spikes
    
    // Reset membrane potential to reset potential
    // (Implementation would extract from neuron data and apply)
    
    // Enter refractory period (set refractory remaining)
    // (Implementation would update neuron data)
    
    // Update spike time
    // (Implementation would record spike time)
}

__m128 Neuron::clampVector(__m128 value, float minVal, float maxVal) {
    __m128 min = _mm_set1_ps(minVal);
    __m128 max = _mm_set1_ps(maxVal);
    value = _mm_max_ps(value, min);
    value = _mm_min_ps(value, max);
    return value;
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
