#include "SensoryPercept.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    // Audio vector properly initialized with capacity but empty
    audio_.reserve(16);  // Reserve space for up to 16 audio samples
    audio_.clear();  // Ensure empty

}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    all.reserve(getTotalSignalCount());
    
    // Vision (flattened)
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

// Get total number of signal components for validation and optimization
size_t SensoryPercept::getTotalSignalCount() const {
    return vision_.size() + touch_.size() + internal_.size() + proprioception_.size() + audio_.size();
}

// Check if all signal vectors have expected sizes
bool SensoryPercept::validateSignalSizes() const {
    bool valid = true;
    
    if (vision_.size() != visionWidth_ * visionHeight_) {
        return false;
    }
    if (touch_.size() != 8) {
        return false;
    }
    if (internal_.size() != 4) {
        return false;
    }
    if (proprioception_.size() != 6) {
        return false;
    }
    
    return valid;
}

} // namespace nlm
