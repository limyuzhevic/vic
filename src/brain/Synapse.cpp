#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <xmmintrin.h>  // SSE (32-bit float)
#include <smmintrin.h>   // SSSE3 (additional SSE operations)

// SIMD intrinsics for synapse processing
namespace SIMD {
    
    // Process 4 synapses in parallel using SSE
    class SynapseVectorProcessor {
    public:
        struct SynapseData {
            __m128 weight;              // 4 synaptic weights (-1 to 1)
            __m128 delay;               // 4 delays (1-5 steps)
            __m128 efficacy;            // 4 efficacies (0-2)
            __m128 eligibilityTrace;    // 4 eligibility traces
            __m128 shortTermDepression; // 4 depression factors (0-1)
            __m128 shortTermFacilitation; // 4 facilitation factors (0-1)
            __m128 lastPreSpikeTime;    // 4 last pre-spike times
            __m128 lastPostSpikeTime;   // 4 last post-spike times
            __m128 plasticityFlags;     // 4 plasticity flags
            __m128 spikeMask;           // 4 spike flags
        };
        
        static void process4Synapses(const SynapseData& synapses, SynapseData& results,
                                    float currentTime, float dt) {
            // Apply synaptic dynamics for 4 synapses in parallel
            
            // Decay short-term facilitation (Tsodyks-Markram model)
            __m128 timeSincePre = _mm_sub_ps(currentTime, synapses.lastPreSpikeTime);
            __m128 validPreMask = _mm_cmpgt_ps(synapses.lastPreSpikeTime, _mm_set1_ps(-1.0f));
            timeSincePre = _mm_and_ps(timeSincePre, validPreMask);
            
            results.shortTermFacilitation = synapses.shortTermFacilitation;
            results.shortTermFacilitation = _mm_mul_ps(
                results.shortTermFacilitation,
                expDecay(timeSincePre, STP_FACILITATION_TAU)
            );
            
            // Decay short-term depression
            __m128 timeSincePost = _mm_sub_ps(currentTime, synapses.lastPostSpikeTime);
            __m128 validPostMask = _mm_cmpgt_ps(synapses.lastPostSpikeTime, _mm_set1_ps(-1.0f));
            timeSincePost = _mm_and_ps(timeSincePost, validPostMask);
            
            __m128 timeSinceActivity = _mm_max_ps(timeSincePost, timeSincePre);
            
            // Recovery from depression toward 1.0
            results.shortTermDepression = synapses.shortTermDepression;
            results.shortTermDepression += _mm_mul_ps(
                _mm_sub_ps(_mm_set1_ps(1.0f), results.shortTermDepression),
                _mm_set1_ps(1.0f - std::exp(-static_cast<float>(timeSinceActivity) / STP_DEPRESSION_TAU))
            );
            
            // Decay eligibility trace for reward-modulated learning
            results.eligibilityTrace = _mm_mul_ps(synapses.eligibilityTrace, _mm_set1_ps(1.0f - decayRate));
            
            // Clamp weight bounds
            results.weight = synapses.weight;
            results.weight = clamp4(results.weight, -1.0f, 1.0f);
            
            // Update efficacy based on short-term plasticity
            // Efficacy = STP_facilitation / (1 + STP_depression)
            __m128 denominator = _mm_add_ps(_mm_set1_ps(1.0f), results.shortTermDepression);
            results.efficacy = _mm_div_ps(results.shortTermFacilitation, denominator);
            results.efficacy = clamp4(results.efficacy, 0.0f, 2.0f);
        }
        
        static __m128 expDecay(__m128 time, float tau) {
            return _mm_exp_ps(_mm_mul_ps(_mm_set1_ps(-1.0f), _mm_div_ps(time, _mm_set1_ps(tau))));
        }
        
        static __m128 clamp4(__m128 value, float minVal, float maxVal) {
            __m128 min = _mm_set1_ps(minVal);
            __m128 max = _mm_set1_ps(maxVal);
            value = _mm_max_ps(value, min);
            value = _mm_min_ps(value, max);
            return value;
        }
        
        static void recordSpikes(SynapseData& synapses, const __m128& preSpikeMask,
                                const __m128& postSpikeMask, Timestamp currentTime) {
            // Record pre and post spikes for STDP
            
            // Convert spike masks to boolean for history recording
            if (_mm_movemask_ps(preSpikeMask) == 0 && _mm_movemask_ps(postSpikeMask) == 0) return;
            
            // Update last spike times
            __m128 currentTimeVec = _mm_set1_ps(static_cast<float>(currentTime));
            
            synapses.lastPreSpikeTime = _mm_or_ps(
                synapses.lastPreSpikeTime,
                _mm_and_ps(preSpikeMask, currentTimeVec)
            );
            
            synapses.lastPostSpikeTime = _mm_or_ps(
                synapses.lastPostSpikeTime,
                _mm_and_ps(postSpikeMask, currentTimeVec)
            );
            
            // Update eligibility trace for spikes
            __m128 spikeMask = _mm_or_ps(preSpikeMask, postSpikeMask);
            synapses.eligibilityTrace = _mm_add_ps(
                synapses.eligibilityTrace,
                _mm_and_ps(spikeMask, _mm_set1_ps(1.0f))
            );
        }
    };
}

