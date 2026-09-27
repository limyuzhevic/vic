#include "AttentionalSelection.hpp"
#include "NeuralWorkingMemory.hpp"
#include "NeuralEpisodicMemory.hpp"
#include "NeuralPlanner.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AttentionalSelection::Impl {
    // Current attention state
    std::vector<NeuronId> currentGoal;
    std::set<NeuronId> focusedNeurons;
    float attentionFocus;  // 0.0 to 1.0
    float attentionSpan;   // How many neurons to attend to
    float selectionThreshold;  // Minimum activation for selection
    float distractibilityThreshold;  // Threshold for reducing distractibility
    
    // Modulation parameters
    float signalNoiseEnhancement;  // Enhances signal-to-noise ratio
    float distractibilityReduction;  // Reduces interference
    float attentionGain;  // Global attention gain
    
    // Statistics
    size_t selectedTracesCount;
    size_t totalDistractibilityReduced;
    float totalAttentionFocus;
    size_t attentionUpdateCount;
    
    // Decay parameters
    float attentionDecayRate;
    float gainDecayRate;
    
    // Integration tracking
    NeuralWorkingMemory* workingMemory;
    NeuralEpisodicMemory* episodicMemory;
    NeuralPlanner* neuralPlanner;
    
    Impl() : attentionFocus(0.0f), attentionSpan(5.0f), selectionThreshold(0.7f),
             distractibilityThreshold(0.8f), signalNoiseEnhancement(1.0f),
             distractibilityReduction(0.5f), attentionGain(1.0f),
             selectedTracesCount(0), totalDistractibilityReduced(0),
             totalAttentionFocus(0.0f), attentionUpdateCount(0),
             attentionDecayRate(0.95f), gainDecayRate(0.9f),
             workingMemory(nullptr), episodicMemory(nullptr), neuralPlanner(nullptr) {}
};

AttentionalSelection::AttentionalSelection() : pImpl(new Impl) {}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::focusAttention(const std::vector<NeuronId>& neurons) {
    // Replace currently focused neurons with new ones
    pImpl->focusedNeurons.clear();
    pImpl->focusedNeurons.insert(neurons.begin(), neurons.end());
    pImpl->attentionFocus = std::min(1.0f, (float)neurons.size() / pImpl->attentionSpan);
    pImpl->selectedTracesCount = neurons.size();
    pImpl->attentionUpdateCount++;
}

void AttentionalSelection::setGoal(const std::vector<NeuronId>& neurons) {
    pImpl->currentGoal = neurons;
    // Focus on goal neurons if attention span permits
    if (!neurons.empty() && pImpl->focusedNeurons.size() < pImpl->attentionSpan) {
        std::vector<NeuronId> goalNeurons;
        for (const auto& neuron : neurons) {
            if (pImpl->focusedNeurons.size() < pImpl->attentionSpan) {
                pImpl->focusedNeurons.insert(neuron);
                goalNeurons.push_back(neuron);
            }
        }
        if (!goalNeurons.empty()) {
            pImpl->attentionFocus = std::min(1.0f, (float)goalNeurons.size() / pImpl->attentionSpan);
        }
    }
    pImpl->attentionUpdateCount++;
}

bool AttentionalSelection::isGazingAt(const std::vector<NeuronId>& neurons) const {
    for (const auto& neuron : neurons) {
        if (pImpl->focusedNeurons.find(neuron) != pImpl->focusedNeurons.end()) {
            return true;
        }
    }
    return false;
}

void AttentionalSelection::enhanceSignalToNoise(float amount) {
    pImpl->signalNoiseEnhancement += amount;
    pImpl->signalNoiseEnhancement = std::max(1.0f, pImpl->signalNoiseEnhancement);
}

void AttentionalSelection::reduceDistractibility(float amount) {
    pImpl->distractibilityReduction += amount;
    pImpl->distractibilityReduction = std::max(0.0f, pImpl->distractibilityReduction);
    pImpl->totalDistractibilityReduced++;
}

