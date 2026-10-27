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
    audio_.resize(0, 0.0f);  // Will be populated with real audio input
    
    // Initialize with default silence pattern
    std::fill(audio_.begin(), audio_.end(), 0.0f);
    
    // Initialize with zero vision pattern
    std::fill(vision_.begin(), vision_.end(), 0.0f);
    
    // Initialize with neutral internal signals
    std::fill(internal_.begin(), internal_.end(), 0.0f);
    
    // Initialize with neutral proprioception
    std::fill(proprioception_.begin(), proprioception_.end(), 0.5f);  // Mid-range default
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Vision (flattened)
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio (with dynamic range for different sound levels)
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

} // namespace nlm
