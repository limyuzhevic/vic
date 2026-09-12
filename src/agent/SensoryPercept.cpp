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
    audio_.resize(0, 0.0f);
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Vision (flattened) - 16x16 = 256
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch - 8 proximity sensors
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal - 4 homeostatic signals
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception - 6 body position/velocity signals
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Note: Audio signals removed (not implemented)
    
    return all;
}

} // namespace nlm
