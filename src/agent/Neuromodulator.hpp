#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Config/Config.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;

// Neuromodulator: Handles reward-based neuromodulation
// Applies dopamine and other neuromodulatory signals based on prediction error
class Neuromodulator {
public:
    Neuromodulator(std::shared_ptr<Config> config);
    ~Neuromodulator();
    
    // Configure with brain reference
    void initialize(std::shared_ptr<Brain> brain);
    
    // Apply neuromodulation based on reward and predicted reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update neuromodulation state with current step
    void update(double timestep);
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const { return dopamineLevel_; }
    
    // Get prediction error
    float getPredictionError() const { return predictionError_; }
    
    // Get expected reward (moving average)
    float getExpectedReward() const { return expectedReward_; }
    
    // Reset neuromodulation state
    void reset();
    
    // Enable/disable reward modulation
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
private:
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Brain reference
    std::shared_ptr<Brain> brain_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Apply reward-modulated weight changes to synapses
    void applyRewardModulatedPlasticity(float plasticityMod, const std::vector<Neuron*>& neurons);
};

} // namespace nlm
