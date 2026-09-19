#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Vision: 16x16 grayscale intensity grid (no RGB, no object labels)
    vision_.resize(16 * 16, 0.0f);
    
    // Touch: 8 proximity/collision sensors (binary collision signals)
    touch_.resize(8, 0.0f);
    
    // Internal: 4 homeostatic/homeostatic signals (energy, temperature, etc.)
    internal_.resize(4, 0.0f);
    
    // Proprioception: 6 body position/velocity signals (joint angles, speed)
    proprioception_.resize(6, 0.0f);
    
    // Audio: 8 audio samples (simple amplitude data, placeholder for future enhancement)
    // In a full implementation, this would process real audio input
    audio_.resize(8, 0.0f);
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
    
    // Audio
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

} // namespace nlm
