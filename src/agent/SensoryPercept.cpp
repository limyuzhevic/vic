#include "SensoryPercept.hpp"

namespace nlm {

// Sensory dimension constants - improve maintainability and reduce magic numbers
namespace {
    // Vision: 16x16 grid (256 total pixels) representing simple grayscale input
    constexpr size_t VISION_WIDTH = 16;
    constexpr size_t VISION_HEIGHT = 16;
    constexpr size_t VISION_SIZE = VISION_WIDTH * VISION_HEIGHT; // 256
    
    // Touch: 8 proximity sensors for collision detection
    constexpr size_t TOUCH_SIZE = 8;
    
    // Internal: 4 homeostatic signals for physiological regulation
    constexpr size_t INTERNAL_SIZE = 4;
    
    // Proprioception: 6 body state signals (position/velocity)
    constexpr size_t PROPRIOCEPTION_SIZE = 6;
    
    // Audio: initially disabled (0 size), configurable if needed
    constexpr size_t AUDIO_SIZE = 0;
}

SensoryPercept::SensoryPercept()
    : visionWidth_(VISION_WIDTH)
    , visionHeight_(VISION_HEIGHT)
    , timestamp_(0.0)
{
    // Initialize sensory vectors using constructor initializer list for efficiency
    // This avoids multiple resize() calls and sets all values to 0.0f in one go
    vision_.assign(VISION_SIZE, 0.0f);
    touch_.assign(TOUCH_SIZE, 0.0f);
    internal_.assign(INTERNAL_SIZE, 0.0f);
    proprioception_.assign(PROPRIOCEPTION_SIZE, 0.0f);
    audio_.assign(AUDIO_SIZE, 0.0f);
}

std::vector<float> SensoryPercept::getAllSignals() const {
    // Get all sensory signals concatenated for brain input processing
    // This creates a flat vector containing all modalities in order
    std::vector<float> all;
    
    // Reserve capacity for performance (avoid reallocations)
    size_t totalSize = vision_.size() + touch_.size() + internal_.size() +
                      proprioception_.size() + audio_.size();
    all.reserve(totalSize);
    
    // Vision signals (flattened in row-major order)
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch sensors (binary collision signals)
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal homeostatic signals
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception signals (body state)
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio-like signals (if enabled)
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

} // namespace nlm
