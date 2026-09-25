#include "SensoryPercept.hpp"
#include "../core/Constants.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(SensoryConstants::VISION_WIDTH)
    , visionHeight_(SensoryConstants::VISION_HEIGHT)
    , timestamp_(0.0)
{
    vision_.resize(SensoryConstants::VISION_WIDTH * SensoryConstants::VISION_HEIGHT, 0.0f);
    touch_.resize(SensoryConstants::TOUCH_SENSORS, 0.0f);
    internal_.resize(SensoryConstants::INTERNAL_SIGNALS, 0.0f);
    proprioception_.resize(SensoryConstants::PROPRIOCEPTION_SENSORS, 0.0f);
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
