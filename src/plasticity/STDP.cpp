#include "STDP.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <functional>
#include <random>
#include <chrono>

namespace nlm {

// Advanced STDP implementation with multiple learning phases
// Supports: pair-based STDP, triplet STDP, and reward-modulated STDP

struct STDP::Impl {
    // Core STDP parameters
    float ltpWeight;      // Long-term potentiation weight
    float ltdWeight;      // Long-term depression weight
    float tauPlus;        // LTP time constant (ms)
    float tauMinus;       // LTD time constant (ms)
    float weightMax;      // Maximum synaptic weight
    float weightMin;      // Minimum synaptic weight
    
    // Advanced features
    bool useTripletModel;         // Enable triplet STDP
    bool useRewardModulation;     // Enable reward-modulated STDP
    bool useLearningThresholds;   // Enable learning thresholds
    
    // Neuromodulator influences
    float dopamineModulation;    // Dopamine modulation factor
    float acetylcholineModulation; // Acetylcholine modulation factor
    
    // Eligibility traces for reward modulation
    std::unordered_map<SynapseId, float> eligibilityTraces;
    
    // Spike history for advanced plasticity
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Learning history
    struct WeightHistoryEntry {
        Timestamp timestamp;
        float weight;
        float learningRate;
    };
    std::vector<WeightHistoryEntry> weightHistory;
    
    // Real STDP parameters
    static constexpr float DEFAULT_LTP_WEIGHT = 0.01f;
    static constexpr float DEFAULT_LTD_WEIGHT = 0.012f;
    static constexpr float DEFAULT_TAU_PLUS = 20.0f;     // ms
    static constexpr float DEFAULT_TAU_MINUS = 20.0f;    // ms
    static constexpr float DEFAULT_WEIGHT_MAX = 1.0f;
    static constexpr float DEFAULT_WEIGHT_MIN = -1.0f;
    
    // Pair-based STDP kernel
    static constexpr float PAIR_BASED_STDP_KERNEL = 0.015f;  // ms^-1
    
    // Triplet STDP parameters
    static constexpr float TRIPLET_TAU_F = 33.0f;   // ms
    static constexpr float TRIPLET_TAU_D = 33.0f;   // ms
    static constexpr float TRIPLET_TAU_P = 20.0f;   // ms
    
    // Reward modulation parameters
    static constexpr float REWARD_DECAY_RATE = 0.95f;
    static constexpr float REWARD_LEARNING_RATE = 0.001f;
    static constexpr float ELIGIBILITY_DECAY = 0.1f;
    
    // Learning thresholds
    static constexpr float MIN_LEARNING_MAGNITUDE = 0.001f;
    static constexpr float MAX_LEARNING_MAGNITUDE = 0.1f;
    
    Impl() : ltpWeight(DEFAULT_LTP_WEIGHT)
           , ltdWeight(DEFAULT_LTD_WEIGHT)
           , tauPlus(DEFAULT_TAU_PLUS)
           , tauMinus(DEFAULT_TAU_MINUS)
           , weightMax(DEFAULT_WEIGHT_MAX)
           , weightMin(DEFAULT_WEIGHT_MIN)
           , useTripletModel(true)
           , useRewardModulation(true)
           , useLearningThresholds(true)
           , dopamineModulation(1.0f)
           , acetylcholineModulation(1.0f) {
        // Initialize eligibility traces with reasonable capacity
        eligibilityTraces.reserve(10000);
        weightHistory.reserve(10000);
    }
    
    // Calculate pair-based STDP learning window
    float calculatePairBasedSTDP(float deltaT) const {
        if (deltaT > 0) { // Post before pre (LTD)
            return -ltdWeight * std::exp(-std::abs(deltaT) / tauMinus);
        } else { // Pre before post (LTP)
            return ltpWeight * std::exp(-std::abs(deltaT) / tauPlus);
        }
    }
    
    // Calculate triplet-based STDP learning window
    float calculateTripletBasedSTDP(float deltaT, bool preBeforePost) const {
        if (useTripletModel && preBeforePost) {
            // Triplet interaction: 1-pre + 2-post interaction
            float tripletEffect = std::exp(-std::abs(deltaT) / TRIPLET_TAU_P) * 
                                  std::exp(-std::abs(deltaT - TRIPLET_TAU_F) / TRIPLET_TAU_F);
            return calculatePairBasedSTDP(deltaT) * (1.0f + 0.5f * tripletEffect);
        } else if (useTripletModel && !preBeforePost) {
            // 1-post + 2-pre interaction
            float tripletEffect = std::exp(-std::abs(deltaT) / TRIPLET_TAU_D) * 
                                  std::exp(-std::abs(deltaT - TRIPLET_TAU_F) / TRIPLET_TAU_F);
            return calculatePairBasedSTDP(deltaT) * (1.0f + 0.3f * tripletEffect);
        }
        return calculatePairBasedSTDP(deltaT);
    }
    
    // Update eligibility trace for reward modulation
    void updateEligibilityTrace(SynapseId synapseId, float delta, float neuromodulatorLevel) {
        float eligibilityTrace = eligibilityTraces.find(synapseId) != eligibilityTraces.end() 
                                ? eligibilityTraces[synapseId] : 0.0f;
        
        // Update trace with learning signal
        eligibilityTrace += delta * neuromodulatorLevel;
        eligibilityTrace *= REWARD_DECAY_RATE;
        
        // Apply eligibility trace decay
        eligibilityTrace *= (1.0f - ELIGIBILITY_DECAY);
        
        eligibilityTraces[synapseId] = eligibilityTrace;
    }
    
