#include "DopamineGatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct DopamineGatedSTDP::Impl {
    // Basic STDP parameters
    float ltpWeight;         // Potentiation weight (A+)
    float ltdWeight;         // Depression weight (A-)
    float timeConstant;      // STDP time constant (τ)
    
    // Dopamine modulation parameters
    float dopamineGain;      // Dopamine modulation gain (γ)
    float baselineDopamine;  // Baseline dopamine level
    float dopamineLevel;     // Current dopamine level
    bool dopamineModulationEnabled;  // Whether dopamine modulation is enabled
    
    // Weight bounds
    float maxWeight;          // Maximum weight bound
    float minWeight;          // Minimum weight bound
    
    // Temporal filtering
    float dopamineFilter;    // Dopamine temporal filter constant
    Timestamp lastDopamineUpdate;  // Time of last dopamine update
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             dopamineGain(0.5f), baselineDopamine(0.1f), dopamineLevel(0.1f),
             dopamineModulationEnabled(true), dopamineFilter(10.0f),
             lastDopamineUpdate(0.0), maxWeight(1.0f), minWeight(-1.0f) {}
};

DopamineGatedSTDP::DopamineGatedSTDP() : pImpl(new Impl) {}

DopamineGatedSTDP::~DopamineGatedSTDP() = default;

void DopamineGatedSTDP::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate standard STDP weight change
    float totalDelta = 0.0f;
    
    // For each pre-synaptic spike
    for (Timestamp preTime : preSpikes) {
        // For each post-synaptic spike
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);  // Δt = t_post - t_pre
            
            if (delta_t > 0) {
                // Pre before post (positive Δt): Basic potentiation
                float basicLTP = pImpl->ltpWeight * std::exp(-delta_t / pImpl->timeConstant);
                // Dopamine modulation: strengthen LTP when dopamine is above baseline
                float dopamineModulation = (pImpl->dopamineLevel > pImpl->baselineDopamine) ? 
                    1.0f + pImpl->dopamineGain * (pImpl->dopamineLevel - pImpl->baselineDopamine) : 
                    1.0f;
                totalDelta += basicLTP * dopamineModulation;
            } else if (delta_t < 0) {
                // Post before pre (negative Δt): Basic depression
                float basicLTD = pImpl->ltdWeight * std::exp(delta_t / pImpl->timeConstant);
                // Dopamine modulation: weaken LTD when dopamine is above baseline
                float dopamineModulation = (pImpl->dopamineLevel > pImpl->baselineDopamine) ? 
                    1.0f + pImpl->dopamineGain * (pImpl->dopamineLevel - pImpl->baselineDopamine) : 
                    1.0f;
                totalDelta -= basicLTD * dopamineModulation;  // Note: subtracting depression
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
    }
}

void DopamineGatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and calculate new weight with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* DopamineGatedSTDP::getName() const {
    return "DopamineGatedSTDP";
}

void DopamineGatedSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float DopamineGatedSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void DopamineGatedSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float DopamineGatedSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void DopamineGatedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float DopamineGatedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void DopamineGatedSTDP::setDopamineGain(float gain) {
    pImpl->dopamineGain = std::clamp(gain, 0.0f, 2.0f);
}

float DopamineGatedSTDP::getDopamineGain() const {
    return pImpl->dopamineGain;
}

void DopamineGatedSTDP::setBaselineDopamine(float baseline) {
    pImpl->baselineDopamine = std::clamp(baseline, 0.0f, 1.0f);
}

float DopamineGatedSTDP::getBaselineDopamine() const {
    return pImpl->baselineDopamine;
}

void DopamineGatedSTDP::updateDopamine(float dopamineLevel) {
    // Update dopamine level with exponential smoothing
    float alpha = pImpl->dopamineFilter;
    pImpl->dopamineLevel = alpha * dopamineLevel + (1.0f - alpha) * pImpl->dopamineLevel;
}

float DopamineGatedSTDP::getDopamineLevel() const {
    return pImpl->dopamineLevel;
}

void DopamineGatedSTDP::setDopamineModulation(bool enabled) {
    pImpl->dopamineModulationEnabled = enabled;
}

bool DopamineGatedSTDP::isDopamineModulationEnabled() const {
    return pImpl->dopamineModulationEnabled;
}

void DopamineGatedSTDP::setMaxWeight(float max) {
    pImpl->maxWeight = std::clamp(max, 0.0f, 10.0f);
}

float DopamineGatedSTDP::getMaxWeight() const {
    return pImpl->maxWeight;
}

void DopamineGatedSTDP::setMinWeight(float min) {
    pImpl->minWeight = std::clamp(min, -10.0f, 0.0f);
}

float DopamineGatedSTDP::getMinWeight() const {
    return pImpl->minWeight;
}

} // namespace nlm