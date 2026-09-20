#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct AttentionalSelection::Impl {
    // Neural competition network
    std::vector<std::vector<float>> competitionMatrix;
    std::vector<float> neuronExcitability;
    std::vector<float> neuronInhibition;
    
    // Competition dynamics
    float globalInhibitionLevel;
    float lateralInhibitionRadius;
    float winnerTakeAllThreshold;
    
    // Salience and bias accumulators
    std::vector<float> salienceValues;
    std::vector<float> biasValues;
    
    // Competition results
    std::vector<NeuronId> currentWinners;
    std::vector<float> winnerConfirmations;
    
    Impl() : 
        globalInhibitionLevel(0.5f),
        lateralInhibitionRadius(2.0f),
        winnerTakeAllThreshold(0.7f) {}
};

AttentionalSelection::AttentionalSelection() : 
    pImpl(new Impl),
    brain_(nullptr),
    inhibitionStrength_(0.5f),
    excitationStrength_(1.5f),
    competitionThreshold_(0.5f) {
    NLM_LOG_INFO("AttentionalSelection created");
}

AttentionalSelection::~AttentionalSelection() {
    NLM_LOG_INFO("AttentionalSelection destroyed");
}

void AttentionalSelection::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize AttentionalSelection with null brain");
        return;
    }
    
    brain_ = brain;
    
    // Initialize salience and bias vectors based on brain structure
    size_t maxNeuronId = 0;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto& neuron : pop->getNeurons()) {
                if (neuron->getId().index() > maxNeuronId) {
                    maxNeuronId = neuron->getId().index();
                }
            }
        }
    }
    
    // Reserve space for all possible neuron IDs
    pImpl->neuronExcitability.resize(maxNeuronId + 1, 0.0f);
    pImpl->neuronInhibition.resize(maxNeuronId + 1, 0.0f);
    pImpl->salienceValues.resize(maxNeuronId + 1, 0.0f);
    pImpl->biasValues.resize(maxNeuronId + 1, 0.0f);
    
    // Initialize competition matrix (symmetric, zero diagonal)
    pImpl->competitionMatrix.resize(maxNeuronId + 1, std::vector<float>(maxNeuronId + 1, 0.0f));
    
    NLM_LOG_INFO("AttentionalSelection initialized with " + std::to_string(maxNeuronId + 1) + " neurons");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    if (!brain_ || competitors.empty()) return std::vector<NeuronId>();
    
    // Reset winners
    pImpl->currentWinners.clear();
    
    // Calculate competition scores for each neuron
    std::vector<std::pair<float, NeuronId>> scores;
    
    for (const auto& neuronId : competitors) {
        float totalExcitability = 0.0f;
        float totalInhibition = 0.0f;
        
        // Calculate net input for this neuron
        for (const auto& otherId : competitors) {
            if (neuronId == otherId) continue;
            
            // Get connection strength between neurons (simplified: use neuron indices)
            size_t idx1 = neuronId.index();
            size_t idx2 = otherId.index();
            
            if (idx1 < pImpl->competitionMatrix.size() && idx2 < pImpl->competitionMatrix[idx1].size()) {
                float connectionStrength = pImpl->competitionMatrix[idx1][idx2];
                
                // Add excitatory influence if competing neuron has higher salience
                totalExcitability += connectionStrength * (1.0f + getSalienceFor(otherId));
                
                // Add inhibitory influence
                totalInhibition += connectionStrength * (1.0f + getInhibitionFor(otherId));
            }
        }
        
        // Apply global inhibition
        totalInhibition += globalInhibition * getInhibitionFor(neuronId);
        
        // Net competition score
        float score = totalExcitability - totalInhibition;
        
        // Apply winner-take-all if above threshold
        if (score > pImpl->winnerTakeAllThreshold) {
            score = 1.0f;  // Winner score
        } else if (score > competitionThreshold_) {
            score = 0.0f;  // Loser score
        } else {
            score = 0.5f;  // Undecided score
        }
        
        scores.emplace_back(score, neuronId);
    }
    
    // Sort by score (descending)
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Select winners based on scores
    for (const auto& scorePair : scores) {
        if (scorePair.first > 0.5f) {  // Winner threshold
            pImpl->currentWinners.push_back(scorePair.second);
        }
    }
    
    // Update neuron inhibition based on competition results
    for (const auto& winner : pImpl->currentWinners) {
        // Increase inhibition for other competing neurons
        for (const auto& otherId : competitors) {
            if (otherId != winner) {
                increaseInhibitionFor(otherId, 0.1f);
            }
        }
    }
    
    return pImpl->currentWinners;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    attendedRegions_.clear();
    attendedRegions_.push_back(region);
    
    // Boost excitation for neurons in focused region
    auto* regionPtr = brain_->getRegion(region);
    if (regionPtr) {
        for (auto& pop : regionPtr->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                increaseExcitabilityFor(neuron->getId(), 0.5f);
            }
        }
    }
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
    
    // Reset all neuron excitability/inhibition
    for (size_t i = 0; i < pImpl->neuronExcitability.size(); ++i) {
        pImpl->neuronExcitability[i] = 0.0f;
        pImpl->neuronInhibition[i] = 0.0f;
    }
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
    if (neuron.index() >= pImpl->neuronInhibition.size()) {
        return inhibitionStrength_;
    }
    return pImpl->neuronInhibition[neuron.index()];
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    if (neuron.index() >= pImpl->neuronExcitability.size()) {
        return excitationStrength_;
    }
    return pImpl->neuronExcitability[neuron.index()];
}

