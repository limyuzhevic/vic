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
    , competitionThreshold_(0.6f)
{
    neuronSalience_.resize(1000, 0.0f);  // Initialize with reasonable capacity
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                                float globalInhibition) {
    winners_.clear();
    float inhibition = globalInhibition * inhibitionStrength_;
    float excitation = excitationStrength_;

    std::vector<std::pair<float, NeuronId>> scores;
    scores.reserve(competitors.size());

    for (NeuronId neuron : competitors) {
        // Get base salience from neuron activity
        float salience = 1.0f;
        if (brain_) {
            auto neurons = brain_->getRegions();
            for (const auto& region : neurons) {
                if (region) {
                    auto popNeurons = region->getAllNeurons();
                    for (auto* n : popNeurons) {
                        if (n && n->getId() == neuron) {
                            salience = std::abs(n->getMembranePotential() - n->getRestingPotential()) / 20.0f;
                            break;
                        }
                    }
                }
            }
        }

        // Apply top-down bias if set
        auto biasIt = topDownBias_.find(neuron);
        if (biasIt != topDownBias_.end()) {
            salience += biasIt->second;
        }

        // Apply bottom-up salience if set
        auto salienceIt = bottomUpSalience_.find(neuron);
        if (salienceIt != bottomUpSalience_.end()) {
            salience += salienceIt->second * excitation;
        }

        // Apply competition
        float adjustedSalience = salience * (1.0f - inhibition);
        scores.push_back({adjustedSalience, neuron});

        // Store salience for later retrieval
        size_t idx = neuron % neuronSalience_.size();
        neuronSalience_[idx] = salience;
    }

    // Select winners based on threshold
    float minThreshold = competitionThreshold_;
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    for (const auto& score : scores) {
        if (score.first >= minThreshold) {
            winners_.push_back(score.second);
        }
    }

    NLM_LOG_INFO("Attention selection: " + std::to_string(winners_.size()) + " winners out of " +
                 std::to_string(competitors.size()) + " competitors");

    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    attendedRegions_.clear();
    attendedRegions_.push_back(region);
    NLM_LOG_INFO("Attention focused on region " + std::to_string(region));
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
    NLM_LOG_INFO("Attention released");
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = std::max(0.0f, std::min(1.0f, strength));
    NLM_LOG_INFO("Attention inhibition strength set to " + std::to_string(inhibitionStrength_));
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = std::max(1.0f, std::min(3.0f, strength));
    NLM_LOG_INFO("Attention excitation strength set to " + std::to_string(excitationStrength_));
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = std::max(0.1f, std::min(1.0f, threshold));
    NLM_LOG_INFO("Attention competition threshold set to " + std::to_string(competitionThreshold_));
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    size_t idx = neuron % neuronSalience_.size();
    return neuronSalience_[idx] * inhibitionStrength_;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    size_t idx = neuron % neuronSalience_.size();
    return neuronSalience_[idx] * excitationStrength_;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay salience over time
    for (auto& salience : neuronSalience_) {
        salience *= 0.99f;  // Natural decay
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    for (NeuronId winner : winners_) {
        if (winner == neuron) {
            return true;
        }
    }
    return false;
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    topDownBias_[neuron] = biasStrength;
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    bottomUpSalience_[neuron] = salienceStrength;
}

void AttentionalSelection::reset() {
    topDownBias_.clear();
    bottomUpSalience_.clear();
    winners_.clear();
    attendedRegions_.clear();
    NLM_LOG_INFO("AttentionalSelection reset");
}

} // namespace nlm
