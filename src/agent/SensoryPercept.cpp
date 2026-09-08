#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Initialize all sensory signal vectors with default values
    vision_.resize(16 * 16, 0.0f);        // 16x16 grayscale vision grid
    touch_.resize(8, 0.0f);              // 8 touch/proximity sensors
    internal_.resize(4, 0.0f);           // 4 internal homeostatic signals
    proprioception_.resize(6, 0.0f);    // 6 proprioceptive (body position) signals
    // Audio intentionally left empty (0 elements) - can be enabled if needed
    audio_.resize(0, 0.0f);
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
