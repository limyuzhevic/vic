#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f)
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
    if (!brain_ || competitors.empty()) return std::vector<NeuronId>();
    
    // Compute attention scores for each competitor
    std::vector<std::pair<NeuronId, float>> scoredNeurons;
    
    for (NeuronId neuronId : competitors) {
        float score = 0.0f;
        
        // Get neuron from brain
        for (auto& region : brain_->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getId() == neuronId) {
                    // Apply attention mechanisms
                    float activity = computeNeuronActivity(neuron);
                    
                    // Top-down bias (goal-directed)
                    float topDownBias = getTopDownBias(neuronId);
                    float attentionFromTopDown = excitationStrength_ * topDownBias;
                    
                    // Bottom-up salience (stimulus-driven)
                    float bottomUpSalience = getBottomUpSalience(neuronId);
                    float attentionFromBottomUp = excitationStrength_ * bottomUpSalience;
                    
                    // Global inhibition from other competitors
                    float globalInhibitionFromOthers = globalInhibition * (competitors.size() - 1);
                    
                    // Competition between neurons
                    float intraRegionCompetition = 0.0f;
                    for (const auto& otherId : competitors) {
                        if (otherId != neuronId) {
                            for (auto& otherRegion : brain_->getRegions()) {
                                for (auto* otherNeuron : otherRegion->getAllNeurons()) {
                                    if (otherNeuron->getId() == otherId) {
                                        float otherActivity = computeNeuronActivity(otherNeuron);
                                        float inhibition = inhibitionStrength_ * otherActivity;
                                        score -= inhibition;
                                    }
                                }
                            }
                        }
                    }
                    
                    // Final score
                    score = activity + attentionFromTopDown + attentionFromBottomUp - 
                            globalInhibitionFromOthers - intraRegionCompetition;
                    
                    scoredNeurons.emplace_back(neuronId, score);
                    break;
                }
            }
        }
    }
    
    // Sort by score descending
    std::sort(scoredNeurons.begin(), scoredNeurons.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Select winners based on threshold
    std::vector<NeuronId> winners;
    float threshold = competitionThreshold_ * static_cast<float>(scoredNeurons.size());
    
    for (size_t i = 0; i < scoredNeurons.size() && i < threshold; ++i) {
        winners.push_back(scoredNeurons[i].first);
    }
    
    // Update internal state
    winners_ = winners;
    
    NLM_LOG_DEBUG("Attentional selection: selected " + std::to_string(winners.size()) + "/" + 
                 std::to_string(competitors.size()) + " winners from competition");
    
    return winners;
}

void AttentionalSelection::focusOnRegion(RegionId regionId) {
    attendedRegions_.clear();
    attendedRegions_.push_back(regionId);
    
    NLM_LOG_INFO("Attention focused on region: " + std::to_string(regionId.index()));
}

void AttentionalSelection::releaseAttention() {
    // Clear all attended regions - allow all regions equal processing
    attendedRegions_.clear();
    NLM_LOG_INFO("Attention released - all regions attended equally");
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = std::clamp(strength, 0.0f, 2.0f);
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = std::clamp(strength, 0.5f, 3.0f);
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    // Get inhibition from attended regions that this neuron belongs to
    float totalInhibition = 0.0f;
    
    if (!brain_) return totalInhibition;
    
    for (const auto& regionId : attendedRegions_) {
        auto* region = brain_->getRegion(regionId);
        if (region) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getId() == neuron) {
                    // Apply inhibition based on neuron type and activity
                    float activity = neuron->getState().membranePotential;
                    float baseInhibition = inhibitionStrength_ * activity;
                    
                    // Additional inhibition for weak neurons
                    float threshold = neuron->getThreshold();
                    float activityRatio = activity / threshold;
                    
                    totalInhibition += baseInhibition * activityRatio;
                }
            }
        }
    }
    
    return totalInhibition;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    // Get excitation from attended regions
    float totalExcitation = 0.0f;
    
    if (!brain_) return totalExcitation;
    
    // Top-down excitation from goals
    for (const auto& biasEntry : topDownBias_) {
        if (biasEntry.first == neuron.value) {
            totalExcitation += excitationStrength_ * biasEntry.second;
        }
    }
    
    // Bottom-up excitation from sensory input
    for (const auto& salienceEntry : bottomUpSalience_) {
        if (salienceEntry.first == neuron.value) {
            totalExcitation += excitationStrength_ * salienceEntry.second;
        }
    }
    
    return totalExcitation;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Update attention dynamics over time
    // Decay salience values over time
    for (auto& biasEntry : topDownBias_) {
        biasEntry.second = std::max(0.0f, biasEntry.second - 0.01f * static_cast<float>(dt));
    }
    
    for (auto& salienceEntry : bottomUpSalience_) {
        salienceEntry.second = std::max(0.0f, salienceEntry.second - 0.005f * static_cast<float>(dt));
    }
    
    // Update attention winners
    // In a real implementation, this would modify neural excitability
    NLM_LOG_DEBUG("Attention dynamics updated");
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    // Check if neuron is in attended regions
    if (attendedRegions_.empty()) return true;  // If no specific focus, all neurons are attended
    
    if (!brain_) return false;
    
    for (const auto& regionId : attendedRegions_) {
        auto* region = brain_->getRegion(regionId);
        if (region) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getId() == neuron) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    topDownBias_[neuron.value] = std::max(0.0f, std::min(1.0f, biasStrength));
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    bottomUpSalience_[neuron.value] = std::max(0.0f, std::min(1.0f, salienceStrength));
}

void AttentionalSelection::reset() {
    topDownBias_.clear();
    bottomUpSalience_.clear();
    attendedRegions_.clear();
    winners_.clear();
    
    NLM_LOG_INFO("AttentionalSelection reset");
}

float AttentionalSelection::computeNeuronActivity(Neuron* neuron) const {
    if (!neuron) return 0.0f;
    
    // Simple activity measure based on membrane potential
    const auto& state = neuron->getState();
    
    // Activity is higher when potential is above resting potential
    float activity = std::max(0.0f, state.membranePotential - state.restingPotential);
    
    // Scale to reasonable range
    return std::min(1.0f, activity / 50.0f);
}

float AttentionalSelection::getTopDownBias(NeuronId neuronId) const {
    auto it = topDownBias_.find(neuronId.value);
    if (it != topDownBias_.end()) {
        return it->second;
    }
    return 0.0f;
}

float AttentionalSelection::getBottomUpSalience(NeuronId neuronId) const {
    auto it = bottomUpSalience_.find(neuronId.value);
    if (it != bottomUpSalience_.end()) {
        return it->second;
    }
    return 0.0f;
}

} // namespace nlm