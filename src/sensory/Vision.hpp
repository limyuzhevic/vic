#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Vision input class
class Vision : public SensoryInput {
public:
    Vision();
    explicit Vision(size_t width, size_t height, size_t channels = 3);
    ~Vision() override;
    
    // Disable copying, enable moving
    Vision(const Vision&) = delete;
    Vision& operator=(const Vision&) = delete;
    Vision(Vision&& other) noexcept;
    Vision& operator=(Vision&& other) noexcept;
    
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

} // namespace nlm
