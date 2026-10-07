#include "PlasticityRule.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct HebbianRule::Impl {
    // STDP parameters (based on biological models)
    float learningRate;
    float ttpLTP;      // Time constant for LTP (pre before post)
    float ttpLTD;      // Time constant for LTD (post before pre)
    float maxWeight;   // Maximum synaptic weight
    float minWeight;   // Minimum synaptic weight
    float weightDecay; // Activity-dependent weight decay
    
    // Homeostatic mechanisms
    float targetMeanWeight;    // Target average synaptic weight
    float homeostaticRate;     // Rate of homeostatic adjustment
    float competitionFactor;   // Competition for resources
    
    // Short-term plasticity
    float facilitation;       // Short-term facilitation
    float depression;         // Short-term depression
    float tsfRate;            // Time constant for facilitation
    float tsdRate;            // Time constant for depression
    
    // Neuromodulatory integration
    float dopaminergicGain;   // Dopamine modulation factor
    float acetylcholineGain; // ACh modulation factor
    
    // Activity history for STDP
    float preActivity;        // Recent presynaptic activity
    float postActivity;       // Recent postsynaptic activity
    float eligibilityTrace;   // Eligibility trace for STDP
    float weightHistory;      // Recent weight history
    
    Impl() : learningRate(0.01f), ttpLTP(20.0f), ttpLTD(20.0f), maxWeight(1.0f), 
             minWeight(0.01f), weightDecay(0.001f), targetMeanWeight(0.5f),
             homeostaticRate(0.005f), competitionFactor(0.8f), facilitation(1.0f),
             depression(1.0f), tsfRate(100.0f), tsdRate(50.0f), 
             dopaminergicGain(1.0f), acetylcholineGain(1.0f),
             preActivity(0.0f), postActivity(0.0f), eligibilityTrace(0.0f),
             weightHistory(targetMeanWeight) {}
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
    
    float deltaWeight = 0.0f;
    
    // Process spike pairs for STDP
    deltaWeight = computeSTDP(preSpikes, postSpikes, dt);
    
    // Apply homeostatic regulation
    float homeostaticAdjustment = computeHomeostaticAdjustment(synapse);
    deltaWeight += homeostaticAdjustment;
    
    // Apply neuromodulatory modulation
    float neuromodulatoryFactor = computeNeuromodulatoryFactor();
    deltaWeight *= neuromodulatoryFactor;
    
    // Apply short-term plasticity modulation
    float STPAdjustment = computeSTPAdjustment();
    deltaWeight *= (1.0f + STPAdjustment);
    
    // Apply learning rate
    deltaWeight *= pImpl->learningRate;
    
    // Ensure weight stays within bounds
    float newWeight = synapse->getWeight() + deltaWeight;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply weight
    synapse->setWeight(newWeight);
    
    // Update activity history
    pImpl->preActivity = 1.0f;
    pImpl->postActivity = 1.0f;
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - pImpl->weightDecay);
    pImpl->weightHistory = newWeight;
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// Compute STDP based on spike timing
float HebbianRule::computeSTDP(const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    float totalDelta = 0.0f;
    
    // Process all spike pairs
    for (const auto& preSpike : preSpikes) {
        for (const auto& postSpike : postSpikes) {
            float timeDiff = postSpike - preSpike;
            
            if (timeDiff > 0) {
                // Postsynaptic spike follows presynaptic spike -> LTP
                float ltpWeight = std::exp(-timeDiff / pImpl->ttpLTP);
                totalDelta += ltpWeight;
            } else if (timeDiff < 0) {
                // Presynaptic spike follows postsynaptic spike -> LTD
                float ltdWeight = std::exp(timeDiff / pImpl->ttpLTD);  // timeDiff is negative
                totalDelta -= ltdWeight;
            }
        }
    }
    
    return totalDelta;
}

// Compute homeostatic adjustment to maintain target weight
float HebbianRule::computeHomeostaticAdjustment(Synapse* synapse) {
    if (!synapse) return 0.0f;
    
    float currentWeight = synapse->getWeight();
    float error = pImpl->targetMeanWeight - currentWeight;
    
    // Apply homeostatic scaling
    float adjustment = error * pImpl->homeostaticRate;
    
    // Apply competition for limited resources
    adjustment *= pImpl->competitionFactor;
    
    return adjustment;
}

// Compute neuromodulatory factor
float HebbianRule::computeNeuromodulatoryFactor() {
    // Integrate dopamine and acetylcholine effects
    float modFactor = 1.0f;
    
    // Dopamine enhances LTP, reduces LTD
    if (pImpl->dopaminergicGain > 1.0f) {
        modFactor *= pImpl->dopaminergicGain;
    }
    
    // Acetylcholine promotes plasticity
    modFactor *= (1.0f + pImpl->acetylcholineGain * 0.5f);
    
    return std::clamp(modFactor, 0.1f, 3.0f);
}

// Compute short-term plasticity adjustment
float HebbianRule::computeSTPAdjustment() {
    // Update STP states
    pImpl->facilitation = std::max(0.1f, pImpl->facilitation * std::exp(-1.0f / pImpl->tsfRate));
    pImpl->depression = std::max(0.1f, pImpl->depression * std::exp(-1.0f / pImpl->tsdRate));
    
    // Facilitation enhances synaptic efficacy, depression reduces it
    return (pImpl->facilitation - 1.0f) - (1.0f - pImpl->depression);
}

} // namespace nlm
