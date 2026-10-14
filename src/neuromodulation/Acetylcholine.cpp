#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionGain;
    float filteringStrength;
    float memoryConsolidationRate;
    SimulationStep lastAttentionUpdate;
    std::vector<NeuronId> attendedNeurons;
    
    Impl() : level(0.0f), baseline(0.05f), attentionGain(2.0f), 
             filteringStrength(1.5f), memoryConsolidationRate(0.8f), 
             lastAttentionUpdate(0) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances plasticity for attended information
    return 0.5f + 0.5f * pImpl->level * pImpl->attentionGain;
}

void Acetylcholine::update(TimestepDuration dt, const std::vector<NeuronId>& neurons, 
                         SimulationStep currentStep) {
    // Update attention dynamics based on acetylcholine level
    float targetLevel = baseline + (pImpl->level - baseline) * pImpl->attentionGain;
    pImpl->level = std::max(baseline, pImpl->level - pImpl->baseline * static_cast<float>(dt));
    
    // Apply cholinergic filtering and attention modulation
    if (currentStep - pImpl->lastAttentionUpdate > 10) { // Update attention every 10 steps
        // Simulate cholinergic attention selection
        for (NeuronId neuron : neurons) {
            if (pImpl->level > baseline * 1.5f) { // High ACh enhances attention
                // Neuron becomes more likely to be attended
                pImpl->attendedNeurons.push_back(neuron);
            } else if (pImpl->level < baseline * 0.5f) { // Low ACh reduces attention
                // Remove from attention if level drops
                pImpl->attendedNeurons.erase(
                    std::remove(pImpl->attendedNeurons.begin(), pImpl->attendedNeurons.end(), neuron),
                    pImpl->attendedNeurons.end()
                );
            }
        }
        pImpl->lastAttentionUpdate = currentStep;
    }
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::max(0.1f, std::min(5.0f, gain));
}

float Acetylcholine::getFilteringStrength() const {
    return pImpl->filteringStrength;
}

void Acetylcholine::setFilteringStrength(float strength) {
    pImpl->filteringStrength = std::max(0.5f, std::min(3.0f, strength));
}

float Acetylcholine::getMemoryConsolidationRate() const {
    return pImpl->memoryConsolidationRate;
}

void Acetylcholine::setMemoryConsolidationRate(float rate) {
    pImpl->memoryConsolidationRate = std::max(0.1f, std::min(2.0f, rate));
}

const std::vector<NeuronId>& Acetylcholine::getAttendedNeurons() const {
    return pImpl->attendedNeurons;
}

void Acetylcholine::clearAttendedNeurons() {
    pImpl->attendedNeurons.clear();
}

float Acetylcholine::computeAttentionalModulation(NeuronId neuron) const {
    // Compute how much attention this neuron receives
    bool isAttended = std::find(pImpl->attendedNeurons.begin(), 
                               pImpl->attendedNeurons.end(), 
                               neuron) != pImpl->attendedNeurons.end();
    
    if (isAttended) {
        // Attended neurons get enhanced gain
        return pImpl->attentionGain * pImpl->level;
    }
    return 1.0f / pImpl->filteringStrength; // Non-attended neurons are suppressed
}

} // namespace nlm