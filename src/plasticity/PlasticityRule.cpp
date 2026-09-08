#include "PlasticityRule.hpp"
#include "LearningRateAdapter.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

struct PlasticityRule::Impl {
    bool enabled;
    LearningRateAdapter* learningRateAdapter;
    
    Impl() : enabled(true), learningRateAdapter(nullptr) {}
};

PlasticityRule::PlasticityRule() : pImpl(new Impl) {}

PlasticityRule::~PlasticityRule() {
    delete pImpl;
}

bool PlasticityRule::isEnabled() const {
    return pImpl->enabled;
}

void PlasticityRule::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
}

void PlasticityRule::setLearningRateAdapter(LearningRateAdapter* adapter) {
    pImpl->learningRateAdapter = adapter;
}

float PlasticityRule::getCurrentLearningRate() const {
    if (pImpl->learningRateAdapter) {
        // We need a way to get the current synapse ID for the learning rate
        // This is a placeholder - in practice, this would be called from within update()
        return pImpl->learningRateAdapter->getLearningRate(SynapseId());
    }
    return 0.01f; // Default learning rate
}

void PlasticityRule::updateLearningRate(Synapse* synapse, 
                                        const std::vector<SynapticWeight>& weightHistory,
                                        const std::vector<float>& stabilityHistory,
                                        const std::vector<float>& performanceHistory,
                                        TimestepDuration dt) {
    if (pImpl->learningRateAdapter && synapse && dt > 0.0f) {
        // Calculate learning history from weight changes
        std::vector<SynapticWeight> weightChanges;
        if (weightHistory.size() > 1) {
            for (size_t i = 1; i < weightHistory.size(); ++i) {
                weightChanges.push_back(weightHistory[i] - weightHistory[i-1]);
            }
        }
        
        // Simple stability and performance metrics
        float stability = 1.0f;
        if (!stabilityHistory.empty()) {
            float totalStability = 0.0f;
            for (float s : stabilityHistory) totalStability += s;
            stability = totalStability / stabilityHistory.size();
        }
        
        float performance = 0.0f;
        if (!performanceHistory.empty()) {
            float totalPerformance = 0.0f;
            for (float p : performanceHistory) totalPerformance += p;
            performance = totalPerformance / performanceHistory.size();
        }
        
        pImpl->learningRateAdapter->update(synapse->getId(), weightChanges, 
                                         std::vector<float>{stability}, 
                                         std::vector<float>{performance}, 
                                         dt);
    }
}

// HebbianRule implementation
struct HebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // TODO PHASE 2: Implement real Hebbian learning
    // Improved biologically realistic Hebbian learning with spike timing windows
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // STDP-like coincidence detection with spike timing windows
    float totalWeightChange = 0.0f;
    const float timingWindow = 20.0f;  // ms - optimal Hebbian window
    
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float timeDiff = static_cast<float>(post - pre);
            
            if (std::abs(timeDiff) < timingWindow) {
                // Hebbian weight change based on spike timing
                // Stronger when spikes are closer in time
                float timingFactor = 1.0f - std::abs(timeDiff) / timingWindow;
                
                // Asymmetric Hebbian: if pre before post, strengthen;
                // if post before pre, weaken (modulated by learning rate)
                if (timeDiff > 0) {
                    totalWeightChange += pImpl->learningRate * timingFactor;
                } else if (timeDiff < 0 && std::abs(timeDiff) > 5.0f) {
                    // Weak anti-Hebbian component for anti-phase firing
                    totalWeightChange -= pImpl->learningRate * 0.3f * timingFactor;
                }
            }
        }
    }
    
    // Apply normalized weight change to prevent runaway excitation
    if (totalWeightChange != 0.0f) {
        // Normalize learning rate based on current weight
        float currentWeight = synapse->getWeight();
        float weightRange = Impl::MAX_WEIGHT - Impl::MIN_WEIGHT;
        float normalizedLearningRate = pImpl->learningRate;
        
        // Apply homeostatic scaling: reduce learning for weights near bounds
        if (std::abs(currentWeight) > 0.8f * weightRange) {
            normalizedLearningRate *= 0.5f;
        }
        
        applyWeightChange(synapse, normalizedLearningRate * totalWeightChange);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// AntiHebbianRule implementation
struct AntiHebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    // Anti-Hebbian update: weaken connections when neurons fire together
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalWeightChange = -pImpl->learningRate;  // Always decrease weight
    applyWeightChange(synapse, totalWeightChange);
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// BCMRule implementation
struct BCMRule::Impl {
    float theta;              // Sliding threshold (BCM threshold)
    float learningRate;       // Base learning rate
    float synapticScaling;    // Synaptic weight scaling factor
    float previousActivity;   // Previous average postsynaptic activity for sliding threshold
    float synapticLTD;        // Long-term depression factor
    float synapticLTP;        // Long-term potentiation factor
    
    Impl() : theta(0.5f), learningRate(0.01f), synapticScaling(1.0f),
             previousActivity(0.0f), synapticLTD(0.01f), synapticLTP(0.012f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    // TODO: Implement real BCM rule logic
    // For now, placeholder implementation
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    // Calculate postsynaptic activity
    float postsynapticActivity = static_cast<float>(postSpikes.size()) / 10.0f;  // Normalized
    postsynapticActivity = std::clamp(postsynapticActivity, 0.0f, 1.0f);
    
    // Update sliding threshold (simplified)
    float alpha = 0.01f;
    pImpl->previousActivity = pImpl->previousActivity * (1.0f - alpha) + 
                             postsynapticActivity * postsynapticActivity * alpha;
    pImpl->theta = pImpl->previousActivity;
    
    // Calculate weight change
    float weightChange = 0.0f;
    float currentWeight = synapse->getWeight();
    
    if (postsynapticActivity > pImpl->theta) {
        // LTP
        weightChange = pImpl->learningRate * (postsynapticActivity - pImpl->theta) * currentWeight;
    } else {
        // LTD
        weightChange = -pImpl->learningRate * pImpl->synapticLTD * currentWeight;
    }
    
    applyWeightChange(synapse, weightChange);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // For BCM rule, apply weight change with soft bounds
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply BCM-specific weight constraints
    float minWeight = -2.0f;
    float maxWeight = 2.0f;
    
    // Apply synaptic scaling to bounds
    newWeight *= pImpl->synapticScaling;
    newWeight = std::clamp(newWeight, minWeight, maxWeight);
    
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setSynapticScaling(float scaling) {
    pImpl->synapticScaling = std::clamp(scaling, 0.5f, 2.0f);
}

float BCMRule::getSynapticScaling() const {
    return pImpl->synapticScaling;
}

} // namespace nlm
