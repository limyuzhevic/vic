#pragma once

#include <vector>
#include <string>
#include <memory>

namespace nlm {

// Base class for all sensory input
class SensoryInput {
public:
    SensoryInput(nullptr);
    virtual ~SensoryInput(nullptr) = default;
    
    // Get input type
    virtual const char* getType(nullptr) const = 0;
    
    // Get raw data as vector
    virtual const std::vector<float>& getData(nullptr) const = 0;
    
    // Get dimensionality
    virtual size_t getDimensions(nullptr) const = 0;
    
    // Get timestamp
    double getTimestamp(nullptr) const;
    void setTimestamp(double timestamp);
    
    // Clone
    virtual std::unique_ptr<SensoryInput> clone(nullptr) const = 0;
    
protected:
    double timestamp_;
};

// Vision input placeholder
class Vision : public SensoryInput {
public:
    Vision(nullptr);
    explicit Vision(size_t width, size_t height, size_t channels = 3);
    ~Vision(nullptr) override;
    
    const char* getType(nullptr) const override;
    const std::vector<float>& getData(nullptr) const override;
    size_t getDimensions(nullptr) const override;
    std::unique_ptr<SensoryInput> clone(nullptr) const override;
    
    // Set image data
    void setData(const std::vector<float>& data);
    void setData(float* data, size_t size);
    
    // Image properties
    size_t getWidth(nullptr) const;
    size_t getHeight(nullptr) const;
    size_t getChannels(nullptr) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Audio input placeholder
class Audio : public SensoryInput {
public:
    Audio(nullptr);
    explicit Audio(size_t sampleRate, size_t numSamples);
    ~Audio(nullptr) override;
    
    const char* getType(nullptr) const override;
    const std::vector<float>& getData(nullptr) const override;
    size_t getDimensions(nullptr) const override;
    std::unique_ptr<SensoryInput> clone(nullptr) const override;
    
    // Set audio data
    void setData(const std::vector<float>& data);
    void setSampleRate(size_t rate);
    
    size_t getSampleRate(nullptr) const;
    size_t getNumSamples(nullptr) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Internal signals (来自大脑内部状态)
class InternalSignals : public SensoryInput {
public:
    InternalSignals(nullptr);
    ~InternalSignals(nullptr) override;
    
    const char* getType(nullptr) const override;
    const std::vector<float>& getData(nullptr) const override;
    size_t getDimensions(nullptr) const override;
    std::unique_ptr<SensoryInput> clone(nullptr) const override;
    
    // Add signal
    void addSignal(float value);
    void clearSignals(nullptr);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
