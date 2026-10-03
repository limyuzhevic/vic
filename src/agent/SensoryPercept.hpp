#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <limits>
#include <string>

namespace nlm {

// Sensory data packet - what the agent perceives
// Contains only low-level sensory signals, NO semantic labels
class SensoryPercept {
public:
    // Constants for validation
    static constexpr size_t MAX_VISION_WIDTH = 256;
    static constexpr size_t MAX_VISION_HEIGHT = 256;
    static constexpr size_t MAX_VISION_SIZE = MAX_VISION_WIDTH * MAX_VISION_HEIGHT;
    static constexpr size_t MAX_TOUCH_SENSORS = 64;
    static constexpr size_t MAX_INTERNAL_SENSORS = 32;
    static constexpr size_t MAX_PROPRIOCEPTION_SENSORS = 20;
    static constexpr size_t MAX_AUDIO_SENSORS = 128;
    
    SensoryPercept();
    ~SensoryPercept() = default;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) {
        // Input validation for vision data
        // Vision size is limited by visionWidth_ * visionHeight_
        if (v.size() > visionWidth_ * visionHeight_) {
            throw std::invalid_argument("SensoryPercept: Vision vector size exceeds vision dimensions");
        }
        vision_ = v;
    }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) {
        // Input validation for touch data
        if (t.size() > MAX_TOUCH_SENSORS) {
            throw std::invalid_argument("SensoryPercept: Touch vector size exceeds maximum allowed");
        }
        touch_ = t;
    }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) {
        // Input validation for internal data
        if (i.size() > MAX_INTERNAL_SENSORS) {
            throw std::invalid_argument("SensoryPercept: Internal vector size exceeds maximum allowed");
        }
        internal_ = i;
    }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) {
        // Input validation for proprioception data
        if (p.size() > MAX_PROPRIOCEPTION_SENSORS) {
            throw std::invalid_argument("SensoryPercept: Proprioception vector size exceeds maximum allowed");
        }
        proprioception_ = p;
    }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) {
        // Input validation for audio data
        if (a.size() > MAX_AUDIO_SENSORS) {
            throw std::invalid_argument("SensoryPercept: Audio vector size exceeds maximum allowed");
        }
        audio_ = a;
    }
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
private:
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    double timestamp_;
};

} // namespace nlm