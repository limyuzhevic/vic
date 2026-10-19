// Implementation for AttentionalSelection - Neural attention through competition
// NOT Transformer attention - uses lateral inhibition and gain modulation

#include "AttentionalSelection.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_set>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    
    // Competition and selection state
    std::vector<RegionId> attendedRegions_;
    std::vector<NeuronId> winners_;
    std::vector<float> neuronSalience_;
    
    // Biases
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
    
    // Dynamic attention parameters
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
    float attentionDecayRate_;
    float winnerAmplification_;
    float inhibitorySurroundRadius_;
    
    // Internal state
    std::vector<float> neuronInhibition_;
    std::vector<float> neuronExcitation_;
    float globalInhibitionLevel_;
    float recentNoveltySum_;
    size_t noveltyHistorySize_;
    
    // Random generator for stochastic attention
    std::mt19937 rng;
    std::uniform_real_distribution<float> randDist;
};

AttentionalSelection::AttentionalSelection() : pImpl(std::make_unique<Impl>()) {
    pImpl->brain = nullptr;
    pImpl->inhibitionStrength_ = 1.0f;
    pImpl->excitationStrength_ = 1.0f;
    pImpl->competitionThreshold_ = 0.6f;
    pImpl->attentionDecayRate_ = 0.95f;
    pImpl->winnerAmplification_ = 2.0f;
    pImpl->inhibitorySurroundRadius_ = 5.0f;
    pImpl->globalInhibitionLevel_ = 0.5f;
    pImpl->recentNoveltySum_ = 0.0f;
    pImpl->noveltyHistorySize_ = 10;
    
    // Seed random generator
    std::random_device rd;
    pImpl->rng.seed(rd());
    pImpl->randDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize based on brain properties
    if (pImpl->brain) {
        size_t totalNeurons = pImpl->brain->getTotalNeuronCount();
        pImpl->neuronInhibition_.resize(totalNeurons, 0.0f);
        pImpl->neuronExcitation_.resize(totalNeurons, 0.0f);
        pImpl->neuronSalience_.resize(totalNeurons, 0.0f);
    }
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                                float globalInhibition) {
    pImpl->winners_.clear();
    
    if (!pImpl->brain || competitors.empty()) {
        return pImpl->winners_;
    }
    
    // Store current global inhibition
    pImpl->globalInhibitionLevel_ = globalInhibition;
    
    // Calculate net scores for each competitor
    std::vector<std::pair<NeuronId, float>> scoredCompetitors;
    
    for (const NeuronId& neuronId : competitors) {
        if (neuronId.value >= pImpl->neuronInhibition_.size()) {
            continue;  // Safety check
        }
        
        // Get current neuron activity
        float activity = 0.0f;
        Neuron* neuron = pImpl->brain->getNeuron(neuronId);
        if (neuron) {
            // Use membrane potential as activity proxy
            activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        }
        
        // Apply top-down bias
        float bias = pImpl->topDownBias_.count(neuronId.value) ? pImpl->topDownBias_[neuronId.value] : 0.0f;
        
        // Apply bottom-up salience
        float salience = pImpl->bottomUpSalience_.count(neuronId.value) ? pImpl->bottomUpSalience_[neuronId.value] : 0.0f;
        
        // Calculate final score
        float score = activity * pImpl->excitationStrength_ + bias + salience;
        
        // Apply global inhibition
        score *= (1.0f - pImpl->globalInhibitionLevel_);
        
        scoredCompetitors.push_back({neuronId, score});
    }
    
    // Sort by score (highest first)
    std::sort(scoredCompetitors.begin(), scoredCompetitors.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Select winners above threshold
    float threshold = pImpl->competitionThreshold_ * 100.0f; // Scale threshold
    size_t winnerCount = 0;
    
    for (const auto& pair : scoredCompetitors) {
        if (pair.second >= threshold) {
            pImpl->winners_.push_back(pair.first);
            winnerCount++;
        } else {
            break; // Winners are sorted, so we can stop early
        }
    }
    
    // Apply winner amplification
    for (size_t i = 0; i < pImpl->winners_.size(); ++i) {
        NeuronId winner = pImpl->winners_[i];
        float amplification = pImpl->winnerAmplification_ * (1.0f + 0.1f * (1.0f - static_cast<float>(i) / std::max<size_t>(1, pImpl->winners_.size())));
        
        pImpl->neuronExcitation_[winner.value] = std::min(1.0f, pImpl->neuronExcitation_[winner.value] * amplification);
        pImpl->neuronInhibition_[winner.value] = std::max(0.0f, pImpl->neuronInhibition_[winner.value] / amplification);
    }
    
    return pImpl->winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (!pImpl->brain) return;
    
    pImpl->attendedRegions_.clear();
    pImpl->attendedRegions_.push_back(region);
    
    // Apply top-down bias to neurons in this region
    auto* regionPtr = pImpl->brain->getRegion(region);
    if (!regionPtr) return;
    
    for (const auto& pop : regionPtr->getPopulations()) {
        for (Neuron* neuron : pop->getNeurons()) {
            NeuronId id = neuron->getId();
            pImpl->topDownBias_[id.value] = 1.5f; // Strong focus bias
            pImpl->neuronSalience_[id.value] = 1.0f;
        }
    }
}

void AttentionalSelection::releaseAttention() {
    pImpl->attendedRegions_.clear();
    
    // Reset all biases and salience
    pImpl->topDownBias_.clear();
    pImpl->bottomUpSalience_.clear();
    pImpl->neuronSalience_.assign(pImpl->neuronSalience_.size(), 0.0f);
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return pImpl->attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    pImpl->inhibitionStrength_ = std::max(0.0f, std::min(2.0f, strength));
}

void AttentionalSelection::setExcitationStrength(float strength) {
    pImpl->excitationStrength_ = std::max(0.0f, std::min(2.0f, strength));
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    pImpl->competitionThreshold_ = std::max(0.0f, std::min(1.0f, threshold));
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    if (neuron.value >= pImpl->neuronInhibition_.size()) return pImpl->globalInhibitionLevel_;
    return pImpl->neuronInhibition_[neuron.value];
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    if (neuron.value >= pImpl->neuronExcitation_.size()) return 0.0f;
    return pImpl->neuronExcitation_[neuron.value];
}

void AttentionalSelection::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Decay attention biases over time
    for (auto& pair : pImpl->topDownBias_) {
        pair.second *= pImpl->attentionDecayRate_;
    }
    
    for (auto& pair : pImpl->bottomUpSalience_) {
        pair.second *= pImpl->attentionDecayRate_;
    }
    
    // Update novelty-based salience
    pImpl->recentNoveltySum_ *= pImpl->attentionDecayRate_;
    
    // Apply lateral inhibition between neurons
    applyLateralInhibition();
    
    // Update region-level attention
    updateRegionAttention();
}

