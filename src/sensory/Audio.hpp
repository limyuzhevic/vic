#pragma once

#include "SensoryInput.hpp"
#include "AudioProcessor.hpp"
#include <vector>

namespace nlm {

// Audio processing - biologically inspired frequency analysis and feature extraction using spike-based neural computation
class AudioProcessor;

class Audio : public SensoryInput {
public:
    Audio();
    explicit Audio(size_t numSamples);
    ~Audio() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Set audio data
    void setData(const std::vector<float>& data);
    void setData(float* data, size_t size);
    
    // Audio properties
    size_t getNumSamples() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm