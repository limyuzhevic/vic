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
    
    // Get config file path for loading brain state
    std::string getConfigFilePath() const { return configFilePath_; }
    void setConfigFilePath(const std::string& path) { configFilePath_ = path; }
    
    // Sensory processing configuration
    bool isAudioEnabled() const { return audioEnabled_; }
    void enableAudio(bool enable) { audioEnabled_ = enable; }
    
    // Signal processing configuration
    void setVisionConfig(float width, float height, float scale = 5.0f, bool normalize = true);
    void setTouchConfig(float scale = 8.0f, bool normalize = true);
    void setInternalConfig(float scale = 5.0f, bool normalize = true);
    void setProprioceptionConfig(float scale = 3.0f, bool normalize = true);
    void setAudioConfig(size_t channels = 0, float scale = 1.0f, bool normalize = true);
    
    // Runtime dimension adjustment
    void adjustVisionDimensions(size_t width, size_t height);
    void adjustTouchDimensions(size_t points);
    void adjustInternalDimensions(size_t signals);
    void adjustProprioceptionDimensions(size_t points);
    
    // Signal processing and normalization
    void normalizeVision();
    void normalizeTouch();
    void normalizeInternal();
    void normalizeProprioception();
    void normalizeAudio();
    
    void scaleVision(float scale);
    void scaleTouch(float scale);
    void scaleInternal(float scale);
    void scaleProprioception(float scale);
    void scaleAudio(float scale);
    
    // Audio processing
    void processAudio() { if (audioEnabled_) processAudioInternal(); }
    void processAudioInternal();
    
    // Enhanced signal processing
    void applyEnvelopeDetection(std::vector<float>& signal);
    void applySpectralFeatures(std::vector<float>& signal);
    void applyTemporalSmoothing(std::vector<float>& signal);
    
    // Noise reduction
    void applyNoiseReduction(std::vector<float>& signal, float threshold = 0.1f);
    
    // Feature extraction
    std::vector<float> extractFrequencyFeatures(const std::vector<float>& audio) const;
    std::vector<float> extractTemporalFeatures(const std::vector<float>& audio) const;
    std::vector<float> extractSpectralFeatures(const std::vector<float>& audio) const;
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    // Metadata for audio
    double getAudioDuration() const { return audioDuration_; }
    void setAudioDuration(double duration) { audioDuration_ = duration; }
    
    size_t getAudioSampleRate() const { return audioSampleRate_; }
    void setAudioSampleRate(size_t rate) { audioSampleRate_ = rate; }
    
    // Audio processing state
    bool isAudioProcessed() const { return audioProcessed_; }
    void resetAudioProcessed() { audioProcessed_ = false; }
    
private:
    // Config file path support
    std::string configFilePath_;
    bool useConfigFile_;
    
    // Audio processing state
    bool audioEnabled_;
    bool audioProcessed_;
    size_t audioChannels_;
    float audioScale_;
    bool audioNormalized_;
    double audioDuration_;
    size_t audioSampleRate_;
    
    // Signal processing state
    std::vector<float> previousVision_;
    bool visionProcessed_;
    bool touchProcessed_;
    bool internalProcessed_;
    bool proprioceptionProcessed_;
    
    // Audio processing cache
    std::vector<float> processedAudio_;
    std::vector<float> audioFeatures_;
    
    // Signal processing cache
    std::vector<float> processedVision_;
    std::vector<float> processedTouch_;
    std::vector<float> processedInternal_;
    std::vector<float> processedProprioception_;
};

} // namespace nlm
