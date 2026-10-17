#include "AdvancedPlasticityRules.hpp"
#include "../../brain/Synapse.hpp"
#include "../../brain/Brain.hpp"
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baseSTDPWeight;        // Base STDP weight
    float learningRate;          // Learning rate
    float rewardScalar;          // Reward scalar for modulation
    float eligibilityDecay;      // Decay rate of eligibility trace
    float eligibilityThreshold;  // Threshold for eligibility trace
    float maxTrace;              // Maximum eligibility trace value
    
    // Reward system integration
    Dopamine* dopamineSystem;    // Pointer to dopamine system
    PredictionError* predError;  // Pointer to prediction error system
    
    Impl() : baseSTDPWeight(0.02f), learningRate(0.1f), rewardScalar(1.0f),
             eligibilityDecay(0.001f), eligibilityThreshold(0.01f),
             maxTrace(1.0f), dopamineSystem(nullptr), predError(nullptr) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {
    // Initialize plugin info
    PlasticityRuleInfo& info = const_cast<PlasticityRuleInfo&>(getPluginInfo());
    info.name = "RewardModulatedSTDP";
    info.version = "2.0.0";
    info.description = "Reward-modulated STDP incorporating dopaminergic signals";
    info.author = "NLM Team";
}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    /*
     * Enhanced Reward-Modulated STDP (R-STDP) implementation
     * 
     * Mathematical formulation:
     * 
     * Δw = A+ * η * δ * exp(-Δt/τ+) * eligibilityTrace
     * Δw = -A- * η * δ * exp(Δt/τ-) * eligibilityTrace
     * 
     * Where:
     *   δ = reward prediction error (dopamine signal)
     *   η = learningRate (baseline)
     *   eligibilityTrace = eligibility trace variable
     *   A+, A- = STDP weights (baseSTDPWeight)
     * 
     * Biological inspiration:
     *   - Dopaminergic modulation of synaptic plasticity
     *   - Reward prediction error signals in reinforcement learning
     *   - Temporal difference learning
     *   - Eligibility traces for delayed rewards
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get reward prediction error signal (dopamine)
    float rewardSignal = 0.0f;
    if (pImpl->dopamineSystem) {
        rewardSignal = pImpl->dopamineSystem->getCurrentDopamine();
    }
    
    // Get prediction error signal
    if (pImpl->predError) {
        rewardSignal += pImpl->predError->getPredictionError();
    }
    
    // Update eligibility traces based on spike activity
    updateEligibilityTraces(synapse, preSpikes, postSpikes, rewardSignal);
    
    // Calculate weight changes using R-STDP
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->baseSTDPWeight * pImpl->learningRate * rewardSignal * 
                             std::exp(-dt / pImpl->timeConstant) * 
                             synapse->getEligibilityTrace();
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->baseSTDPWeight * pImpl->learningRate * rewardSignal * 
                             std::exp(dt / pImpl->timeConstant) * 
                             synapse->getEligibilityTrace();
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
    
    // Update eligibility trace based on weight change
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace + delta);
    
    // Clamp eligibility trace
    if (std::abs(synapse->getEligibilityTrace()) > pImpl->maxTrace) {
        synapse->setEligibilityTrace(std::copysign(pImpl->maxTrace, synapse->getEligibilityTrace()));
    }
}

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

const char* RewardModulatedSTDP::getDescription() const {
    return "Reward-modulated STDP incorporating dopaminergic signals";
}

const char* RewardModulatedSTDP::getVersion() const {
    return "2.0.0";
}

void RewardModulatedSTDP::setBaseSTDPWeight(float weight) {
    pImpl->baseSTDPWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseSTDPWeight() const {
    return pImpl->baseSTDPWeight;
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setRewardScalar(float scalar) {
    pImpl->rewardScalar = std::clamp(scalar, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getRewardScalar() const {
    return pImpl->rewardScalar;
}

void RewardModulatedSTDP::setEligibilityDecay(float decay) {
    pImpl->eligibilityDecay = std::clamp(decay, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getEligibilityDecay() const {
    return pImpl->eligibilityDecay;
}

void RewardModulatedSTDP::updateEligibilityTraces(Synapse* synapse,
                                                  const std::vector<Timestamp>& preSpikes,
                                                  const std::vector<Timestamp>& postSpikes,
                                                  float rewardSignal) {
    if (!synapse) return;
    
    // Update eligibility trace based on recent spike activity
    float currentTrace = synapse->getEligibilityTrace();
    
    // Decay eligibility trace
    synapse->decayEligibilityTrace(pImpl->eligibilityDecay);
    
    // Add new eligibility based on spike correlation
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            // Only add eligibility for recent spike pairs
            if (std::abs(dt) < 100.0f) {
                float eligibilityIncrement = std::exp(-std::abs(dt) / 10.0f);
                
                // Modulate by reward signal
                if (rewardSignal > 0) {
                    eligibilityIncrement *= rewardSignal * pImpl->rewardScalar;
                }
                
                // Update eligibility trace
                float newTrace = currentTrace + eligibilityIncrement;
                synapse->setEligibilityTrace(std::min(newTrace, pImpl->maxTrace));
                currentTrace = newTrace;
            }
        }
    }
}

std::unordered_map<std::string, std::string> RewardModulatedSTDP::getParameters() const {
    std::unordered_map<std::string, std::string> params;
    params["base_stdp_weight"] = std::to_string(pImpl->baseSTDPWeight);
    params["learning_rate"] = std::to_string(pImpl->learningRate);
    params["reward_scalar"] = std::to_string(pImpl->rewardScalar);
    params["eligibility_decay"] = std::to_string(pImpl->eligibilityDecay);
    params["eligibility_threshold"] = std::to_string(pImpl->eligibilityThreshold);
    params["max_trace"] = std::to_string(pImpl->maxTrace);
    return params;
}

void RewardModulatedSTDP::setParameters(const std::unordered_map<std::string, std::string>& params) {
    auto it = params.find("base_stdp_weight");
    if (it != params.end()) {
        setBaseSTDPWeight(std::stof(it->second));
    }
    
    it = params.find("learning_rate");
    if (it != params.end()) {
        setLearningRate(std::stof(it->second));
    }
    
    it = params.find("reward_scalar");
    if (it != params.end()) {
        setRewardScalar(std::stof(it->second));
    }
    
    it = params.find("eligibility_decay");
    if (it != params.end()) {
        setEligibilityDecay(std::stof(it->second));
    }
    
    it = params.find("eligibility_threshold");
    if (it != params.end()) {
        pImpl->eligibilityThreshold = std::stof(it->second);
    }
    
    it = params.find("max_trace");
    if (it != params.end()) {
        pImpl->maxTrace = std::stof(it->second);
    }
}

bool RewardModulatedSTDP::hasParameter(const std::string& name) const {
    static const std::vector<std::string> paramNames = {
        "base_stdp_weight", "learning_rate", "reward_scalar", 
        "eligibility_decay", "eligibility_threshold", "max_trace"
    };
    return std::find(paramNames.begin(), paramNames.end(), name) != paramNames.end();
}

const char* RewardModulatedSTDP::getParameterName(size_t index) const {
    static const std::vector<std::string> paramNames = {
        "base_stdp_weight", "learning_rate", "reward_scalar", 
        "eligibility_decay", "eligibility_threshold", "max_trace"
    };
    if (index < paramNames.size()) {
        return paramNames[index].c_str();
    }
    return nullptr;
}

std::unique_ptr<PlasticityRule> RewardModulatedSTDP::clone() const {
    auto clone = std::make_unique<RewardModulatedSTDP>();
    *clone->pImpl = *pImpl; // Deep copy
    return clone;
}

bool RewardModulatedSTDP::validate() const {
    if (pImpl->learningRate < 0.0f || pImpl->learningRate > 1.0f) return false;
    if (pImpl->rewardScalar < 0.0f || pImpl->rewardScalar > 10.0f) return false;
    if (pImpl->eligibilityDecay < 0.0f || pImpl->eligibilityDecay > 1.0f) return false;
    if (pImpl->baseSTDPWeight < 0.0f || pImpl->baseSTDPWeight > 1.0f) return false;
    return true;
}

void RewardModulatedSTDP::initialize(Brain* brain) {
    if (brain) {
        pImpl->dopamineSystem = brain->getDopamine();
        pImpl->predError = brain->getPredictionErrorSignal();
    }
}

void RewardModulatedSTDP::shutdown() {
    pImpl->dopamineSystem = nullptr;
    pImpl->predError = nullptr;
}

} // namespace nlm