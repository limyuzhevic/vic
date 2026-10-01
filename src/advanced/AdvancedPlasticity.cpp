#include "AdvancedPlasticity.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>
#include <numeric>

namespace nlm {

struct AdvancedPlasticity::Impl {
    AdvancedPlasticityConfig config;
    
    // Active rule management
    std::map<std::string, PlasticityRuleType> ruleTypes;
    std::map<std::string, std::map<std::string, float>> ruleParameters;
    std::map<std::string, std::function<void(Synapse*, const std::vector<Timestamp>&,
                                            const std::vector<Timestamp>&, TimestepDuration,
                                            float)>> customRules;
    std::string activeRuleName;
    
    // Trace management
    std::map<std::string, std::vector<float>> preTraces;
    std::map<std::string, std::vector<float>> postTraces;
    std::map<std::string, std::vector<float>> eligibilityTraces;
    
    // Synaptic tagging
    std::map<std::string, std::map<NeuronId, std::vector<float>>> synapticTags;
    
    // Statistics
    float totalWeightChange;
    float totalPlasticityEvents;
    std::map<std::string, float> ruleActivity;
    
    Impl() : totalWeightChange(0.0f), totalPlasticityEvents(0.0f) {
        loadDefaultConfig();
        resetConfig();
    }
    
    void loadDefaultConfig() {
        // Load default STDP parameters
        config.stdpLTPWeight = 0.01f;
        config.stdpLTDWeight = 0.012f;
        config.stdpTimeConstant = 20.0f;
        config.stdpTraceDecay = 0.95f;
        
        // Hebbian parameters
        config.hebbianLearningRate = 0.01f;
        config.hebbianThreshold = 0.5f;
        
        // BCM parameters
        config.bcmLearningRate = 0.01f;
        config.bcmThreshold = 0.02f;
        config.bcmSlidingThreshold = 0.1f;
        
        // Oja parameters
        config.ojaLearningRate = 0.01f;
        config.ojaRegularization = 0.001f;
        
        // HTT parameters
        config.httPreWeight = 0.01f;
        config.httPostWeight = 0.01f;
        
        // Homeostatic parameters
        config.homeostaticTarget = 0.02f;  // 2% firing rate target
        config.homeostaticLearningRate = 0.001f;
        config.homeostaticTimeConstant = 1000.0f;
        
        // Neuromodulation factors
        config.rewardModulationFactor = 1.0f;
        config.predictionErrorModulation = 0.5f;
        config.noveltyModulation = 0.3f;
        
        // Synaptic constraints
        config.minWeight = -1.0f;
        config.maxWeight = 1.0f;
        config.weightChangeLimit = 0.1f;
    }
    
