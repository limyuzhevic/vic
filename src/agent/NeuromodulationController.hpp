#pragma once

#include "../brain/Brain.hpp"
#include "../plasticity/STDP.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>
#include <vector>

namespace nlm {

class NeuromodulationController {
public:
    NeuromodulationController(std::shared_ptr<Brain> brain);
    ~NeuromodulationController() = default;
    
    // Apply reward modulation
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get neuromodulation level (dopamine)
    float getNeuromodulationLevel() const { return dopamineLevel_; }
    
    // Get prediction error
    float getPredictionError() const { return predictionError_; }
    
    // Get expected reward
    float getExpectedReward() const { return expectedReward_; }
    
    // Get developmental age
    double getDevelopmentalAge() const { return developmentalAge_; }
    
    // Get plasticity modifier
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Configuration
    void enable(bool enable) { enabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    bool isEnabled() const { return enabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    // Update curiosity level (for backward compatibility)
    void updateCuriosityLevel(float level) { curiosityLevel_ = level; }
    
    // Reset controller state
    void reset();
    
private:
    // Helper functions for plasticity calculations
    float calculatePlasticityFactor(float dopamineLevel);
    float calculateSynaptogenesisRate(float plasticityModifier);
    float calculatePruningRate(float plasticityModifier);
    
    std::shared_ptr<Brain> brain_;
    
    // State
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool enabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
};

} // namespace nlm