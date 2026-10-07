#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Reserve memory for better performance
    vision_.reserve(16 * 16);
    vision_.resize(16 * 16, 0.0f);
    
    touch_.reserve(8);
    touch_.resize(8, 0.0f);
    
    internal_.reserve(4);
    internal_.resize(4, 0.0f);
    
    proprioception_.reserve(6);
    proprioception_.resize(6, 0.0f);
    
    // Audio vector - can be resized later based on configuration
    audio_.clear();
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Reserve space for all signals to avoid reallocations
    size_t totalSize = vision_.size() + touch_.size() + internal_.size() + proprioception_.size() + audio_.size();
    all.reserve(totalSize);
    
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
