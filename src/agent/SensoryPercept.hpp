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
    
    // Get total expected signal count (for validation)
    size_t getTotalSignalCount() const;
    
    // Setters with bounds checking
    void setVision(const std::vector<float>& v);
    void setTouch(const std::vector<float>& t);
    void setInternal(const std::vector<float>& i);
    void setProprioception(const std::vector<float>& p);
    void setAudio(const std::vector<float>& a);
    
    // Validation methods
    bool isVisionValid(const std::vector<float>& v) const;
    bool isTouchValid(const std::vector<float>& t) const;
    bool isInternalValid(const std::vector<float>& i) const;
    bool isProprioceptionValid(const std::vector<float>& p) const;
    
    // Configuration
    void setVisionDimensions(size_t width, size_t height);
    void setTouchCount(size_t count);
    void setInternalCount(size_t count);
    void setProprioceptionCount(size_t count);
    
    // Get configuration
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    size_t getTouchCount() const { return touch_.size(); }
    size_t getInternalCount() const { return internal_.size(); }
    size_t getProprioceptionCount() const { return proprioception_.size(); }
    size_t getAudioCount() const { return audio_.size(); }
    
    // Setters with bounds checking
    void setVision(const std::vector<float>& v);
    void setTouch(const std::vector<float>& t);
    void setInternal(const std::vector<float>& i);
    void setProprioception(const std::vector<float>& p);
    void setAudio(const std::vector<float>& a);
    
    // Validation methods
    bool isVisionValid(const std::vector<float>& v) const;
    bool isTouchValid(const std::vector<float>& t) const;
    bool isInternalValid(const std::vector<float>& i) const;
    bool isProprioceptionValid(const std::vector<float>& p) const;
    
    // Configuration
    void setVisionDimensions(size_t width, size_t height);
    void setTouchCount(size_t count);
    void setInternalCount(size_t count);
    void setProprioceptionCount(size_t count);
    
    // Get configuration
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    size_t getTouchCount() const { return touch_.size(); }
    size_t getInternalCount() const { return internal_.size(); }
    size_t getProprioceptionCount() const { return proprioception_.size(); }
    size_t getAudioCount() const { return audio_.size(); }
};

} // namespace nlm
