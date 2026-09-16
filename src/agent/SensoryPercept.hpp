#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Sensory data packet - what the agent perceives
// Contains only low-level sensory signals, NO semantic labels
class SensoryPercept {
public:
    SensoryPercept();
    ~SensoryPercept() = default;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) {
        // Bounds checking
        size_t expectedSize = visionWidth_ * visionHeight_;
        if (v.size() != expectedSize) {
            // Resize to expected size to maintain consistency
            vision_.resize(expectedSize, 0.0f);
        } else {
            vision_ = v;
        }
    }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) {
        // Validate touch size (expected 8 based on constructor)
        if (t.size() <= touch_.size()) {
            touch_ = t;
        } else {
            touch_.resize(t.size(), 0.0f);
        }
    }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) {
        // Validate internal size (expected 4 based on constructor)
        if (i.size() <= internal_.size()) {
            internal_ = i;
        } else {
            internal_.resize(i.size(), 0.0f);
        }
    }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) {
        // Validate proprioception size (expected 6 based on constructor)
        if (p.size() <= proprioception_.size()) {
            proprioception_ = p;
        } else {
            proprioception_.resize(p.size(), 0.0f);
        }
    }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) {
        // Validate audio size - ensure reasonable minimum
        if (a.size() > 0) {
            audio_ = a;
        } else {
            audio_.resize(512, 0.0f);  // Default to reasonable audio buffer size
        }
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
