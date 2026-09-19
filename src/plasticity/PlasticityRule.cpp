#include "PlasticityRule.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct BCMRule::Impl {
    float theta;           // BCM threshold for potentiation
    float learningRate;
    float homeostaticFactor;
    
    Impl() : theta(0.5f), learningRate(0.01f), homeostaticFactor(0.1f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::setTheta(float theta) {
    pImpl->theta = theta;
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikeHistory,
                    const std::vector<Timestamp>& postSpikeHistory,
                    TimestepDuration dt) {
    if (!synapse) return;
    
    // BCM rule: credit assignment based on postsynaptic activity
    // w <- w + eta * (post - theta) * pre
    // Where theta is the sliding threshold
    
    if (preSpikeHistory.empty()) {
        return;
    }
    
    // Calculate postsynaptic activity (average firing rate)
    float postActivity = 0.0f;
    if (!postSpikeHistory.empty()) {
        postActivity = static_cast<float>(postSpikeHistory.size()) / 
                      (postSpikeHistory.back() - postSpikeHistory.front() + 1.0f);
    }
    
    // Calculate presynaptic activity (average firing rate)
    float preActivity = static_cast<float>(preSpikeHistory.size()) / 
                       (preSpikeHistory.back() - preSpikeHistory.front() + 1.0f);
    
    // BCM learning rule
    float weightChange = 0.0f;
    
    // For each presynaptic spike, apply Hebbian-like rule modulated by postsynaptic activity
    for (size_t i = 0; i < preSpikeHistory.size(); ++i) {
        // BCM: potentiate if post > theta, depress if post < theta
        float postContribution = (postActivity - pImpl->theta);
        
        // Add homeostatic regulation
        postContribution *= (1.0f - pImpl->homeostaticFactor * std::abs(postActivity - 0.5f));
        
        // Scale by learning rate and presynaptic activity
        weightChange += pImpl->learningRate * postContribution * preActivity;
    }
    
    // Apply weight change with bounds
    if (std::abs(weightChange) > 0.001f) {
        float newWeight = synapse->getWeight() + weightChange;
        
        // Constrain weight
        const float MIN_WEIGHT = -1.0f;
        const float MAX_WEIGHT = 1.0f;
        
        if (newWeight < MIN_WEIGHT) newWeight = MIN_WEIGHT;
        if (newWeight > MAX_WEIGHT) newWeight = MAX_WEIGHT;
        
        synapse->setWeight(newWeight);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Constrain weight
    const float MIN_WEIGHT = -1.0f;
    const float MAX_WEIGHT = 1.0f;
    
    if (newWeight < MIN_WEIGHT) newWeight = MIN_WEIGHT;
    if (newWeight > MAX_WEIGHT) newWeight = MAX_WEIGHT;
    
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

struct AntiHebbianRule::Impl {
    float learningRate;
    float strength;
    
    Impl() : learningRate(0.01f), strength(1.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikeHistory,
                           const std::vector<Timestamp>& postSpikeHistory,
                           TimestepDuration dt) {
    if (!synapse) return;
    
    // Anti-Hebbian: decrease weight when pre and post fire together
    // w <- w - eta * pre * post
    
    if (preSpikeHistory.empty() || postSpikeHistory.empty()) {
        return;
    }
    
    // Calculate average firing rates
    float preRate = static_cast<float>(preSpikeHistory.size()) / 
                   (preSpikeHistory.back() - preSpikeHistory.front() + 1.0f);
    float postRate = static_cast<float>(postSpikeHistory.size()) / 
                    (postSpikeHistory.back() - postSpikeHistory.front() + 1.0f);
    
    // Anti-Hebbian weight change
    float weightChange = -pImpl->learningRate * preRate * postRate * pImpl->strength;
    
    // Apply weight change with bounds
    if (std::abs(weightChange) > 0.001f) {
        float newWeight = synapse->getWeight() + weightChange;
        
        // Constrain weight
        const float MIN_WEIGHT = -1.0f;
        const float MAX_WEIGHT = 1.0f;
        
        if (newWeight < MIN_WEIGHT) newWeight = MIN_WEIGHT;
        if (newWeight > MAX_WEIGHT) newWeight = MAX_WEIGHT;
        
        synapse->setWeight(newWeight);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Constrain weight
    const float MIN_WEIGHT = -1.0f;
    const float MAX_WEIGHT = 1.0f;
    
    if (newWeight < MIN_WEIGHT) newWeight = MIN_WEIGHT;
    if (newWeight > MAX_WEIGHT) newWeight = MAX_WEIGHT;
    
    synapse->setWeight(newWeight);
}

struct OjaRule::Impl {
    float learningRate;
    float normalizationFactor;
    
    Impl() : learningRate(0.01f), normalizationFactor(0.5f) {}
};

OjaRule::OjaRule() : pImpl(new Impl) {}

OjaRule::~OjaRule() = default;

void OjaRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float OjaRule::getLearningRate() const {
    return pImpl->learningRate;
}

void OjaRule::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikeHistory,
                   const std::vector<Timestamp>& postSpikeHistory,
                   TimestepDuration dt) {
    if (!synapse) return;
    
    // Oja's rule: competitive synaptic normalization
    // w <- w + eta * y * (x - y * w)
    // Where y is postsynaptic activity, x is presynaptic activity
    
    if (preSpikeHistory.empty()) {
        return;
    }
    
    // Calculate synaptic weight
    float w = synapse->getWeight();
    
    // Calculate postsynaptic activity (simplified)
    float y = 1.0f;  // Assume postsynaptic neuron fires
    if (!postSpikeHistory.empty()) {
        y = static_cast<float>(postSpikeHistory.size()) / 
           (postSpikeHistory.back() - postSpikeHistory.front() + 1.0f);
    }
    
    // Calculate presynaptic activity (simplified)
    float x = 1.0f;  // Assume presynaptic neuron fires
    if (!preSpikeHistory.empty()) {
        x = static_cast<float>(preSpikeHistory.size()) / 
           (preSpikeHistory.back() - preSpikeHistory.front() + 1.0f);
    }
    
    // Oja's learning rule
    float weightChange = pImpl->learningRate * y * (x - y * w * pImpl->normalizationFactor);
    
    // Apply weight change with bounds
    if (std::abs(weightChange) > 0.001f) {
        float newWeight = w + weightChange;
        
        // Constrain weight
        const float MIN_WEIGHT = -1.0f;
        const float MAX_WEIGHT = 1.0f;
        
        if (newWeight < MIN_WEIGHT) newWeight = MIN_WEIGHT;
        if (newWeight > MAX_WEIGHT) newWeight = MAX_WEIGHT;
        
        synapse->setWeight(newWeight);
    }
}

void OjaRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Constrain weight
    const float MIN_WEIGHT = -1.0f;
    const float MAX_WEIGHT = 1.0f;
    
    if (newWeight < MIN_WEIGHT) newWeight = MIN_WEIGHT;
    if (newWeight > MAX_WEIGHT) newWeight = MAX_WEIGHT;
    
    synapse->setWeight(newWeight);
}

struct RewardModulatedSTDP::Impl {
    float ltpWeight;
    float ltdWeight;
    float tau;
    float learningRate;
    float predictionError;  // From reward prediction error
    float dopamineLevel;   // Neuromodulatory signal
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), tau(20.0f), learningRate(1.0f), 
             predictionError(0.0f), dopamineLevel(0.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikeHistory,
                                 const std::vector<Timestamp>& postSpikeHistory,
                                 TimestepDuration dt) {
    if (!synapse) return;
    
    // R-STDP: STDP modulated by reward prediction error
    // Weight changes scaled by dopamine (prediction error)
    
    if (preSpikeHistory.empty() || postSpikeHistory.empty()) {
        return;
    }
    
    float weightChange = 0.0f;
    
    // Scale STDP parameters by dopamine level (neuromodulatory signal)
    float modFactor = 1.0f + pImpl->dopamineLevel;
    
    // Check for pre-before-post spikes (LTP)
    for (size_t i = 0; i < preSpikeHistory.size(); ++i) {
        for (size_t j = 0; j < postSpikeHistory.size(); ++j) {
            if (postSpikeHistory[j] > preSpikeHistory[i]) {
                float delta_t = postSpikeHistory[j] - preSpikeHistory[i];
                if (delta_t < pImpl->tau) {
                    float stdpUpdate = pImpl->ltpWeight * 
                                     std::exp(-delta_t / pImpl->tau);
                    weightChange += stdpUpdate * modFactor;
                }
            }
        }
    }
    
    // Check for post-before-pre spikes (LTD)
    for (size_t i = 0; i < postSpikeHistory.size(); ++i) {
        for (size_t j = 0; j < preSpikeHistory.size(); ++j) {
            if (preSpikeHistory[j] > postSpikeHistory[i]) {
                float delta_t = preSpikeHistory[j] - postSpikeHistory[i];
                if (delta_t < pImpl->tau) {
                    float stdpUpdate = pImpl->ltdWeight * 
                                     std::exp(-delta_t / pImpl->tau);
                    weightChange -= stdpUpdate * modFactor;
                }
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(weightChange) > 0.001f) {
        float newWeight = synapse->getWeight() + weightChange;
        
        // Constrain weight
        const float MIN_WEIGHT = -1.0f;
        const float MAX_WEIGHT = 1.0f;
        
        if (newWeight < MIN_WEIGHT) newWeight = MIN_WEIGHT;
        if (newWeight > MAX_WEIGHT) newWeight = MAX_WEIGHT;
        
        synapse->setWeight(newWeight);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Constrain weight
    const float MIN_WEIGHT = -1.0f;
    const float MAX_WEIGHT = 1.0f;
    
    if (newWeight < MIN_WEIGHT) newWeight = MIN_WEIGHT;
    if (newWeight > MAX_WEIGHT) newWeight = MAX_WEIGHT;
    
    synapse->setWeight(newWeight);
}

void RewardModulatedSTDP::setPredictionError(float error) {
    pImpl->predictionError = error;
    // Update dopamine level based on prediction error
    pImpl->dopamineLevel = error;
}

void RewardModulatedSTDP::setDopamineLevel(float level) {
    pImpl->dopamineLevel = level;
}

} // namespace nlm