    // Get eligibility trace for a synapse
    float getEligibilityTrace(SynapseId synapseId) const {
        auto it = eligibilityTraces.find(synapseId);
        return it != eligibilityTraces.end() ? it->second : 0.0f;
    }
    
    // Apply learning threshold
    float applyLearningThreshold(float learningChange) const {
        if (useLearningThresholds) {
            float magnitude = std::abs(learningChange);
            if (magnitude < MIN_LEARNING_MAGNITUDE) {
                return 0.0f; // Below threshold - no learning
            }
            if (magnitude > MAX_LEARNING_MAGNITUDE) {
                // Clamp to maximum
                return (learningChange > 0) ? MAX_LEARNING_MAGNITUDE : -MAX_LEARNING_MAGNITUDE;
            }
        }
        return learningChange;
    }
    
    // Update weight with bounds and modulation
    void updateWeight(float& currentWeight, float learningChange, float neuromodulation) {
        // Apply learning threshold
        learningChange = applyLearningThreshold(learningChange);
        
        // Apply neuromodulation
        float modulatedChange = learningChange * neuromodulation;
        
        // Update weight with bounds
        currentWeight += modulatedChange;
        
        // Clamp to valid range
        currentWeight = std::clamp(currentWeight, weightMin, weightMax);
        
        // Record weight history
        WeightHistoryEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.weight = currentWeight;
        entry.learningRate = modulatedChange;
        weightHistory.push_back(entry);
        
        // Keep history size manageable
        if (weightHistory.size() > 10000) {
            weightHistory.erase(weightHistory.begin(), weightHistory.begin() + 1000);
        }
    }
    
    // Calculate total neuromodulation factor
    float getTotalNeuromodulation() const {
        float totalMod = dopamineModulation * acetylcholineModulation;
        // Clamp modulation to reasonable range
        totalMod = std::clamp(totalMod, 0.1f, 5.0f);
        return totalMod;
    }
    
    // Clear old entries to prevent memory bloat
    void cleanupOldEntries() {
        // Remove eligibility traces for inactive synapses
        for (auto it = eligibilityTraces.begin(); it != eligibilityTraces.end(); ) {
            if (it->second < 1e-6f) { // Very small trace
                it = eligibilityTraces.erase(it);
            } else {
                ++it;
            }
        }
    }
};

STDP::STDP() : pImpl(std::make_unique<Impl>()) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) {
    if (!synapse) {
        NLM_LOG_ERROR("STDP update called with null synapse");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return; // No spike activity to learn from
    }
    
    // Calculate total learning change from all spike pairs
    float totalLearningChange = 0.0f;
    
    // Pair-based STDP calculation
    for (const auto& preTime : preSpikes) {
        for (const auto& postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime) - static_cast<float>(preTime);
            
            float learningChange;
            if (pImpl->useTripletModel) {
                learningChange = pImpl->calculateTripletBasedSTDP(deltaT, deltaT > 0);
            } else {
                learningChange = pImpl->calculatePairBasedSTDP(deltaT);
            }
            
            totalLearningChange += learningChange;
        }
    }
    
    // Apply neuromodulation and learning thresholds
    float neuromodulation = pImpl->getTotalNeuromodulation();
    totalLearningChange *= neuromodulation;
    
    // Apply learning threshold
    float clampedLearningChange = pImpl->applyLearningThreshold(totalLearningChange);
    
    // Update eligibility trace for reward-modulated STDP
    if (pImpl->useRewardModulation) {
        SynapseId synapseId = synapse->getId();
        pImpl->updateEligibilityTrace(synapseId, clampedLearningChange, pImpl->dopamineModulation);
    }
    
    // Apply weight change
    float currentWeight = synapse->getWeight();
    pImpl->updateWeight(currentWeight, clampedLearningChange, neuromodulation);
    synapse->setWeight(currentWeight);
    
    // Clean up old entries to prevent memory leaks
    pImpl->cleanupOldEntries();
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        NLM_LOG_ERROR("STDP weight change called with null synapse");
        return;
    }
    
    // Apply the weight change
    float currentWeight = synapse->getWeight();
    pImpl->updateWeight(currentWeight, delta, 1.0f); // No neuromodulation for direct changes
    synapse->setWeight(currentWeight);
}

const char* STDP::getName() const {
    return "AdvancedSTDP";
}

void STDP::setLTPWeight(float weight) {
    if (weight < 0) {
        NLM_LOG_WARNING("LTP weight cannot be negative: " + std::to_string(weight));
        return;
    }
    pImpl->ltpWeight = weight;
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    if (weight < 0) {
        NLM_LOG_WARNING("LTD weight cannot be negative: " + std::to_string(weight));
        return;
    }
    pImpl->ltdWeight = weight;
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    if (tau <= 0) {
        NLM_LOG_WARNING("Time constant must be positive: " + std::to_string(tau));
        return;
    }
    pImpl->tauPlus = tau;
    pImpl->tauMinus = tau;
}

float STDP::getTimeConstant() const {
    return pImpl->tauPlus;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm