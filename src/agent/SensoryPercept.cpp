#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(AgentBrainConfig::VISION_WIDTH)
    , visionHeight_(AgentBrainConfig::VISION_HEIGHT)
    , timestamp_(0.0)
{
    vision_.resize(AgentBrainConfig::VISION_TOTAL, 0.0f);
    touch_.resize(AgentBrainConfig::TOUCH_TOTAL, 0.0f);
    internal_.resize(AgentBrainConfig::INTERNAL_TOTAL, 0.0f);
    proprioception_.resize(AgentBrainConfig::PROPRIOCEPTION_TOTAL, 0.0f);
    audio_.resize(0, 0.0f);
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Reserve capacity for all signals upfront to avoid reallocations
    all.reserve(vision_.size() + touch_.size() + internal_.size() + 
                proprioception_.size() + audio_.size());
    
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
