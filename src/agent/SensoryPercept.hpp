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
    // Default constructor with predefined dimensions
    SensoryPercept();
    
    // Constructor with configurable dimensions
    SensoryPercept(size_t visionWidth, size_t visionHeight, 
                   size_t touchSize, size_t internalSize,
                   size_t proprioceptionSize, size_t audioSize);
    
    ~SensoryPercept() = default;
    
    // Configure sensory dimensions
    void configure(size_t visionWidth, size_t visionHeight,
                   size_t touchSize, size_t internalSize,
                   size_t proprioceptionSize, size_t audioSize);
    
    // Initialize with default dimensions
    void initializeDefaultDimensions();
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { vision_ = v; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    size_t getVisionArea() const { return visionWidth_ * visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    size_t getTouchSize() const { return touch_.size(); }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    size_t getInternalSize() const { return internal_.size(); }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    size_t getProprioceptionSize() const { return proprioception_.size(); }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    size_t getAudioSize() const { return audio_.size(); }
    
    // Get total sensory input size expected by brain
    size_t getTotalSensoryInputSize() const;
    
    // Validate that all sensory data has correct size
    bool validate() const;
    
    // Resize vectors if needed (maintain content)
    void resizeVectors();
    
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
    
    // Internal buffers for config values
    size_t configuredVisionWidth_;
    size_t configuredVisionHeight_;
    size_t configuredTouchSize_;
    size_t configuredInternalSize_;
    size_t configuredProprioceptionSize_;
    size_t configuredAudioSize_;
};

} // namespace nlm