    void resetConfig() {
        // Reset trace arrays
        preTraces.clear();
        postTraces.clear();
        eligibilityTraces.clear();
        
        // Reset tag arrays
        synapticTags.clear();
        
        // Reset statistics
        totalWeightChange = 0.0f;
        totalPlasticityEvents = 0.0f;
        ruleActivity.clear();
        
        activeRuleName = "STDP";
    }
};

AdvancedPlasticity::AdvancedPlasticity() : pImpl(std::make_unique<Impl>()) {
}

AdvancedPlasticity::~AdvancedPlasticity() = default;

void AdvancedPlasticity::configure(const AdvancedPlasticityConfig& config) {
    pImpl->config = config;
    NLM_LOG_INFO("Advanced plasticity configured");
}

void AdvancedPlasticity::updateConfig(const std::string& key, float value) {
    // Implementation would update specific config values based on key
    // For now, just log the change
    NLM_LOG_INFO("Updated advanced plasticity config: " + key + " = " + std::to_string(value));
}

float AdvancedPlasticity::getConfigValue(const std::string& key) const {
    // Implementation would retrieve specific config values based on key
    // For now, return 0.0f
    return 0.0f;
}

void AdvancedPlasticity::addPlasticityRule(const std::string& name, PlasticityRuleType type,
                                          const std::map<std::string, float>& parameters) {
    pImpl->ruleTypes[name] = type;
    pImpl->ruleParameters[name] = parameters;
    
    // Initialize trace arrays for new rule
    std::string traceKey = "pre_" + name;
    pImpl->preTraces[traceKey].assign(1000, 0.0f);  // Initialize with 1000 time steps
    
    traceKey = "post_" + name;
    pImpl->postTraces[traceKey].assign(1000, 0.0f);
    
    traceKey = "eligibility_" + name;
    pImpl->eligibilityTraces[traceKey].assign(1000, 0.0f);
    
    NLM_LOG_INFO("Added plasticity rule: " + name + " (type: " + std::to_string(static_cast<int>(type)) + ")");
}

bool AdvancedPlasticity::removePlasticityRule(const std::string& name) {
    auto it = pImpl->ruleTypes.find(name);
    if (it != pImpl->ruleTypes.end()) {
        pImpl->ruleTypes.erase(it);
        pImpl->ruleParameters.erase(name);
        
        // Remove traces
        pImpl->preTraces.erase("pre_" + name);
        pImpl->postTraces.erase("post_" + name);
        pImpl->eligibilityTraces.erase("eligibility_" + name);
        
        if (pImpl->activeRuleName == name) {
            pImpl->activeRuleName = "";
        }
        
        NLM_LOG_INFO("Removed plasticity rule: " + name);
        return true;
    }
    return false;
}

bool AdvancedPlasticity::setActiveRule(const std::string& name) {
    auto it = pImpl->ruleTypes.find(name);
    if (it != pImpl->ruleTypes.end()) {
        pImpl->activeRuleName = name;
        NLM_LOG_INFO("Set active plasticity rule: " + name);
        return true;
    }
    return false;
}

std::string AdvancedPlasticity::getActiveRule() const {
    return pImpl->activeRuleName;
}

void AdvancedPlasticity::updateSynapse(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes, TimestepDuration dt,
                                      float neuromodulationFactor) {
    if (!synapse) return;
    
    pImpl->totalPlasticityEvents++;
    
    // Apply active rule
    if (!pImpl->activeRuleName.empty()) {
        auto ruleIt = pImpl->ruleTypes.find(pImpl->activeRuleName);
        if (ruleIt != pImpl->ruleTypes.end()) {
            // Apply rule based on type
            switch (ruleIt->second) {
                case PlasticityRuleType::STDP:
                    applySTDPUpdate(synapse, preSpikes, postSpikes, dt, neuromodulationFactor);
                    break;
                case PlasticityRuleType::Hebbian:
                    applyHebbianUpdate(synapse, preSpikes, postSpikes, dt);
                    break;
                case PlasticityRuleType::BCM:
                    applyBCMUpdate(synapse, preSpikes, postSpikes, dt);
                    break;
                case PlasticityRuleType::Oja:
                    applyOjaUpdate(synapse, preSpikes, postSpikes, dt);
                    break;
                case PlasticityRuleType::Homeostatic:
                    applyHomeostaticUpdate(synapse, dt);
                    break;
                default:
                    applySTDPUpdate(synapse, preSpikes, postSpikes, dt, neuromodulationFactor);
                    break;
            }
        }
    }
    
    // Update traces
    updateTrace(synapse, preSpikes, "pre", dt);
    updateTrace(synapse, postSpikes, "post", dt);
    updateTrace(synapse, {}, "eligibility", dt);
    
    // Apply constraints
    enforceWeightLimits(synapse);
}

void AdvancedPlasticity::updateTrace(Synapse* synapse, const std::vector<Timestamp>& spikeTimes,
                                    const std::string& traceType, TimestepDuration dt) {
    if (!synapse) return;
    
    std::string traceKey = traceType + "_" + std::to_string(synapse->getId().index());
    
    if (traceType == "pre") {
        if (pImpl->preTraces.find(traceKey) == pImpl->preTraces.end()) {
            pImpl->preTraces[traceKey].assign(1000, 0.0f);
        }
        
        // Update pre-synaptic trace
        for (float spikeTime : spikeTimes) {
            int index = static_cast<int>(spikeTime / dt) % pImpl->preTraces[traceKey].size();
            pImpl->preTraces[traceKey][index] = 1.0f;
        }
        
        // Decay trace
        float decay = pImpl->config.stdpTraceDecay;
        for (float& value : pImpl->preTraces[traceKey]) {
            value *= decay;
        }
    } 
    else if (traceType == "post") {
        if (pImpl->postTraces.find(traceKey) == pImpl->postTraces.end()) {
            pImpl->postTraces[traceKey].assign(1000, 0.0f);
        }
        
        // Update post-synaptic trace
        for (float spikeTime : spikeTimes) {
            int index = static_cast<int>(spikeTime / dt) % pImpl->postTraces[traceKey].size();
            pImpl->postTraces[traceKey][index] = 1.0f;
        }
        
        // Decay trace
        float decay = pImpl->config.stdpTraceDecay;
        for (float& value : pImpl->postTraces[traceKey]) {
            value *= decay;
        }
    }
    
    else if (traceType == "eligibility") {
        if (pImpl->eligibilityTraces.find(traceKey) == pImpl->eligibilityTraces.end()) {
            pImpl->eligibilityTraces[traceKey].assign(1000, 0.0f);
        }
        
        // Update eligibility trace based on pre and post traces
        float preTrace = getTraceValue(synapse, "pre");
        float postTrace = getTraceValue(synapse, "post");
        
        // Calculate eligibility trace
        float eligibility = (preTrace * postTrace) * pImpl->config.stdpTraceDecay;
        
        int index = static_cast<int>(dt * 1000) % pImpl->eligibilityTraces[traceKey].size();  // Simplified
        pImpl->eligibilityTraces[traceKey][index] = eligibility;
    }
}

float AdvancedPlasticity::getTraceValue(Synapse* synapse, const std::string& traceType) const {
    if (!synapse) return 0.0f;
    
    std::string traceKey = traceType + "_" + std::to_string(synapse->getId().index());
    
    if (traceType == "pre") {
        auto it = pImpl->preTraces.find(traceKey);
        if (it != pImpl->preTraces.end()) {
            return *std::max_element(it->second.begin(), it->second.end());
        }
    } else if (traceType == "post") {
        auto it = pImpl->postTraces.find(traceKey);
        if (it != pImpl->postTraces.end()) {
            return *std::max_element(it->second.begin(), it->second.end());
        }
    } else if (traceType == "eligibility") {
        auto it = pImpl->eligibilityTraces.find(traceKey);
        if (it != pImpl->eligibilityTraces.end()) {
            return *std::max_element(it->second.begin(), it->second.end());
        }
    }
    
    return 0.0f;
}

void AdvancedPlasticity::tagSynapse(Synapse* synapse, const std::string& tagName,
                                    float tagStrength, float tagLifetime) {
    if (!synapse) return;
    
    std::string traceKey = "tag_" + std::to_string(synapse->getId().index());
    
    if (pImpl->synapticTags.find(traceKey) == pImpl->synapticTags.end()) {
        pImpl->synapticTags[traceKey] = std::map<NeuronId, std::vector<float>>();
    }
    
    auto& tagMap = pImpl->synapticTags[traceKey];
    if (tagMap.find(tagName) == tagMap.end()) {
        tagMap[tagName].assign(1000, 0.0f);
    }
    
    int index = static_cast<int>(tagLifetime) % tagMap[tagName].size();  // Simplified
    tagMap[tagName][index] = tagStrength;
    
    NLM_LOG_INFO("Tagged synapse " + std::to_string(synapse->getId().index()) + 
                 " with tag " + tagName + " strength " + std::to_string(tagStrength));
}

bool AdvancedPlasticity::hasTag(Synapse* synapse, const std::string& tagName) const {
    if (!synapse) return false;
    
    std::string traceKey = "tag_" + std::to_string(synapse->getId().index());
    auto it = pImpl->synapticTags.find(traceKey);
    if (it != pImpl->synapticTags.end()) {
        return it->second.find(tagName) != it->second.end();
    }
    return false;
}

float AdvancedPlasticity::getTagStrength(Synapse* synapse, const std::string& tagName) const {
    if (!synapse) return 0.0f;
    
    std::string traceKey = "tag_" + std::to_string(synapse->getId().index());
    auto it = pImpl->synapticTags.find(traceKey);
    if (it != pImpl->synapticTags.end()) {
        auto tagIt = it->second.find(tagName);
        if (tagIt != it->second.end()) {
            return *std::max_element(tagIt->second.begin(), tagIt->second.end());
        }
    }
    return 0.0f;
}

void AdvancedPlasticity::adjustHomeostasis(const std::vector<Synapse*>& synapses,
                                          TimestepDuration dt, float targetActivity) {
    float totalActivity = 0.0f;
    for (Synapse* synapse : synapses) {
        // Simplified: assume postsynaptic neuron activity is related to synaptic weight
        float activity = std::abs(synapse->getWeight()) * 0.1f;  // Approximation
        totalActivity += activity;
    }
    
    if (!synapses.empty()) {
        float currentAverage = totalActivity / synapses.size();
        float error = targetActivity - currentAverage;
        
        // Adjust synaptic weights based on error
        float weightChange = error * pImpl->config.homeostaticLearningRate;
        
        for (Synapse* synapse : synapses) {
            float newWeight = synapse->getWeight() + weightChange * dt;
            newWeight = std::clamp(newWeight, pImpl->config.minWeight, pImpl->config.maxWeight);
            synapse->setWeight(newWeight);
        }
        
        pImpl->ruleActivity["homeostatic"] += std::abs(weightChange);
    }
}

void AdvancedPlasticity::applyPatternLearning(Synapse* synapse, const std::vector<float>& pattern,
                                             float learningRate, const std::string& learningRule) {
    if (!synapse) return;
    
    float weightChange = 0.0f;
    
    if (learningRule == "Oja") {
        // Oja learning rule for PCA
        float output = pattern[0];  // Simplified
        weightChange = learningRate * output * synapse->getWeight() - learningRate * 
                      pImpl->config.ojaRegularization * synapse->getWeight() * synapse->getWeight();
    } else if (learningRule == "Hebbian") {
        // Simple Hebbian learning
        float correlation = 0.0f;
        for (float value : pattern) {
            correlation += value;
        }
        correlation /= pattern.size();
        weightChange = learningRate * correlation;
    }
    
    // Apply weight change
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, pImpl->config.minWeight, pImpl->config.maxWeight);
    synapse->setWeight(newWeight);
    