void AttentionalSelection::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Decay neuron excitability/inhibition over time
    for (size_t i = 0; i < pImpl->neuronExcitability.size(); ++i) {
        pImpl->neuronExcitability[i] *= (1.0f - dt * 0.01f);
        pImpl->neuronInhibition[i] *= (1.0f - dt * 0.01f);
        
        // Ensure they don't go negative
        pImpl->neuronExcitability[i] = std::max(0.0f, pImpl->neuronExcitability[i]);
        pImpl->neuronInhibition[i] = std::max(0.0f, pImpl->neuronInhibition[i]);
    }
    
    // Update salience values based on neural activity
    updateSalienceBasedOnActivity();
    
    // Reset salience decay
    for (size_t i = 0; i < pImpl->salienceValues.size(); ++i) {
        pImpl->salienceValues[i] *= (1.0f - dt * 0.005f);
        pImpl->biasValues[i] *= (1.0f - dt * 0.005f);
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    if (attendedRegions_.empty()) return false;
    
    auto* region = brain_->getRegion(attendedRegions_[0]);
    if (!region) return false;
    
    for (auto& pop : region->getPopulations()) {
        for (auto* n : pop->getNeurons()) {
            if (n->getId() == neuron) return true;
        }
    }
    
    return false;
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    size_t idx = neuron.index();
    if (idx < pImpl->biasValues.size()) {
        pImpl->biasValues[idx] += biasStrength * 10.0f;  // Strong bias effect
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    size_t idx = neuron.index();
    if (idx < pImpl->salienceValues.size()) {
        pImpl->salienceValues[idx] += salienceStrength * 10.0f;  // Strong salience effect
    }
}

void AttentionalSelection::reset() {
    // Clear all competition states
    pImpl->currentWinners.clear();
    pImpl->winnerConfirmations.clear();
    
    // Reset all neuron states
    for (size_t i = 0; i < pImpl->neuronExcitability.size(); ++i) {
        pImpl->neuronExcitability[i] = 0.0f;
        pImpl->neuronInhibition[i] = 0.0f;
    }
    
    // Clear attended regions
    attendedRegions_.clear();
    
    NLM_LOG_INFO("AttentionalSelection reset");
}

// Helper functions
float AttentionalSelection::getSalienceFor(NeuronId neuron) const {
    size_t idx = neuron.index();
    if (idx >= pImpl->salienceValues.size()) return 0.0f;
    return pImpl->salienceValues[idx];
}

void AttentionalSelection::increaseExcitabilityFor(NeuronId neuron, float amount) {
    size_t idx = neuron.index();
    if (idx < pImpl->neuronExcitability.size()) {
        pImpl->neuronExcitability[idx] += amount;
        pImpl->neuronExcitability[idx] = std::min(2.0f, pImpl->neuronExcitability[idx]);
    }
}

void AttentionalSelection::increaseInhibitionFor(NeuronId neuron, float amount) {
    size_t idx = neuron.index();
    if (idx < pImpl->neuronInhibition.size()) {
        pImpl->neuronInhibition[idx] += amount;
        pImpl->neuronInhibition[idx] = std::min(1.0f, pImpl->neuronInhibition[idx]);
    }
}

void AttentionalSelection::updateSalienceBasedOnActivity() {
    // Calculate salience based on neural firing rates
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->getId().index() < pImpl->salienceValues.size()) {
                    // Salience based on firing rate and recent activity
                    float firingRate = neuron->getState().firingRate;
                    pImpl->salienceValues[neuron->getId().index()] += firingRate * 0.1f;
                }
            }
        }
    }
}

} // namespace nlm