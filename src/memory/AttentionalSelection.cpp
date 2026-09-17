# Critical Fixes Summary

## 1. IMPLEMENT MISSING AttentionalSelection.cpp
**File**: `src/memory/AttentionalSelection.cpp`

This is critical because:
- Brain.hpp declares `std::unique_ptr<AttentionalSelection> attention` in Impl
- Brain.cpp initializes `attention = std::make_unique<AttentionalSelection>()`
- Brain.cpp calls `pImpl->attention->initialize(this)` and uses it
- But the implementation was never created - it's just a header

The brain cannot be fully functional without this cognitive system.

```cpp
// AttentionalSelection Implementation
// This implements neural attention through competitive selection

#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AttentionalSelection::Impl {
    // Neuron competition state
    std::vector<float> competitionStrength;
    std::vector<float> inhibitionLevel;
    
    Impl() {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f) {
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(
    const std::vector<NeuronId>& competitors, float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Compute activity levels for competitors
    std::vector<float> activities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias (from maps)
        auto salIt = bottomUpSalience_.find(neuron.value);
        float salience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        auto biasIt = topDownBias_.find(neuron.value);
        float bias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        activities[i] = salience + bias;
        totalActivity += activities[i];
    }
    
    if (totalActivity < 0.001f) {
        // No strong competitors - all equal
        return competitors;
    }
    
    // Competition: neurons inhibit each other based on relative activity
    for (size_t i = 0; i < competitors.size(); ++i) {
        for (size_t j = 0; j < competitors.size(); ++j) {
            if (i == j) continue;
            
            float relativeActivity = activities[i] / (activities[j] + 0.001f);
            
            if (relativeActivity > 1.5f) {
                // i is much stronger than j - apply inhibition to j
                if (brain_) {
                    brain_->injectCurrent(competitors[j], -globalInhibition * inhibitionStrength_);
                }
                pImpl->inhibitionLevel.push_back(globalInhibition * inhibitionStrength_);
            }
        }
    }
    
    // Winners are neurons with above-threshold activity
    float threshold = competitionThreshold_ * totalActivity / competitors.size();
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activities[i] >= threshold) {
            winners_.push_back(competitors[i]);
            
            // Apply excitation to winners
            if (brain_) {
                brain_->injectCurrent(competitors[i], excitationStrength_ * 2.0f);
            }
        }
    }
    
    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
        attendedRegions_.push_back(region);
    }
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = strength;
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = strength;
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = threshold;
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    for (const auto& level : pImpl->inhibitionLevel) {
        if (level > 0.0f) return level;
    }
    return 0.0f;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    return excitationStrength_;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay inhibition levels
    for (auto& level : pImpl->inhibitionLevel) {
        level *= (1.0f - dt * 0.1f);
    }
    pImpl->inhibitionLevel.erase(
        std::remove(pImpl->inhibitionLevel.begin(), pImpl->inhibitionLevel.end(), 0.0f),
        pImpl->inhibitionLevel.end()
    );
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    // Check if neuron belongs to an attended region
    for (RegionId region : attendedRegions_) {
        if (brain_) {
            if (auto* brainRegion = brain_->getRegion(region)) {
                auto neurons = brainRegion->getAllNeurons();
                for (auto* n : *neurons) {
                    if (n->getId() == neuron) return true;
                }
            }
        }
    }
    return false;
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    topDownBias_[neuron.value] = biasStrength;
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    bottomUpSalience_[neuron.value] = salienceStrength;
}

void AttentionalSelection::reset() {
    winners_.clear();
    attendedRegions_.clear();
    topDownBias_.clear();
    bottomUpSalience_.clear();
    pImpl->inhibitionLevel.clear();
}

} // namespace nlm
