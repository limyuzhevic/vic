#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Configuration for sensory modalities
struct SensoryConfig {
    // Vision parameters
    size_t visionWidth = 16;                 // Vision grid width
    size_t visionHeight = 16;                // Vision grid height
    bool visionEnabled = true;               // Enable vision sensing
    
    // Touch parameters
    size_t touchSensors = 8;                 // Number of touch/proximity sensors
    bool touchEnabled = true;                // Enable touch sensing
    
    // Internal state parameters
    size_t internalSensors = 4;              // Number of internal homeostatic sensors
    bool internalEnabled = true;             // Enable internal sensing
    
    // Proprioception parameters
    size_t proprioceptionSensors = 6;       // Number of proprioception sensors
    bool proprioceptionEnabled = true;      // Enable proprioception
    
    // Audio parameters
    size_t audioSampleRate = 0;              // Audio sample rate (0 = disabled)
    size_t audioSamples = 0;                 // Number of audio samples
    bool audioEnabled = false;               // Enable audio sensing
    
    // Signal processing
    float visionSignalRange = 1.0f;         // Vision signal range [0, range]
    float touchSignalRange = 1.0f;          // Touch signal range [0, range]
    float internalSignalRange = 2.0f;       // Internal signal range
    float proprioceptionSignalRange = 2.0f; // Proprioception signal range
    
    // Temporal parameters
    double sensorUpdateRate = 0.1;          // Sensor update rate (seconds)
    bool enableTimestamps = true;           // Enable timestamps
    
    // Configuration validation
    bool isValid() const {
        if (visionEnabled && visionWidth == 0) return false;
        if (visionEnabled && visionHeight == 0) return false;
        if (touchEnabled && touchSensors == 0) return false;
        if (internalEnabled && internalSensors == 0) return false;
        if (proprioceptionEnabled && proprioceptionSensors == 0) return false;
        if (audioEnabled && audioSampleRate == 0) return false;
        if (audioEnabled && audioSamples == 0) return false;
        return true;
    }
    
    // Get total signal count
    size_t getTotalSignalCount() const {
        size_t total = 0;
        if (visionEnabled) total += visionWidth * visionHeight;
        if (touchEnabled) total += touchSensors;
        if (internalEnabled) total += internalSensors;
        if (proprioceptionEnabled) total += proprioceptionSensors;
        if (audioEnabled) total += audioSamples;
        return total;
    }
    
    // Get formatted string for logging
    std::string toString() const {
        std::string result = "SensoryConfig:";
        if (visionEnabled) result += " vision:" + std::to_string(visionWidth) + "x" + std::to_string(visionHeight);
        if (touchEnabled) result += " touch:" + std::to_string(touchSensors);
        if (internalEnabled) result += " internal:" + std::to_string(internalSensors);
        if (proprioceptionEnabled) result += " proprio:" + std::to_string(proprioceptionSensors);
        if (audioEnabled) result += " audio:" + std::to_string(audioSamples);
        return result;
    }
};

// Enhanced SensoryPercept with configurable modalities and validation
class SensoryPercept {
public:
    explicit SensoryPercept(const SensoryConfig& config = SensoryConfig());
    ~SensoryPercept() = default;
    
    // Set sensory configuration
    void setConfig(const SensoryConfig& config) {
        config_ = config;
        updateSignalBuffers();
    }
    
    const SensoryConfig& getConfig() const { return config_; }
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { 
        vision_ = v; 
        if (config_.enableTimestamps) setTimestamp(currentTime_);
    }
    
    size_t getVisionWidth() const { return config_.visionWidth; }
    size_t getVisionHeight() const { return config_.visionHeight; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { 
        touch_ = t; 
        if (config_.enableTimestamps) setTimestamp(currentTime_);
    }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { 
        internal_ = i; 
        if (config_.enableTimestamps) setTimestamp(currentTime_);
    }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { 
        proprioception_ = p; 
        if (config_.enableTimestamps) setTimestamp(currentTime_);
    }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { 
        audio_ = a; 
        if (config_.enableTimestamps) setTimestamp(currentTime_);
    }
    
    // Update time (for timestamps)
    void updateTime(double time) { currentTime_ = time; }
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; currentTime_ = t; }
    
    // Validate current signal dimensions
    bool validateSignalDimensions() const;
    
    // Reset all signals
    void reset();
    
    // Update all signals with new data
    void updateSignals(const std::vector<float>& visionData = {},
                       const std::vector<float>& touchData = {},
                       const std::vector<float>& internalData = {},
                       const std::vector<float>& proprioceptionData = {},
                       const std::vector<float>& audioData = {});
    
    // Get signal information for logging
    std::string getSignalInfo() const;
    
private:
    // Update signal buffers based on current configuration
    void updateSignalBuffers();
    
    SensoryConfig config_;
    
    // Signal buffers
    std::vector<float> vision_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    
    // Timestamps
    double timestamp_;
    double currentTime_;
};

} // namespace nlm
