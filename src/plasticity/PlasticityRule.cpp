#include "PlasticityRule.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <numeric>
#include <tuple>

namespace nlm {

struct HebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float weightChangeScale;
    
    // STDP parameters
    float stdpLearningRate;      // STDP learning rate
    float stdpTimeWindow;        // STDP time window (ms)
    float stdpTauPre;            // Pre-synaptic trace decay
    float stdpTauPost;           // Post-synaptic trace decay
    
    // Eligibility traces for credit assignment
    std::unordered_map<SynapseId, float> preTraces;     // Pre-synaptic eligibility traces
    std::unordered_map<SynapseId, float> postTraces;    // Post-synaptic eligibility traces
    
    // Neuromodulation influence
    float neuromodulationStrength;  // How much neuromodulators affect plasticity
    float explorationRate;         // Exploration vs exploitation balance
    
    // Homeostatic mechanisms
    float targetWeight;           // Target weight for homeostasis
    float homeostaticRate;        // Homeostatic adjustment rate
    float weightHistorySize;      // Number of previous weight samples for homeostasis
    
    Impl() : 
        learningRate(0.01f),
        maxWeight(10.0f),
        minWeight(-10.0f),
        weightChangeScale(1.0f),
        stdpLearningRate(0.005f),
        stdpTimeWindow(20.0f),    // 20ms STDP window
        stdpTauPre(10.0f),        // 10ms pre-synaptic trace decay
        stdpTauPost(20.0f),       // 20ms post-synaptic trace decay
        neuromodulationStrength(1.0f),
        explorationRate(0.1f),
        targetWeight(0.0f),
        homeostaticRate(0.001f),
        weightHistorySize(100)
    {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float dtFloat = static_cast<float>(dt);
    
    // Update eligibility traces
    updateEligibilityTraces(synapse->getId(), preSpikes, postSpikes, dtFloat);
    
    // Apply Hebbian learning
    applyHebbianLearning(synapse, preSpikes, postSpikes, dtFloat);
    
    // Apply STDP (Spike-Timing-Dependent Plasticity)
    applySTDP(synapse, preSpikes, postSpikes, dtFloat);
    
    // Apply homeostatic plasticity
    applyHomeostaticPlasticity(synapse, dtFloat);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Clamp weight to valid range
    float clampedWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply neuromodulation influence
    float neuromodulatedWeight = clampedWeight * pImpl->neuromodulationStrength;
    
    synapse->setWeight(neuromulatedWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian+STDP";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = maxWeight;
}

float HebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void HebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = minWeight;
}

float HebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

void HebbianRule::setSTDPParameters(float learningRate, float timeWindow, float tauPre, float tauPost) {
    pImpl->stdpLearningRate = learningRate;
    pImpl->stdpTimeWindow = timeWindow;
    pImpl->stdpTauPre = tauPre;
    pImpl->stdpTauPost = tauPost;
}

void HebbianRule::setNeuromodulationStrength(float strength) {
    pImpl->neuromodulationStrength = strength;
}

float HebbianRule::getNeuromodulationStrength() const {
    return pImpl->neuromodulationStrength;
}

void HebbianRule::setExplorationRate(float rate) {
    pImpl->explorationRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getExplorationRate() const {
    return pImpl->explorationRate;
}

void HebbianRule::setHomeostaticTarget(float target) {
    pImpl->targetWeight = target;
}

float HebbianRule::getHomeostaticTarget() const {
    return pImpl->targetWeight;
}

void HebbianRule::setHomeostaticRate(float rate) {
    pImpl->homeostaticRate = rate;
}

float HebbianRule::getHomeostaticRate() const {
    return pImpl->homeostaticRate;
}

void HebbianRule::updateEligibilityTraces(const SynapseId& synapseId,
                                           const std::vector<Timestamp>& preSpikes,
                                           const std::vector<Timestamp>& postSpikes,
                                           float dt) {
    // Update pre-synaptic traces
    for (Timestamp spikeTime : preSpikes) {
        float deltaT = static_cast<float>(spikeTime);
        // Pre-synaptic trace increases when pre-synaptic neuron spikes
        pImpl->preTraces[synapseId] += std::exp(-deltaT / pImpl->stdpTauPre);
    }
    
    // Update post-synaptic traces
    for (Timestamp spikeTime : postSpikes) {
        float deltaT = static_cast<float>(spikeTime);
        // Post-synaptic trace increases when post-synaptic neuron spikes
        pImpl->postTraces[synapseId] += std::exp(-deltaT / pImpl->stdpTauPost);
    }
    
    // Decay traces
    for (auto& pair : pImpl->preTraces) {
        pair.second *= std::exp(-dt / pImpl->stdpTauPre);
    }
    for (auto& pair : pImpl->postTraces) {
        pair.second *= std::exp(-dt / pImpl->stdpTauPost);
    }
}

void HebbianRule::applyHebbianLearning(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       float dt) {
    if (!synapse) return;
    
    // Calculate correlation-based learning
    size_t coincidences = 0;
    float totalPreTime = 0.0f;
    float totalPostTime = 0.0f;
    
    // Simplified correlation detection
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float timeDiff = std::abs(static_cast<float>(pre - post));
            if (timeDiff < 10.0f) {  // 10ms coincidence window
                coincidences++;
                totalPreTime += pre;
                totalPostTime += post;
            }
        }
    }
    
