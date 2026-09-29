#include "SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

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
    
    if (config_.enableDebugLogging) {
        NLM_LOG_INFO("SensoryPercept initialized with vision: " + 
                     std::to_string(config_.visionWidth) + "x" + 
                     std::to_string(config_.visionHeight) +
                     ", touch: " + std::to_string(config_.touchSensors) +
                     ", internal: " + std::to_string(config_.internalSignals) +
                     ", proprioception: " + std::to_string(config_.proprioceptionSensors));
    }
}

void SensoryPercept::validateVision(const std::vector<float>& vision) const {
    if (!config_.enableInputValidation) return;
    
    // Check if vision size matches expected dimensions
    size_t expectedSize = config_.visionWidth * config_.visionHeight;
    if (vision.size() != expectedSize) {
        NLM_LOG_WARNING("Vision size mismatch: expected " + std::to_string(expectedSize) + 
                        ", got " + std::to_string(vision.size()));
    }
    
    // Check if all values are in valid range [0, 1]
    for (size_t i = 0; i < vision.size(); ++i) {
        if (vision[i] < 0.0f || vision[i] > 1.0f) {
            NLM_LOG_WARNING("Vision value " + std::to_string(vision[i]) + 
                           " at index " + std::to_string(i) + " outside valid range [0, 1]");
        }
    }
}

void SensoryPercept::validateTouch(const std::vector<float>& touch) const {
    if (!config_.enableInputValidation) return;
    
    // Check if touch size matches expected
    if (touch.size() != config_.touchSensors) {
        NLM_LOG_WARNING("Touch sensor size mismatch: expected " + std::to_string(config_.touchSensors) + 
                        ", got " + std::to_string(touch.size()));
    }
    
    // Check if all values are in valid range [0, 1]
    for (size_t i = 0; i < touch.size(); ++i) {
        if (touch[i] < 0.0f || touch[i] > 1.0f) {
            NLM_LOG_WARNING("Touch sensor value " + std::to_string(touch[i]) + 
                           " at index " + std::to_string(i) + " outside valid range [0, 1]");
        }
    }
}

void SensoryPercept::validateInternal(const std::vector<float>& internal) const {
    if (!config_.enableInputValidation) return;
    
    // Check if internal signal size matches expected
    if (internal.size() != config_.internalSignals) {
        NLM_LOG_WARNING("Internal signal size mismatch: expected " + std::to_string(config_.internalSignals) + 
                        ", got " + std::to_string(internal.size()));
    }
    
    // Check if all values are in valid range [0, 1]
    for (size_t i = 0; i < internal.size(); ++i) {
        if (internal[i] < 0.0f || internal[i] > 1.0f) {
            NLM_LOG_WARNING("Internal signal value " + std::to_string(internal[i]) + 
                           " at index " + std::to_string(i) + " outside valid range [0, 1]");
        }
    }
}

void SensoryPercept::validateProprioception(const std::vector<float>& proprioception) const {
    if (!config_.enableInputValidation) return;
    
    // Check if proprioception size matches expected
    if (proprioception.size() != config_.proprioceptionSensors) {
        NLM_LOG_WARNING("Proprioception sensor size mismatch: expected " + std::to_string(config_.proprioceptionSensors) + 
                        ", got " + std::to_string(proprioception.size()));
    }
    
    // Check if all values are in valid range [-1, 1] (normalized body position/velocity)
    for (size_t i = 0; i < proprioception.size(); ++i) {
        if (proprioception[i] < -1.0f || proprioception[i] > 1.0f) {
            NLM_LOG_WARNING("Proprioception sensor value " + std::to_string(proprioception[i]) + 
                           " at index " + std::to_string(i) + " outside valid range [-1, 1]");
        }
    }
}

void SensoryPercept::validateAudio(const std::vector<float>& audio) const {
    if (!config_.enableInputValidation) return;
    
    // Check if audio size matches expected
    if (audio.size() != config_.audioChannels) {
        NLM_LOG_WARNING("Audio channel size mismatch: expected " + std::to_string(config_.audioChannels) + 
                        ", got " + std::to_string(audio.size()));
    }
    
    // Check if all values are in valid range [0, 1]
    for (size_t i = 0; i < audio.size(); ++i) {
        if (audio[i] < 0.0f || audio[i] > 1.0f) {
            NLM_LOG_WARNING("Audio channel value " + std::to_string(audio[i]) + 
                           " at index " + std::to_string(i) + " outside valid range [0, 1]");
        }
    }
}

size_t SensoryPercept::getTotalSignalCount() const {
    return vision_.size() + touch_.size() + internal_.size() + 
           proprioception_.size() + audio_.size();
}

float SensoryPercept::getSignalRange() const {
    float minVal = std::numeric_limits<float>::max();
    float maxVal = std::numeric_limits<float>::lowest();
    
    // Find min and max across all signals
    auto updateRange = [&](const std::vector<float>& signals) {
        if (!signals.empty()) {
            minVal = std::min(minVal, *std::min_element(signals.begin(), signals.end()));
            maxVal = std::max(maxVal, *std::max_element(signals.begin(), signals.end()));
        }
    };
    
    updateRange(vision_);
    updateRange(touch_);
    updateRange(internal_);
    updateRange(proprioception_);
    updateRange(audio_);
    
    if (maxVal - minVal < std::numeric_limits<float>::epsilon()) {
        return 1.0f;  // Avoid division by zero
    }
    
    return maxVal - minVal;
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
