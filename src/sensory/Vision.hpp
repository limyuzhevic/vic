#pragma once

#include "SensoryInput.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Helper functions for vision processing
inline std::vector<float> convertInput(const SensoryInput& input) {
    return input.getData();
}

inline std::unique_ptr<SensoryInput> createInput(const std::vector<float>& data) {
    auto vision = std::make_unique<Vision>(1, data.size(), 1);
    vision->setData(data);
    return vision;
}

class Vision : public SensoryInput {
public:
    Vision();
    Vision(size_t width, size_t height, size_t channels);
    ~Vision() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    void setData(const std::vector<float>& data);
    void setData(float* data, size_t size);
    
    size_t getWidth() const;
    size_t getHeight() const;
    size_t getChannels() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
