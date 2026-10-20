#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AttentionalSelection::Impl {
    class Brain* brain;
    std::vector<std::string> attendedRegions;
    float inhibitionStrength;
    float excitationStrength;
    float competitionThreshold;
    std::vector<float> neuronInhibition;
    std::vector<float> neuronExcitation;
    std::vector<bool> attendedNeurons;
    std::vector<size_t> winners;
    std::vector<float> topDownBias;
    std::vector<float> bottomUpSalience;
    float globalInhibition;
    float attentionDecayRate;
    
    Impl() 
        : brain(nullptr)
        , inhibitionStrength(0.5f)
        , excitationStrength(1.5f)
        , competitionThreshold(0.3f)
        , globalInhibition(1.0f)
        , attentionDecayRate(0.1f) {}
};

AttentionalSelection::AttentionalSelection() : pImpl(new Impl) {}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Attentional selection system initialized");
    
    // Initialize vectors based on potential neural population size
    // In practice, this would be set up when brain regions are created
    pImpl->neuronInhibition.resize(100, 0.0f);  // Placeholder size
    pImpl->neuronExcitation.resize(100, 0.0f);
    pImpl->attendedNeurons.resize(100, false);
    pImpl->topDownBias.resize(100, 0.0f);
    pImpl->bottomUpSalience.resize(100, 0.0f);
}

float AttentionalSelection::process_competition(const std::vector<float>& competitors,
                                               float globalInhibition) {
    if (competitors.empty()) {
        return 0.0f;
    }
    
    // Apply global inhibition
    std::vector<float> adjustedCompetitors = competitors;
    for (size_t i = 0; i < adjustedCompetitors.size(); ++i) {
        adjustedCompetitors[i] = std::max(0.0f, adjustedCompetitors[i] - globalInhibition);
    }
    
    // Apply competition (winner-take-all)
    float maxValue = 0.0f;
    size_t winnerIndex = 0;
    
    for (size_t i = 0; i < adjustedCompetitors.size(); ++i) {
        if (adjustedCompetitors[i] > maxValue) {
            maxValue = adjustedCompetitors[i];
            winnerIndex = i;
        }
    }
    
    // Apply inhibition to other neurons
    std::vector<float> inhibitionMap(adjustedCompetitors.size(), 0.0f);
    inhibitionMap[winnerIndex] = pImpl->inhibitionStrength;
    
    for (size_t i = 0; i < inhibitionMap.size(); ++i) {
        if (i != winnerIndex) {
            inhibitionMap[i] = pImpl->inhibitionStrength * 0.5f;
        }
    }
    
    // Store winners and inhibition values
    pImpl->winners.clear();
    for (size_t i = 0; i < adjustedCompetitors.size(); ++i) {
        if (i != winnerIndex && inhibitionMap[i] > pImpl->competitionThreshold) {
            pImpl->winners.push_back(i);
        }
    }
    
    // Update inhibition and excitation for neurons
    for (size_t i = 0; i < pImpl->neuronInhibition.size(); ++i) {
        pImpl->neuronInhibition[i] = inhibitionMap[i % inhibitionMap.size()];
        pImpl->neuronExcitation[i] = pImpl->excitationStrength;
        pImpl->attendedNeurons[i] = (i == winnerIndex);
    }
    
    // Log attention selection
    std::string winnerStr = "Winner: " + std::to_string(winnerIndex);
    NLM_LOG_DEBUG(winnerStr.c_str());
    
    return maxValue;
}

void AttentionalSelection::focus_on_region(const std::string& regionId) {
    // Check if region exists in brain
    if (pImpl->brain) {
        auto regions = pImpl->brain->getRegions();
        for (const auto& region : regions) {
            // In real implementation, would check region name/ID
            pImpl->attendedRegions.push_back(region->getName());
        }
    }
    
    NLM_LOG_INFO(("Attending to region: " + regionId).c_str());
}

void AttentionalSelection::release_attention() {
    pImpl->attendedRegions.clear();
    pImpl->attendedNeurons.assign(pImpl->attendedNeurons.size(), false);
    NLM_LOG_INFO("Attention released");
}

std::vector<std::string> AttentionalSelection::get_attended_regions() const {
    return pImpl->attendedRegions;
}

void AttentionalSelection::set_inhibition_strength(float strength) {
    pImpl->inhibitionStrength = std::max(0.0f, strength);
}

void AttentionalSelection::set_excitation_strength(float strength) {
    pImpl->excitationStrength = std::max(0.0f, strength);
}

void AttentionalSelection::set_competition_threshold(float threshold) {
    pImpl->competitionThreshold = std::max(0.0f, threshold);
}

float AttentionalSelection::get_inhibition_for(size_t neuronId) const {
    if (neuronId < pImpl->neuronInhibition.size()) {
        return pImpl->neuronInhibition[neuronId];
    }
    return 0.0f;
}

float AttentionalSelection::get_excitation_for(size_t neuronId) const {
    if (neuronId < pImpl->neuronExcitation.size()) {
        return pImpl->neuronExcitation[neuronId];
    }
    return 0.0f;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay attention over time
    for (size_t i = 0; i < pImpl->topDownBias.size(); ++i) {
        pImpl->topDownBias[i] = std::max(0.0f, pImpl->topDownBias[i] - pImpl->attentionDecayRate * static_cast<float>(dt));
    }
    
    for (size_t i = 0; i < pImpl->bottomUpSalience.size(); ++i) {
        pImpl->bottomUpSalience[i] = std::max(0.0f, pImpl->bottomUpSalience[i] - pImpl->attentionDecayRate * static_cast<float>(dt));
    }
}

bool AttentionalSelection::is_attended(size_t neuronId) const {
    if (neuronId < pImpl->attendedNeurons.size()) {
        return pImpl->attendedNeurons[neuronId];
    }
    return false;
}

std::vector<size_t> AttentionalSelection::get_winners() const {
    return pImpl->winners;
}

void AttentionalSelection::apply_top_down_bias(size_t neuronId, float biasStrength) {
    if (neuronId < pImpl->topDownBias.size()) {
        pImpl->topDownBias[neuronId] += biasStrength;
    }
}

void AttentionalSelection::apply_bottom_up_salience(size_t neuronId, float salienceStrength) {
    if (neuronId < pImpl->bottomUpSalience.size()) {
        pImpl->bottomUpSalience[neuronId] += salienceStrength;
    }
}

void AttentionalSelection::reset() {
    pImpl->attendedRegions.clear();
    pImpl->neuronInhibition.assign(pImpl->neuronInhibition.size(), 0.0f);
    pImpl->neuronExcitation.assign(pImpl->neuronExcitation.size(), 0.0f);
    pImpl->attendedNeurons.assign(pImpl->attendedNeurons.size(), false);
    pImpl->winners.clear();
    pImpl->topDownBias.assign(pImpl->topDownBias.size(), 0.0f);
    pImpl->bottomUpSalience.assign(pImpl->bottomUpSalience.size(), 0.0f);
    NLM_LOG_INFO("Attentional selection system reset");
}

} // namespace nlm