#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    
    // Compute attentional weights for each neuron
    std::vector<float> neuronWeights;
    
    Impl() : brain(nullptr), inhibitionStrength_(0.5f), excitationStrength_(1.5f), competitionThreshold_(0.5f) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.5f)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(
    const std::vector<NeuronId>& competitors,
    float globalInhibition) {
    if (!brain_ || competitors.empty()) return {};
    
    // Initialize weights vector
    pImpl->neuronWeights.resize(competitors.size(), 0.0f);
    
    // Get base weights for each neuron
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get top-down bias if available
        float topDownBias = 0.0f;
        auto it = pImpl->topDownBias_.find(neuron.index());
        if (it != pImpl->topDownBias_.end()) {
            topDownBias = it->second;
        }
        
        // Get bottom-up salience if available
        float bottomUpSalience = 0.0f;
        it = pImpl->bottomUpSalience_.find(neuron.index());
        if (it != pImpl->bottomUpSalience_.end()) {
            bottomUpSalience = it->second;
        }
        
        // Calculate combined weight
        float baseWeight = topDownBias + bottomUpSalience + 1.0f;  // Base level is 1.0
        float effectiveWeight = baseWeight * excitationStrength_;
        
        // Apply global inhibition
        effectiveWeight *= (1.0f - globalInhibition);
        
        // Apply winner-take-all competition
        if (i > 0) {
            float maxCompetitorWeight = 0.0f;
            for (size_t j = 0; j < i; ++j) {
                maxCompetitorWeight = std::max(maxCompetitorWeight, pImpl->neuronWeights[j]);
            }
            
            if (effectiveWeight > maxCompetitorWeight * competitionThreshold_) {
                // This neuron wins competition
                pImpl->neuronWeights[i] = effectiveWeight;
            } else {
                pImpl->neuronWeights[i] = 0.0f;  // Suppress losing neurons
            }
        } else {
            pImpl->neuronWeights[i] = effectiveWeight;
        }
    }
    
    // Find winners based on competition threshold
    winners_.clear();
    float maxWeight = 0.0f;
    for (size_t i = 0; i < competitors.size(); ++i) {
        maxWeight = std::max(maxWeight, pImpl->neuronWeights[i]);
    }
    
    if (maxWeight > 0.0f) {
        for (size_t i = 0; i < competitors.size(); ++i) {
            if (pImpl->neuronWeights[i] > maxWeight * competitionThreshold_) {
                winners_.push_back(competitors[i]);
            }
        }
    }
    
    NLM_LOG_INFO(std::to_string(winners_.size()) + " attentional winners selected");
    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (!brain_) return;
    
    attendedRegions_.clear();
    attendedRegions_.push_back(region);
    
    // Apply attention bias to neurons in this region
    auto* brainRegion = brain_->getRegion(region);
    if (brainRegion) {
        for (const auto& pop : brainRegion->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                applyTopDownBias(neuron->getId(), 2.0f);  // Strong bias for focus
            }
        }
    }
    
    NLM_LOG_INFO("Attention focused on region " + std::to_string(region.index()));
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
    
    // Clear all biases
    pImpl->topDownBias_.clear();
    pImpl->bottomUpSalience_.clear();
    
    NLM_LOG_INFO("Attention released");
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = std::max(0.0f, std::min(strength, 2.0f));
    NLM_LOG_INFO("Inhibition strength set to " + std::to_string(inhibitionStrength_));
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = std::max(0.0f, std::min(strength, 5.0f));
    NLM_LOG_INFO("Excitation strength set to " + std::to_string(excitationStrength_));
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = std::max(0.1f, std::min(threshold, 2.0f));
    NLM_LOG_INFO("Competition threshold set to " + std::to_string(competitionThreshold_));
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    if (!brain_) return 0.0f;
    
    // Get base inhibition
    float inhibition = inhibitionStrength_;
    
    // Get top-down bias
    auto it = pImpl->topDownBias_.find(neuron.index());
    if (it != pImpl->topDownBias_.end()) {
        inhibition *= (1.0f - it->second);  // Bias reduces inhibition
    }
    
    return inhibition;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    if (!brain_) return 0.0f;
    
    // Get base excitation
    float excitation = excitationStrength_;
    
    // Get bottom-up salience
    auto it = pImpl->bottomUpSalience_.find(neuron.index());
    if (it != pImpl->bottomUpSalience_.end()) {
        excitation *= (1.0f + it->second);  // Salience increases excitation
    }
    
    return excitation;
}

void AttentionalSelection::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update attentional dynamics based on neural activity
    float totalActivity = 0.0f;
    size_t activeNeuronCount = 0;
    
    // Calculate total neural activity in attended regions
    for (RegionId region : attendedRegions_) {
        auto* brainRegion = brain_->getRegion(region);
        if (brainRegion) {
            for (const auto& pop : brainRegion->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    if (state.firingState == FiringState::Refractory) {
                        totalActivity += state.firingRate;
                        activeNeuronCount++;
                    }
                }
            }
        }
    }
    
    if (activeNeuronCount > 0) {
        float averageActivity = totalActivity / static_cast<float>(activeNeuronCount);
        
        // Adapt attentional parameters based on activity levels
        if (averageActivity > 20.0f) {  // High activity - increase focus
            inhibitionStrength_ = std::max(0.1f, inhibitionStrength_ * 0.9f);
            excitationStrength_ = std::min(3.0f, excitationStrength_ * 1.1f);
        } else if (averageActivity < 5.0f) {  // Low activity - decrease focus
            inhibitionStrength_ = std::min(1.0f, inhibitionStrength_ * 1.1f);
            excitationStrength_ = std::max(0.5f, excitationStrength_ * 0.9f);
        }
        
        NLM_LOG_INFO(std::to_string(static_cast<int>(averageActivity)) + " Hz average activity, attentional parameters adapted");
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    // Check if neuron is in an attended region
    for (RegionId region : attendedRegions_) {
        auto* brainRegion = brain_->getRegion(region);
        if (brainRegion) {
            for (const auto& pop : brainRegion->getPopulations()) {
                for (const auto* neuronPtr : pop->getNeurons()) {
                    if (neuronPtr->getId() == neuron) {
                        return true;
                    }
                }
            }
        }
    }
    
    // Check if neuron has strong top-down bias
    auto it = pImpl->topDownBias_.find(neuron.index());
    if (it != pImpl->topDownBias_.end() && it->second > 1.0f) {
        return true;
    }
    
    return false;
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    pImpl->topDownBias_[neuron.index()] = std::max(0.0f, biasStrength);
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    pImpl->bottomUpSalience_[neuron.index()] = std::max(0.0f, salienceStrength);
}

void AttentionalSelection::reset() {
    winners_.clear();
    pImpl->neuronWeights.clear();
    pImpl->topDownBias_.clear();
    pImpl->bottomUpSalience_.clear();
    
    NLM_LOG_INFO("AttentionalSelection reset");
}

} // namespace nlm