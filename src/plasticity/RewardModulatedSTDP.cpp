#include "RewardModulatedSTDP.hpp"
#include "STDP.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include "../../core/Logger/Logger.hpp"

namespace nlm {

struct RewardModulatedSTDP::Impl {
    // Base STDP rule for spike-timing dependent plasticity
    std::shared_ptr<STDP> baseSTDP;
    
    // Dopamine neuromodulation
    std::shared_ptr<Dopamine> dopamine;
    
    // R-STDP specific parameters
    float threshold;              // Minimum RPE for plasticity
    float modulationStrength;     // Overall modulation strength
    float ltpModulation;          // Modulation of LTP by dopamine
    float ltdModulation;          // Modulation of LTD by dopamine
    float timeConstant;           // Time constant for eligibility trace
    
    // State tracking
    float accumulatedRPE;         // Total accumulated reward prediction error
    float totalWeightChanges;     // Total weight changes applied
    float adaptationRate;         // Rate of adaptation to environmental changes
    float valueFunctionChange;    // Change in value function estimate
    
    // Policy gradient components
    float policyGradient;         // Gradient of policy with respect to parameters
    float explorationBonus;       // Exploration contribution to learning
    float exploitationValue;      // Exploitation contribution to learning
    
    // Learning statistics
    RSTDPStats stats;
    
    // Eligibility traces (for reward-modulated learning)
    std::unordered_map<SynapseId, float> eligibilityTraces;
    Timestamp lastUpdateTime;
    
    // Temporal dynamics
    std::vector<float> rpeHistory;          // History of reward prediction errors
    std::vector<float> weightChangeHistory; // History of weight changes
    std::vector<Timestamp> timeStamps;     // Timestamps for history entries
    
    // Integration with value function
    float predictedValue;        // Predicted value of current state
    float actualValue;           // Actual received value
    
    // Environmental complexity
    float environmentComplexity; // Measure of environmental complexity
    
    // Initialize with biologically plausible parameters
    Impl() {
        // Initialize base STDP
        baseSTDP = std::make_shared<STDP>();
        baseSTDP->configure(0.01f, 0.012f, 20.0f);
        
        // Initialize dopamine modulation
        dopamine = nullptr;
        
        // R-STDP parameters
        threshold = 0.01f;           // Minimum RPE for effective plasticity
        modulationStrength = 1.0f;   // Baseline modulation strength
        ltpModulation = 1.2f;        // 20% enhancement of LTP
        ltdModulation = 0.8f;        // 20% reduction of LTD
        timeConstant = 100.0f;       // Longer time constant for eligibility traces
        
        // State tracking
        accumulatedRPE = 0.0f;
        totalWeightChanges = 0.0f;
        adaptationRate = 0.1f;
        valueFunctionChange = 0.0f;
        
        // Policy gradient components
        policyGradient = 0.0f;
        explorationBonus = 0.0f;
        exploitationValue = 0.0f;
        
        // Initialize statistics
        stats.totalRPEApplied = 0.0f;
        stats.totalWeightChanges = 0.0f;
        stats.plasticityEfficiency = 0.0f;
        stats.explorationInfluence = 0.0f;
        stats.exploitationInfluence = 0.0f;
        stats.adaptationRate = adaptationRate;
        
        // Temporal state
        lastUpdateTime = 0.0;
        predictedValue = 0.0f;
        actualValue = 0.0f;
        
        // Environmental complexity
        environmentComplexity = 1.0f;
        
        NLM_LOG_INFO("RewardModulatedSTDP system initialized with biologically realistic parameters");
    }
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(std::make_unique<Impl>()) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty() || dt <= 0.0) {
        return;
    }
    
    // Get current prediction error from dopamine system
    float rpe = 0.0f;
    if (dopamine) {
        rpe = dopamine->getPredictionError();
        pImpl->stats.totalRPEApplied += std::abs(rpe);
    }
    
    // Update eligibility traces based on spike timing
    float eligibilityTrace = 0.0f;
    
