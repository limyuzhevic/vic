#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    
    // Neural dynamics for attention
    std::vector<float> globalInhibition_;
    std::vector<float> neuronalActivation_;
    std::vector<float> topDownSignals_;
    std::vector<float> bottomUpSignals_;
    
    // Competition dynamics
    std::vector<float> recurrentWeights_;
    std::vector<float> inhibitionMatrix_;
    
    // Attention state
    float globalGain_;
    float competitionThreshold_;
    float noiseLevel_;
    
    // Neural parameters
    float timeConstant_;
    float decayRate_;
    
    // Random number generator
    std::mt19937 rng;
    std::normal_distribution<float> noiseDist;
    
    Impl() : brain(nullptr), globalGain_(1.0f), competitionThreshold_(0.5f), 
             noiseLevel_(0.01f), timeConstant_(10.0f), decayRate_(0.1f),
             rng(std::chrono::steady_clock::now().time_since_epoch().count()),
             noiseDist(0.0f, 1.0f) {}
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

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                             float globalInhibition) {
    if (!brain_ || competitors.empty()) {
        return {};
    }
    
    // Get current neural activations
    std::vector<float> activations;
    activations.reserve(competitors.size());
    
    for (NeuronId neuronId : competitors) {
        float activation = 0.0f;
        
        // Get neuron activation from working memory or direct state
        if (brain_->getWorkingMemory()) {
            activation = brain_->getWorkingMemory()->getNeuronActivation(neuronId);
        } else {
            // Fallback to default activation
            activation = 0.5f;
        }
        
        // Add top-down bias
        auto it = pImpl->topDownBias_.find(neuronId.index());
        if (it != pImpl->topDownBias_.end()) {
            activation += it->second * excitationStrength_;
        }
        
        // Add bottom-up salience
        it = pImpl->bottomUpSalience_.find(neuronId.index());
        if (it != pImpl->bottomUpSalience_.end()) {
            activation += it->second * excitationStrength_;
        }
        
        // Apply global inhibition
        activation *= (1.0f - globalInhibition * inhibitionStrength_);
        
        // Add neural noise for competition dynamics
        activation += pImpl->noiseDist(pImpl->rng) * pImpl->noiseLevel_;
        
        // Clamp to reasonable range
        activation = std::max(0.0f, std::min(1.0f, activation));
        activations.push_back(activation);
    }
    
    // Update neural dynamics with recurrent connections
    updateNeuralDynamics(activations);
    
    // Apply competitive inhibition (lateral inhibition)
    applyLateralInhibition(activations);
    
    // Select winners based on threshold
    std::vector<NeuronId> winners;
    winners.reserve(activations.size());
    
    for (size_t i = 0; i < activations.size(); ++i) {
        if (activations[i] > competitionThreshold_) {
            winners.push_back(competitors[i]);
        }
    }
    
    // If no winners, select top neuron
    if (winners.empty() && !activations.empty()) {
        size_t bestIdx = std::max_element(activations.begin(), activations.end()) - activations.begin();
        winners.push_back(competitors[bestIdx]);
    }
    
    // Update winners for reference
    winners_.clear();
    for (NeuronId winner : winners) {
        winners_.push_back(winner);
    }
    
    NLM_LOG_INFO("AttentionalSelection processed " + std::to_string(competitors.size()) + 
                 " competitors, selected " + std::to_string(winners.size()) + " winners");
    
    return winners;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (!brain_) return;
    
    // Clear current attended regions
    attendedRegions_.clear();
    
    // Add the focused region
    attendedRegions_.push_back(region);
    
    // Apply strong top-down bias to neurons in this region
    for (auto& regionPtr : brain_->getRegions()) {
        if (regionPtr->getId() == region) {
            for (auto& pop : regionPtr->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Strong top-down bias to enhance attention to this region
                    applyTopDownBias(neuron->getId(), 2.0f * inhibitionStrength_);
                }
            }
        }
    }
    
    NLM_LOG_INFO("AttentionalSelection focusing on region " + std::to_string(region.index()));
}

