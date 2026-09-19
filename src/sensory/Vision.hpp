#pragma once

#include "SensoryInput.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Vision : public SensoryInput {
public:
    Vision();
    Vision(size_t width, size_t height, size_t channels = 3);
    
    NeuronType getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    Timestamp getTimestamp() const override;
    void setTimestamp(Timestamp timestamp) override;
    void reset() override;
    
    size_t getWidth() const;
    size_t getHeight() const;
    size_t getChannels() const;
    
    void setData(const std::vector<float>& data);
    
private:
    std::vector<float> data_;
    size_t width_;
    size_t height_;
    size_t channels_;
    Timestamp timestamp_;
};

} // namespace nlm