    // Compute base STDP contribution
    if (pImpl->baseSTDP) {
        // Create a temporary synapse for STDP calculation
        // Note: This is a simplified approach - in a real implementation,
        // we'd need proper synapse management
        pImpl->baseSTDP->update(synapse, preSpikes, postSpikes, dt);
        
        // Get the weight change from base STDP
        float baseWeightChange = synapse->getEligibilityTrace();
        
        // Apply dopamine modulation to eligibility trace
        if (std::abs(rpe) > pImpl->threshold) {
            // Compute modulation factor based on RPE sign and magnitude
            float modulationFactor = 1.0f;
            if (rpe > 0.0f) {
                // Positive RPE (better than expected): enhance LTP
                modulationFactor = pImpl->ltpModulation;
            } else {
                // Negative RPE (worse than expected): suppress LTD
                modulationFactor = pImpl->ltdModulation;
            }
            
            // Apply modulation with temporal decay
            float temporalDecay = std::exp(-static_cast<float>(dt) / pImpl->timeConstant);
            eligibilityTrace = baseWeightChange * modulationFactor * temporalDecay;
            
            // Store eligibility trace
            pImpl->eligibilityTraces[synapse->getId()] = eligibilityTrace;
        }
    }
    
    // Update statistics
    pImpl->totalWeightChanges += std::abs(eligibilityTrace);
    
    // Update history
    pImpl->rpeHistory.push_back(rpe);
    pImpl->weightChangeHistory.push_back(eligibilityTrace);
    pImpl->timeStamps.push_back(pImpl->lastUpdateTime);
    
    // Maintain history sizes
    const size_t maxHistorySize = 1000;
    if (pImpl->rpeHistory.size() > maxHistorySize) {
        pImpl->rpeHistory.erase(pImpl->rpeHistory.begin());
        pImpl->weightChangeHistory.erase(pImpl->weightChangeHistory.begin());
        pImpl->timeStamps.erase(pImpl->timeStamps.begin());
    }
    
    // Update adaptation rate based on environmental complexity
    adaptToEnvironmentComplexity(pImpl->environmentComplexity);
    
    NLM_LOG_DEBUG("R-STDP update: synapse=" + std::to_string(synapse->getId().index()) +
                 " RPE=" + std::to_string(rpe) +
                 " eligibility=" + std::to_string(eligibilityTrace) +
                 " complexity=" + std::to_string(pImpl->environmentComplexity));
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply base weight change
    pImpl->baseSTDP->applyWeightChange(synapse, delta);
    
    // Update statistics
    pImpl->stats.totalWeightChanges += std::abs(delta);
    
    // Update adaptation based on weight change
    adaptToEnvironmentComplexity(pImpl->environmentComplexity);
    
