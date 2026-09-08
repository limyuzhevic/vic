#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    vision_.resize(16 * 16, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    // Audio vector remains empty (not implemented in this version)
}

std::vector<float> SensoryPercept::getAllSignals() const {
    // Pre-allocate for efficiency
    std::vector<float> all;
    all.reserve(vision_.size() + touch_.size() + internal_.size() + proprioception_.size() + audio_.size());
    
    // Vision (flattened)
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio (if implemented in future)
    // all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

} // namespace nlm
