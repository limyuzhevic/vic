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

void SensoryPercept::normalizeVision(float min, float max) {
    if (vision_.empty()) return;
    
    // Clamp values to [0, 1] first
    for (float& pixel : vision_) {
        pixel = std::max(0.0f, std::min(1.0f, pixel));
    }
    
    // Apply linear normalization
    float range = max - min;
    if (range > 0.0f) {
        for (float& pixel : vision_) {
            pixel = (pixel - min) / range;
        }
    } else {
        // If min == max, set all values to 0.5
        std::fill(vision_.begin(), vision_.end(), 0.5f);
    }
}

} // namespace nlm