    pImpl->totalWeightChange += std::abs(weightChange);
}

void AdvancedPlasticity::applyRewardModulation(Synapse* synapse, float reward,
                                               float decayRate) {
    if (!synapse) return;
    
    // Apply reward modulation to eligibility trace
    float eligibility = getTraceValue(synapse, "eligibility");
    float modulatedEligibility = eligibility * (1.0f + reward * pImpl->config.rewardModulationFactor);
    
    // Convert back to weight change
    float weightChange = modulatedEligibility * decayRate;
    
    // Apply weight change
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, pImpl->config.minWeight, pImpl->config.maxWeight);
    synapse->setWeight(newWeight);
    
    pImpl->totalWeightChange += std::abs(weightChange);
    pImpl->ruleActivity["reward_modulated"] += std::abs(weightChange);
}

void AdvancedPlasticity::applyPredictionError(Synapse* synapse, float error,
                                            float learningRate) {
    if (!synapse) return;
    
    // Apply prediction error learning
    float weightChange = error * learningRate * pImpl->config.predictionErrorModulation;
    
    // Apply weight change
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, pImpl->config.minWeight, pImpl->config.maxWeight);
    synapse->setWeight(newWeight);
    
    pImpl->totalWeightChange += std::abs(weightChange);
    pImpl->ruleActivity["prediction_error"] += std::abs(weightChange);
}

