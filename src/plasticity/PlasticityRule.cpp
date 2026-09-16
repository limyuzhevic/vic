#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float trace;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f), trace(0.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Real Hebbian learning: "Neurons that fire together, wire together"
    // Weight change is proportional to the correlation of pre- and post-synaptic activity
    // 
    // Algorithm:
    // 1. For each pair of pre- and post-synaptic spikes within a time window:
    //    - Increase synaptic weight (LTP)
    // 2. Accumulate spike trace for eligibility
    // 3. Update weight based on trace and learning rate
    // 4. Enforce weight bounds to prevent instability
    
    if (!synapse || preSpikes.empty() || postSpikes.empty() || dt <= 0.0f) {
        return;
    }
    
    // Calculate spike correlations within Hebbian time window
    float correlation = 0.0f;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float timeDiff = std::abs(pre - post);
            if (timeDiff < 10.0f) {  // 10ms Hebbian window
                // Exponential decay of correlation with time difference
                correlation += std::exp(-timeDiff / 5.0f);
            }
        }
    }
    
    if (correlation > 0.0f) {
        // Update eligibility trace
        pImpl->trace += correlation * dt;
        
        // Apply weight change based on trace
        float weightChange = pImpl->learningRate * pImpl->trace;
        
        // Update synaptic weight
        synapse->addToWeight(weightChange);
        
        // Decay eligibility trace
        pImpl->trace *= 0.95f;
        
        // Enforce weight bounds to prevent instability
        float currentWeight = synapse->getWeight();
        if (currentWeight > pImpl->maxWeight) {
            synapse->setWeight(pImpl->maxWeight);
        } else if (currentWeight < pImpl->minWeight) {
            synapse->setWeight(pImpl->minWeight);
        }
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        return;
    }
    
    synapse->addToWeight(delta);
    
    // Enforce weight bounds to prevent synaptic instability
    float currentWeight = synapse->getWeight();
    if (currentWeight > pImpl->maxWeight) {
        synapse->setWeight(pImpl->maxWeight);
    } else if (currentWeight < pImpl->minWeight) {
        synapse->setWeight(pImpl->minWeight);
    }
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

void HebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = maxWeight;
}

float HebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

float HebbianRule::getTrace() const {
    return pImpl->trace;
}

} // namespace nlm
