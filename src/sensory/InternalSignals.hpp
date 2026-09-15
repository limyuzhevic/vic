#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Real internal signals processing using NLM's neural machinery
// Implements biologically plausible homeostatic regulation:
// - Hunger, thirst, fatigue processing
// - Predictive coding of bodily states
// - Neuromodulatory integration (ACh, Serotonin, Noradrenaline)
// - Feedback loops for homeostatic regulation
// - Interoception and exteroception integration

class InternalSignalsProcessor;

// Internal signals representing physiological states
// Similar to real biological signals but represented in NLM's spiking neural framework
class InternalSignals : public SensoryInput {
public:
    InternalSignals();
    ~InternalSignals() override;
    
    // Get signal type identifier
    const char* getType() const override;
    
    // Get raw signal data
    const std::vector<float>& getData() const override;
    
    // Get signal dimensionality
    size_t getDimensions() const override;
    
    // Clone this signal
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Add a specific physiological signal
    void addSignal(float value);
    
    // Clear all signals
    void clearSignals();
    
    // Get all homeostatic system signals
    const std::vector<float>& getHomeostaticSignals() const;
    
    // Get hunger levels
    const std::vector<float>& getHungerLevels() const;
    
    // Get thirst levels
    const std::vector<float>& getThirstLevels() const;
    
    // Get fatigue levels
    const std::vector<float>& getFatigueLevels() const;
    
    // Get metabolic rates
    const std::vector<float>& getMetabolicRates() const;
    
    // Get stress levels
    const std::vector<float>& getStressLevels() const;
    
    // Get prediction errors
    const std::vector<float>& getPredictionErrors() const;
    
    // Set homeostatic setpoint
    void setHomeostaticSetpoint(float setpoint);
    
    // Update interoceptive noise level
    void updateInteroceptiveNoise(float noise);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

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
    
    // Get hunger levels
    const std::vector<float>& getHungerLevels() const;
    
    // Get thirst levels
    const std::vector<float>& getThirstLevels() const;
    
    // Get fatigue levels
    const std::vector<float>& getFatigueLevels() const;
    
    // Get metabolic rates
    const std::vector<float>& getMetabolicRates() const;
    
    // Get stress levels
    const std::vector<float>& getStressLevels() const;
    
    // Get prediction errors
    const std::vector<float>& getPredictionErrors() const;
    
    // Update setpoint
    void updateSetpoint(float newSetpoint);
    
    // Update noise level
    void updateNoiseLevel(float noise);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
