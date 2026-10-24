#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace nlm {

// SensoryPercept configuration parameters
struct SensoryPerceptConfig {
    // Vision configuration
    size_t visionWidth;                 // Vision width (default: 16)
    size_t visionHeight;                // Vision height (default: 16)
    float visionScale;                  // Vision scaling factor (default: 1.0f)
    bool visionEnabled;                 // Whether vision processing is enabled (default: true)
    
    // Touch configuration
    size_t touchCount;                  // Number of touch sensors (default: 8)
    float touchSensitivity;             // Touch sensitivity factor (default: 1.0f)
    bool touchEnabled;                  // Whether touch processing is enabled (default: true)
    
    // Internal signals configuration
    size_t internalSignalCount;         // Number of internal signals (default: 4)
    float internalScale;                // Internal signal scaling factor (default: 1.0f)
    bool internalEnabled;               // Whether internal signal processing is enabled (default: true)
    
    // Proprioception configuration
    size_t proprioceptionCount;         // Number of proprioception signals (default: 6)
    float proprioceptionScale;          // Proprioception scaling factor (default: 1.0f)
    bool proprioceptionEnabled;        // Whether proprioception processing is enabled (default: true)
    
    // Audio configuration
    size_t audioSampleRate;             // Audio sample rate (default: 0 = disabled)
    size_t audioNumSamples;             // Number of audio samples (default: 0 = disabled)
    float audioScale;                   // Audio scaling factor (default: 1.0f)
    bool audioEnabled;                  // Whether audio processing is enabled (default: false)
    
    // Signal processing configuration
    float noveltyThreshold;             // Novelty detection threshold (default: 0.01f)
    float noveltyDecay;                 // Novelty decay factor (default: 0.99f)
    float signalCompression;            // Signal compression factor (default: 1.0f)
    bool enableCompression;             // Whether to enable signal compression (default: false)
    
    // Memory and performance configuration
    size_t maxVisionBufferSize;         // Maximum vision buffer size (default: 65536)
    size_t maxTouchBufferSize;          // Maximum touch buffer size (default: 512)
    size_t maxInternalBufferSize;       // Maximum internal buffer size (default: 256)
    size_t maxProprioceptionBufferSize; // Maximum proprioception buffer size (default: 384)
    
    // Timestamp configuration
    bool enableTimestamps;              // Whether to enable timestamps (default: true)
    double timestampUpdateInterval;     // Timestamp update interval (default: 0.001)
    
    // Configuration loading
    void loadFromConfig(const Config& config);
    
    // Default configuration
    static SensoryPerceptConfig getDefaultConfig() {
        SensoryPerceptConfig config;
        
        // Vision configuration
        config.visionWidth = 16;
        config.visionHeight = 16;
        config.visionScale = 1.0f;
        config.visionEnabled = true;
        
        // Touch configuration
        config.touchCount = 8;
        config.touchSensitivity = 1.0f;
        config.touchEnabled = true;
        
        // Internal signals configuration
        config.internalSignalCount = 4;
        config.internalScale = 1.0f;
        config.internalEnabled = true;
        
        // Proprioception configuration
        config.proprioceptionCount = 6;
        config.proprioceptionScale = 1.0f;
        config.proprioceptionEnabled = true;
        
        // Audio configuration
        config.audioSampleRate = 0; // Disabled by default
        config.audioNumSamples = 0;
        config.audioScale = 1.0f;
        config.audioEnabled = false;
        
        // Signal processing
        config.noveltyThreshold = 0.01f;
        config.noveltyDecay = 0.99f;
        config.signalCompression = 1.0f;
        config.enableCompression = false;
        
        // Memory and performance
        config.maxVisionBufferSize = 65536;
        config.maxTouchBufferSize = 512;
        config.maxInternalBufferSize = 256;
        config.maxProprioceptionBufferSize = 384;
        
        // Timestamp
        config.enableTimestamps = true;
        config.timestampUpdateInterval = 0.001;
        
        return config;
    }
};

// Sensory data packet - what the agent perceives
// Contains only low-level sensory signals, NO semantic labels
class SensoryPercept {
public:
    SensoryPercept(const SensoryPerceptConfig& config = SensoryPerceptConfig::getDefaultConfig());
    ~SensoryPercept() = default;
    
    // Set configuration
    void setConfig(const SensoryPerceptConfig& config) { config_ = config; resizeBuffers(); }
    const SensoryPerceptConfig& getConfig() const { return config_; }
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { 
        vision_ = v; 
        if (config_.enableTimestamps) {
            timestamp_ = getCurrentTime();
        }
    }
    void setVision(size_t width, size_t height, const std::vector<float>& v);
    size_t getVisionWidth() const { return config_.visionWidth; }
    size_t getVisionHeight() const { return config_.visionHeight; }
    size_t getVisionSize() const { return vision_.size(); }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { 
        touch_ = t; 
        if (config_.enableTimestamps) {
            timestamp_ = getCurrentTime();
        }
    }
    size_t getTouchSize() const { return config_.touchCount; }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { 
        internal_ = i; 
        if (config_.enableTimestamps) {
            timestamp_ = getCurrentTime();
        }
    }
    size_t getInternalSize() const { return config_.internalSignalCount; }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { 
        proprioception_ = p; 
        if (config_.enableTimestamps) {
            timestamp_ = getCurrentTime();
        }
    }
    size_t getProprioceptionSize() const { return config_.proprioceptionCount; }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { 
        audio_ = a; 
        if (config_.enableTimestamps) {
            timestamp_ = getCurrentTime();
        }
    }
    size_t getAudioSize() const { return config_.audioNumSamples; }
    bool isAudioEnabled() const { return config_.audioEnabled; }
    
    // Get all signals concatenated (for brain input) with configurable scaling and compression
    std::vector<float> getAllSignals() const;
    std::vector<float> getAllSignalsCompressed() const;
    
    // Timestamp management
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    double getCurrentTime() const;
    
    // Signal quality and validation
    bool validateVision() const;
    bool validateTouch() const;
    bool validateInternal() const;
    bool validateProprioception() const;
    bool validateAudio() const;
    bool validateAll() const;
    
    // Signal compression and decompression
    std::vector<float> compressSignal(const std::vector<float>& signal) const;
    std::vector<float> decompressSignal(const std::vector<float>& signal) const;
    
    // Signal statistics
    float getVisionMean() const;
    float getVisionStdDev() const;
    float getTouchMean() const;
    float getInternalMean() const;
    float getProprioceptionMean() const;
    float getAudioMean() const;
    
    // Signal modification
    void normalizeVision();
    void normalizeTouch();
    void normalizeInternal();
    void normalizeProprioception();
    void normalizeAudio();
    void scaleAllSignals(float factor);
    void offsetAllSignals(float offset);
    
    // Memory management
    void resizeBuffers();
    size_t getMemoryUsage() const;
    void clear();
    
    // Comparison and novelty detection
    float compareVision(const SensoryPercept& other) const;
    bool isNovel(const SensoryPercept& previous, float threshold = -1.0f) const;
    
    // Debug and logging
    void logSignalStats() const;
    std::string toString() const;
    
private:
    SensoryPerceptConfig config_;
    
    // Sensory signals
    std::vector<float> vision_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    
    // Timestamp
    double timestamp_;
    
    // Helper methods
    void resizeVector(std::vector<float>& vec, size_t targetSize, float defaultValue = 0.0f);
    float calculateMean(const std::vector<float>& signal) const;
    float calculateStdDev(const std::vector<float>& signal, float mean) const;
    void applyCompression(std::vector<float>& signal) const;
};

} // namespace nlm