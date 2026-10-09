#include "core/Types/Constants.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(NeuralConstants::VISION_WIDTH)
    , visionHeight_(NeuralConstants::VISION_HEIGHT)
    , timestamp_(0.0)
{
    vision_.resize(NeuralConstants::VISION_WIDTH * NeuralConstants::VISION_HEIGHT, 0.0f);
    touch_.resize(NeuralConstants::TOUCH_SENSORS, 0.0f);
    internal_.resize(NeuralConstants::INTERNAL_SIGNALS, 0.0f);
    proprioception_.resize(NeuralConstants::PROPRIOCEPTION_SENSORS, 0.0f);
    audio_.resize(NeuralConstants::AUDIO_CHANNELS, 0.0f);
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