void AdvancedPlasticity::applyTripletSTDP(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes, TimestepDuration dt) {
    if (!synapse) return;
    
    // Simplified triplet STDP implementation
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potentiation
                float delta = pImpl->config.stdpLTPWeight * std::exp(-dt / pImpl->config.stdpTimeConstant);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: depression
                float delta = -pImpl->config.stdpLTDWeight * std::exp(dt / pImpl->config.stdpTimeConstant);
                totalDelta += delta;
            }
        }
    }
    
    // Apply triplet factor
    totalDelta *= 1.5f;  // Triplet multiplier
    
    // Apply weight change
    float newWeight = synapse->getWeight() + totalDelta;
    newWeight = std::clamp(newWeight, pImpl->config.minWeight, pImpl->config.maxWeight);
    synapse->setWeight(newWeight);
    
    pImpl->totalWeightChange += std::abs(totalDelta);
    pImpl->ruleActivity["triplet_stdp"] += std::abs(totalDelta);
}

void AdvancedPlasticity::applyRewardModulatedSTDP(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                                                 const std::vector<Timestamp>& postSpikes,
                                                 float reward, TimestepDuration dt) {
    if (!synapse) return;
    
    // Reward-modulated STDP
    applySTDPUpdate(synapse, preSpikes, postSpikes, dt, 1.0f + reward * pImpl->config.rewardModulationFactor);
}

