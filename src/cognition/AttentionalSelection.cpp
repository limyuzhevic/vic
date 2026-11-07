#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct AttentionalSelection::Impl {
    class Brain* brain;
    NeuronId selectedItem;
    float attentionStrength;
    float inhibitionStrength;
    float excitationStrength;
    std::vector<NeuronId> attentionHistory;
    float attentionDecayRate;
    
    Impl() : brain(nullptr), attentionStrength(0.0f), 
             inhibitionStrength(0.5f), excitationStrength(1.5f),
             attentionDecayRate(0.95f) {}
};

AttentionalSelection::AttentionalSelection() : pImpl(std::make_unique<Impl>()) {}

AttentionalSelection::~AttentionalSelection() = default;

bool AttentionalSelection::initialize(class Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize AttentionalSelection with null brain");
        return false;
    }
    
    pImpl->brain = brain;
    reset();
    
    NLM_LOG_INFO("AttentionalSelection initialized");
    return true;
}

void AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors) {
    if (competitors.empty()) return;
    
    // Simple winner-take-all selection based on competition strength
    // In a real implementation, this would involve more sophisticated competition dynamics
    float maxStrength = 0.0f;
    NeuronId winner = competitors.empty() ? INVALID_NEURON_ID : competitors[0];
    
    for (const auto& neuronId : competitors) {
        // Simple heuristic: higher index = stronger (placeholder)
        float strength = static_cast<float>(neuronId.index()) * pImpl->excitationStrength -
                        pImpl->inhibitionStrength;
        if (strength > maxStrength) {
            maxStrength = strength;
            winner = neuronId;
        }
    }
    
    pImpl->selectedItem = winner;
    pImpl->attentionStrength = maxStrength;
    pImpl->attentionHistory.push_back(winner);
    
    NLM_LOG_DEBUG("Attention selected neuron " + std::to_string(winner.index()) + 
                  " with strength " + std::to_string(attentionStrength));
}

NeuronId AttentionalSelection::getSelectedItem() const {
    return pImpl->selectedItem;
}

float AttentionalSelection::getAttentionStrength() const {
    return pImpl->attentionStrength;
}

void AttentionalSelection::setAttentionStrength(float strength) {
    pImpl->attentionStrength = strength;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay attention over time
    pImpl->attentionStrength *= pImpl->attentionDecayRate;
    
    // Keep attention history trimmed
    if (pImpl->attentionHistory.size() > 100) {
        pImpl->attentionHistory.erase(pImpl->attentionHistory.begin());
    }
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    pImpl->inhibitionStrength = strength;
}

float AttentionalSelection::getInhibitionStrength() const {
    return pImpl->inhibitionStrength;
}

void AttentionalSelection::setExcitationStrength(float strength) {
    pImpl->excitationStrength = strength;
}

float AttentionalSelection::getExcitationStrength() const {
    return pImpl->excitationStrength;
}

void AttentionalSelection::reset() {
    pImpl->selectedItem = INVALID_NEURON_ID;
    pImpl->attentionStrength = 0.0f;
    pImpl->attentionHistory.clear();
    NLM_LOG_INFO("AttentionalSelection reset");
}

} // namespace nlm