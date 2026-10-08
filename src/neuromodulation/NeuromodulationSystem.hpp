#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// NeuromodulationSystem configuration
struct NeuromodulationConfig {
    // Global settings
    bool enabled;
    float globalDecayRate;
    float baselineLevel;
    
    // Individual neuromodulator settings
    struct DopamineConfig {
        float baseline;
        float phasicStrength;
        float adaptationRate;
        float rewardSensitivity;
        float predictionErrorPrecision;
    } dopamine;
    
    struct AcetylcholineConfig {
        float baseline;
        float cholinergicTone;
        float attentionGain;
        float memoryConsolidationRate;
    } acetylcholine;
    
    struct NorepinephrineConfig {
        float baseline;
        float arousalLevel;
        float vigilance;
        float locusCoeruleusGain;
    } norepinephrine;
    
    struct SerotoninConfig {
        float baseline;
        float moodModulation;
        float impulsivity;
        float behavioralInhibition;
        float socialBehaviorModulation;
    } serotonin;
    
    NeuromodulationConfig() : 
        enabled(true),
        globalDecayRate(0.05f),
        baselineLevel(0.1f) {
        // Initialize with enhanced default values
        dopamine.baseline = 0.05f;
        dopamine.phasicStrength = 1.0f;
        dopamine.adaptationRate = 0.1f;
        dopamine.rewardSensitivity = 1.0f;
        dopamine.predictionErrorPrecision = 0.8f;
        
        acetylcholine.baseline = 0.05f;
        acetylcholine.cholinergicTone = 0.5f;
        acetylcholine.attentionGain = 1.5f;
        acetylcholine.memoryConsolidationRate = 1.2f;
        
        norepinephrine.baseline = 0.03f;
        norepinephrine.arousalLevel = 0.5f;
        norepinephrine.vigilance = 0.8f;
        norepinephrine.locusCoeruleusGain = 1.3f;
        
        serotonin.baseline = 0.04f;
        serotonin.moodModulation = 0.6f;
        serotonin.impulsivity = 0.4f;
        serotonin.behavioralInhibition = 0.7f;
        serotonin.socialBehaviorModulation = 0.8f;
    }
};

// Neuromodulation summary for reporting
struct NeuromodulationSummary {
    // Individual neuromodulator levels
    float dopamineLevel;
    float acetylcholineLevel;
    float norepinephrineLevel;
    float serotoninLevel;
    
    // External signals
    float predictionErrorLevel;
    float rewardLevel;
    
    // Overall system metrics
    float totalModulation;
    float neuromodulatorActivity;
    std::vector<std::string> activeNeuromodulators;
};

// Comprehensive neuromodulation system for coordinating all neuromodulators
class NeuromodulationSystem {
public:
    NeuromodulationSystem();
    ~NeuromodulationSystem();
    
    // Disable copying
    NeuromodulationSystem(const NeuromodulationSystem&) = delete;
    NeuromodulationSystem& operator=(const NeuromodulationSystem&) = delete;
    
    // Initialize with configuration
    void initialize(const NeuromodulationConfig& config);
    
    // Update neuromodulators
    void update(TimestepDuration dt, float predictionError, float reward);
    
    // Signal events
    void signalReward(float reward);
    void signalPredictionError(float error);
    
    // Get current levels
    float getDopamineLevel() const;
    float getAcetylcholineLevel() const;
    float getNorepinephrineLevel() const;
    float getSerotoninLevel() const;
    float getPredictionErrorLevel() const;
    float getRewardLevel() const;
    
    // Get overall system metrics
    float getTotalModulation() const;
    float getNeuromodulatorActivity() const;
    std::vector<std::string> getActiveNeuromodulators() const;
    
    // Configuration access
    NeuromodulationConfig& getConfig();
    const NeuromodulationConfig& getConfig() const;
    
    // Enable/disable neuromodulation
    void enableNeuromodulation(bool enabled);
    bool isNeuromodulationEnabled() const;
    
    // Get summary
    NeuromodulationSummary getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void storeSignalHistory(const std::string& name, float value);
    void calculateModulationLevels();
};

} // namespace nlm