void AdvancedPlasticity::applyBCM(Synapse* synapse, float postsynapticActivity,
                                 const std::vector<Timestamp>& postSpikes, TimestepDuration dt) {
    if (!synapse) return;
    
    // Simplified BCM implementation
    float slidingThreshold = pImpl->config.bcmSlidingThreshold;
    
    // Update sliding threshold based on postsynaptic activity
    float activityChange = postsynapticActivity - slidingThreshold;
    slidingThreshold += pImpl->config.bcmLearningRate * activityChange * dt;
    
    // Clamp threshold
    slidingThreshold = std::clamp(slidingThreshold, pImpl->config.bcmThreshold, 1.0f);
    
    // Apply BCM weight change
    float weightChange = pImpl->config.bcmLearningRate * postsynapticActivity * 
                        std::max(0.0f, postsynapticActivity - slidingThreshold);
    
    // Apply weight change
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, pImpl->config.minWeight, pImpl->config.maxWeight);
    synapse->setWeight(newWeight);
    
    pImpl->totalWeightChange += std::abs(weightChange);
    pImpl->ruleActivity["BCM"] += std::abs(weightChange);
}

void AdvancedPlasticity::applyOja(Synapse* synapse, const std::vector<float>& output,
                                 float learningRate) {
    if (!synapse) return;
    
    // Oja learning rule for PCA
    float weightChange = learningRate * output[0] * synapse->getWeight() - learningRate * 
                         pImpl->config.ojaRegularization * synapse->getWeight() * synapse->getWeight();
    
    // Apply weight change
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, pImpl->config.minWeight, pImpl->config.maxWeight);
    synapse->setWeight(newWeight);
    
    pImpl->totalWeightChange += std::abs(weightChange);
    pImpl->ruleActivity["oja"] += std::abs(weightChange);
}

