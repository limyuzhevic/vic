#include "AdvancedPlasticityRules.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>

namespace nlm {

struct BCMRule::Impl {
    float theta;              // BCM threshold variable
    float thetaMax;            // Maximum theta value
    float thetaMin;            // Minimum theta value
    float tau;                 // Adaptation time constant
    float learningRate;        // Learning rate
    float pin;                 // BCM pinning constant
    bool hebbianModulation;    // Whether to use Hebbian modulation
    float hebbianWeight;       // Hebbian weight for modulation
    float activityMean;        // Running mean of postsynaptic activity
    float activityVariance;    // Running variance of postsynaptic activity
    size_t sampleCount;         // Number of samples collected
    
    Impl() : theta(0.0f), thetaMax(1.0f), thetaMin(0.0f), tau(20.0f),
             learningRate(0.01f), pin(0.1f), hebbianModulation(true),
             hebbianWeight(1.0f), activityMean(0.0f), activityVariance(0.0f),
             sampleCount(0) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {
    // Initialize plugin info
    PlasticityRuleInfo& info = const_cast<PlasticityRuleInfo&>(getPluginInfo());
    info.name = "BCMRule";
    info.version = "2.0.0";
    info.description = "Bienenstock-Cooper-Munro (BCM) learning rule with adaptive threshold";
    info.author = "NLM Team";
}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    /*
     * Enhanced BCM (Bienenstock-Cooper-Munro) learning implementation
     * 
     * Mathematical formulation:
     * 
     * if postActivity > theta:
     *   Δw = η * (preActivity - pin) * postActivity * hebbianWeight
     * else:
     *   Δw = η * (preActivity - pin) * postActivity * hebbianWeight
     * 
     * theta(t+1) = theta(t) + (tau)^-1 * (τ-1) * (postActivity - theta)
     * 
     * Where:
     *   θ = threshold variable (adapts to postsynaptic activity)
     *   η = learningRate
     *   pin = BCM pinning constant
     *   τ = time constant for theta adaptation
     * 
     * Biological inspiration:
     *   - Synaptic normalization for stability
     *   - Competitive learning in cortex
     *   - Activity-dependent synaptic modification
     *   - Sparse coding through threshold adaptation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate postsynaptic activity (rate of post-synaptic spikes)
    float postActivity = static_cast<float>(postSpikes.size()) / (dt + 0.001f);
    
    // Update running statistics for postsynaptic activity
    if (pImpl->sampleCount < 1000) {
        pImpl->sampleCount++;
        float deltaMean = postActivity - pImpl->activityMean;
        pImpl->activityMean += deltaMean / static_cast<float>(pImpl->sampleCount);
        
        float deltaVar = (postActivity - pImpl->activityMean) * (postActivity - pImpl->activityMean);
        pImpl->activityVariance += deltaVar / static_cast<float>(pImpl->sampleCount);
    }
    
    // Calculate pre-synaptic activity
    float preActivity = static_cast<float>(preSpikes.size()) / (dt + 0.001f);
    
    // Compute weight change using BCM rule
    float delta = 0.0f;
    
    if (pImpl->hebbianModulation) {
        // Standard BCM with Hebbian modulation
        delta = pImpl->learningRate * (preActivity - pImpl->pin) * postActivity * pImpl->hebbianWeight;
    } else {
        // Simplified BCM without Hebbian modulation
        delta = pImpl->learningRate * (preActivity - pImpl->pin) * postActivity;
    }
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
    
    // Update BCM threshold (theta)
    float thetaChange = (postActivity - pImpl->theta) / pImpl->tau;
    pImpl->theta += thetaChange;
    
    // Clamp theta to reasonable bounds
    pImpl->theta = std::clamp(pImpl->theta, pImpl->thetaMin, pImpl->thetaMax);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    // BCM typically keeps weights positive, but allow for normalization
    newWeight = std::clamp(newWeight, 0.0f, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCMRule";
}

const char* BCMRule::getDescription() const {
    return "Bienenstock-Cooper-Munro (BCM) learning rule with adaptive threshold";
}

const char* BCMRule::getVersion() const {
    return "2.0.0";
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setTau(float tau) {
    pImpl->tau = std::max(1.0f, tau);
}

float BCMRule::getTau() const {
    return pImpl->tau;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setPin(float pin) {
    pImpl->pin = std::clamp(pin, 0.0f, 1.0f);
}

float BCMRule::getPin() const {
    return pImpl->pin;
}

void BCMRule::setThetaUpdateRule(const std::string& rule) {
    // For now, just track the rule choice
    if (rule == "adaptive") {
        // Adaptive theta update
    } else if (rule == "fixed") {
        // Fixed theta
    }
}

const char* BCMRule::getThetaUpdateRule() const {
    return pImpl->hebbianModulation ? "adaptive" : "fixed";
}

void BCMRule::setHebbianModulation(bool modulate) {
    pImpl->hebbianModulation = modulate;
}

bool BCMRule::getHebbianModulation() const {
    return pImpl->hebbianModulation;
}

std::unordered_map<std::string, std::string> BCMRule::getParameters() const {
    std::unordered_map<std::string, std::string> params;
    params["theta"] = std::to_string(pImpl->theta);
    params["theta_max"] = std::to_string(pImpl->thetaMax);
    params["theta_min"] = std::to_string(pImpl->thetaMin);
    params["tau"] = std::to_string(pImpl->tau);
    params["learning_rate"] = std::to_string(pImpl->learningRate);
    params["pin"] = std::to_string(pImpl->pin);
    params["hebbian_modulation"] = pImpl->hebbianModulation ? "true" : "false";
    params["hebbian_weight"] = std::to_string(pImpl->hebbianWeight);
    return params;
}

void BCMRule::setParameters(const std::unordered_map<std::string, std::string>& params) {
    auto it = params.find("theta");
    if (it != params.end()) {
        setTheta(std::stof(it->second));
    }
    
    it = params.find("theta_max");
    if (it != params.end()) {
        pImpl->thetaMax = std::stof(it->second);
    }
    
    it = params.find("theta_min");
    if (it != params.end()) {
        pImpl->thetaMin = std::stof(it->second);
    }
    
    it = params.find("tau");
    if (it != params.end()) {
        setTau(std::stof(it->second));
    }
    
    it = params.find("learning_rate");
    if (it != params.end()) {
        setLearningRate(std::stof(it->second));
    }
    
    it = params.find("pin");
    if (it != params.end()) {
        setPin(std::stof(it->second));
    }
    
    it = params.find("hebbian_modulation");
    if (it != params.end()) {
        setHebbianModulation(it->second == "true" || it->second == "1");
    }
    
    it = params.find("hebbian_weight");
    if (it != params.end()) {
        setHebbianWeight(std::stof(it->second));
    }
}

bool BCMRule::hasParameter(const std::string& name) const {
    static const std::vector<std::string> paramNames = {
        "theta", "theta_max", "theta_min", "tau", "learning_rate", 
        "pin", "hebbian_modulation", "hebbian_weight"
    };
    return std::find(paramNames.begin(), paramNames.end(), name) != paramNames.end();
}

const char* BCMRule::getParameterName(size_t index) const {
    static const std::vector<std::string> paramNames = {
        "theta", "theta_max", "theta_min", "tau", "learning_rate", 
        "pin", "hebbian_modulation", "hebbian_weight"
    };
    if (index < paramNames.size()) {
        return paramNames[index].c_str();
    }
    return nullptr;
}

std::unique_ptr<PlasticityRule> BCMRule::clone() const {
    auto clone = std::make_unique<BCMRule>();
    *clone->pImpl = *pImpl; // Deep copy
    return clone;
}

bool BCMRule::validate() const {
    if (pImpl->learningRate < 0.0f || pImpl->learningRate > 1.0f) return false;
    if (pImpl->pin < 0.0f || pImpl->pin > 1.0f) return false;
    if (pImpl->tau < 1.0f || pImpl->tau > 1000.0f) return false;
    if (pImpl->thetaMax <= pImpl->thetaMin) return false;
    return true;
}

} // namespace nlm