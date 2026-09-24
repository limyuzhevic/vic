#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Sensory data packet - what the agent perceives
// Contains only low-level sensory signals, NO semantic labels
// Provides a standardized interface for all sensory modalities
class SensoryPercept {
public:
    SensoryPercept();
    ~SensoryPercept() = default;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0, representing light intensity
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { vision_ = v; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    // Each value represents proximity to nearby objects (0.0 = far, 1.0 = touching)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    // Internal state signals (homeostatic signals)
    // Representations of agent's internal needs (hunger, energy, etc.)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    // Proprioception: body position/velocity signals
    // Joint angles, velocity, and body state information
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    // Audio-like signals (if enabled)
    // Frequency and amplitude data for sound processing
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    // Get all signals concatenated (for brain input)
    // Returns: vision (flattened) + touch + internal + proprioception + audio
    std::vector<float> getAllSignals() const;
    
    // Timestamp of sensory measurement
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    // Validation methods for debugging
    // Check if sensory data is consistent and complete
    bool validateSensoryData() const;
    
    // Get sensory data summary for logging
    std::string getSensorySummary() const;
    
private:
    // Vision buffer - fixed size for brain compatibility
    // Can be resized during initialization
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    
    // Touch sensor buffer - proximity to objects
    std::vector<float> touch_;
    
    // Internal state buffer - agent's needs and homeostasis
    std::vector<float> internal_;
    
    // Proprioception buffer - body state information
    std::vector<float> proprioception_;
    
    // Audio buffer - sound signal data (if enabled)
    std::vector<float> audio_;
    
    // Time of sensory measurement
    double timestamp_;
    
    // Data validation flags
    mutable bool dataValid_;
};

} // namespace nlm