void AttentionalSelection::adjustAttentionGain(float gain) {
    pImpl->attentionGain += gain;
    pImpl->attentionGain = std::max(0.0f, std::min(2.0f, pImpl->attentionGain));
}

float AttentionalSelection::getAttentionFocus() const {
    return pImpl->attentionFocus;
}

const std::vector<NeuronId>& AttentionalSelection::getCurrentGoal() const {
    return pImpl->currentGoal;
}

const std::set<NeuronId>& AttentionalSelection::getFocusedNeurons() const {
    return pImpl->focusedNeurons;
}

void AttentionalSelection::updateWorkingMemorySelection(NeuralWorkingMemory* workingMemory) {
    pImpl->workingMemory = workingMemory;
    if (workingMemory && !pImpl->focusedNeurons.empty()) {
        // Strengthen traces for focused neurons
        for (const auto& neuronId : pImpl->focusedNeurons) {
            float currentStrength = workingMemory->getTraceStrength(neuronId);
            workingMemory->setTraceStrength(neuronId, currentStrength * pImpl->signalNoiseEnhancement);
        }
    }
}

void AttentionalSelection::updateEpisodicMemoryEncoding(NeuralEpisodicMemory* episodicMemory) {
    pImpl->episodicMemory = episodicMemory;
    if (episodicMemory && !pImpl->focusedNeurons.empty()) {
        // Enhance encoding of attended experiences
        float encodingEnhancement = pImpl->signalNoiseEnhancement * 0.5f;
        // Note: episodic memory encoding enhancement would be implemented here
        // depending on the episodic memory interface
    }
}

void AttentionalSelection::integrateWithNeuralPlanner(NeuralPlanner* planner) {
    pImpl->neuralPlanner = planner;
    if (planner && !pImpl->currentGoal.empty()) {
        // Set planning goal based on attention goal
        planner->setPlanningGoal(pImpl->currentGoal);
    }
}

void AttentionalSelection::setAttentionSpan(float span) {
    pImpl->attentionSpan = std::max(1.0f, span);
}

void AttentionalSelection::setSelectionThreshold(float threshold) {
    pImpl->selectionThreshold = std::min(1.0f, std::max(0.0f, threshold));
}

void AttentionalSelection::setDistractibilityThreshold(float threshold) {
    pImpl->distractibilityThreshold = std::min(1.0f, std::max(0.0f, threshold));
}

void AttentionalSelection::decayAttention(float dt) {
    pImpl->attentionFocus *= std::pow(pImpl->attentionDecayRate, dt);
    pImpl->signalNoiseEnhancement *= std::pow(pImpl->gainDecayRate, dt);
    pImpl->distractibilityReduction = std::max(0.0f, pImpl->distractibilityReduction - dt * 0.1f);
}

void AttentionalSelection::reset() {
    pImpl->currentGoal.clear();
    pImpl->focusedNeurons.clear();
    pImpl->attentionFocus = 0.0f;
    pImpl->signalNoiseEnhancement = 1.0f;
    pImpl->distractibilityReduction = 0.0f;
    pImpl->attentionGain = 1.0f;
    pImpl->selectedTracesCount = 0;
    pImpl->totalDistractibilityReduced = 0;
    pImpl->totalAttentionFocus = 0.0f;
    pImpl->attentionUpdateCount = 0;
}

size_t AttentionalSelection::getSelectedTracesCount() const {
    return pImpl->selectedTracesCount;
}

size_t AttentionalSelection::getTotalDistractibilityReduced() const {
    return pImpl->totalDistractibilityReduced;
}

float AttentionalSelection::getAverageAttentionFocus() const {
    if (pImpl->attentionUpdateCount > 0) {
        return pImpl->totalAttentionFocus / pImpl->attentionUpdateCount;
    }
    return 0.0f;
}

} // namespace nlm
