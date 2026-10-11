// AntiHebbian.cpp - Implementation of Anti-Hebbian learning rule

#include "AntiHebbian.hpp"
#include "Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct AntiHebbian::Impl {
    // Anti-Hebbian parameters
    float learningRate = 0.01f;
    float inhibitionThreshold = 0.5f;
    float maxWeight = 1.0f;
    float minWeight = -1.0f;
    
    // Competition tracking for anti-Hebbian learning
    std::unordered_map<NeuronId, float> inhibitionSignals;
    std::unordered_map<NeuronId, float> synapticCompetition;
    
    Impl() {
        inhibitionSignals.reserve(10000);
        synapticCompetition.reserve(10000);
    }
};

AntiHebbian::AntiHebbian() : pImpl(new Impl) {}

AntiHebbian::~AntiHebbian() {
    delete pImpl;
}

void AntiHebbian::update(Synapse* synapse, 
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Anti-Hebbian principle: suppress correlations, promote competition
    // Apply inhibitory learning based on activity patterns
    calculateAntiHebbianWeightChange(synapse, preSpikes, postSpikes);
}

void AntiHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply weight bounds and anti-Hebbian constraints
    newWeight = std::max(pImpl->minWeight, std::min(pImpl->maxWeight, newWeight));
    
    // Anti-Hebbian rule: maintain weight diversity
    // Stronger synapses get depressed, weaker synapses get potentiated
    float homeostaticFactor = calculateHomeostaticFactor(currentWeight);
    newWeight = currentWeight + (newWeight - currentWeight) * homeostaticFactor;
    
    synapse->setWeight(newWeight);
}

const char* AntiHebbian::getName() const {
    return "AntiHebbian";
}

void AntiHebbian::setInhibitionThreshold(float threshold) {
    pImpl->inhibitionThreshold = std::max(0.0f, threshold);
}

float AntiHebbian::getInhibitionThreshold() const {
    return pImpl->inhibitionThreshold;
}

void AntiHebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::max(pImpl->minWeight, maxWeight);
}

float AntiHebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void AntiHebbian::calculateAntiHebbianWeightChange(Synapse* synapse, 
                                                  const std::vector<Timestamp>& preSpikes,
                                                  const std::vector<Timestamp>& postSpikes) {
    NeuronId preNeuron = synapse->getSourceNeuron().index();
    NeuronId postNeuron = synapse->getDestinationNeuron().index();
    
    // Calculate pre- and post-synaptic activities
    float preActivity = calculateNeuronalActivity(preSpikes);
    float postActivity = calculateNeuronalActivity(postSpikes);
    
    // Anti-Hebbian weight change: Δw = -η * x_pre * x_post
    // This suppresses correlated activity and promotes competition
    float correlation = preActivity * postActivity;
    float weightChange = -pImpl->learningRate * correlation;
    
    // Apply activity-dependent inhibition
    float inhibitionSignal = calculateInhibitionSignal(preNeuron, postNeuron);
    weightChange *= (1.0f + inhibitionSignal);
    
    // Apply competitive dynamics
    float competitionFactor = calculateCompetitionFactor(preNeuron, postNeuron);
    weightChange *= competitionFactor;
    
    applyWeightChange(synapse, weightChange);
}

float AntiHebbian::calculateHomeostaticFactor(float currentWeight) const {
    // Homeostatic regulation maintains synaptic stability
    // Stronger synapses get down-weighted, weaker synapses get up-weighted
    float homeostaticTarget = 0.0f;
    float homeostaticStrength = 0.01f;
    
    return 1.0f - homeostaticStrength * (currentWeight - homeostaticTarget);
}

float AntiHebbian::calculateInhibitionSignal(NeuronId preNeuron, NeuronId postNeuron) {
    // Calculate inhibitory signals based on recent activity
    float preInhibition = pImpl->inhibitionSignals[preNeuron];
    float postInhibition = pImpl->inhibitionSignals[postNeuron];
    
    // Combine inhibition signals
    float totalInhibition = preInhibition + postInhibition;
    
    // Normalize to reasonable range
    totalInhibition = std::max(0.0f, std::min(1.0f, totalInhibition));
    
    // Update inhibition signals
    pImpl->inhibitionSignals[preNeuron] = std::max(0.0f, preInhibition - 0.1f);
    pImpl->inhibitionSignals[postNeuron] = std::max(0.0f, postInhibition - 0.1f);
    
    return totalInhibition * pImpl->inhibitionThreshold;
}

float AntiHebbian::calculateCompetitionFactor(NeuronId preNeuron, NeuronId postNeuron) {
    // Calculate competition between pre- and post-synaptic neurons
    float preCompetition = pImpl->synapticCompetition[preNeuron];
    float postCompetition = pImpl->synapticCompetition[postNeuron];
    
    // Higher competition reduces weight change
    float totalCompetition = preCompetition + postCompetition;
    
    // Normalize competition factor
    float competitionFactor = std::max(0.1f, 1.0f - totalCompetition * 0.5f);
    
    // Update competition signals
    pImpl->synapticCompetition[preNeuron] = std::min(1.0f, preCompetition + 0.05f);
    pImpl->synapticCompetition[postNeuron] = std::min(1.0f, postCompetition + 0.05f);
    
    return competitionFactor;
}

float AntiHebbian::calculateNeuronalActivity(const std::vector<Timestamp>& spikes) const {
    if (spikes.empty()) return 0.0f;
    
    // Calculate average firing rate
    float totalActivity = 0.0f;
    for (Timestamp spikeTime : spikes) {
        // Recent spikes have more impact
        float weight = 1.0f / (1.0f + spikeTime * 0.001f);
        totalActivity += weight;
    }
    
    return totalActivity / static_cast<float>(spikes.size());
}

} // namespace nlm