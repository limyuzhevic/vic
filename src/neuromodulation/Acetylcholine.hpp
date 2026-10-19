#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Enhanced with biological accuracy

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Attention modulation
    void modulateAttention(float attentionStrength);
    float getAttentionModulation() const;
    
    // Memory consolidation
    void enhanceMemoryConsolidation(float memoryStrength);
    float getMemoryEnhancement() const;
    
    // Theta rhythm coupling
    void setThetaFrequency(float frequency);
    float getThetaFrequency() const;
    void phaseWithTheta(float phaseOffset);
    
    // Learning rate modulation
    float getLearningRateModulation() const;
    
    // Sensory processing enhancement
    float enhanceSensoryProcessing(float inputStrength) const;
    
    // Neuromodulator interaction methods
    NeuromodulatorInteraction getInteractionWith(const Neuromodulator& other) const;
    float getSynergyMultiplier(const Neuromodulator& other) const;
    float getAntagonismMultiplier(const Neuromodulator& other) const;
    
    // State tracking
    float getHistoryAverage(unsigned int window) const;
    float getPeakLevel() const;
    float getTimeAboveThreshold(float threshold) const;
    
    // Biological range management
    bool isWithinBiologicalRange(float level) const;
    float getMinLevel() const;
    float getMaxLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm