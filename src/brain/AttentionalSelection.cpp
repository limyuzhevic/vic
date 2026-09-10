#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    
    // Attention dynamics
    std::vector<float> neuronActivationLevels;
    std::vector<float> attentionWeights;
    
    // Competition dynamics
    std::vector<float> globalInhibition;
    std::vector<float> localExcitation;
    
    // Winner-take-all mechanisms
    std::vector<NeuronId> attendedNeurons;
    std::vector<NeuronId> suppressedNeurons;
    
    // Salience computation
    std::unordered_map<uint64_t, float> stimulusSalience;
    std::unordered_map<uint64_t, float> goalRelevance;
    
    Impl() : brain(nullptr) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.8f)
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
    if (!brain_ || competitors.empty()) {
        return {};
    }
    
    // Get activation levels of competing neurons
    std::vector<float> activations;
    activations.reserve(competitors.size());
    
    for (const auto& neuronId : competitors) {
        float activation = getNeuronActivation(neuronId);
        activations.push_back(activation);
    }
    
    // Apply global inhibition
    float maxActivation = *std::max_element(activations.begin(), activations.end());
    if (maxActivation > 0.0f) {
        for (size_t i = 0; i < activations.size(); ++i) {
            activations[i] = activations[i] / (globalInhibition + 1.0f);
        }
    }
    
    // Find winners using normalized activation
    std::vector<NeuronId> winners;
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activations[i] > competitionThreshold_) {
            winners.push_back(competitors[i]);
        }
    }
    
    // If no clear winner, select highest activation neuron
    if (winners.empty() && !activations.empty()) {
        size_t maxIdx = std::max_element(activations.begin(), activations.end()) - activations.begin();
        winners.push_back(competitors[maxIdx]);
    }
    
    // Update attention state
    pImpl->attendedNeurons = winners;
    pImpl->suppressedNeurons.clear();
    
    // Apply lateral inhibition to suppress other neurons
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (std::find(winners.begin(), winners.end(), competitors[i]) == winners.end()) {
            pImpl->suppressedNeurons.push_back(competitors[i]);
        }
    }
    
    return winners;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (!brain_) return;
    
    // Reset attention to all regions first
    releaseAttention();
    
    // Focus on specified region
    pImpl->attendedRegions_.clear();
    pImpl->attendedRegions_.push_back(region);
    
    // Increase activation for neurons in this region
    auto* regionPtr = brain_->getRegion(region);
    if (regionPtr) {
        for (auto& pop : regionPtr->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                float currentAct = getNeuronActivation(neuron->getId());
                float enhancedAct = std::min(1.0f, currentAct * 2.0f); // Boost activation
                neuronSalience_[neuron->getId().value] = enhancedAct;
            }
        }
    }
}

void AttentionalSelection::releaseAttention() {
    pImpl->attendedRegions_.clear();
    pImpl->attendedNeurons.clear();
    pImpl->suppressedNeurons.clear();
    
    // Reset salience levels
    for (auto& pair : pImpl->stimulusSalience) {
        pair.second *= 0.5f; // Decay salience
    }
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return pImpl->attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = std::clamp(strength, 0.0f, 1.0f);
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = std::clamp(strength, 0.0f, 2.0f);
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    if (brain_) {
        auto* regionPtr = brain_->getRegion(neuron.getId() / 1000);
        if (regionPtr) {
            for (auto& pop : regionPtr->getPopulations()) {
                for (auto* n : pop->getNeurons()) {
                    if (n->getId() == neuron) {
                        // Calculate inhibition based on surrounding activity
                        float inhibition = inhibitionStrength_ * 0.5f;
                        
                        // Check if neuron is suppressed
                        if (std::find(pImpl->suppressedNeurons.begin(), 
                                     pImpl->suppressedNeurons.end(), neuron) != pImpl->suppressedNeurons.end()) {
                            inhibition += 0.5f;
                        }
                        
                        return inhibition;
                    }
                }
            }
        }
    }
    return inhibitionStrength_;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    float excitation = excitationStrength_ * 1.0f;
    
    // Add attention-based excitation
    auto it = pImpl->stimulusSalience.find(neuron.value);
    if (it != pImpl->stimulusSalience.end()) {
        excitation += it->second * 0.5f;
    }
    
    // Check if neuron is attended
    if (std::find(pImpl->attendedNeurons.begin(), pImpl->attendedNeurons.end(), neuron) != pImpl->attendedNeurons.end()) {
        excitation *= 2.0f;
    }
    
    return excitation;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay salience over time
    for (auto& pair : pImpl->stimulusSalience) {
        pair.second *= (1.0f - dt * 0.1f);
    }
    
    // Update attention weights based on competition
    if (pImpl->attendedNeurons.empty() && !pImpl->suppressedNeurons.empty()) {
        // Gradually release suppression
        float decay = dt * 0.01f;
        for (auto& neuronId : pImpl->suppressedNeurons) {
            auto it = pImpl->stimulusSalience.find(neuronId.value);
            if (it != pImpl->stimulusSalience.end()) {
                it->second *= (1.0f - decay);
            }
        }
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return std::find(pImpl->attendedNeurons.begin(), pImpl->attendedNeurons.end(), neuron) != pImpl->attendedNeurons.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    if (biasStrength > 0.0f) {
        pImpl->goalRelevance[neuron.value] = biasStrength;
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    if (salienceStrength > 0.0f) {
        pImpl->stimulusSalience[neuron.value] = salienceStrength;
    }
}

void AttentionalSelection::reset() {
    releaseAttention();
    pImpl->stimulusSalience.clear();
    pImpl->goalRelevance.clear();
    pImpl->neuronActivationLevels.clear();
    pImpl->attentionWeights.clear();
}

float AttentionalSelection::getNeuronActivation(NeuronId neuron) const {
    // Get actual neuron activation from brain if available
    if (brain_) {
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* n : pop->getNeurons()) {
                    if (n->getId() == neuron) {
                        return std::abs(n->getState().membranePotential - n->getState().restingPotential) / 20.0f;
                    }
                }
            }
        }
    }
    return 0.0f;
}

} // namespace nlm