#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Forward declaration
class Metrics;

// Internal signals processor
// PLACEHOLDER - Phase 2 will process signals from within the brain

class InternalSignalsProcessor {
public:
    InternalSignalsProcessor();
    ~InternalSignalsProcessor();
    
    // Process internal signals with metrics
    void process(const InternalSignals& signals, std::shared_ptr<Metrics> metrics = nullptr);
    
    // Get processed representation
    const std::vector<float>& getProcessedSignals() const;
    
    // Get homeostasis signals
    const std::vector<float>& getHomeostaticSignals() const;
    
    // Get metrics collector
    const std::shared_ptr<Metrics>& getMetrics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
