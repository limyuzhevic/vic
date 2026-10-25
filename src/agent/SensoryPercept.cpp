#include "SensoryPercept.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>

namespace nlm {

SensoryPercept::SensoryPercept(std::shared_ptr<Config> config)
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
    , validateInput_(true)
    , filterInput_(false)
    , touchSensorCount_(8)
    , internalSensorCount_(4)
    , proprioceptionSensorCount_(6)
    , audioSensorCount_(0)
{
    // Load configuration values if available
    if (config) {
        visionWidth_ = config->getOr<size_t>("vision_width", 16);
        visionHeight_ = config->getOr<size_t>("vision_height", 16);
        touchSensorCount_ = config->getOr<size_t>("touch_sensor_count", 8);
        internalSensorCount_ = config->getOr<size_t>("internal_sensor_count", 4);
        proprioceptionSensorCount_ = config->getOr<size_t>("proprioception_sensor_count", 6);
        audioSensorCount_ = config->getOr<size_t>("audio_sensor_count", 0);
        
        validateInput_ = config->getOr<bool>("enable_sensory_validation", true);
        filterInput_ = config->getOr<bool>("enable_sensory_filtering", false);
    }
    
    // Initialize vectors based on configured sizes
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(touchSensorCount_, 0.0f);
    internal_.resize(internalSensorCount_, 0.0f);
    proprioception_.resize(proprioceptionSensorCount_, 0.0f);
    audio_.resize(audioSensorCount_, 0.0f);
}

void SensoryPercept::configureSensoryDimensions(
    size_t visionWidth,
    size_t visionHeight,
    size_t touchCount,
    size_t internalCount,
    size_t proprioceptionCount,
    size_t audioCount) {
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
    touchSensorCount_ = touchCount;
    internalSensorCount_ = internalCount;
    proprioceptionSensorCount_ = proprioceptionCount;
    audioSensorCount_ = audioCount;
    
    // Resize vectors to new dimensions
    vision_.resize(visionWidth * visionHeight, 0.0f);
    touch_.resize(touchCount, 0.0f);
    internal_.resize(internalCount, 0.0f);
    proprioception_.resize(proprioceptionCount, 0.0f);
    audio_.resize(audioCount, 0.0f);
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

bool SensoryPercept::validateInputSize(const std::vector<float>& input, size_t expectedSize) const {
    if (!validateInput_) {
        return true;
    }
    
    return input.size() == expectedSize;
}

void SensoryPercept::filterInput(std::vector<float>& input) const {
    if (!filterInput_ || input.empty()) {
        return;
    }
    
    // Simple filter: remove values outside [-1.0, 1.0] range
    for (float& value : input) {
        if (value < -1.0f) value = -1.0f;
        else if (value > 1.0f) value = 1.0f;
    }
}

} // namespace nlm
