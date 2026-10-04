#include "SensoryPercept.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Initialize with default values based on standard configuration
    // These values can be overridden by the world configuration
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
}

void SensoryPercept::validateVisionData() const {
    // Validate vision data dimensions match expected size
    if (vision_.size() != visionWidth_ * visionHeight_) {
        // This should not happen in normal operation, but we log it for debugging
        NLM_LOG_WARNING("Vision data size mismatch: expected " + 
                       std::to_string(visionWidth_ * visionHeight_) + 
                       " but got " + std::to_string(vision_.size()));
    }
    
    // Validate vision values are within valid range [0.0, 1.0]
    for (size_t i = 0; i < vision_.size(); ++i) {
        if (vision_[i] < 0.0f || vision_[i] > 1.0f) {
            NLM_LOG_WARNING("Vision value out of range at index " + 
                           std::to_string(i) + ": " + std::to_string(vision_[i]));
        }
    }
}

void SensoryPercept::validateTouchData() const {
    // Validate touch data dimensions
    if (touch_.size() != 8) {
        NLM_LOG_WARNING("Touch data size mismatch: expected 8 but got " + 
                       std::to_string(touch_.size()));
    }
    
    // Validate touch values are within reasonable range
    for (size_t i = 0; i < touch_.size(); ++i) {
        if (touch_[i] < 0.0f || touch_[i] > 2.0f) {  // Allow some margin
            NLM_LOG_WARNING("Touch value out of range at index " + 
                           std::to_string(i) + ": " + std::to_string(touch_[i]));
        }
    }
}

void SensoryPercept::validateInternalData() const {
    // Validate internal data dimensions
    if (internal_.size() != 4) {
        NLM_LOG_WARNING("Internal data size mismatch: expected 4 but got " + 
                       std::to_string(internal_.size()));
    }
    
    // Validate internal values are within reasonable ranges
    // Index 0: Energy level (0.0 to 1.0)
    if (internal_[0] < 0.0f || internal_[0] > 1.0f) {
        NLM_LOG_WARNING("Internal energy value out of range: " + 
                       std::to_string(internal_[0]));
    }
    
    // Index 1: Health (0.0 to 1.0)
    if (internal_[1] < 0.0f || internal_[1] > 1.0f) {
        NLM_LOG_WARNING("Internal health value out of range: " + 
                       std::to_string(internal_[1]));
    }
    
    // Index 2: Movement state (0.0 to 1.0)
    if (internal_[2] < 0.0f || internal_[2] > 1.0f) {
        NLM_LOG_WARNING("Internal movement value out of range: " + 
                       std::to_string(internal_[2]));
    }
    
    // Index 3: Turning state (0.0 to 1.0)
    if (internal_[3] < 0.0f || internal_[3] > 1.0f) {
        NLM_LOG_WARNING("Internal turning value out of range: " + 
                       std::to_string(internal_[3]));
    }
}

void SensoryPercept::validateProprioceptionData() const {
    // Validate proprioception data dimensions
    if (proprioception_.size() != 6) {
        NLM_LOG_WARNING("Proprioception data size mismatch: expected 6 but got " + 
                       std::to_string(proprioception_.size()));
    }
    
    // Validate proprioception values are within [0.0, 1.0] range
    for (size_t i = 0; i < proprioception_.size(); ++i) {
        if (proprioception_[i] < 0.0f || proprioception_[i] > 1.0f) {
            NLM_LOG_WARNING("Proprioception value out of range at index " + 
                           std::to_string(i) + ": " + std::to_string(proprioception_[i]));
        }
    }
}

std::vector<float> SensoryPercept::getAllSignals() const {
    // Validate data before returning
    validateVisionData();
    validateTouchData();
    validateInternalData();
    validateProprioceptionData();
    
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
