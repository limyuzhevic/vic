// NeuromodulatorManager.h
#pragma once

#include "AgentBrain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <memory>

namespace nlm {

/**
 * NeuromodulatorManager handles all neuromodulator state and calculations
 * Consolidates duplicate logic and provides unified interface
 */
class NeuromodulatorManager {
public:
    NeuromodulatorManager(std::shared_ptr<Brain> brain, AgentBrain* parent);
    ~ NeuromodulatorManager();
    
    // Initialize manager
    void initialize(const SimpleWorld& world);
    
    // Update all neuromodulators based on current state
    void update(double timestep);
    
    // Apply reward-based neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    
    // Get current neuromodulator levels
    float getDopamineLevel() const { return dopamineLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Get integrated plasticity factor considering all neuromodulators
    float getIntegratedPlasticityFactor() const;
    
    // Get curiosity-based exploration modifier
    float getCuriosityExplorationModifier() const;
    
    // Reset all neuromodulator state
    void reset();
    
    // Update developmental stage effects on neuromodulators
    void updateDevelopmentEffects(double timestep);
    
    // Set developmental stage based on age
    void setDevelopmentalStageFromAge(double age);
    
private:
    // Helper methods for neuromodulator calculations
    void calculateNovelty(const std::vector<float>& vision);
    void calculateCuriosity();
    void updateDopamine(float predictionError);
    float calculateDopamineFromPredictionError(float error) const;
    float calculatePlasticityFromNeuromodulators() const;
    void applyStructuralPlasticityEffects();
    
    // State variables
    std::shared_ptr<Brain> brain_;
    AgentBrain* parent_; // For accessing AgentBrain methods
    
    // Neuromodulator levels
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development and plasticity
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    float sensoryNoveltyDecay_;
    
    // Previous state for novelty detection
    std::vector<float> previousVision_;
    
    // Cached values for performance
    mutable float cachedPlasticityFactor_;
    mutable bool plasticityCacheValid_;
};
