// Modern C++ Implementation of PlasticityRule.hpp with Strategy Pattern
// This file implements the modern C++ version of the PlasticityRule.hpp header

#include "PlasticityRule.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// PlasticityRule Factory Implementation
std::expected<std::unique_ptr<PlasticityRule>, PlasticityError> 
PlasticityRule::createRule(const std::string& ruleType, const PlasticityConfig& config) {
    if (ruleType == "Hebbian") {
        return std::make_unique<HebbianRule>(config);
    } else if (ruleType == "AntiHebbian") {
        return std::make_unique<AntiHebbianRule>(config);
    } else if (ruleType == "BCM") {
        return std::make_unique<BCMRule>(config);
    } else if (ruleType == "STDP") {
        return std::make_unique<STDPRule>(config);
    }
    return std::unexpected(PlasticityError::InvalidParameters);
}

// PlasticityRuleFactory Implementation
std::expected<std::unique_ptr<PlasticityRule>, PlasticityError> 
PlasticityRuleFactory::createRule(const std::string& ruleType, const PlasticityConfig& config) {
    return PlasticityRule::createRule(ruleType, config);
}

// HebbianRule Implementation
HebbianRule::HebbianRule(const PlasticityConfig& ruleConfig) : PlasticityRule(ruleConfig) {}

HebbianRule::~HebbianRule() = default;

std::optional<PlasticityError> HebbianRule::update(
    Synapse& synapse,
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes,
    TimestepDuration dt) {
    
    if (!config.enabled) {
        return std::unexpected(PlasticityError::PlasticityDisabled);
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return std::unexpected(PlasticityError::InsufficientData);
    }
    
    // Simplified Hebbian update: strengthen synapse for correlated activity
    float correlation = calculateCorrelation(preSpikes, postSpikes);
    
    if (correlation < getMinCorrelation()) {
        return std::nullopt;  // No significant correlation, no update
    }
    
    SynapticWeight delta = config.learningRate * static_cast<float>(correlation);
    
    return applyWeightChange(synapse, delta);
}

std::optional<PlasticityError> HebbianRule::applyWeightChange(
    Synapse& synapse, SynapticWeight delta) {
    
    float newWeight = synapse.getWeight() + delta;
    
    if (newWeight < config.minWeight || newWeight > config.maxWeight) {
        return std::unexpected(PlasticityError::WeightBoundsViolation);
    }
    
    synapse.setWeight(newWeight);
    return std::nullopt;
}

float HebbianRule::calculateCorrelation(
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes) const {
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    float sumPre = 0.0f, sumPost = 0.0f;
    float sumPrePost = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        sumPre += preTime;
        for (Timestamp postTime : postSpikes) {
            sumPost += postTime;
            sumPrePost += preTime * postTime;
        }
    }
    
    float meanPre = sumPre / preSpikes.size();
    float meanPost = sumPost / (preSpikes.size() * postSpikes.size());
    
    float numerator = sumPrePost - preSpikes.size() * postSpikes.size() * meanPre * meanPost;
    float denominator = std::sqrt(sumPrePost - meanPre * sumPost);
    
    return denominator > 0.0f ? numerator / denominator : 0.0f;
}

// AntiHebbianRule Implementation
AntiHebbianRule::AntiHebbianRule(const PlasticityConfig& ruleConfig) : PlasticityRule(ruleConfig) {}

std::optional<PlasticityError> AntiHebbianRule::update(
    Synapse& synapse,
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes,
    TimestepDuration dt) {
    
    if (!config.enabled) {
        return std::unexpected(PlasticityError::PlasticityDisabled);
    }
    
    // Simplified anti-Hebbian: weaken synapse for correlated activity
    float correlation = calculateCorrelation(preSpikes, postSpikes);
    
    if (correlation < getMinCorrelation()) {
        return std::nullopt;
    }
    
    SynapticWeight delta = -config.learningRate * static_cast<float>(correlation);
    
    return applyWeightChange(synapse, delta);
}

std::optional<PlasticityError> AntiHebbianRule::applyWeightChange(
    Synapse& synapse, SynapticWeight delta) {
    
    float newWeight = synapse.getWeight() + delta;
    
    if (newWeight < config.minWeight || newWeight > config.maxWeight) {
        return std::unexpected(PlasticityError::WeightBoundsViolation);
    }
    
    synapse.setWeight(newWeight);
    return std::nullopt;
}

// BCMRule Implementation
BCMRule::BCMRule(const PlasticityConfig& ruleConfig) : PlasticityRule(ruleConfig) {}

std::optional<PlasticityError> BCMRule::update(
    Synapse& synapse,
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes,
    TimestepDuration dt) {
    
    if (!config.enabled) {
        return std::unexpected(PlasticityError::PlasticityDisabled);
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return std::unexpected(PlasticityError::InsufficientData);
    }
    
    // Simplified BCM update: use post-synaptic activity (rho)
    float rho = calculatePostSynapticActivity(postSpikes);
    float theta = getTheta();
    
    float delta = rho > theta ? config.learningRate * (rho - theta) : 0.0f;
    
    return applyWeightChange(synapse, delta);
}

std::optional<PlasticityError> BCMRule::applyWeightChange(
    Synapse& synapse, SynapticWeight delta) {
    
    float newWeight = synapse.getWeight() + delta;
    
    if (newWeight < config.minWeight || newWeight > config.maxWeight) {
        return std::unexpected(PlasticityError::WeightBoundsViolation);
    }
    
    synapse.setWeight(newWeight);
    return std::nullopt;
}

float BCMRule::calculatePostSynapticActivity(
    const std::vector<Timestamp>& postSpikes) const {
    
    if (postSpikes.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (Timestamp t : postSpikes) {
        sum += t;
    }
    
    return sum / postSpikes.size();
}

// STDPRule Implementation
STDPRule::STDPRule(const PlasticityConfig& ruleConfig) : PlasticityRule(ruleConfig) {}

std::optional<PlasticityError> STDPRule::update(
    Synapse& synapse,
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes,
    TimestepDuration dt) {
    
    if (!config.enabled) {
        return std::unexpected(PlasticityError::PlasticityDisabled);
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return std::unexpected(PlasticityError::InsufficientData);
    }
    
    // STDP update: consider timing relationships between spikes
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: Potentiation
                float delta = getLTPWeight() * std::exp(-dt / config.timeConstant);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: Depression
                float delta = -getLTDWeight() * std::exp(dt / config.timeConstant);
                totalDelta += delta;
            }
        }
    }
    
    return applyWeightChange(synapse, totalDelta);
}

std::optional<PlasticityError> STDPRule::applyWeightChange(
    Synapse& synapse, SynapticWeight delta) {
    
    float newWeight = synapse.getWeight() + delta;
    
    if (newWeight < config.minWeight || newWeight > config.maxWeight) {
        return std::unexpected(PlasticityError::WeightBoundsViolation);
    }
    
    synapse.setWeight(newWeight);
    return std::nullopt;
}

} // namespace nlm