#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Proprioception input - body position/velocity signals
class Proprioception : public SensoryInput {
public:
    Proprioception();
    ~Proprioception() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Set proprioception data
    void setData(const std::vector<float>& data);
    
    // Proprioception properties
    size_t getNumJointSignals() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
