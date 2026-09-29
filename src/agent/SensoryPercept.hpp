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
    // Configuration structure for sensory processing
    struct Configuration {
        size_t visionWidth = 16;                    // Vision grid width
        size_t visionHeight = 16;                   // Vision grid height
        size_t touchSensors = 8;                   // Number of touch sensors
        size_t internalSignals = 4;                // Number of internal signals
        size_t proprioceptionSensors = 6;         // Number of proprioception sensors
        size_t audioChannels = 0;                   // Number of audio channels
        bool enableInputValidation = true;         // Enable input validation
        bool enableDebugLogging = false;           // Enable detailed debug logging
        float signalNormalizationFactor = 1.0f;    // General signal normalization factor
        
        Configuration() = default;
    };

    SensoryPercept();
    ~SensoryPercept() = default;
    
    // Set configuration
    void setConfiguration(const Configuration& config) { config_ = config; }
    const Configuration& getConfiguration() const { return config_; }
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { 
        if (config_.enableInputValidation) {
            validateVision(v);
        }
        vision_ = v; 
    }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { 
        if (config_.enableInputValidation) {
            validateTouch(t);
        }
        touch_ = t; 
    }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { 
        if (config_.enableInputValidation) {
            validateInternal(i);
        }
        internal_ = i; 
    }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { 
        if (config_.enableInputValidation) {
            validateProprioception(p);
        }
        proprioception_ = p; 
    }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { 
        if (config_.enableInputValidation) {
            validateAudio(a);
        }
        audio_ = a; 
    }
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    // Validation methods
    void validateVision(const std::vector<float>& vision) const;
    void validateTouch(const std::vector<float>& touch) const;
    void validateInternal(const std::vector<float>& internal) const;
    void validateProprioception(const std::vector<float>& proprioception) const;
    void validateAudio(const std::vector<float>& audio) const;
    
    // Utility methods
    size_t getTotalSignalCount() const;
    float getSignalRange() const;
    
private:
    Configuration config_;
    
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
