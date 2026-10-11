// Hebbian.cpp - Full implementation of Hebbian learning

#include "Hebbian.hpp"
#include "Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct Hebbian::Impl {
    // Learning parameters
    float learningRate = 0.01f;
    float maxWeight = 1.0f;
    float minWeight = -1.0f;
    
    // Correlation tracking for Hebbian learning
    std::unordered_map<NeuronId, float> neuronActivity;
    std::unordered_map<NeuronId, float> recentActivity;
    
    // Hebbian learning buffer for batch updates
    struct LearningEvent {
        NeuronId preNeuron;
        NeuronId postNeuron;
        float correlation;
        Timestamp timestamp;
    };
    std::vector<LearningEvent> learningEvents;
    static constexpr size_t MAX_LEARNING_EVENTS = 10000;
    
    // homeostatic regulation
    float homeostaticTarget = 0.0f;
    float homeostaticStrength = 0.01f;
    
    Impl() {
        neuronActivity.reserve(10000);
        recentActivity.reserve(10000);
        learningEvents.reserve(MAX_LEARNING_EVENTS);
    }
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() {
    delete pImpl;
}

void Hebbian::update(Synapse* synapse, 
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Record spike activity for correlation calculation
    NeuronId preNeuron = synapse->getSourceNeuron().index();
    NeuronId postNeuron = synapse->getDestinationNeuron().index();
    
    // Update recent activity
    pImpl->neuronActivity[preNeuron] = 1.0f;
    pImpl->neuronActivity[postNeuron] = 1.0f;
    pImpl->recentActivity[preNeuron] = 1.0f;
    pImpl->recentActivity[postNeuron] = 1.0f;
    
    // Calculate Hebbian weight change based on correlation
    calculateHebbianWeightChange(synapse, preSpikes, postSpikes);
    
    // Apply homeostatic regulation
    applyHomeostaticRegulation(synapse);
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply weight bounds
    newWeight = std::max(pImpl->minWeight, std::min(pImpl->maxWeight, newWeight));
    
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::max(pImpl->minWeight, maxWeight);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void Hebbian::calculateHebbianWeightChange(Synapse* synapse, 
                                           const std::vector<Timestamp>& preSpikes,
                                           const std::vector<Timestamp>& postSpikes) {
    NeuronId preNeuron = synapse->getSourceNeuron().index();
    NeuronId postNeuron = synapse->getDestinationNeuron().index();
    
    // Calculate correlation between pre- and post-synaptic activity
    float preActivity = calculateNeuronalActivity(preNeuron, preSpikes);
    float postActivity = calculateNeuronalActivity(postNeuron, postSpikes);
    
    // Compute Hebbian weight change: Δw = η * x_pre * x_post
    float correlation = preActivity * postActivity;
    float weightChange = pImpl->learningRate * correlation;
    
    // Apply the change with synaptic efficacy scaling
    float synapticEfficacy = synapse->getEligibilityTrace();
    weightChange *= synapticEfficacy;
    
    // Store learning event for potential delayed application
    if (pImpl->learningEvents.size() < pImpl->MAX_LEARNING_EVENTS) {
        Hebbian::Impl::LearningEvent event;
        event.preNeuron = preNeuron;
        event.postNeuron = postNeuron;
        event.correlation = correlation;
        event.timestamp = pImpl->recentActivity[postNeuron];
        pImpl->learningEvents.push_back(event);
    }
    
    applyWeightChange(synapse, weightChange);
}

void Hebbian::applyHomeostaticRegulation(Synapse* synapse) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    
    // Homeostatic regulation maintains synaptic balance
    // Stronger synapses get depressed, weaker synapses get potentiated
    float homeostaticChange = pImpl->homeostaticStrength * 
                             (pImpl->homeostaticTarget - currentWeight);
    
    if (std::abs(homeostaticChange) > 0.001f) {
        applyWeightChange(synapse, -homeostaticChange);
    }
}

float Hebbian::calculateNeuronalActivity(NeuronId neuronId, 
                                        const std::vector<Timestamp>& spikes) const {
    if (spikes.empty()) return 0.0f;
    
    // Calculate average firing rate over the spike history
    float totalActivity = 0.0f;
    for (Timestamp spikeTime : spikes) {
        // Simple activity model based on spike density
        totalActivity += 1.0f / (1.0f + spikeTime * 0.001f); // Weight recent spikes more
    }
    
    return totalActivity / static_cast<float>(spikes.size());
}

} // namespace nlm