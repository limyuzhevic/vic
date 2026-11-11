#pragma once

#include "SensoryInput.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

// Internal signals processor for brain homeostasis
// REAL IMPLEMENTATION - Phase 2 real internal signal processing
class InternalSignalsProcessor {
public:
    InternalSignalsProcessor();
    ~InternalSignalsProcessor();
    
    // Process internal signals through neural network
    void process(const InternalSignals& signals);
    
    // Get processed representation
    const std::vector<float>& getProcessedSignals() const;
    
    // Get homeostasis signals
    const std::vector<float>& getHomeostaticSignals() const;
    
    // Get homeostatic state
    const std::vector<float>& getHomeostaticState() const;
    
    // Check if processor is ready
    bool isReady() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
