#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Sensory data packet - what the agent perceives
// Contains only low-level sensory signals, NO semantic labels
class SensoryPercept {
public:
    SensoryPercept(std::shared_ptr<Config> config = nullptr);
    ~SensoryPercept() = default;
    
    // Configure sensory dimensions
    void configureSensoryDimensions(
        size_t visionWidth,
        size_t visionHeight,
        size_t touchCount,
        size_t internalCount,
        size_t proprioceptionCount,
        size_t audioCount);
    
    // Enable/disable sensory processing features
    void enableSensoryValidation(bool enable) { validateInput_ = enable; }
    void enableSensoryFiltering(bool enable) { filterInput_ = enable; }
    
    bool isSensoryValidationEnabled() const { return validateInput_; }
    bool isSensoryFilteringEnabled() const { return filterInput_; }
    
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
    // Helper method to validate input against configured dimensions
    bool validateInputSize(const std::vector<float>& input, size_t expectedSize) const;
    
    // Helper method to filter input (placeholder for advanced filtering)
    void filterInput(std::vector<float>& input) const;
    
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    double timestamp_;
    
    // Configuration flags
    bool validateInput_;
    bool filterInput_;
    
    // Configuration values
    size_t touchSensorCount_;
    size_t internalSensorCount_;
    size_t proprioceptionSensorCount_;
    size_t audioSensorCount_;
};

} // namespace nlm
