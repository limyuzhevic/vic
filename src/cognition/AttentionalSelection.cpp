#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    std::vector<std::pair<NeuronId, float>> attentionWeights;
    
    Impl() : brain(nullptr) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salience) {
    // Store bottom-up salience
    pImpl->attentionWeights.push_back({neuron, salience * excitationStrength_});
    
    // Add noise to prevent perfect selection
    float noise = (brain_ && brain_->getRandomGenerator()) ? 
        brain_->getRandomGenerator()->uniformReal(-0.1f, 0.1f) : 0.0f;
    
    // Update attention weights
    for (auto& weight : pImpl->attentionWeights) {
        if (weight.first == neuron) {
            weight.second = salience * excitationStrength_ + noise;
        }
    }
}

void AttentionalSelection::applyTopDownSelection(NeuronId neuron, float weight) {
    // Apply top-down selection based on goals
    bool found = false;
    for (auto& aw : pImpl->attentionWeights) {
        if (aw.first == neuron) {
            aw.second = weight * inhibitionStrength_;
            found = true;
            break;
        }
    }
    
    if (!found) {
        pImpl->attentionWeights.push_back({neuron, weight * inhibitionStrength_});
    }
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors) {
    std::vector<NeuronId> result;
    
    if (competitors.empty()) return result;
    
    // Find neuron with highest attention score
    float bestScore = -1000.0f;
    NeuronId bestNeuron = 0;
    
    for (NeuronId neuron : competitors) {
        float score = computeAttentionScore(neuron);
        if (score > bestScore) {
            bestScore = score;
            bestNeuron = neuron;
        }
    }
    
    if (bestScore > 0.0f) {
        result.push_back(bestNeuron);
        NLM_LOG_INFO("AttentionalSelection selected neuron " + 
                    std::to_string(bestNeuron.index()) + 
                    " with score " + std::to_string(bestScore));
    } else {
        NLM_LOG_INFO("AttentionalSelection: no neuron selected (all scores <= 0)");
    }
    
    return result;
}

void AttentionalSelection::update(double timestep) {
    // Decay old attention weights
    for (auto& aw : pImpl->attentionWeights) {
        aw.second *= (1.0f - timestep);
    }
    
    // Remove weights that have decayed too much
    pImpl->attentionWeights.erase(
        std::remove_if(pImpl->attentionWeights.begin(), pImpl->attentionWeights.end(),
            [this](const std::pair<NeuronId, float>& aw) {
                return aw.second < 0.01f;  // Very small weight
            }),
        pImpl->attentionWeights.end()
    );
}

float AttentionalSelection::computeAttentionScore(NeuronId neuron) const {
    // Find this neuron's attention weight
    for (const auto& aw : pImpl->attentionWeights) {
        if (aw.first == neuron) {
            return aw.second;
        }
    }
    return 0.0f;
}

void AttentionalSelection::reset() {
    pImpl->attentionWeights.clear();
    NLM_LOG_INFO("AttentionalSelection reset");
}

} // namespace nlm
