#include "SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Validate minimum sizes
    if (visionWidth_ == 0) {
        NLM_LOG_WARNING("Vision width is 0, setting to default 16");
        visionWidth_ = 16;
    }
    if (visionHeight_ == 0) {
        NLM_LOG_WARNING("Vision height is 0, setting to default 16");
        visionHeight_ = 16;
    }
    
    size_t expectedVisionSize = visionWidth_ * visionHeight_;
    vision_.resize(expectedVisionSize, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
    
    // Validate initialization
    if (vision_.size() != expectedVisionSize) {
        NLM_LOG_ERROR("Failed to initialize vision buffer correctly");
    }
    
    NLM_LOG_DEBUG("SensoryPercept initialized: vision=" + std::to_string(vision_.size()) + 
                  ", touch=" + std::to_string(touch_.size()) + 
                  ", internal=" + std::to_string(internal_.size()) + 
                  ", proprioception=" + std::to_string(proprioception_.size()));
}

bool SensoryPercept::setVision(const std::vector<float>& vision, size_t width, size_t height) {
    if (!validateSizeParameters(width, height, vision.size(), "vision")) {
        return false;
    }
    
    visionWidth_ = width;
    visionHeight_ = height;
    vision_ = vision;
    
    // Ensure size matches
    if (vision_.size() != visionWidth_ * visionHeight_) {
        NLM_LOG_ERROR("Vision size mismatch after setVision");
        vision_.clear();
        vision_.resize(visionWidth_ * visionHeight_, 0.0f);
        return false;
    }
    
    NLM_LOG_DEBUG("Vision set: " + std::to_string(vision_.size()) + " values, dimensions " + 
                  std::to_string(visionWidth_) + "x" + std::to_string(visionHeight_));
    return true;
}

bool SensoryPercept::setTouch(const std::vector<float>& touch) {
    if (touch.size() != 8) {
        NLM_LOG_WARNING("Touch percept expected 8 values, got " + std::to_string(touch.size()) + ". Resizing.");
        touch_.resize(8, 0.0f);
    }
    
    if (touch.empty()) {
        NLM_LOG_WARNING("Touch percept is empty");
        return false;
    }
    
    for (size_t i = 0; i < std::min(touch.size(), touch_.size()); ++i) {
        if (!validateFloatRange(touch[i], "touch", i)) {
            NLM_LOG_WARNING("Touch value at index " + std::to_string(i) + " is invalid");
        }
        touch_[i] = touch[i];
    }
    
    NLM_LOG_DEBUG("Touch set: " + std::to_string(touch_.size()) + " values");
    return true;
}

bool SensoryPercept::setInternal(const std::vector<float>& internal) {
    if (internal.size() != 4) {
        NLM_LOG_WARNING("Internal signals expected 4 values, got " + std::to_string(internal.size()) + ". Resizing.");
        internal_.resize(4, 0.0f);
    }
    
    if (internal.empty()) {
        NLM_LOG_WARNING("Internal signals are empty");
        return false;
    }
    
    for (size_t i = 0; i < std::min(internal.size(), internal_.size()); ++i) {
        if (!validateFloatRange(internal[i], "internal", i)) {
            NLM_LOG_WARNING("Internal signal value at index " + std::to_string(i) + " is invalid");
        }
        internal_[i] = internal[i];
    }
    
    NLM_LOG_DEBUG("Internal signals set: " + std::to_string(internal_.size()) + " values");
    return true;
}

bool SensoryPercept::setProprioception(const std::vector<float>& proprioception) {
    if (proprioception.size() != 6) {
        NLM_LOG_WARNING("Proprioception expected 6 values, got " + std::to_string(proprioception.size()) + ". Resizing.");
        proprioception_.resize(6, 0.0f);
    }
    
    if (proprioception.empty()) {
        NLM_LOG_WARNING("Proprioception is empty");
        return false;
    }
    
    for (size_t i = 0; i < std::min(proprioception.size(), proprioception_.size()); ++i) {
        if (!validateFloatRange(proprioception[i], "proprioception", i)) {
            NLM_LOG_WARNING("Proprioception value at index " + std::to_string(i) + " is invalid");
        }
        proprioception_[i] = proprioception[i];
    }
    
    NLM_LOG_DEBUG("Proprioception set: " + std::to_string(proprioception_.size()) + " values");
    return true;
}

bool SensoryPercept::validateSizeParameters(size_t expectedSize, size_t providedSize, 
                                            const std::string& componentName) {
    if (providedSize == 0) {
        NLM_LOG_WARNING(componentName + " has zero size");
    }
    if (providedSize < expectedSize) {
        NLM_LOG_WARNING(componentName + " has insufficient data: expected " + 
                        std::to_string(expectedSize) + " values, got " + std::to_string(providedSize));
        return false;
    }
    if (providedSize > expectedSize) {
        NLM_LOG_WARNING(componentName + " has excessive data: expected " + 
                        std::to_string(expectedSize) + " values, got " + std::to_string(providedSize) + ". Truncating.");
    }
    return true;
}

bool SensoryPercept::validateFloatRange(float value, const std::string& componentName, size_t index) {
    if (!std::isfinite(value)) {
        NLM_LOG_WARNING(componentName + " value at index " + std::to_string(index) + 
                        " is not finite: " + std::to_string(value));
        return false;
    }
    return true;
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Validate all components before combining
    if (vision_.empty() && !touch_.empty()) {
        NLM_LOG_WARNING("Vision is empty but touch has data - vision may be missing");
    }
    
    if (touch_.empty() && !internal_.empty()) {
        NLM_LOG_WARNING("Touch is empty but internal has data - touch may be missing");
    }
    
    if (internal_.empty() && !proprioception_.empty()) {
        NLM_LOG_WARNING("Internal is empty but proprioception has data - internal may be missing");
    }
    
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
    
    // Validate final signal vector
    size_t expectedTotal = (visionWidth_ * visionHeight_) + 8 + 4 + 6;
    if (all.size() != expectedTotal) {
        NLM_LOG_ERROR("Signal vector size mismatch: expected " + std::to_string(expectedTotal) + 
                      ", got " + std::to_string(all.size()));
    }
    
    NLM_LOG_DEBUG("Combined " + std::to_string(all.size()) + " signals for brain input");
    return all;
}

} // namespace nlm