void AttentionalSelection::applyLateralInhibition() {
    if (!pImpl->brain) return;
    
    // Clear previous inhibition/excitation
    std::fill(pImpl->neuronInhibition_.begin(), pImpl->neuronInhibition_.end(), pImpl->globalInhibitionLevel_);
    std::fill(pImpl->neuronExcitation_.begin(), pImpl->neuronExcitation_.end(), 0.0f);
    
    // Calculate inhibition based on spatial proximity and competition
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (size_t i = 0; i < pop->getNeurons().size(); ++i) {
                Neuron* neuron1 = pop->getNeurons()[i];
                if (!neuron1) continue;
                
                // Calculate competition with other neurons in same population
                float maxCompetition = 0.0f;
                for (size_t j = 0; j < pop->getNeurons().size(); ++j) {
                    if (i == j) continue;
                    
                    Neuron* neuron2 = pop->getNeurons()[j];
                    if (!neuron2) continue;
                    
                    // Calculate similarity based on position (if available)
                    float competition = 1.0f; // Simplified - assumes all neurons compete equally
                    maxCompetition = std::max(maxCompetition, competition);
                }
                
                // Apply inhibition
                pImpl->neuronInhibition_[neuron1->getId().value] += maxCompetition * pImpl->inhibitionStrength_ * 0.1f;
            }
        }
    }
    
    // Apply winner amplification
    for (const NeuronId& winner : pImpl->winners_) {
        if (winner.value < pImpl->neuronInhibition_.size()) {
            pImpl->neuronExcitation_[winner.value] = pImpl->winnerAmplification_ * 2.0f;
            pImpl->neuronInhibition_[winner.value] = std::max(0.0f, pImpl->neuronInhibition_[winner.value] * 0.5f);
        }
    }
}

void AttentionalSelection::updateRegionAttention() {
    if (!pImpl->brain) return;
    
    // Reset region attention if not specifically focused
    if (pImpl->attendedRegions_.empty()) {
        // Equal attention to all regions
        float baseAttention = 1.0f / static_cast<float>(pImpl->brain->getRegionCount());
        for (size_t i = 0; i < pImpl->brain->getRegionCount(); ++i) {
            RegionId regionId(i + 1);
            // Store region attention in salience
            std::vector<float> regionSalience(pImpl->brain->getTotalNeuronCount(), baseAttention * 0.1f);
            // Simplified - in reality would map region to neurons
        }
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    if (neuron.value >= pImpl->neuronExcitation_.size()) return false;
    return pImpl->neuronExcitation_[neuron.value] > 0.5f;
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    pImpl->topDownBias_[neuron.value] = biasStrength;
    pImpl->neuronSalience_[neuron.value] = std::max(pImpl->neuronSalience_[neuron.value], biasStrength * 0.5f);
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    pImpl->bottomUpSalience_[neuron.value] = salienceStrength;
    pImpl->neuronSalience_[neuron.value] = std::max(pImpl->neuronSalience_[neuron.value], salienceStrength);
}

void AttentionalSelection::reset() {
    pImpl->attendedRegions_.clear();
    pImpl->winners_.clear();
    pImpl->topDownBias_.clear();
    pImpl->bottomUpSalience_.clear();
    pImpl->neuronInhibition_.assign(pImpl->neuronInhibition_.size(), pImpl->globalInhibitionLevel_);
    pImpl->neuronExcitation_.assign(pImpl->neuronExcitation_.size(), 0.0f);
    pImpl->neuronSalience_.assign(pImpl->neuronSalience_.size(), 0.0f);
}

} // namespace nlm