    if (coincidences > 0) {
        // Correlated firing enhances synaptic strength
        float avgPreTime = totalPreTime / coincidences;
        float avgPostTime = totalPostTime / coincidences;
        float correlation = std::exp(-std::abs(avgPreTime - avgPostTime) / 5.0f);
        
        float weightChange = pImpl->learningRate * correlation * static_cast<float>(coincidences);
        applyWeightChange(synapse, weightChange);
    }
}

void HebbianRule::applySTDP(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            float dt) {
    if (!synapse) return;
    
    float dtFloat = static_cast<float>(dt);
    
    // STDP: Strengthen synapse if pre-synaptic spike precedes post-synaptic spike
    // STDP: Weaken synapse if post-synaptic spike precedes pre-synaptic spike
    
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float deltaT = static_cast<float>(post - pre);
            
            if (std::abs(deltaT) < pImpl->stdpTimeWindow) {
                if (deltaT > 0) {
                    // Pre before post: long-term potentiation (LTP)
                    float weightChange = pImpl->stdpLearningRate * 
                                       std::exp(-deltaT / pImpl->stdpTauPre) * 
                                       pImpl->preTraces[synapse->getId()];
                    applyWeightChange(synapse, weightChange);
                } else if (deltaT < 0) {
                    // Post before pre: long-term depression (LTD)
                    float weightChange = -pImpl->stdpLearningRate * 
                                       std::exp(deltaT / pImpl->stdpTauPost) * 
                                       pImpl->postTraces[synapse->getId()];
                    applyWeightChange(synapse, weightChange);
                }
            }
        }
    }
}

void HebbianRule::applyHomeostaticPlasticity(Synapse* synapse, float dt) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    
    // Simple homeostatic mechanism: push weight towards target
    float homeostaticChange = pImpl->homeostaticRate * 
                             (pImpl->targetWeight - currentWeight) * dt;
    
    applyWeightChange(synapse, homeostaticChange);
}

float HebbianRule::getPreTrace(const SynapseId& synapseId) const {
    auto it = pImpl->preTraces.find(synapseId);
    if (it != pImpl->preTraces.end()) {
        return it->second;
    }
    return 0.0f;
}

float HebbianRule::getPostTrace(const SynapseId& synapseId) const {
    auto it = pImpl->postTraces.find(synapseId);
    if (it != pImpl->postTraces.end()) {
        return it->second;
    }
    return 0.0f;
}

void HebbianRule::resetTraces() {
    pImpl->preTraces.clear();
    pImpl->postTraces.clear();
}

} // namespace nlm