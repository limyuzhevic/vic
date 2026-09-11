#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Touch input - proximity collision signals
class Touch : public SensoryInput {
public:
    Touch();
    ~Touch() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Set touch data
    void setData(const std::vector<float>& data);
    
    // Touch properties
    size_t getNumSensors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