    NLM_LOG_DEBUG("R-STDP weight change: synapse=" + std::to_string(synapse->getId().index()) +
                 " delta=" + std::to_string(delta) +
                 " total_changes=" + std::to_string(pImpl->stats.totalWeightChanges));
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setDopamineModulation(std::shared_ptr<Dopamine> dopamine) {
    pImpl->dopamine = dopamine;
    NLM_LOG_INFO("R-STDP dopamine modulation set: " + std::string(dopamine->getName()));
}

void RewardModulatedSTDP::setBaseSTDP(std::shared_ptr<STDP> stdp) {
    pImpl->baseSTDP = stdp;
    NLM_LOG_INFO("R-STDP base STDP set");
}

void RewardModulatedSTDP::setThreshold(float threshold) {
    pImpl->threshold = std::max(0.001f, threshold);
}

void RewardModulatedSTDP::setModulationStrength(float strength) {
    pImpl->modulationStrength = std::max(0.1f, std::min(5.0f, strength));
}

void RewardModulatedSTDP::setLTPModulation(float ltpMod) {
    pImpl->ltpModulation = std::max(0.5f, ltpMod);
}

void RewardModulatedSTDP::setLTDModulation(float ltdMod) {
    pImpl->ltdModulation = std::max(0.1f, ltdMod);
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::max(10.0f, std::min(1000.0f, tau));
}

void RewardModulatedSTDP::synchronizeWithDopamine() {
    if (!dopamine) return;
    
    // Get current dopamine state
    float rpe = dopamine->getPredictionError();
    float valueFunction = dopamine->getValueFunction();
    
    // Update predicted and actual values
    pImpl->actualValue = valueFunction;
    
    // Update policy gradient based on RPE
    pImpl->policyGradient = rpe * pImpl->modulationStrength;
    
    // Update exploration and exploitation components
    float predictionAccuracy = 1.0f - std::abs(rpe) / std::max(std::abs(valueFunction), 1.0f);
    pImpl->explorationBonus = (1.0f - predictionAccuracy) * pImpl->modulationStrength;
    pImpl->exploitationValue = predictionAccuracy * pImpl->modulationStrength;
    
    // Update statistics
    pImpl->stats.explorationInfluence = pImpl->explorationBonus;
    pImpl->stats.exploitationInfluence = pImpl->exploitationValue;
    
    NLM_LOG_DEBUG("R-STDP synchronized with dopamine: RPE=" + std::to_string(rpe) +
                 " value_function=" + std::to_string(valueFunction) +
                 " policy_gradient=" + std::to_string(pImpl->policyGradient));
}

void RewardModulatedSTDP::reset() {
    pImpl->eligibilityTraces.clear();
    pImpl->rpeHistory.clear();
    pImpl->weightChangeHistory.clear();
    pImpl->timeStamps.clear();
    
    pImpl->accumulatedRPE = 0.0f;
    pImpl->totalWeightChanges = 0.0f;
    pImpl->valueFunctionChange = 0.0f;
    pImpl->policyGradient = 0.0f;
    pImpl->explorationBonus = 0.0f;
    pImpl->exploitationValue = 0.0f;
    
    // Reset statistics
    pImpl->stats.totalRPEApplied = 0.0f;
    pImpl->stats.totalWeightChanges = 0.0f;
    pImpl->stats.plasticityEfficiency = 0.0f;
    pImpl->stats.explorationInfluence = 0.0f;
    pImpl->stats.exploitationInfluence = 0.0f;
    
    NLM_LOG_INFO("R-STDP system reset");
}

const RewardModulatedSTDP::RSTDPStats& RewardModulatedSTDP::getStats() const {
    // Calculate plasticity efficiency
    if (pImpl->totalWeightChanges > 0.0f) {
        pImpl->stats.plasticityEfficiency = pImpl->stats.totalRPEApplied / pImpl->totalWeightChanges;
    }
    
    // Update adaptation rate in stats
    pImpl->stats.adaptationRate = pImpl->adaptationRate;
    
    return pImpl->stats;
}

void RewardModulatedSTDP::integrateWithValueFunction(float valueChange) {
    // Update value function change
    pImpl->valueFunctionChange = valueChange;
    
    // Update predicted value
    pImpl->predictedValue += valueChange;
    
    // Update statistics based on value change
    if (std::abs(valueChange) > pImpl->threshold) {
        // Significant value change indicates important learning
        pImpl->adaptationRate *= (1.0f + std::abs(valueChange) * 0.1f);
        pImpl->adaptationRate = std::clamp(pImpl->adaptationRate, 0.01f, 1.0f);
    }
    
    NLM_LOG_DEBUG("R-STDP integrated with value function: value_change=" + std::to_string(valueChange) +
                 " predicted_value=" + std::to_string(pImpl->predictedValue) +
                 " adaptation_rate=" + std::to_string(pImpl->adaptationRate));
}

void RewardModulatedSTDP::adaptToEnvironmentComplexity(float complexity) {
    // Adapt to environmental complexity
    pImpl->environmentComplexity = std::clamp(complexity, 0.1f, 10.0f);
    
    // Adjust parameters based on complexity
    if (pImpl->environmentComplexity > 1.0f) {
        // Complex environment: increase learning rate, decrease threshold
        pImpl->adaptationRate = 0.15f * std::min(1.0f, pImpl->environmentComplexity / 5.0f);
        pImpl->threshold *= (1.0f - (pImpl->environmentComplexity - 1.0f) * 0.1f);
        pImpl->timeConstant *= (1.0f + (pImpl->environmentComplexity - 1.0f) * 0.5f);
    } else {
        // Simple environment: decrease learning rate, increase threshold
        pImpl->adaptationRate = 0.05f;
        pImpl->threshold *= (1.0f + (1.0f - pImpl->environmentComplexity) * 0.1f);
        pImpl->timeConstant *= (1.0f - (1.0f - pImpl->environmentComplexity) * 0.2f);
    }
    
    // Update stats
    pImpl->stats.adaptationRate = pImpl->adaptationRate;
    
    NLM_LOG_DEBUG("R-STDP adapted to environment complexity: complexity=" + std::to_string(complexity) +
                 " adaptation_rate=" + std::to_string(pImpl->adaptationRate) +
                 " threshold=" + std::to_string(pImpl->threshold));
}

void RewardModulatedSTDP::applyPolicyGradient(float policyGradient) {
    pImpl->policyGradient = policyGradient;
    
    // Apply policy gradient to weight changes
    float policyWeightChange = policyGradient * pImpl->modulationStrength;
    
    // Update exploitation value based on policy gradient
    pImpl->exploitationValue = std::abs(policyGradient) * pImpl->modulationStrength;
    
    // Update statistics
    pImpl->stats.exploitationInfluence = pImpl->exploitationValue;
    
    NLM_LOG_DEBUG("R-STDP applied policy gradient: gradient=" + std::to_string(policyGradient) +
                 " weight_change=" + std::to_string(policyWeightChange));
}

} // namespace nlm