void AdvancedPlasticity::applyConstrainedHebbian(Synapse* synapse,
                                                const std::vector<Timestamp>& preSpikes,
                                                const std::vector<Timestamp>& postSpikes,
                                                float maxWeightChange) {
    if (!synapse) return;
    
    // Calculate Hebbian weight change
    float totalDelta = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Simple correlation
            if (preTime != postTime) {
                totalDelta += 0.01f;  // Fixed correlation factor
            }
        }
    }
    
    // Apply constraint
    totalDelta = std::clamp(totalDelta, -maxWeightChange, maxWeightChange);
    
    // Apply weight change
    float newWeight = synapse->getWeight() + totalDelta;
    newWeight = std::clamp(newWeight, pImpl->config.minWeight, pImpl->config.maxWeight);
    synapse->setWeight(newWeight);
    
    pImpl->totalWeightChange += std::abs(totalDelta);
    pImpl->ruleActivity["constrained_hebbian"] += std::abs(totalDelta);
}

void AdvancedPlasticity::applySTDPUpdate(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes, 
                                        TimestepDuration dt, float neuromodulationFactor) {
    if (!synapse) return;
    
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potentiation
                float delta = pImpl->config.stdpLTPWeight * std::exp(-dt / pImpl->config.stdpTimeConstant);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: depression
                float delta = -pImpl->config.stdpLTDWeight * std::exp(dt / pImpl->config.stdpTimeConstant);
                totalDelta += delta;
            }
        }
    }
    
    // Apply neuromodulation factor
    totalDelta *= neuromodulationFactor;
    
    // Apply weight change
    float newWeight = synapse->getWeight() + totalDelta;
    newWeight = std::clamp(newWeight, pImpl->config.minWeight, pImpl->config.maxWeight);
    synapse->setWeight(newWeight);
    
    pImpl->totalWeightChange += std::abs(totalDelta);
    pImpl->ruleActivity["stdp"] += std::abs(totalDelta);
}

void AdvancedPlasticity::applyHebbianUpdate(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                                          const std::vector<Timestamp>& postSpikes, TimestepDuration dt) {
    if (!synapse) return;
    
    // Calculate Hebbian weight change based on spike correlations
    float totalDelta = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Simple Hebbian: if both fired in same time window, potentiate
            float timeDiff = std::abs(static_cast<float>(postTime - preTime));
            if (timeDiff < dt * 2) {  // Within 2 time steps
                totalDelta += pImpl->config.hebbianLearningRate;
            }
        }
    }
    
    // Apply weight change
    float newWeight = synapse->getWeight() + totalDelta;
    newWeight = std::clamp(newWeight, pImpl->config.minWeight, pImpl->config.maxWeight);
    synapse->setWeight(newWeight);
    
    pImpl->totalWeightChange += std::abs(totalDelta);
    pImpl->ruleActivity["hebbian"] += std::abs(totalDelta);
}

void AdvancedPlasticity::applyHomeostaticUpdate(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Apply homeostatic weight change
    float activity = std::abs(synapse->getWeight()) * 0.1f;  // Approximation
    float error = pImpl->config.homeostaticTarget - activity;
    
    float weightChange = error * pImpl->config.homeostaticLearningRate * dt;
    
    // Apply weight change
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, pImpl->config.minWeight, pImpl->config.maxWeight);
    synapse->setWeight(newWeight);
    
    pImpl->totalWeightChange += std::abs(weightChange);
    pImpl->ruleActivity["homeostatic"] += std::abs(weightChange);
}

void AdvancedPlasticity::applyOjaUpdate(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes, TimestepDuration dt) {
    // Simplified Oja update
    applyOja(synapse, {synapse->getWeight()}, pImpl->config.ojaLearningRate);
}

