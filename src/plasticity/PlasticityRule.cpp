#include "PlasticityRule.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float regularizationStrength;  // For homeostasis
    float neuromodulationFactor;     // Dopamine modulation
    float competitionFactor;         // For competition effects
    
    Impl() : learningRate(0.01f), regularizationStrength(0.001f), 
             neuromodulationFactor(1.0f), competitionFactor(0.1f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Enhanced Hebbian learning with biological accuracy
    if (!preSpikes.empty() && !postSpikes.empty() && synapse) {
        // Calculate spike timing relationships
        std::vector<float> spikeTimeDifferences;
        
        for (Timestamp pre : preSpikes) {
            for (Timestamp post : postSpikes) {
                float deltaT = static_cast<float>(post - pre);  // Post minus pre
                if (std::abs(deltaT) < 100.0f) {  // Consider spikes within 100ms
                    spikeTimeDifferences.push_back(deltaT);
                }
            }
        }
        
        if (!spikeTimeDifferences.empty()) {
            // Enhanced STDP-like Hebbian learning
            float totalLTP = 0.0f;  // Long-term potentiation
            float totalLTD = 0.0f;  // Long-term depression
            
            for (float deltaT : spikeTimeDifferences) {
                if (deltaT > 0) {
                    // Pre before post: LTP
                    float ltpFactor = std::exp(-deltaT / 20.0f);
                    totalLTP += ltpFactor * pImpl->learningRate;
                } else if (deltaT < 0) {
                    // Post before pre: LTD
                    float ltdFactor = std::exp(deltaT / 20.0f);
                    totalLTD += ltdFactor * pImpl->learningRate * 0.9f;  // Slightly stronger depression
                }
            }
            
            // Apply neuromodulation (dopamine factor)
            float neuromodulatedLTP = totalLTP * pImpl->neuromodulationFactor;
            float neuromodulatedLTD = totalLTD * pImpl->neuromodulationFactor;
            
            // Apply homeostatic regularization
            float currentWeight = synapse->getWeight();
            float homeostaticCorrection = pImpl->regularizationStrength * 
                                         (std::abs(currentWeight) - 1.0f);
            
            // Calculate net weight change
            float netChange = (neuromodulatedLTP - neuromodulatedLTD) + homeostaticCorrection;
            
            // Apply competition effects based on overall network activity
            if (!preSpikes.empty() && !postSpikes.empty()) {
                float competitionEffect = pImpl->competitionFactor * 
                                          static_cast<float>(preSpikes.size() + postSpikes.size()) / 10.0f;
                netChange *= (1.0f + competitionEffect);
            }
            
            // Apply metabolic constraints
            float metabolicCost = synapse->getMetabolicCost();
            if (metabolicCost > 2.0f) {
                // Reduce plasticity when metabolic load is high
                netChange *= (2.0f - metabolicCost) / 2.0f;
            }
            
            // Apply weight change with bounds
            float newWeight = currentWeight + netChange;
            
            // Apply weight limits (biological constraints)
            newWeight = std::clamp(newWeight, -1.0f, 1.0f);
            
            synapse->setWeight(newWeight);
        }
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse) {
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight + delta;
        
        // Apply biological weight bounds
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
        synapse->setWeight(newWeight);
    }
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

void HebbianRule::setRegularizationStrength(float strength) {
    pImpl->regularizationStrength = std::clamp(strength, 0.0001f, 0.01f);
}

void HebbianRule::setNeuromodulationFactor(float factor) {
    pImpl->neuromodulationFactor = std::clamp(factor, 0.0f, 2.0f);
}

void HebbianRule::setCompetitionFactor(float factor) {
    pImpl->competitionFactor = std::clamp(factor, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

float HebbianRule::getRegularizationStrength() const {
    return pImpl->regularizationStrength;
}

float HebbianRule::getNeuromodulationFactor() const {
    return pImpl->neuromodulationFactor;
}

float HebbianRule::getCompetitionFactor() const {
    return pImpl->competitionFactor;
}

} // namespace nlm
