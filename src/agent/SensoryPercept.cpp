#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Validate sensory modality dimensions
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
    
    // Validate that dimensions are positive
    if (visionWidth_ == 0 || visionHeight_ == 0) {
        // This would cause zero-sized vision array, which could cause issues
        visionWidth_ = 16;
        visionHeight_ = 16;
    }
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Validate that all sensory arrays are properly sized
    // Vision (flattened)
    if (!vision_.empty()) {
        all.insert(all.end(), vision_.begin(), vision_.end());
    }
    
    // Touch
    if (!touch_.empty()) {
        all.insert(all.end(), touch_.begin(), touch_.end());
    }
    
    // Internal
    if (!internal_.empty()) {
        all.insert(all.end(), internal_.begin(), internal_.end());
    }
    
    // Proprioception
    if (!proprioception_.empty()) {
        all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    }
    
    // Audio
    if (!audio_.empty()) {
        all.insert(all.end(), audio_.begin(), audio_.end());
    }
    
    return all;
}

} // namespace nlm
