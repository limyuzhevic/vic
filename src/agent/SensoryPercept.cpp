#include "SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Initialize all sensory channels with default sizes
    vision_.resize(16 * 16, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    // Audio channel is empty by default - can be enabled with setAudioChannelSize()
}

void SensoryPercept::resizeAudioChannel(size_t numSamples) {
    audio_.resize(numSamples, 0.0f);
}

void SensoryPercept::clear() {
    // Reset all sensory channels to zero
    std::fill(vision_.begin(), vision_.end(), 0.0f);
    std::fill(touch_.begin(), touch_.end(), 0.0f);
    std::fill(internal_.begin(), internal_.end(), 0.0f);
    std::fill(proprioception_.begin(), proprioception_.end(), 0.0f);
    std::fill(audio_.begin(), audio_.end(), 0.0f);
}

bool SensoryPercept::validateChannelSizes() const {
    bool valid = true;
    
    // Validate vision: should be visionWidth_ * visionHeight_
    size_t expectedVisionSize = visionWidth_ * visionHeight_;
    if (vision_.size() != expectedVisionSize) {
        NLM_LOG_ERROR("Vision channel size mismatch: expected " + 
                     std::to_string(expectedVisionSize) + 
                     ", got " + std::to_string(vision_.size()));
        valid = false;
    }
    
    // Validate other channels
    if (touch_.size() != 8) {
        NLM_LOG_ERROR("Touch channel size mismatch: expected 8, got " + 
                     std::to_string(touch_.size()));
        valid = false;
    }
    
    if (internal_.size() != 4) {
        NLM_LOG_ERROR("Internal channel size mismatch: expected 4, got " + 
                     std::to_string(internal_.size()));
        valid = false;
    }
    
    if (proprioception_.size() != 6) {
        NLM_LOG_ERROR("Proprioception channel size mismatch: expected 6, got " + 
                     std::to_string(proprioception_.size()));
        valid = false;
    }
    
    return valid;
}

float SensoryPercept::getMaxSignal() const {
    float maxVal = 0.0f;
    
    // Check all channels
    for (float val : vision_) maxVal = std::max(maxVal, std::abs(val));
    for (float val : touch_) maxVal = std::max(maxVal, std::abs(val));
    for (float val : internal_) maxVal = std::max(maxVal, std::abs(val));
    for (float val : proprioception_) maxVal = std::max(maxVal, std::abs(val));
    for (float val : audio_) maxVal = std::max(maxVal, std::abs(val));
    
    return maxVal;
}

float SensoryPercept::getTotalSignalPower() const {
    float power = 0.0f;
    
    // Sum squared values across all channels
    for (float val : vision_) power += val * val;
    for (float val : touch_) power += val * val;
    for (float val : internal_) power += val * val;
    for (float val : proprioception_) power += val * val;
    for (float val : audio_) power += val * val;
    
    return power;
}

std::vector<float> SensoryPercept::getAllSignals() const {
    // Pre-allocate result vector for efficiency
    size_t totalSize = vision_.size() + touch_.size() + internal_.size() + 
                      proprioception_.size() + audio_.size();
    std::vector<float> all;
    all.reserve(totalSize);
    
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
