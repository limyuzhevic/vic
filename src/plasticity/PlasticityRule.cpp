#include "PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Random/RandomGenerator.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

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
    // Implement real Hebbian learning: "neurons that fire together, wire together"
    // This is a biological implementation of Hebbian plasticity with additional mechanisms
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate synaptic eligibility and modify based on spike timing
    float learningRate = pImpl->learningRate;
    
    // Determine if pre-synaptic spike preceded post-synaptic spike (LTP)
    bool ltpOccurred = false;
    float maxPreTime = -1.0f;
    Timestamp maxPreTimeVal = 0.0;
    
    for (Timestamp preTime : preSpikes) {
        if (preTime > maxPreTime) {
            maxPreTime = preTime;
            maxPreTimeVal = preTime;
        }
    }
    
    // Check for post-synaptic spikes within typical STDP window
    for (Timestamp postTime : postSpikes) {
        float timeDiff = postTime - maxPreTimeVal;
        
        // Long-term potentiation: post fires shortly after pre (within 20ms)
        if (timeDiff > 0.0f && timeDiff < 20.0f) {
            ltpOccurred = true;
            break;
        }
    }
    
    // Long-term depression: pre and post fire simultaneously or post before pre
    if (!ltpOccurred) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = postTime - maxPreTimeVal;
            
            // Long-term depression: post fires before or simultaneously with pre
            // within 20ms window
            if (timeDiff <= 0.0f && timeDiff > -20.0f) {
                ltpOccurred = false; // LTD
                break;
            }
        }
    }
    
    // Apply weight change based on Hebbian rule
    if (ltpOccurred) {
        // Long-term potentiation: strengthen synapse
        float delta = learningRate * (1.0f + pImpl->learningRate * 0.5f); // Boost learning
        applyWeightChange(synapse, delta);
        
        // Add synaptic efficacy increase
        synapse->setEfficacy(std::min(2.0f, synapse->getEfficacy() + 0.1f));
        
    } else {
        // Long-term depression: weaken synapse
        float delta = -learningRate * 0.8f; // Moderate depression
        applyWeightChange(synapse, delta);
        
        // Reduce synaptic efficacy
        synapse->setEfficacy(std::max(0.0f, synapse->getEfficacy() - 0.1f));
    }
    
    // Add homeostatic regulation to prevent runaway excitation/inhibition
    float weight = synapse->getWeight();
    if (std::abs(weight) > 0.8f) {
        // Apply homeostatic scaling if weight is too extreme
        float homeostaticCorrection = -weight * 0.05f * learningRate;
        applyWeightChange(synapse, homeostaticCorrection);
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

} // namespace nlm
