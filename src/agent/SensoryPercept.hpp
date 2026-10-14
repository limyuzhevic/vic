#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

// SensoryPercept: Agent sensory data packet
// Contains what the agent perceives from the environment
// Only low-level sensory signals, NO semantic labels or object information
// 
// This class represents the complete sensory input that an agent receives:
// - Vision: Simple grayscale intensity grid (no RGB, no object labels)
// - Touch: Binary collision/proximity sensors
// - Internal: Homeostatic and metabolic state signals
// - Proprioception: Body position and movement sensors
// - Audio: Sound intensity signals (if enabled)
// 
// Sensory data format:
// - All values are normalized to range [0.0, 1.0] for neural input
// - Vision: Row-major grid, width × height dimensions
// - Touch: Array of collision signals
// - Internal: Array of internal state signals
// - Proprioception: Body position and velocity
// - Audio: Sound intensity samples
// 
// The brain receives all signals concatenated for unified processing:
// [vision data] + [touch data] + [internal data] + [proprioception data] + [audio data]
// 
// Used by AgentBrain for sensorimotor integration in the brain-world loop.
class SensoryPercept {
public:
    SensoryPercept();
    ~SensoryPercept() = default;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { vision_ = v; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
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
