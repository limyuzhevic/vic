#include "AntiHebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxDepression;
    float minWeight;
    float maxWeight;
    
    Impl() : learningRate(0.01f), maxDepression(0.02f), minWeight(-1.0f), maxWeight(1.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    /*
     * Anti-Hebbian learning implementation based on spike-timing covariance
     * 
     * Mathematical formulation (Anti-Hebbian covariance rule):
     * Δw = -η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Where:
     *   η = learning rate (anti-hebbian parameter)
     *   ⟨pre⟩ = average firing rate of pre-synaptic neuron
     *   ⟨post⟩ = average firing rate of post-synaptic neuron  
     *   ⟨pre * post⟩ = average co-activity (correlation) between neurons
     * 
     * Biological inspiration:
     *   - "Neurons that fire together, wire apart"
     *   - Synaptic depression when correlated activity occurs
     *   - Forms inhibitory connections and balances E/I ratio
     *   - Important for stabilizing network activity
     *   - Used in forming inhibitory circuits and preventing runaway excitation
     *   
     * Implementation details:
     *   - Uses same covariance calculation as Hebbian but with opposite sign
     *   - Biological correlation window (50ms) for spike pairing
     *   - Applied to form appropriate E/I balance
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate average firing rates (per ms)
    float preRate = static_cast<float>(preSpikes.size()) / 1000.0f;  // spikes per ms
    float postRate = static_cast<float>(postSpikes.size()) / 1000.0f; // spikes per ms
    
    // Calculate average co-activity within biological window (50ms)
    size_t coactivityCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(std::abs(postTime - preTime));
            if (dt < 50.0f) {  // 50ms biological correlation window
                ++coactivityCount;
            }
        }
    }
    
    // Calculate average co-activity rate (spikes per ms²)
    float avgCoactivity = static_cast<float>(coactivityCount) / 
                         (1000.0f * static_cast<float>(preSpikes.size() * postSpikes.size()) + 1.0f);
    
    // Calculate covariance: ⟨pre*post⟩ - ⟨pre⟩⟨post⟩
    float covariance = avgCoactivity - (preRate * postRate);
    
    // Apply anti-Hebbian learning rule (opposite sign of Hebbian)
    float delta = -pImpl->learningRate * covariance;
    
    // Apply with bounds and minimum threshold
    if (std::abs(delta) > 1e-8f) {
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::setMaxDepression(float max) {
    pImpl->maxDepression = std::clamp(max, 0.0f, 1.0f);
}

float AntiHebbianRule::getMaxDepression() const {
    return pImpl->maxDepression;
}

} // namespace nlm