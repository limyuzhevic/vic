#include "../cognition/AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct AttentionalSelection::Impl {
    std::unordered_map<uint64_t, float> bottomUpSalience_;
    std::unordered_map<uint64_t, float> topDownBias_;
    
    Impl() {}
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
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    
    // Initialize attention maps
    pImpl->bottomUpSalience_.clear();
    pImpl->topDownBias_.clear();
    
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                               float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Compute activity levels for competitors
    std::vector<float> activities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias (from maps)
        auto salIt = pImpl->bottomUpSalience_.find(neuron.value);
        float salience = (salIt != pImpl->bottomUpSalience_.end()) ? salIt->second : 0.0f;
        auto biasIt = pImpl->topDownBias_.find(neuron.value);
        float bias = (biasIt != pImpl->topDownBias_.end()) ? biasIt->second : 0.0f;
        
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
    inhibitionStrength_ = std::clamp(strength, 0.0f, 1.0f);
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = std::clamp(strength, 0.0f, 5.0f);
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    return inhibitionStrength_;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    return excitationStrength_;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay attention biases over time
    for (auto& pair : pImpl->bottomUpSalience_) {
        pair.second *= (1.0f - dt * 0.01f);
    }
    
    for (auto& pair : pImpl->topDownBias_) {
        pair.second *= (1.0f - dt * 0.01f);
    }
    
    // Remove very weak biases
    pImpl->bottomUpSalience_.erase(
        std::remove_if(pImpl->bottomUpSalience_.begin(), pImpl->bottomUpSalience_.end(),
                       [](const auto& pair) { return pair.second < 0.001f; }),
        pImpl->bottomUpSalience_.end());
    
    pImpl->topDownBias_.erase(
        std::remove_if(pImpl->topDownBias_.begin(), pImpl->topDownBias_.end(),
                       [](const auto& pair) { return pair.second < 0.001f; }),
        pImpl->topDownBias_.end());
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return pImpl->bottomUpSalience_.find(neuron.value) != pImpl->bottomUpSalience_.end() ||
           pImpl->topDownBias_.find(neuron.value) != pImpl->topDownBias_.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    if (biasStrength > 0.0f) {
        pImpl->topDownBias_[neuron.value] = biasStrength;
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    if (salienceStrength > 0.0f) {
        pImpl->bottomUpSalience_[neuron.value] = salienceStrength;
    }
}

void AttentionalSelection::reset() {
    pImpl->bottomUpSalience_.clear();
    pImpl->topDownBias_.clear();
}

} // namespace nlm
