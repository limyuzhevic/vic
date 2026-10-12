#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept(int visionW, int visionH, int touchCount,
                               int internalCount, int proprioceptionCount,
                               const AudioFormat& audioFormat)
    : visionWidth_(visionW)
    , visionHeight_(visionH)
    , timestamp_(0.0)
    , audioFormat_(audioFormat)
{
    // Validate input parameters
    if (visionW <= 0 || visionH <= 0) {
        NLM_LOG_ERROR("Invalid vision dimensions: " + std::to_string(visionW) + 
                     "x" + std::to_string(visionH));
        visionWidth_ = 16;  // Default fallback
        visionHeight_ = 16;
    }
    
    if (touchCount < 0) {
        NLM_LOG_ERROR("Negative touch count: " + std::to_string(touchCount));
        touchCount = 8;  // Default fallback
    }
    
    if (internalCount < 0) {
        NLM_LOG_ERROR("Negative internal count: " + std::to_string(internalCount));
        internalCount = 4;  // Default fallback
    }
    
    if (proprioceptionCount < 0) {
        NLM_LOG_ERROR("Negative proprioception count: " + std::to_string(proprioceptionCount));
        proprioceptionCount = 6;  // Default fallback
    }
    
    // Reserve memory upfront for better performance
    reserveMemory();
    
    // Initialize with zeros
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(static_cast<size_t>(touchCount), 0.0f);
    internal_.resize(static_cast<size_t>(internalCount), 0.0f);
    proprioception_.resize(static_cast<size_t>(proprioceptionCount), 0.0f);
    audio_.resize(0, 0.0f);
}

std::vector<float> SensoryPercept::getAllSignals() const {
    // Pre-validate state
    if (!validate()) {
        NLM_LOG_WARNING("SensoryPercept validation failed in getAllSignals()");
        // Return empty signal as safe fallback
        return std::vector<float>();
    }
    
    // Reserve memory based on total expected signal count
    std::vector<float> all;
    all.reserve(getTotalSignalCount());
    
    // Vision (flattened) - optimized with direct insertion
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

bool SensoryPercept::validate() const noexcept {
    // Check for reasonable vector sizes
    bool valid = true;
    
    // Validate vision bounds (values should be in expected range)
    for (float v : vision_) {
        if (v < 0.0f || v > 1.0f) {
            valid = false;
            break;
        }
    }
    
    // Audio vector size should be multiple of channels if audio is present
    if (!audio_.empty()) {
        if (audioFormat_.channels <= 0) {
            valid = false;
        } else {
            // Check if audio length is multiple of channel count
            if (audio_.size() % audioFormat_.channels != 0) {
                valid = false;
            }
        }
    }
    
    return valid;
}

size_t SensoryPercept::getTotalSignalCount() const noexcept {
    return visionWidth_ * visionHeight_ + touch_.size() + 
           internal_.size() + proprioception_.size() + audio_.size();
}

SensoryPercept& SensoryPercept::operator=(SensoryPercept&& other) noexcept {
    if (this != &other) {
        // Move vectors efficiently
        vision_ = std::move(other.vision_);
        touch_ = std::move(other.touch_);
        internal_ = std::move(other.internal_);
        proprioception_ = std::move(other.proprioception_);
        audio_ = std::move(other.audio_);
        
        // Copy scalar values
        visionWidth_ = other.visionWidth_;
        visionHeight_ = other.visionHeight_;
        audioFormat_ = other.audioFormat_;
        timestamp_ = other.timestamp_;
        
        // Clear source values
        other.visionWidth_ = 0;
        other.visionHeight_ = 0;
        other.timestamp_ = 0.0;
        other.audio_.clear();
    }
    return *this;
}

SensoryPercept& SensoryPercept::operator=(const SensoryPercept& other) {
    if (this != &other) {
        // Deep copy vectors
        vision_ = other.vision_;
        touch_ = other.touch_;
        internal_ = other.internal_;
        proprioception_ = other.proprioception_;
        audio_ = other.audio_;
        
        // Copy scalar values
        visionWidth_ = other.visionWidth_;
        visionHeight_ = other.visionHeight_;
        audioFormat_ = other.audioFormat_;
        timestamp_ = other.timestamp_;
    }
    return *this;
}

void SensoryPercept::reserveMemory() noexcept {
    // Reserve reasonable amounts of memory for vectors
    // These are upper bounds based on typical agent configuration
    vision_.reserve(256);  // Max 16x16 vision
    touch_.reserve(16);    // Max 8 touch sensors (but reserve twice that)
    internal_.reserve(8);   // Max 4 internal sensors
    proprioception_.reserve(12); // Max 6 proprioceptive signals
    audio_.reserve(0);     // Audio size depends on format
}

} // namespace nlm