void AdvancedPlasticity::resetTraces(Synapse* synapse) {
    if (!synapse) return;
    
    std::string traceKey = "pre_" + std::to_string(synapse->getId().index());
    if (pImpl->preTraces.find(traceKey) != pImpl->preTraces.end()) {
        for (float& value : pImpl->preTraces[traceKey]) {
            value = 0.0f;
        }
    }
    
    traceKey = "post_" + std::to_string(synapse->getId().index());
    if (pImpl->postTraces.find(traceKey) != pImpl->postTraces.end()) {
        for (float& value : pImpl->postTraces[traceKey]) {
            value = 0.0f;
        }
    }
    
    traceKey = "eligibility_" + std::to_string(synapse->getId().index());
    if (pImpl->eligibilityTraces.find(traceKey) != pImpl->eligibilityTraces.end()) {
        for (float& value : pImpl->eligibilityTraces[traceKey]) {
            value = 0.0f;
        }
    }
}

void AdvancedPlasticity::enforceWeightLimits(Synapse* synapse) {
    if (!synapse) return;
    
    float weight = synapse->getWeight();
    weight = std::clamp(weight, pImpl->config.minWeight, pImpl->config.maxWeight);
    synapse->setWeight(weight);
}

void AdvancedPlasticity::setCustomLearningRule(const std::string& name,
                                              std::function<void(Synapse*, const std::vector<Timestamp>&,
                                                               const std::vector<Timestamp>&, TimestepDuration,
                                                               float)> customRule) {
    pImpl->customRules[name] = customRule;
    NLM_LOG_INFO("Set custom learning rule: " + name);
}

void AdvancedPlasticity::tagSynapse(Synapse* synapse, const std::string& tagName,
                                    float tagStrength, float tagLifetime) {
    if (!synapse) return;
    
    // Implementation would store tags for later retrieval
    NLM_LOG_INFO("Tagged synapse " + std::to_string(synapse->getId().index()) + 
                 " with tag " + tagName + " strength " + std::to_string(tagStrength));
}

bool AdvancedPlasticity::hasTag(Synapse* synapse, const std::string& tagName) const {
    if (!synapse) return false;
    
    // Implementation would check if synapse has the specified tag
    return false;
}

float AdvancedPlasticity::getTagStrength(Synapse* synapse, const std::string& tagName) const {
    if (!synapse) return 0.0f;
    
    // Implementation would return tag strength
    return 0.0f;
}

AdvancedPlasticity::PlasticityStats AdvancedPlasticity::calculateStats() const {
    PlasticityStats stats;
    
    stats.totalWeightChange = pImpl->totalWeightChange;
    stats.plasticityRate = pImpl->totalPlasticityEvents > 0 ? 
                           pImpl->totalWeightChange / pImpl->totalPlasticityEvents : 0.0f;
    
    // Copy rule stats
    stats.ruleStats = pImpl->ruleActivity;
    
    // Calculate trace stats (simplified)
    for (const auto& tracePair : pImpl->preTraces) {
        float avgTrace = 0.0f;
        for (float value : tracePair.second) {
            avgTrace += value;
        }
        if (!tracePair.second.empty()) {
            avgTrace /= tracePair.second.size();
        }
        stats.traceStats[tracePair.first] = avgTrace;
    }
    
    // Calculate homeostatic activity
    if (!pImpl->ruleActivity.empty()) {
        float totalActivity = 0.0f;
        for (const auto& activity : pImpl->ruleActivity) {
            totalActivity += activity.second;
        }
        stats.homeostaticActivity = totalActivity / pImpl->ruleActivity.size();
    }
    
    // Calculate neuromodulation effect (simplified)
    stats.neuromodulationEffect = 1.0f;  // Placeholder
    
    return stats;
}

