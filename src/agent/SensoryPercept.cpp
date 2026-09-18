#include "SensoryPercept.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

SensoryPercept::SensoryPercept(std::shared_ptr<Config> config)
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Get configuration values if available
    if (config) {
        visionWidth_ = config->getOr<size_t>("sensory_vision_width", 16);
        visionHeight_ = config->getOr<size_t>("sensory_vision_height", 16);
        
        // Get other sensory dimensions from config
        touch_.resize(config->getOr<size_t>("sensory_touch_count", 8), 0.0f);
        internal_.resize(config->getOr<size_t>("sensory_internal_count", 4), 0.0f);
        proprioception_.resize(config->getOr<size_t>("sensory_proprioception_count", 6), 0.0f);
        
        // Audio remains optional for now
        // audio_.resize(config->getOr<size_t>("sensory_audio_count", 0), 0.0f);
    }
    
    // Resize vectors based on config values
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
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
