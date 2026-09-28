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
    
    // Set entire data vector
    virtual void setData(const std::vector<float>& data) = 0;
    
    // Set data at index (used by Brain.cpp)
    virtual void setDataAt(size_t index, float value) = 0;
    
    // Get data by index
    virtual float getDataAt(size_t index) const = 0;
    
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
    
    // Set/get data at index (used by prediction system)
    void setDataAt(size_t index, float value);
    float getDataAt(size_t index) const;
    
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
    
    // Set/get data at index (used by prediction system)
    void setDataAt(size_t index, float value);
    float getDataAt(size_t index) const;
    
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
    
    // Set/get data at index (used by prediction system)
    void setDataAt(size_t index, float value);
    float getDataAt(size_t index) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
