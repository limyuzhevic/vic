#include "AttentionalSelection.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct AttentionalSelection::Impl {
    // Neuron salience and bias structure
    struct NeuronState {
        float bottomUpSalience;
        float topDownBias;
        float globalGain;
        float localInhibition;
        SimulationStep lastUpdated;
        
        NeuronState() : bottomUpSalience(0.0f), topDownBias(0.0f), 
                       globalGain(1.0f), localInhibition(0.5f), lastUpdated(0) {}
    };
    
    std::unordered_map<uint64_t, NeuronState> neuronStates_;
    std::mt19937 rng_;
    
    Impl() : rng_(std::random_device{}()) {}
};

AttentionalSelection::AttentionalSelection() : brain_(nullptr), inhibitionStrength_(0.5f), 
                                            excitationStrength_(1.0f), competitionThreshold_(0.7f) {
    pImpl = std::make_unique<Impl>();
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    brain_ = brain;
    pImpl->rng_.seed(std::random_device{}());
    
    // Initialize neuron states if needed
    if (brain_) {
        auto neuronIds = brain_->getAllNeuronIds();
        for (NeuronId id : neuronIds) {
            pImpl->neuronStates_[static_cast<uint64_t>(id)].lastUpdated = 0;
        }
    }
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    if (!brain_ || competitors.empty()) return {};
    
    // Calculate net attention scores for each competitor
    std::vector<std::pair<NeuronId, float>> scores;
    
    for (NeuronId neuronId : competitors) {
        auto it = pImpl->neuronStates_.find(static_cast<uint64_t>(neuronId));
        if (it == pImpl->neuronStates_.end()) {
            // Initialize new neuron state
            pImpl->neuronStates_[static_cast<uint64_t>(neuronId)] = Impl::NeuronState();
            it = pImpl->neuronStates_.find(static_cast<uint64_t>(neuronId));
        }
        
        auto& state = it->second;
        
        // Get neuron activation
        float activation = 0.0f;
        auto* neuron = brain_->getNeuron(neuronId);
        if (neuron) {
            activation = neuron->getMembranePotential();
        }
        
        // Calculate attention score
        float netScore = activation * excitationStrength_;
        netScore -= state.localInhibition * inhibitionStrength_;
        netScore += state.topDownBias * 2.0f;
        netScore += state.bottomUpSalience * 1.5f;
        
        // Apply global inhibition
        netScore *= (1.0f - globalInhibition);
        
        // Apply threshold
        if (netScore < competitionThreshold_) {
            netScore = 0.0f;
        }
        
        scores.emplace_back(neuronId, netScore);
    }
    
    // Sort by score (descending)
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Select winners (neurons with positive scores)
    std::vector<NeuronId> winners;
    for (const auto& score : scores) {
        if (score.second > 0.0f) {
            winners.push_back(score.first);
        } else {
            break;  // No more positive scores after sorting
        }
    }
    
    // Update winners vector
    winners_ = winners;
    
    // Update neuron states
    updateNeuronStates(scores);
    
    return winners;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (!brain_) return;
    
    // Get all neurons in the specified region
    auto* regionPtr = brain_->getRegion(region);
    if (!regionPtr) return;
    
    auto regionNeurons = regionPtr->getAllNeurons();
    
    // Apply strong top-down bias to region neurons
    for (auto* neuron : regionNeurons) {
        if (neuron) {
            uint64_t neuronId = static_cast<uint64_t>(neuron->getId());
            auto& state = pImpl->neuronStates_[neuronId];
            state.topDownBias = 1.0f;  // Strong top-down attention
            state.lastUpdated = 0;
        }
    }
    
    // Remove top-down bias from other neurons
    for (auto& kv : pImpl->neuronStates_) {
        if (kv.first < 1000000) {  // Approximate region check (simplified)
            if (kv.second.topDownBias > 0.0f) {
                // Check if this neuron is in our focused region
                bool inFocusedRegion = false;
                for (auto* n : regionPtr->getAllNeurons()) {
                    if (static_cast<uint64_t>(n->getId()) == kv.first) {
                        inFocusedRegion = true;
                        break;
                    }
                }
                
                if (!inFocusedRegion) {
                    kv.second.topDownBias = 0.0f;
                }
            }
        }
    }
    
    attendedRegions_.clear();
    attendedRegions_.push_back(region);
}

void AttentionalSelection::releaseAttention() {
    // Clear all biases
    for (auto& kv : pImpl->neuronStates_) {
        kv.second.topDownBias = 0.0f;
        kv.second.bottomUpSalience = 0.0f;
    }
    
    attendedRegions_.clear();
    winners_.clear();
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
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
    auto it = pImpl->neuronStates_.find(static_cast<uint64_t>(neuron));
    if (it != pImpl->neuronStates_.end()) {
        return it->second.localInhibition;
    }
    return inhibitionStrength_;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    auto it = pImpl->neuronStates_.find(static_cast<uint64_t>(neuron));
    if (it != pImpl->neuronStates_.end()) {
        float net = it->second.bottomUpSalience + it->second.topDownBias;
        return std::clamp(net, 0.0f, 2.0f);
    }
    return excitationStrength_;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay top-down biases over time
    for (auto& kv : pImpl->neuronStates_) {
        auto& state = kv.second;
        
        state.topDownBias *= (1.0f - dt * 0.01f);  // Decay with time constant
        state.bottomUpSalience *= (1.0f - dt * 0.02f);
        
        // Update local inhibition based on global dynamics
        state.localInhibition = inhibitionStrength_ * (1.0f + 0.1f * sin(dt * 10.0f));
        
        state.lastUpdated++;
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    auto it = pImpl->neuronStates_.find(static_cast<uint64_t>(neuron));
    if (it != pImpl->neuronStates_.end()) {
        return it->second.topDownBias > 0.1f || it->second.bottomUpSalience > 0.1f;
    }
    return false;
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    auto& state = pImpl->neuronStates_[static_cast<uint64_t>(neuron)];
    state.topDownBias = std::clamp(biasStrength, 0.0f, 1.0f);
    state.lastUpdated = 0;
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    auto& state = pImpl->neuronStates_[static_cast<uint64_t>(neuron)];
    state.bottomUpSalience = std::clamp(salienceStrength, 0.0f, 1.0f);
    state.lastUpdated = 0;
}

void AttentionalSelection::reset() {
    pImpl->neuronStates_.clear();
    attendedRegions_.clear();
    winners_.clear();
}

void AttentionalSelection::updateNeuronStates(const std::vector<std::pair<NeuronId, float>>& scores) {
    for (const auto& score : scores) {
        auto it = pImpl->neuronStates_.find(static_cast<uint64_t>(score.first));
        if (it != pImpl->neuronStates_.end()) {
            auto& state = it->second;
            
            // Update based on current attention score
            if (score.second > 0.5f) {
                // High attention increases gain
                state.globalGain += (score.second - 0.5f) * 0.1f;
                state.globalGain = std::clamp(state.globalGain, 0.5f, 2.0f);
            } else if (score.second < -0.5f) {
                // Low attention decreases gain
                state.globalGain += (score.second + 0.5f) * 0.1f;
                state.globalGain = std::clamp(state.globalGain, 0.1f, 1.0f);
            }
        }
    }
}

} // namespace nlm
