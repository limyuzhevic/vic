#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Enumeration types for signal processing
enum class SignalType {
    Vision,
    Touch,
    Internal,
    Proprioception,
    Audio,
    Combined
};

// Structure for signal statistics
struct signal_stats {
    std::vector<float> values;      // Raw signal values
    float mean;                      // Statistical mean
    float variance;                  // Statistical variance
    float min;                       // Minimum value
    float max;                       // Maximum value
    float range;                     // Range (max - min)
};

// Sensory data packet - what the agent perceives
// Contains only low-level sensory signals, NO semantic labels
// Enhanced with advanced signal processing capabilities
class SensoryPercept {
public:
    SensoryPercept();
    ~SensoryPercept();
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { vision_ = v; visionNormalized_ = false; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    size_t getVisionSize() const { return vision_.size(); }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { touch_ = t; touchNormalized_ = false; }
    size_t getTouchSize() const { return touch_.size(); }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { internal_ = i; internalNormalized_ = false; }
    size_t getInternalSize() const { return internal_.size(); }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; proprioceptionNormalized_ = false; }
    size_t getProprioceptionSize() const { return proprioception_.size(); }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; audioNormalized_ = false; }
    size_t getAudioSize() const { return audio_.size(); }
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Enhanced signal processing methods
    std::vector<float> getNormalizedSignals();
    std::vector<float> getZeroMeanSignals();
    signal_stats getSignalStatistics(SignalType signalType) const;
    
    // Signal validation and normalization
    void validateSignals() const;
    void normalizeAllSignals();
    void zeroMeanAllSignals();
    
    // Advanced signal processing
    void applyFilter(SignalType signalType, const std::vector<float>& kernel);
    void addNoise(float noiseLevel);
    void scaleSignals(float visionScale, float touchScale,
                     float internalScale, float proprioceptionScale,
                     float audioScale);
    
    // Individual signal normalization methods
    void normalizeVision();
    void normalizeTouch();
    void normalizeInternal();
    void normalizeProprioception();
    void normalizeAudio();
    
    void zeroMeanVision();
    void zeroMeanTouch();
    void zeroMeanInternal();
    void zeroMeanProprioception();
    void zeroMeanAudio();
    
    // Utility methods
    float calculateMean(const std::vector<float>& values) const;
    float calculateVariance(const std::vector<float>& values, float mean) const;
    
    // Configuration and state management
    void setSignalValidationEnabled(bool enabled) { validationEnabled_ = enabled; }
    bool isSignalValidationEnabled() const { return validationEnabled_; }
    
    void setAutoNormalization(bool enabled) { autoNormalization_ = enabled; }
    bool isAutoNormalizationEnabled() const { return autoNormalization_; }
    
    bool isVisionNormalized() const { return visionNormalized_; }
    bool isTouchNormalized() const { return touchNormalized_; }
    bool isInternalNormalized() const { return internalNormalized_; }
    bool isProprioceptionNormalized() const { return proprioceptionNormalized_; }
    bool isAudioNormalized() const { return audioNormalized_; }
    
    bool isVisionZeroMean() const { return visionZeroMean_; }
    bool isTouchZeroMean() const { return touchZeroMean_; }
    bool isInternalZeroMean() const { return internalZeroMean_; }
    bool isProprioceptionZeroMean() const { return proprioceptionZeroMean_; }
    bool isAudioZeroMean() const { return audioZeroMean_; }
    
    size_t getTotalSignalCount() const {
        return vision_.size() + touch_.size() + internal_.size() +
               proprioception_.size() + audio_.size();
    }
    
    // Timestamp management
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    double getCurrentTime() const { return timestamp_; }
    
    // Signal state management
    void resetSignalStates() {
        visionNormalized_ = false;
        touchNormalized_ = false;
        internalNormalized_ = false;
        proprioceptionNormalized_ = false;
        audioNormalized_ = false;
        visionZeroMean_ = false;
        touchZeroMean_ = false;
        internalZeroMean_ = false;
        proprioceptionZeroMean_ = false;
        audioZeroMean_ = false;
    }
    
    // Convenience methods for common operations
    void normalizeAllAndZeroMean() {
        normalizeAllSignals();
        zeroMeanAllSignals();
    }
    
    void applyBandPassFilter(const std::vector<float>& kernel) {
        applyFilter(SignalType::Combined, kernel);
    }
    
    // Error handling
    bool hasSignalErrors() const {
        return validationEnabled_ && !vision_.empty();
    }
    
    std::string getSignalStatus() const {
        std::string status = "SensoryPercept Status:\n";
        status += "Vision: " + std::to_string(vision_.size()) + " values (normalized: " +
                 std::to_string(visionNormalized_) + ")\n";
        status += "Touch: " + std::to_string(touch_.size()) + " values (normalized: " +
                 std::to_string(touchNormalized_) + ")\n";
        status += "Internal: " + std::to_string(internal_.size()) + " values (normalized: " +
                 std::to_string(internalNormalized_) + ")\n";
        status += "Proprioception: " + std::to_string(proprioception_.size()) + " values (normalized: " +
                 std::to_string(proprioceptionNormalized_) + ")\n";
        status += "Audio: " + std::to_string(audio_.size()) + " values (normalized: " +
                 std::to_string(audioNormalized_) + ")\n";
        return status;
    }
    
    // Performance optimization hints
    void reserveMemory() {
        vision_.shrink_to_fit();
        touch_.shrink_to_fit();
        internal_.shrink_to_fit();
        proprioception_.shrink_to_fit();
        audio_.shrink_to_fit();
    }
    
    void setMaxCapacity(size_t maxVisionSize, size_t maxTouchSize,
                       size_t maxInternalSize, size_t maxProprioceptionSize,
                       size_t maxAudioSize) {
        vision_.reserve(maxVisionSize);
        touch_.reserve(maxTouchSize);
        internal_.reserve(maxInternalSize);
        proprioception_.reserve(maxProprioceptionSize);
        audio_.reserve(maxAudioSize);
    }
    
private:
    // Core sensory data
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    double timestamp_;
    
    // Signal processing state
    bool visionNormalized_;
    bool touchNormalized_;
    bool internalNormalized_;
    bool proprioceptionNormalized_;
    bool audioNormalized_;
    bool visionZeroMean_;
    bool touchZeroMean_;
    bool internalZeroMean_;
    bool proprioceptionZeroMean_;
    bool audioZeroMean_;
    
    // Configuration flags
    bool validationEnabled_;
    bool autoNormalization_;
};

} // namespace nlm