bool AdvancedPlasticity::exportConfiguration(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for export: " + filepath);
            return false;
        }
        
        // Export configuration (simplified JSON-like format)
        file << "AdvancedPlasticityConfig {\n";
        file << "  activeRule = \"" << pImpl->activeRuleName << "\"\n";
        file << "  stdpLTPWeight = " << pImpl->config.stdpLTPWeight << "\n";
        file << "  stdpLTDWeight = " << pImpl->config.stdpLTDWeight << "\n";
        file << "  stdpTimeConstant = " << pImpl->config.stdpTimeConstant << "\n";
        file << "  hebbianLearningRate = " << pImpl->config.hebbianLearningRate << "\n";
        file << "  homeostaticTarget = " << pImpl->config.homeostaticTarget << "\n";
        file << "  minWeight = " << pImpl->config.minWeight << "\n";
        file << "  maxWeight = " << pImpl->config.maxWeight << "\n";
        file << "}\n";
        
        file.close();
        NLM_LOG_INFO("Plasticity configuration exported to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception exporting plasticity configuration: ") + e.what());
        return false;
    }
}

bool AdvancedPlasticity::importConfiguration(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for import: " + filepath);
            return false;
        }
        
        // Parse configuration (simplified)
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("activeRule = \"") != std::string::npos) {
                size_t start = line.find("\"") + 1;
                size_t end = line.rfind("\"");
                if (start != std::string::npos && end != std::string::npos) {
                    pImpl->activeRuleName = line.substr(start, end - start);
                }
            } else if (line.find("stdpLTPWeight = ") != std::string::npos) {
                size_t pos = line.find(" = ");
                if (pos != std::string::npos) {
                    pImpl->config.stdpLTPWeight = std::stof(line.substr(pos + 3));
                }
            } else if (line.find("stdpLTDWeight = ") != std::string::npos) {
                size_t pos = line.find(" = ");
                if (pos != std::string::npos) {
                    pImpl->config.stdpLTDWeight = std::stof(line.substr(pos + 3));
                }
            }
        }
        
        file.close();
        NLM_LOG_INFO("Plasticity configuration imported from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception importing plasticity configuration: ") + e.what());
        return false;
    }
}

std::vector<std::string> AdvancedPlasticity::getRuleNames() const {
    std::vector<std::string> names;
    for (const auto& rulePair : pImpl->ruleTypes) {
        names.push_back(rulePair.first);
    }
    return names;
}

PlasticityRuleType AdvancedPlasticity::getRuleType(const std::string& name) const {
    auto it = pImpl->ruleTypes.find(name);
    if (it != pImpl->ruleTypes.end()) {
        return it->second;
    }
    return PlasticityRuleType::STDP;
}

const AdvancedPlasticityConfig& AdvancedPlasticity::getConfig() const {
    return pImpl->config;
}

void AdvancedPlasticity::logStatus() const {
    PlasticityStats stats = calculateStats();
    
    NLM_LOG_INFO("=== Advanced Plasticity Status ===");
    NLM_LOG_INFO("Active Rule: " + pImpl->activeRuleName);
    NLM_LOG_INFO("Total Plasticity Events: " + std::to_string(pImpl->totalPlasticityEvents));
    NLM_LOG_INFO("Total Weight Change: " + std::to_string(stats.totalWeightChange));
    NLM_LOG_INFO("Plasticity Rate: " + std::to_string(stats.plasticityRate));
    
    NLM_LOG_INFO("--- Rule Activity ---");
    for (const auto& activity : pImpl->ruleActivity) {
        NLM_LOG_INFO("  " + activity.first + ": " + std::to_string(activity.second));
    }
    
    NLM_LOG_INFO("--- Configuration ---");
    NLM_LOG_INFO("STDP LTP Weight: " + std::to_string(pImpl->config.stdpLTPWeight));
    NLM_LOG_INFO("STDP LTD Weight: " + std::to_string(pImpl->config.stdpLTDWeight));
    NLM_LOG_INFO("STDP Time Constant: " + std::to_string(pImpl->config.stdpTimeConstant));
    NLM_LOG_INFO("Hebbian Learning Rate: " + std::to_string(pImpl->config.hebbianLearningRate));
    NLM_LOG_INFO("Homeostatic Target: " + std::to_string(pImpl->config.homeostaticTarget));
    
    NLM_LOG_INFO("=== End Advanced Plasticity Status ===");
}

} // namespace nlm
