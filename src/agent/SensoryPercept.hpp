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
    bool setVision(const std::vector<float>& v);
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    bool setTouch(const std::vector<float>& t);
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    bool setInternal(const std::vector<float>& i);
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    bool setProprioception(const std::vector<float>& p);
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a);
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
private:
    bool validateSizeParameters(size_t expectedSize, size_t providedSize, 
                                const std::string& componentName);
    bool validateFloatRange(float value, const std::string& componentName, size_t index);
    
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
