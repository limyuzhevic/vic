#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct HebbianRule::Impl {
    float learningRate;
    float timeConstant;     // Time constant for spike timing window (ms)
    float minWeight;        // Minimum synaptic weight
    float maxWeight;        // Maximum synaptic weight
    
    Impl() : learningRate(0.01f), timeConstant(10.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
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
    
    // Real Hebbian learning implementation based on spike-timing correlation
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Compute correlated firing between pre and post neurons
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            // Close spike timing correlation
            // Hebbian potentiation is strongest when pre fires shortly before post
            if (dt >= 0 && dt <= tau) {
                float correlation = std::exp(-dt / tau);
                totalDelta += correlation;
            }
        }
    }
    
    // Apply weight change with bounds and biological constraints
    if (totalDelta > 0.0f) {
        // Scale by learning rate and current synaptic efficacy
        float efficacy = synapse->getEfficacy();
        float delta = pImpl->learningRate * totalDelta * efficacy;
        
        // Apply weight change
        applyWeightChange(synapse, delta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to biologically plausible bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float HebbianRule::getTimeConstant() const {
    return pImpl->timeConstant;
}

void HebbianRule::configure(float learningRate, float timeConstant) {
    setLearningRate(learningRate);
    setTimeConstant(timeConstant);
}

} // namespace nlm