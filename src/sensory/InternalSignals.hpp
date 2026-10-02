#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Advanced internal signals processing using NLM's neural machinery
// Implements homeostatic regulation, autonomic balance, and endocrine integration
// with the brain's spiking neural network for physiological state monitoring

class InternalSignalsProcessor {
public:
    InternalSignalsProcessor();
    ~InternalSignalsProcessor();
    
    // Configure internal signals processing parameters
    void configure(float homeostaticHomeostasis = 1.0f,
                   float autonomicAdaptation = 0.5f,
                   float endocrineSecretion = 0.3f,
                   float stressResponse = 0.0f,
                   float temporalSmoothing = 0.7f);
    
    // Process internal signals through neural machinery
    // Optional brain parameter allows direct neural integration
    void process(const InternalSignals& signals, class Brain* brain = nullptr);
    
    // Get processed signal representation
    const std::vector<float>& getProcessedSignals() const;
    
    // Get homeostatic signals (derived from internal state)
    const std::vector<float>& getHomeostaticSignals() const;
    
    // Get signal vector with fixed size output for API compatibility
    void getSignalVector(float* output, size_t maxSize) const;
    
    // Get signal dimensionality
    size_t getSignalDimensions() const;
    
    // Get current processing parameters
    float getHomeostaticHomeostasis() const;
    float getAutonomicAdaptation() const;
    float getEndocrineSecretion() const;
    float getStressResponse() const;
    
    // Reset processor state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm