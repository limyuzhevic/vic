#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , competitionStrength_(0.5f)
    , learningRate_(0.01f)
    , selectionThreshold_(0.3f)
    , noveltyWeight_(1.0f)
    , urgencyWeight_(0.5f)
    , relevanceWeight_(1.0f)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processSensoryInput(
    const std::vector<float>& sensorySignals,
    const std::vector<NeuronId>& availableNeurons) {
    
    if (sensorySignals.empty() || availableNeurons.empty()) {
        return std::vector<NeuronId>();
    }
    
    // Competitive selection of attentional focus
    std::vector<NeuronId> selectedFocus = competitiveSelection(sensorySignals, availableNeurons);
    
    if (!selectedFocus.empty()) {
        currentFocus_ = selectedFocus;
        
        // Update attentional weights
        for (size_t i = 0; i < std::min(sensorySignals.size(), availableNeurons.size()); ++i) {
            NeuronId neuron = availableNeurons[i];
            float signal = sensorySignals[i];
            
            if (signal > selectionThreshold_) {
                // Increase attention weight for strong signals
                attentionWeights_[neuron] = std::min(1.0f, attentionWeights_[neuron] + learningRate_);
            } else {
                // Decrease attention weight for weak signals
                attentionWeights_[neuron] = std::max(0.0f, attentionWeights_[neuron] - learningRate_);
            }
        }
        
        normalizeAttentionWeights();
        
        NLM_LOG_DEBUG("AttentionalSelection selected " + std::to_string(selectedFocus.size()) + " neurons");
    }
    
    return selectedFocus;
}

void AttentionalSelection::updateBasedOnReward(
    float reward,
    const std::vector<NeuronId>& selectedNeurons) {
    
    if (reward <= 0.0f) {
        return;
    }
    
    for (NeuronId neuron : selectedNeurons) {
        auto it = attentionWeights_.find(neuron);
        if (it != attentionWeights_.end()) {
            // Strengthen attentional weight for rewarded neurons
            it->second = std::min(1.0f, it->second + learningRate_ * reward);
        } else {
            attentionWeights_[neuron] = learningRate_ * reward;
        }
    }
    
    normalizeAttentionWeights();
    
    // Update attention history
    for (NeuronId neuron : selectedNeurons) {
        attentionHistory_[neuron].push_back(reward);
        // Keep only recent history
        if (attentionHistory_[neuron].size() > 10) {
            attentionHistory_[neuron].erase(attentionHistory_[neuron].begin());
        }
    }
    
    NLM_LOG_DEBUG("AttentionalSelection updated based on reward " + std::to_string(reward));
}

std::vector<NeuronId> AttentionalSelection::getCurrentFocus() const {
    return currentFocus_;
}

float AttentionalSelection::getAttentionGain(NeuronId neuron) const {
    auto it = attentionWeights_.find(neuron);
    if (it == attentionWeights_.end()) {
        return 0.0f;
    }
    return it->second;
}

bool AttentionalSelection::deservesAttention(float novelty,
                                             float urgency,
                                             float relevance) const {
    
    float attentionScore = (novelty * noveltyWeight_ +
                           urgency * urgencyWeight_ +
                           relevance * relevanceWeight_);
    
    return attentionScore > selectionThreshold_;
}

float AttentionalSelection::getAverageAttentionalFocus() const {
    if (currentFocus_.empty() || attentionWeights_.empty()) {
        return 0.0f;
    }
    
    float totalFocus = 0.0f;
    for (NeuronId neuron : currentFocus_) {
        totalFocus += getAttentionGain(neuron);
    }
    
    return totalFocus / currentFocus_.size();
}

size_t AttentionalSelection::getActiveAttentionalNeurons() const {
    return currentFocus_.size();
}

void AttentionalSelection::clear() {
    currentFocus_.clear();
    attentionWeights_.clear();
    attentionHistory_.clear();
    NLM_LOG_INFO("AttentionalSelection cleared");
}

void AttentionalSelection::learnFromReward(
    float reward,
    const std::vector<NeuronId>& neurons,
    const std::vector<float>& attentionValues) {
    
    if (reward <= 0.0f || neurons.empty()) {
        return;
    }
    
    for (size_t i = 0; i < neurons.size() && i < attentionValues.size(); ++i) {
        NeuronId neuron = neurons[i];
        float attentionValue = attentionValues[i];
        
        // Hebbian learning: strengthen connections between reward and attention
        float currentWeight = getAttentionGain(neuron);
        float newWeight = currentWeight + learningRate_ * reward * attentionValue;
        
        attentionWeights_[neuron] = std::clamp(newWeight, 0.0f, 1.0f);
    }
    
    normalizeAttentionWeights();
}

void AttentionalSelection::updateBasedOnPredictionError(
    float error,
    const std::vector<NeuronId>& currentFocus) {
    
    if (error <= 0.0f) {
        return;
    }
    
    // Update attentional weights based on prediction error
    for (NeuronId neuron : currentFocus) {
        auto it = attentionWeights_.find(neuron);
        if (it != attentionWeights_.end()) {
            // Increase attention for neurons involved in prediction errors
            it->second = std::min(1.0f, it->second + learningRate_ * error);
        }
    }
    
    normalizeAttentionWeights();
}

std::vector<float> AttentionalSelection::getNeuralAttentionPattern() const {
    std::vector<float> pattern;
    pattern.reserve(attentionWeights_.size());
    
    for (const auto& weightPair : attentionWeights_) {
        pattern.push_back(weightPair.second);
    }
    
    return pattern;
}

std::vector<NeuronId> AttentionalSelection::competitiveSelection(
    const std::vector<float>& signals,
    const std::vector<NeuronId>& neurons) {
    
    if (signals.empty() || neurons.empty() || signals.size() != neurons.size()) {
        return std::vector<NeuronId>();
    }
    
    // Compute effective attention considering competition
    std::vector<float> effectiveSignals;
    effectiveSignals.reserve(signals.size());
    
    for (size_t i = 0; i < signals.size(); ++i) {
        float signal = signals[i];
        if (signal > selectionThreshold_) {
            // Apply competition strength
            float effectiveSignal = signal * (1.0f - competitionStrength_);
            effectiveSignals.push_back(effectiveSignal);
        }
    }
    
    // Find neuron with maximum effective signal
    if (effectiveSignals.empty()) {
        return std::vector<NeuronId>();
    }
    
    float maxSignal = *std::max_element(effectiveSignals.begin(), effectiveSignals.end());
    
    if (maxSignal < selectionThreshold_) {
        return std::vector<NeuronId>();
    }
    
    // Return neuron with maximum signal
    std::vector<NeuronId> selectedNeurons;
    for (size_t i = 0; i < effectiveSignals.size(); ++i) {
        if (std::abs(effectiveSignals[i] - maxSignal) < 0.001f) {
            selectedNeurons.push_back(neurons[i]);
        }
    }
    
    return selectedNeurons;
}

float AttentionalSelection::computeAttentionGain(
    NeuronId neuron,
    float recentReward,
    const std::vector<float>& attentionHistory) const {
    
    // Base attention from current weight
    float currentGain = getAttentionGain(neuron);
    
    // Modify based on recent reward and history
    float historyFactor = 0.0f;
    if (!attentionHistory.empty()) {
        float avgHistory = std::accumulate(attentionHistory.begin(),
                                          attentionHistory.end(), 0.0f) / attentionHistory.size();
        historyFactor = avgHistory;
    }
    
    // Combine factors
    float attentionGain = currentGain * 0.6f + recentReward * 0.3f + historyFactor * 0.1f;
    
    return std::clamp(attentionGain, 0.0f, 1.0f);
}

void AttentionalSelection::normalizeAttentionWeights() {
    if (attentionWeights_.empty()) {
        return;
    }
    
    float totalWeight = 0.0f;
    for (const auto& weightPair : attentionWeights_) {
        totalWeight += weightPair.second;
    }
    
    if (totalWeight > 0.0f) {
        for (auto& weightPair : attentionWeights_) {
            weightPair.second /= totalWeight;
        }
    }
}

} // namespace nlm
