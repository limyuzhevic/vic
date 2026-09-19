#pragma once

#include "Neuromodulator.hpp"
#include "ACh.hpp"
#include "NE.hpp"
#include "5HT.hpp"
#include "Dopamine.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class Brain;

// NeuromodulationManager: Coordinates all neuromodulatory systems
// Integrates dopamine, acetylcholine, norepinephrine, and serotonin systems
// Manages cross-talk between neuromodulators and coordinates their effects
// on neural plasticity and behavior

class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Initialize with brain reference for system coordination
    void initialize(Brain* brain);
    
    // Update all neuromodulators and compute integrated effects
    void updateAll(TimestepDuration dt);
    
    // Signal processing from brain systems
    void processReward(float reward);
    void processPredictionError(float error);
    void processNovelty(float novelty);
    void processAttention(float salience, float focus);
    void processSocialFeedback(float feedback);
    void processImpulsivity(float level);
    void processStress(float stressLevel);
    
    // Get neuromodulator levels for monitoring
    float getAChLevel() const;
    float getNELevel() const;
    float get5HTLevel() const;
    float getDALevel() const;
    
    // Get integrated plasticity modulation factor
    float getIntegratedPlasticityFactor() const;
    
    // Get attention modulation factor from ACh
    float getAttentionModulationFactor() const;
    
    // Get arousal modulation factor from NE
    float getArousalModulationFactor() const;
    
    // Get mood modulation factor from 5-HT
    float getMoodModulationFactor() const;
    
    // Reset all neuromodulators to baseline
    void reset();
    
    // Configure neuromodulator parameters
    void configureAChParameters(float baseline, float decayRate, float releaseRate);
    void configureNEParameters(float baseline, float decayRate, float releaseRate);
    void configure5HTParameters(float baseline, float decayRate, float releaseRate);
    void configureDAParameters(float baseline, float decayRate, float releaseRate);
    
    // Enable/disable neuromodulator systems
    void enableACh(bool enable);
    void enableNE(bool enable);
    void enable5HT(bool enable);
    void enableDA(bool enable);
    
    // Get neuromodulator instance pointers
    ACh* getACh() const;
    NE* getNE() const;
    Serotonin* get5HT() const;
    Dopamine* getDA() const;
    
    // Get neuromodulator name for logging
    const char* getAChName() const;
    const char* getNEName() const;
    const char* get5HTName() const;
    const char* getDAName() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm