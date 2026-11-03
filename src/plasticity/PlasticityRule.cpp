#include "PlasticityRule.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float threshold;
    float etaL;              // Learning rate for potentiation
    float etaH;              // Learning rate for depression
    float BCM_threshold;     // BCM theory threshold parameter
    
    Impl() : learningRate(0.01f), threshold(0.0f), etaL(0.02f), etaH(0.01f), BCM_threshold(0.5f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Real Hebbian learning based on BCM theory (Bienenstock-Cooper-Munro)
    // Incorporates weight dependence and homeostatic regulation
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get current synaptic weight
    float w = synapse->getWeight();
    
    // Compute eligibility trace based on spike timing
    // BCM theory: synaptic change depends on correlation with postsynaptic activity
    float eligibility = 0.0f;
    
    // Calculate spike correlations using temporal window
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            // Temporal difference
            float delta_t = (postSpikes[j] - preSpikes[i]) * 1000.0f; // Convert to ms
            
            // Gaussian temporal window centered at zero
            // Reflects biological synaptic delay structure
            float temporal_window = std::exp(-std::pow(delta_t / 10.0f, 2.0f) / 2.0f);
            
            // Weight-dependent modification
            if (delta_t > 0) {
                // Pre-before-post: potentiation
                // BCM theory: potentiation when w < threshold
                if (w < pImpl->threshold) {
                    eligibility += temporal_window * pImpl->etaL;
                }
            } else {
                // Post-before-pre: depression
                // BCM theory: depression when w > threshold
                if (w > pImpl->threshold) {
                    eligibility += temporal_window * pImpl->etaH;
                }
            }
        }
    }
    
    // Apply weight change with soft bounds
    if (std::abs(eligibility) > 1e-6f) {
        // Weight-dependent scaling for stability
        float weight_factor = 1.0f - std::abs(w);
        
        // Ensure weight stays within valid bounds
        float max_change = (1.0f - std::abs(w)) * pImpl->learningRate;
        eligibility = std::clamp(eligibility, -max_change, max_change);
        
        // Apply to weight
        synapse->addToWeight(eligibility);
        
        // Enforce hard bounds for numerical stability
        float new_weight = synapse->getWeight();
        if (synapse->getType() == SynapseType::Excitatory) {
            new_weight = std::max(0.0f, std::min(new_weight, 1.0f));
        } else if (synapse->getType() == SynapseType::Inhibitory) {
            new_weight = std::max(-1.0f, std::min(new_weight, 0.0f));
        }
        synapse->setWeight(new_weight);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with BCM-like normalization
    float current_weight = synapse->getWeight();
    float new_weight = current_weight + delta;
    
    // Soft bounds to prevent runaway excitation/inhibition
    if (synapse->getType() == SynapseType::Excitatory) {
        new_weight = std::max(0.0f, std::min(new_weight, 1.0f));
    } else if (synapse->getType() == SynapseType::Inhibitory) {
        new_weight = std::max(-1.0f, std::min(new_weight, 0.0f));
    }
    
    synapse->setWeight(new_weight);
}

const char* HebbianRule::getName() const {
    return "Hebbian (BCM)";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.001f, std::min(rate, 0.1f));
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float HebbianRule::getThreshold() const {
    return pImpl->threshold;
}

void HebbianRule::setEtaL(float etaL) {
    pImpl->etaL = std::max(0.001f, std::min(etaL, 0.1f));
}

float HebbianRule::getEtaL() const {
    return pImpl->etaL;
}

void HebbianRule::setEtaH(float etaH) {
    pImpl->etaH = std::max(0.001f, std::min(etaH, 0.1f));
}

float HebbianRule::getEtaH() const {
    return pImpl->etaH;
}

void HebbianRule::setBCMThreshold(float threshold) {
    pImpl->BCM_threshold = std::max(0.1f, std::min(threshold, 2.0f));
}

float HebbianRule::getBCMThreshold() const {
    return pImpl->BCM_threshold;
}

} // namespace nlm
