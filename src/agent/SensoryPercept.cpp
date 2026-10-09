#include "SensoryPercept.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

SensoryPercept::SensoryPercept(std::shared_ptr<Config> config)
    : config_(config)
    , timestamp_(0.0)
{
    // Load configuration values with defaults
    auto getOr = [&](const std::string& key, auto defaultValue) {
        if (config_ && config_->has(key)) {
            return config_->getOr<typename std::decay<decltype(defaultValue)>::type>(key, defaultValue);
        }
        return defaultValue;
    };
    
    // Get vision dimensions from config
    visionWidth_ = getOr("vision_width", 16);
    visionHeight_ = getOr("vision_height", 16);
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    
    // Get sensor counts from config
    touchCount_ = getOr("vision_touch_count", 8);
    internalCount_ = getOr("vision_internal_count", 4);
    proprioceptionCount_ = getOr("vision_proprioception_count", 6);
    audioCount_ = getOr("vision_audio_count", 0);
    
    touch_.resize(touchCount_, 0.0f);
    internal_.resize(internalCount_, 0.0f);
    proprioception_.resize(proprioceptionCount_, 0.0f);
    audio_.resize(audioCount_, 0.0f);
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
