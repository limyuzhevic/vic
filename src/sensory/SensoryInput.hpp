#pragma once

#include <vector>
#include <string>
#include <memory>

namespace nlm {

// Vision processing (placeholder)
// PLACEHOLDER - Phase 2 will implement real vision processing using NLM's own neurons

class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Process vision input
    void process(const Vision& input);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Vision processing methods
    void applyFilter(const std::vector<float>& kernel);
    void normalize(float targetMean = 0.0f, float targetStdDev = 1.0f);
    void scale(float factor);
    void resize(size_t newWidth, size_t newHeight);
    std::vector<float> getChannelData(size_t channel) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Audio processing (placeholder)
// PLACEHOLDER - Phase 2 will implement real audio processing using NLM's own neurons

class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();
    
    // Process audio input
    void process(const Audio& input);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Audio processing methods
    void applyGain(float gainDb);
    void setSampleRate(size_t rate);
    float getDuration() const;
    std::vector<float> getFrequency() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Base class for all sensory input
class SensoryInput {
public:
    SensoryInput();
    virtual ~SensoryInput() = default;
    
    // Disable copying, enable moving
    SensoryInput(const SensoryInput&) = delete;
    SensoryInput& operator=(const SensoryInput&) = delete;
    SensoryInput(SensoryInput&&) noexcept = default;
    SensoryInput& operator=(SensoryInput&&) noexcept = default;
    
    // Get input type
    virtual const char* getType() const = 0;
    
    // Get raw data as vector
    virtual const std::vector<float>& getData() const = 0;
    
    // Get dimensionality
    virtual size_t getDimensions() const = 0;
    
    // Get timestamp
    double getTimestamp() const;
    void setTimestamp(double timestamp);
    
    // Clone
    virtual std::unique_ptr<SensoryInput> clone() const = 0;
    
    // Error handling
    virtual bool hasError() const { return false; }
    virtual std::string getErrorMessage() const { return ""; }
    
protected:
    double timestamp_;
    mutable std::string error_message_;
    bool has_error_;
};

// Vision input placeholder
class Vision : public SensoryInput {
public:
    Vision();
    explicit Vision(size_t width, size_t height, size_t channels = 3);
    ~Vision() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Set image data
    void setData(const std::vector<float>& data);
    void setData(float* data, size_t size);
    
    // Image properties
    size_t getWidth() const;
    size_t getHeight() const;
    size_t getChannels() const;
    
    // Vision-specific methods
    void applyFilter(const std::vector<float>& kernel);
    void normalize(float targetMean = 0.0f, float targetStdDev = 1.0f);
    void scale(float factor);
    void resize(size_t newWidth, size_t newHeight);
    std::vector<float> getChannelData(size_t channel) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Audio input placeholder
class Audio : public SensoryInput {
public:
    Audio();
    explicit Audio(size_t sampleRate, size_t numSamples);
    ~Audio() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Set audio data
    void setData(const std::vector<float>& data);
    void setSampleRate(size_t rate);
    
    size_t getSampleRate() const;
    size_t getNumSamples() const;
    
    // Audio-specific methods
    void applyGain(float gainDb);
    float getDuration() const;
    std::vector<float> getFrequency() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Internal signals (来自大脑内部状态)
class InternalSignals : public SensoryInput {
public:
    InternalSignals();
    ~InternalSignals() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Add signal
    void addSignal(float value);
    void clearSignals();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
