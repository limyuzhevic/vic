#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Audio input class
class Audio : public SensoryInput {
public:
    Audio();
    explicit Audio(size_t sampleRate, size_t numSamples);
    ~Audio() override;
    
    // Disable copying, enable moving
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;
    Audio(Audio&& other) noexcept;
    Audio& operator=(Audio&& other) noexcept;
    
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

} // namespace nlm