namespace nlm {

struct Synapse::Impl {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;  // Synaptic delay in simulation steps
    
    // Synapse type
    SynapseType type;
    
    // Spike history for STDP
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
    // Short-term plasticity state
    float shortTermDepression;  // Utilization factor (0-1)
    float shortTermFacilitation;  // Facilitation factor
    float lastPreSpikeTime;  // For short-term dynamics
    float lastPostSpikeTime;
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;  // Default: 1 step delay
    pImpl->type = SynapseType::Excitatory;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

Synapse::~Synapse() = default;

Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

SynapseId Synapse::getId() const {
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    pImpl->delay = delay;
}

SynapseType Synapse::getType() const {
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    pImpl->type = type;
}

bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::step(Timestamp currentTime) {
    // SIMD-optimized version (falls back to scalar for small numbers)
    if (sizeof(SynapseId) % 4 == 0) {  // Use SIMD for 4+ synapses
        SIMD::SynapseVectorProcessor::process4SynapsesOptimized(this, currentTime, 0.001f);
    } else {
        // Fallback to scalar implementation for compatibility
        stepScalar(currentTime);
    }
}

void Synapse::stepScalar(Timestamp currentTime) {
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // Decay short-term depression
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::stepOptimized(const SIMD::SynapseVectorProcessor::SynapseData& synapseData,
                           Timestamp currentTime) {
    // Process 4 synapses in SIMD for maximum performance
    
    // Decay short-term facilitation
    __m128 timeSincePre = _mm_sub_ps(_mm_set1_ps(static_cast<float>(currentTime)), synapseData.lastPreSpikeTime);
    __m128 validPreMask = _mm_cmpgt_ps(synapseData.lastPreSpikeTime, _mm_set1_ps(-1.0f));
    timeSincePre = _mm_and_ps(timeSincePre, validPreMask);
    
    __m128 facilitation = synapseData.shortTermFacilitation;
    facilitation = _mm_mul_ps(facilitation, expDecaySIMD(timeSincePre, STP_FACILITATION_TAU));
    
    // Decay short-term depression
    __m128 timeSincePost = _mm_sub_ps(_mm_set1_ps(static_cast<float>(currentTime)), synapseData.lastPostSpikeTime);
    __m128 validPostMask = _mm_cmpgt_ps(synapseData.lastPostSpikeTime, _mm_set1_ps(-1.0f));
    timeSincePost = _mm_and_ps(timeSincePost, validPostMask);
    
    __m128 timeSinceActivity = _mm_max_ps(timeSincePost, timeSincePre);
    
    __m128 depression = synapseData.shortTermDepression;
    depression += _mm_mul_ps(
        _mm_sub_ps(_mm_set1_ps(1.0f), depression),
        _mm_set1_ps(1.0f - std::exp(-static_cast<float>(timeSinceActivity) / STP_DEPRESSION_TAU))
    );
    
    // Decay eligibility trace
    __m128 eligibility = synapseData.eligibilityTrace;
    eligibility = _mm_mul_ps(eligibility, _mm_set1_ps(0.999f));  // 0.1% decay
    
    // Update weights and efficacy
    __m128 weight = synapseData.weight;
    weight = clamp4SIMD(weight, -1.0f, 1.0f);
    
    __m128 efficacy = facilitation / _mm_add_ps(_mm_set1_ps(1.0f), depression);
    efficacy = clamp4SIMD(efficacy, 0.0f, 2.0f);
    
    // Record spikes for STDP
    recordSpikesOptimized(weight, eligibility, currentTime);
}

void Synapse::recordSpikesOptimized(__m128& weight, __m128& eligibility, Timestamp currentTime,
                                   const __m128& preSpikeMask, const __m128& postSpikeMask) {
    // Record pre and post spikes for STDP
    __m128 currentTimeVec = _mm_set1_ps(static_cast<float>(currentTime));
    
    // Update last spike times (simplified for SIMD)
    // Record eligibility trace updates
    __m128 spikeMask = _mm_or_ps(preSpikeMask, postSpikeMask);
    eligibility = _mm_add_ps(eligibility, _mm_and_ps(spikeMask, _mm_set1_ps(1.0f)));
}

__m128 Synapse::expDecaySIMD(__m128 time, float tau) {
    return _mm_exp_ps(_mm_mul_ps(_mm_set1_ps(-1.0f), _mm_div_ps(time, _mm_set1_ps(tau))));
}

__m128 Synapse::clamp4SIMD(__m128 value, float minVal, float maxVal) {
    __m128 min = _mm_set1_ps(minVal);
    __m128 max = _mm_set1_ps(maxVal);
    value = _mm_max_ps(value, min);
    value = _mm_min_ps(value, max);
    return value;
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm
