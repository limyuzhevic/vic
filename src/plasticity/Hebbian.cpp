#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>

namespace nlm {

struct HebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float baseline;
    bool useVariance;
    float correlationWindow;
    float variance;
    float activityMean;
    size_t sampleCount;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             baseline(0.0f), useVariance(false), correlationWindow(100.0f),
             variance(0.0f), activityMean(0.0f), sampleCount(0) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {
    // Initialize plugin info
    PlasticityRuleInfo& info = const_cast<PlasticityRuleInfo&>(getPluginInfo());
    info.name = "HebbianRule";
    info.version = "2.0.0";
    info.description = "Enhanced Hebbian learning with variance tracking and configurable parameters";
    info.author = "NLM Team";
}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    /*
     * Enhanced Hebbian learning implementation with variance tracking
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Advanced features:
     * - Running variance and mean estimation
     * - Configurable correlation window
     * - Baseline subtraction for stability
     * - Optional variance-based learning
     * 
     * Where:
     *   η = learningRate
     *   baseline = baseline (for stability)
     *   Window = correlationWindow (temporal correlation)
     * 
     * Biological inspiration:
     *   - Reflects synaptic tagging and capture
     *   - Short-term plasticity interactions
     *   - Homeostatic scaling mechanisms
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (with temporal window)
    size_t correlationCount = 0;
    size_t preSpikeCount = 0;
    size_t postSpikeCount = 0;
    
    // Update activity statistics
    preSpikeCount = preSpikes.size();
    postSpikeCount = postSpikes.size();
    
    // Calculate covariance with temporal windowing
    float covariance = 0.0f;
    float preMean = static_cast<float>(preSpikeCount) / preSpikes.size();
    float postMean = static_cast<float>(postSpikeCount) / postSpikes.size();
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within correlation window as correlated
            if (std::abs(dt) < pImpl->correlationWindow) {
                ++correlationCount;
                covariance += (1.0f - preMean) * (1.0f - postMean);
            }
        }
    }
    
    // Update running statistics
    if (pImpl->sampleCount < 1000) {
        pImpl->sampleCount++;
        float deltaPre = static_cast<float>(preSpikeCount) - pImpl->activityMean;
        pImpl->activityMean += deltaPre / static_cast<float>(pImpl->sampleCount);
        
        float deltaVar = static_cast<float>(correlationCount) - pImpl->variance;
        pImpl->variance += deltaVar / static_cast<float>(pImpl->sampleCount);
    }
    
    // Compute weight change based on enhanced Hebbian rule
    float delta = 0.0f;
    
    if (pImpl->useVariance) {
        // Variance-based learning for more stable convergence
        float normalizedCovariance = covariance / (preSpikeCount * postSpikeCount + 1e-6f);
        delta = pImpl->learningRate * normalizedCovariance - pImpl->baseline;
    } else {
        // Standard Hebbian with baseline for stability
        delta = pImpl->learningRate * static_cast<float>(correlationCount) - pImpl->baseline;
    }
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "HebbianRule";
}

const char* HebbianRule::getDescription() const {
    return "Enhanced Hebbian learning with variance tracking and configurable parameters";
}

const char* HebbianRule::getVersion() const {
    return "2.0.0";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float HebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void HebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float HebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

void HebbianRule::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float HebbianRule::getBaseline() const {
    return pImpl->baseline;
}

void HebbianRule::setUseVariance(bool useVariance) {
    pImpl->useVariance = useVariance;
}

bool HebbianRule::getUseVariance() const {
    return pImpl->useVariance;
}

void HebbianRule::setCorrelationWindow(float window) {
    pImpl->correlationWindow = std::max(0.0f, window);
}

float HebbianRule::getCorrelationWindow() const {
    return pImpl->correlationWindow;
}

std::unordered_map<std::string, std::string> HebbianRule::getParameters() const {
    std::unordered_map<std::string, std::string> params;
    params["learning_rate"] = std::to_string(pImpl->learningRate);
    params["max_weight"] = std::to_string(pImpl->maxWeight);
    params["min_weight"] = std::to_string(pImpl->minWeight);
    params["baseline"] = std::to_string(pImpl->baseline);
    params["use_variance"] = pImpl->useVariance ? "true" : "false";
    params["correlation_window"] = std::to_string(pImpl->correlationWindow);
    return params;
}

void HebbianRule::setParameters(const std::unordered_map<std::string, std::string>& params) {
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
    
    it = params.find("use_variance");
    if (it != params.end()) {
        setUseVariance(it->second == "true" || it->second == "1");
    }
    
    it = params.find("correlation_window");
    if (it != params.end()) {
        setCorrelationWindow(std::stof(it->second));
    }
}

bool HebbianRule::hasParameter(const std::string& name) const {
    static const std::vector<std::string> paramNames = {
        "learning_rate", "max_weight", "min_weight", "baseline", 
        "use_variance", "correlation_window"
    };
    return std::find(paramNames.begin(), paramNames.end(), name) != paramNames.end();
}

const char* HebbianRule::getParameterName(size_t index) const {
    static const std::vector<std::string> paramNames = {
        "learning_rate", "max_weight", "min_weight", "baseline", 
        "use_variance", "correlation_window"
    };
    if (index < paramNames.size()) {
        return paramNames[index].c_str();
    }
    return nullptr;
}

std::unique_ptr<PlasticityRule> HebbianRule::clone() const {
    auto clone = std::make_unique<HebbianRule>();
    *clone->pImpl = *pImpl; // Deep copy
    return clone;
}

bool HebbianRule::validate() const {
    if (pImpl->learningRate < 0.0f || pImpl->learningRate > 1.0f) return false;
    if (pImpl->maxWeight <= pImpl->minWeight) return false;
    if (pImpl->correlationWindow < 0.0f) return false;
    return true;
}

} // namespace nlm