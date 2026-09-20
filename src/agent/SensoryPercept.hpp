#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>
#include <stdexcept>

namespace nlm {

// SensoryPercept: What the agent perceives
// Contains only low-level sensory signals, NO semantic labels
class SensoryPercept {
public:
    // Sensory signal constants for configuration
    static constexpr size_t VISION_WIDTH = 16;
    static constexpr size_t VISION_HEIGHT = 16;
    static constexpr size_t VISION_SIZE = VISION_WIDTH * VISION_HEIGHT;
    static constexpr size_t TOUCH_SIZE = 8;
    static constexpr size_t INTERNAL_SIZE = 4;
    static constexpr size_t PROPRIOCEPTION_SIZE = 6;
    
    SensoryPercept();
    ~SensoryPercept() = default;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v);
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t);
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i);
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p);
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a);
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    // Validation methods
    bool isValid() const;
    void validateSignalBounds() const;
    
    // Convenience methods for common operations
    float getVisionIntensity(size_t x, size_t y) const;
    void clear();
    
private:
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    double timestamp_;
    
    // Helper methods for validation
    void validateVectorSize(const std::vector<float>& vec, size_t expectedSize, 
                           const std::string& fieldName) const;
};

} // namespace nlm
