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
    float minWeight;
    float maxWeight;
    float covarianceThreshold;
    
    Impl() : learningRate(0.01f), minWeight(-1.0f), maxWeight(1.0f), covarianceThreshold(0.0f) {}
};

HebbianRule::HebbianRule(float learningRate) : pImpl(new Impl) {
    pImpl->learningRate = learningRate;
}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    // Implement the covariance Hebbian learning rule: Δw = η * (⟨pre·post⟩ - ⟨pre⟩⟨post⟩)
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Compute average pre- and post-synaptic activities
    float avgPre = 0.0f;
    for (Timestamp t : preSpikes) avgPre += t;
    avgPre /= static_cast<float>(preSpikes.size());
    
    float avgPost = 0.0f;
    for (Timestamp t : postSpikes) avgPost += t;
    avgPost /= static_cast<float>(postSpikes.size());
    
    // Compute covariance (pre·post - ⟨pre⟩⟨post⟩)
    float covariance = 0.0f;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            covariance += (static_cast<float>(pre) * static_cast<float>(post)) - (avgPre * avgPost);
        }
    }
    covariance /= static_cast<float>(preSpikes.size() * postSpikes.size());
    
    // Apply learning rule with adaptive modulation
    float delta = pImpl->learningRate * covariance;
    
    // Apply developmental and neuromodulatory scaling
    if (synapse->getParentBrain()) {
        auto* brain = synapse->getParentBrain();
        DevelopmentalStage stage = brain->getDevelopmentalStage();
        
        // Developmental modulation
        float devMod = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial: devMod = 1.0f; break;
            case DevelopmentalStage::CriticalPeriod: devMod = 0.8f; break;
            case DevelopmentalStage::Maturation: devMod = 0.5f; break;
            case DevelopmentalStage::Adult: devMod = 0.2f; break;
        }
        delta *= devMod;
        
        // Neuromodulatory scaling (dopamine, acetylcholine)
        if (auto* dopamine = brain->getDopamine()) {
            float dopLevel = dopamine->getLevel();
            delta *= (0.5f + 0.5f * dopLevel);  // Scale 0.5x to 1.5x
        }
        if (auto* acetylcholine = brain->getACh()) {
            float achLevel = acetylcholine->getLevel();
            delta *= (1.0f + achLevel);  // Scale 1x to 2x
        }
    }
    
    // Apply weight change with biological constraints
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp to biological weight bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
    
    // Update synaptic efficacy for future learning
    float efficacy = synapse->getEfficacy();
    float activityFactor = 1.0f;
    
    // Simple stability mechanism - don't over-strengthen synapses
    if (newWeight > pImpl->maxWeight * 0.8f) {
        activityFactor = 0.5f;
    }
    
    // Update eligibility trace for reward-modulated learning
    if (delta != 0.0f) {
        synapse->addToEligibilityTrace(delta * activityFactor);
    }
}

const char* HebbianRule::getName() const {
    return "HebbianRule";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::configure(float learningRate) {
    setLearningRate(learningRate);
    // Additional configuration could be added here
}

float HebbianRule::getDevelopmentalModulation() const {
    return pImpl->learningRate * 1.0f;  // Simplified - could be more sophisticated
}

void HebbianRule::applyDevelopmentalModulation(DevelopmentalStage stage) {
    float modulation = 1.0f;
    switch (stage) {
        case DevelopmentalStage::Initial: modulation = 1.0f; break;
        case DevelopmentalStage::CriticalPeriod: modulation = 0.8f; break;
        case DevelopmentalStage::Maturation: modulation = 0.5f; break;
        case DevelopmentalStage::Adult: modulation = 0.2f; break;
    }
    // Apply to learning rate
    setLearningRate(pImpl->learningRate * modulation);
}

float HebbianRule::getNeuromodulatoryEffect() const {
    return pImpl->learningRate;  // Could incorporate dopamine, acetylcholine
}

void HebbianRule::applyNeuromodulation(float level) {
    // Neuromodulators scale learning rate
    setLearningRate(pImpl->learningRate * (1.0f + level));
}

} // namespace nlm