#pragma once

#include "SensoryInput.hpp"
#include <vector>

namespace nlm {

// Internal signals processor
// PLACEHOLDER - Phase 2 will process signals from within the brain

class InternalSignals : public SensoryInput {
public:
    InternalSignals();
    ~InternalSignals() override;
    
    // Get input type
    const char* getType() const override;
    
    // Get raw data as vector
    const std::vector<float>& getData() const override;
    
    // Get dimensionality
    size_t getDimensions() const override;
    
    // Get timestamp
    double getTimestamp() const;
    void setTimestamp(double timestamp);
    
    // Clone
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Add signal
    void addSignal(float value);
    void clearSignals();
    
    // Get signal count
    size_t getSignalCount() const;
    
    // Get signal at index
    float getSignal(size_t index) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    double timestamp_;
};

} // namespace nlm
