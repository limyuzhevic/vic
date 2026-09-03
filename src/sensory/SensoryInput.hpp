#pragma once

#include <vector>
#include <string>
#include <memory>

namespace nlm {

// Base class for all sensory input
class SensoryInput {
public:
    SensoryInput();
    virtual ~SensoryInput() = default;
    
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
    
protected:
    double timestamp_;
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
