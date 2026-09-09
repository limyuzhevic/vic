#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Pre-allocate with known sizes instead of resize to avoid unnecessary reallocations
    vision_.assign(16 * 16, 0.0f);
    touch_.assign(8, 0.0f);
    internal_.assign(4, 0.0f);
    proprioception_.assign(6, 0.0f);
    // Don't initialize audio_ since it's empty
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Pre-allocate capacity for better performance
    size_t totalSize = vision_.size() + touch_.size() + internal_.size() + 
                      proprioception_.size() + audio_.size();
    all.reserve(totalSize);
    
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

} // namespace nlm
