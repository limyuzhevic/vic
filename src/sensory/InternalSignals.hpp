#pragma once

#include "SensoryInput.hpp"
#include "../brain/Brain.hpp"
#include "core/Types/Types.hpp"

namespace nlm {

// Internal signals processor
// PLACEHOLDER - Phase 2 will process signals from within the brain

class InternalSignalsProcessor {
public:
    InternalSignalsProcessor();
    ~InternalSignalsProcessor();
    
    // Process internal signals
    void process(const InternalSignals& signals);
    
    // Get processed representation
    const std::vector<float>& getProcessedSignals() const;
    
    // Get homeostasis signals
    const std::vector<float>& getHomeostaticSignals() const;
    
    // Set brain reference for neural integration
    void setBrain(Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