void AttentionalSelection::releaseAttention() {
    // Clear top-down biases
    topDownBias_.clear();
    
    // Allow all regions to be attended
    attendedRegions_.clear();
    
    // Get all regions for potential attention
    if (brain_) {
        for (auto& region : brain_->getRegions()) {
            attendedRegions_.push_back(region->getId());
        }
    }
    
    NLM_LOG_INFO("AttentionalSelection released attention - all regions available");
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = std::max(0.0f, std::min(1.0f, strength));
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = std::max(0.0f, std::min(2.0f, strength));
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = std::max(0.0f, std::min(1.0f, threshold));
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    auto it = pImpl->globalInhibition_.find(neuron.index());
    return it != pImpl->globalInhibition_.end() ? it->second : 0.0f;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    // Find best excitation from top-down and bottom-up signals
    float topDown = 0.0f, bottomUp = 0.0f;
    
    auto it = pImpl->topDownBias_.find(neuron.index());
    if (it != pImpl->topDownBias_.end()) {
        topDown = it->second;
    }
    
    it = pImpl->bottomUpSalience_.find(neuron.index());
    if (it != pImpl->bottomUpSalience_.end()) {
        bottomUp = it->second;
    }
    
    return topDown + bottomUp;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Update attention dynamics
    pImpl->globalGain_ += (1.0f - pImpl->globalGain_) * pImpl->decayRate_ * dt;
    
    // Decay top-down and bottom-up biases
    for (auto& pair : pImpl->topDownBias_) {
        pair.second *= (1.0f - pImpl->decayRate_ * dt);
    }
    
    for (auto& pair : pImpl->bottomUpSalience_) {
        pair.second *= (1.0f - pImpl->decayRate_ * dt);
    }
    
    // Update recurrent weights for maintenance
    updateRecurrentDynamics(dt);
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    pImpl->topDownBias_[neuron.index()] += biasStrength;
    
    // Also add to attended regions if we have brain context
    if (brain_ && !attendedRegions_.empty()) {
        // Mark neuron as part of attended region
        for (RegionId region : attendedRegions_) {
            if (brain_->getRegion(region)) {
                // Neuron is considered attended
                break;
            }
        }
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    pImpl->bottomUpSalience_[neuron.index()] += salienceStrength;
}

void AttentionalSelection::reset() {
    topDownBias_.clear();
    bottomUpSalience_.clear();
    
    attendedRegions_.clear();
    if (brain_) {
        for (auto& region : brain_->getRegions()) {
            attendedRegions_.push_back(region->getId());
        }
    }
    
    winners_.clear();
    
    pImpl->globalInhibition_.clear();
    pImpl->neuronalActivation_.clear();
    pImpl->topDownSignals_.clear();
    pImpl->bottomUpSignals_.clear();
    pImpl->recurrentWeights_.clear();
    pImpl->inhibitionMatrix_.clear();
    
    pImpl->globalGain_ = 1.0f;
    pImpl->competitionThreshold_ = 0.5f;
    pImpl->noiseLevel_ = 0.01f;
}

// Helper methods for neural dynamics

void AttentionalSelection::updateNeuralDynamics(const std::vector<float>& activations) {
    // Store activations in neural dynamics
    pImpl->neuronalActivation_ = activations;
    
    // Update recurrent weights based on activation patterns
    for (size_t i = 0; i < activations.size(); ++i) {
        float targetWeight = activations[i] * pImpl->globalGain_;
        
        if (i < pImpl->recurrentWeights_.size()) {
            // Soft update with momentum
            pImpl->recurrentWeights_[i] += (targetWeight - pImpl->recurrentWeights_[i]) * 0.1f;
        }
    }
}

void AttentionalSelection::applyLateralInhibition(std::vector<float>& activations) const {
    if (activations.empty()) return;
    
    // Simple lateral inhibition: reduce activation of nearby competitors
    size_t n = activations.size();
    
    for (size_t i = 0; i < n; ++i) {
        float maxNeighbor = 0.0f;
        
        // Find maximum among other neurons
        for (size_t j = 0; j < n; ++j) {
            if (i != j) {
                maxNeighbor = std::max(maxNeighbor, activations[j]);
            }
        }
        
        // Apply lateral inhibition
        float inhibition = inhibitionStrength_ * maxNeighbor;
        activations[i] = std::max(0.0f, activations[i] - inhibition);
    }
}

void AttentionalSelection::updateRecurrentDynamics(TimestepDuration dt) {
    // Update recurrent weights with decay
    for (float& weight : pImpl->recurrentWeights_) {
        weight *= (1.0f - decayRate_ * dt);
    }
}

} // namespace nlm
