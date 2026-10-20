#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Vision;
class Audio;
class InternalSignals;

// SensoryModality enumeration for flexible sensory processing
enum class SensoryModality {
    Vision,
    Touch,
    Internal,
    Proprioception,
    Audio,
    Vestibular,
    Olfactory,
    Gustatory
};

// SensoryPercept: Enhanced container for all sensory inputs received by the agent
// Provides configurable modalities, advanced processing, and Pythonic interfaces
class SensoryPercept {
public:
    SensoryPercept();
    ~SensoryPercept();
    
    // Disable copying, enable moving
    SensoryPercept(const SensoryPercept&) = delete;
    SensoryPercept& operator=(const SensoryPercept&) = delete;
    SensoryPercept(SensoryPercept&&) noexcept;
    SensoryPercept& operator=(SensoryPercept&&) noexcept;
    
    // Enhanced sensory configuration methods
    void configureModality(SensoryModality modality, size_t dimensions, const std::string& name = "");
    void configureModality(SensoryModality modality, size_t width, size_t height, size_t channels = 3);
    void configureModality(SensoryModality modality, size_t sampleRate, size_t numSamples);
    
    bool hasModality(SensoryModality modality) const;
    size_t getModalityDimensions(SensoryModality modality) const;
    std::string getModalityName(SensoryModality modality) const;
    
    // Vision data with advanced processing
    const Vision& getVision() const { return vision_; }
    Vision& getVision() { return vision_; }
    void setVision(const Vision& vision) { vision_ = vision; }
    void setVision(size_t width, size_t height, const std::vector<float>& data);
    void setVision(size_t width, size_t height, size_t channels,
                   const std::vector<float>& data);
    void enhanceVision(float enhancement);
    void applyVisionFilter(const std::vector<float>& kernel);
    void clearVision() { vision_.setData(std::vector<float>()); }
    
    // Multi-touch support (not just binary contact)
    const std::vector<std::pair<size_t, float>>& getTouch() const { return touch_; }
    std::vector<std::pair<size_t, float>>& getTouch() { return touch_; }
    void setTouch(const std::vector<std::pair<size_t, float>>& touch) { touch_ = touch; }
    void addTouch(size_t index, float pressure, float intensity = 1.0f, 
                  float duration = 0.0f, float friction = 0.0f);
    void clearTouch() { touch_.clear(); }
    float getTouchPressure(size_t index) const;
    float getTouchIntensity(size_t index) const;
    
    // Advanced internal signals with state tracking
    const std::vector<std::pair<size_t, float>>& getInternal() const { return internal_; }
    std::vector<std::pair<size_t, float>>& getInternal() { return internal_; }
    void setInternal(const std::vector<std::pair<size_t, float>>& internal) { internal_ = internal; }
    void addInternalSignal(size_t index, float value, float timestamp = 0.0f,
                          float decayRate = 0.1f, bool isModulated = false);
    void clearInternal() { internal_.clear(); }
    float getInternalValue(size_t index) const;
    float getInternalTimestamp(size_t index) const;
    float getInternalDecayRate(size_t index) const;
    
    // Enhanced proprioception with velocity and acceleration
    const std::vector<std::tuple<float, float, float, float, float, float>>& getProprioception() const { return proprioception_; }
    std::vector<std::tuple<float, float, float, float, float, float>>& getProprioception() { return proprioception_; }
    void setProprioception(const std::vector<std::tuple<float, float, float, float, float, float>>& proprioception) { proprioception_ = proprioception; }
    void setProprioception(float x, float y, float orientation, float velocityX, float velocityY, float angularVelocity);
    void setProprioception(float x, float y, float orientation, 
                           float velocityX, float velocityY, float angularVelocity,
                           float accelerationX, float accelerationY, float angularAcceleration);
    void updateProprioception(float deltaX, float deltaY, float deltaOrientation,
                             float deltaVelocityX, float deltaVelocityY, float deltaAngularVelocity);
    void clearProprioception() { proprioception_.clear(); }
    
    // Audio with spectral analysis support
    const Audio& getAudio() const { return audio_; }
    Audio& getAudio() { return audio_; }
    void setAudio(const Audio& audio) { audio_ = audio; }
    void setAudio(size_t sampleRate, const std::vector<float>& samples);
    void setAudio(size_t sampleRate, const std::vector<float>& samples,
                  const std::vector<float>& spectrum);
    void addAudioSample(float sample, float spectrumValue = 0.0f);
    void clearAudio() { audio_.setData(std::vector<float>()); }
    float getAudioEnergy(size_t start = 0, size_t end = 0) const;
    float getAudioPitch(size_t start = 0, size_t end = 0) const;
    
    // Auditory scene analysis
    struct SoundSource {
        float x, y, z;           // Position
        float level;             // Sound level (dB)
        float frequency;         // Dominant frequency
        float directionality;    // Sound directionality
        float arrivalTime;       // When sound was detected
    };
    std::vector<SoundSource> getSoundSources() const { return soundSources_; }
    void addSoundSource(float x, float y, float z, float level, float frequency);
    void clearSoundSources() { soundSources_.clear(); }
    
    // All signals concatenated for neural input (enhanced)
    std::vector<float> getAllSignals() const;
    std::vector<std::pair<SensoryModality, float>> getAllSignalsWithModality() const;
    size_t getTotalSignalCount() const;
    
    // Enhanced temporal processing
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double timestamp) { timestamp_ = timestamp; }
    void advanceTimestamp(double dt) { timestamp_ += dt; }
    
    // Reset for new episode with modality-specific cleanup
    void reset();
    void resetModality(SensoryModality modality);
    
    // Comparison and copying
    bool operator==(const SensoryPercept& other) const;
    bool operator!=(const SensoryPercept& other) const { return !(*this == other); }
    
    // Enhanced serialization
    void saveToFile(const std::string& filepath, bool compress = false) const;
    void loadFromFile(const std::string& filepath);
    std::string serialize() const;
    void deserialize(const std::string& data);
    
    // Statistics and diagnostics
    std::string summary() const;
    float getVisionEntropy() const;
    float getTouchEntropy() const;
    float getInternalEntropy() const;
    float getAudioComplexity() const;
    float getTemporalCoherence() const;
    
    // Signal processing utilities
    void applyFilter(const std::vector<float>& signal, const std::vector<float>& kernel);
    void normalizeSignal(SensoryModality modality, float targetMean = 0.0f, float targetStd = 1.0f);
    void applyGain(SensoryModality modality, float gainFactor);
    void applyNoiseReduction(SensoryModality modality, float threshold);
    
    // Neural preparation utilities
    std::vector<float> prepareForNeuralInput(SensoryModality modality) const;
    void applyNeuralPreprocessing(const std::vector<std::string>& preprocessingSteps);
    
    // Statistics and diagnostics for neural processing
    float getSignalRange(SensoryModality modality) const;
    float getSignalMean(SensoryModality modality) const;
    float getSignalStdDev(SensoryModality modality) const;
    float getSignalSkew(SensoryModality modality) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Legacy fields for backward compatibility
    Vision vision_;
    std::vector<float> touch_legacy_;
    std::vector<float> internal_legacy_;
    std::vector<float> proprioception_legacy_;
    Audio audio_;
    double timestamp_;
    
    // Enhanced fields
    std::vector<std::pair<size_t, float>> touch_;
    std::vector<std::pair<size_t, float>> internal_;
    std::vector<std::tuple<float, float, float, float, float, float>> proprioception_;
    std::vector<SoundSource> soundSources_;
};

} // namespace nlm
