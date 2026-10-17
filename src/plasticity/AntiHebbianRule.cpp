#include "AdvancedPlasticityRules.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float baseline;
    float correlationWindow;
    float activityMean;
    size_t sampleCount;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             baseline(0.0f), correlationWindow(100.0f),
             activityMean(0.0f), sampleCount(0) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {
    // Initialize plugin info
    PlasticityRuleInfo& info = const_cast<PlasticityRuleInfo&>(getPluginInfo());
    info.name = "AntiHebbianRule";
    info.version = "2.0.0";
    info.description = "Anti-Hebbian learning rule that decreases weight when neurons fire together";
    info.author = "NLM Team";
}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    /*
     * Enhanced Anti-Hebbian learning implementation
     * 
     * Mathematical formulation:
     * Δw = -η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * This implements "neurons that fire together, disconnect together"
     * Anti-Hebbian plasticity promotes competition and sparse activity patterns
     * 
     * Biological inspiration:
     *   - Mirror neurons with inhibitory components
     *   - Homeostatic plasticity for stability
     *   - Competitive learning in sensory systems
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (with temporal window)
    size_t correlationCount = 0;
    size_t preSpikeCount = preSpikes.size();
    size_t postSpikeCount = postSpikes.size();
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within correlation window as correlated
            if (std::abs(dt) < pImpl->correlationWindow) {
                ++correlationCount;
            }
        }
    }
    
    // Update activity statistics
    if (pImpl->sampleCount < 1000) {
        pImpl->sampleCount++;
        float delta = static_cast<float>(correlationCount) - pImpl->activityMean;
        pImpl->activityMean += delta / static_cast<float>(pImpl->sampleCount);
    }
    
    // Compute weight change based on anti-Hebbian rule
    // More correlated activity = more weight reduction
    float delta = -pImpl->learningRate * static_cast<float>(correlationCount) - pImpl->baseline;
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbianRule";
}

const char* AntiHebbianRule::getDescription() const {
    return "Anti-Hebbian learning rule that decreases weight when neurons fire together";
}

const char* AntiHebbianRule::getVersion() const {
    return "2.0.0";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float AntiHebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void AntiHebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float AntiHebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

std::unordered_map<std::string, std::string> AntiHebbianRule::getParameters() const {
    std::unordered_map<std::string, std::string> params;
    params["learning_rate"] = std::to_string(pImpl->learningRate);
    params["max_weight"] = std::to_string(pImpl->maxWeight);
    params["min_weight"] = std::to_string(pImpl->minWeight);
    params["baseline"] = std::to_string(pImpl->baseline);
    params["correlation_window"] = std::to_string(pImpl->correlationWindow);
    return params;
}

void AntiHebbianRule::setParameters(const std::unordered_map<std::string, std::string>& params) {
    auto it = params.find("learning_rate");
    if (it != params.end()) {
        setLearningRate(std::stof(it->second));
    }
    
    it = params.find("max_weight");
    if (it != params.end()) {
        setMaxWeight(std::stof(it->second));
    }
    
    it = params.find("min_weight");
    if (it != params.end()) {
        setMinWeight(std::stof(it->second));
    }
    
    it = params.find("baseline");
    if (it != params.end()) {
        setBaseline(std::stof(it->second));
    }
    
    it = params.find("correlation_window");
    if (it != params.end()) {
        setCorrelationWindow(std::stof(it->second));
    }
}

bool AntiHebbianRule::hasParameter(const std::string& name) const {
    static const std::vector<std::string> paramNames = {
        "learning_rate", "max_weight", "min_weight", "baseline", "correlation_window"
    };
    return std::find(paramNames.begin(), paramNames.end(), name) != paramNames.end();
}

const char* AntiHebbianRule::getParameterName(size_t index) const {
    static const std::vector<std::string> paramNames = {
        "learning_rate", "max_weight", "min_weight", "baseline", "correlation_window"
    };
    if (index < paramNames.size()) {
        return paramNames[index].c_str();
    }
    return nullptr;
}

std::unique_ptr<PlasticityRule> AntiHebbianRule::clone() const {
    auto clone = std::make_unique<AntiHebbianRule>();
    *clone->pImpl = *pImpl; // Deep copy
    return clone;
}

bool AntiHebbianRule::validate() const {
    if (pImpl->learningRate < 0.0f || pImpl->learningRate > 1.0f) return false;
    if (pImpl->maxWeight <= pImpl->minWeight) return false;
    if (pImpl->correlationWindow < 0.0f) return false;
    return true;
}

} // namespace nlm