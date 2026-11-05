#pragma once

#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include <memory>
#include <vector>

namespace nlm {

class Brain;

class BrainNeuromodulation {
public:
    BrainNeuromodulation() = default;
    ~BrainNeuromodulation() = default;
    
    // Configuration and setup
    void configureFromConfig(const Config* config);
    void setNeuromodulationParameters(float baseDopamine, float curiosityGain, float noveltyThreshold);
    
    // Neuromodulation updates (called during step)
    void updateAllNeuromodulation(Brain* brain, TimestepDuration timestep);
    void updateDopamine(Brain* brain, TimestepDuration timestep);
    void updateCuriosity(Brain* brain, float noveltyLevel, float predictionError, TimestepDuration timestep);
    void updateNovelty(Brain* brain, const SensoryInput& sensoryInput, TimestepDuration timestep);
    void updatePredictionError(Brain* brain, const SensoryInput& sensoryInput, TimestepDuration timestep);
    
    // Neuromodulation access and control
    Dopamine* getDopamine() { return dopamine_.get(); }
    Curiosity* getCuriosity() { return curiosity_.get(); }
    Novelty* getNovelty() { return novelty_.get(); }
    PredictionError* getPredictionError() { return predictionError_.get(); }
    
    // Neuromodulation statistics and diagnostics
    void collectNeuromodulationStats(Brain* brain);
    float getDopamineLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionErrorLevel() const { return predictionErrorLevel_; }
    float getNeuromodulationInfluence() const { return neuromodulationInfluence_; }
    
    // Neuromodulation system management
    void resetNeuromodulation(Brain* brain);
    void applyExternalStimuli(Brain* brain, float reward, float curiosityDrive);
    
    // Neuromodulation-specific processing
    void processRewardPrediction(Brain* brain, float reward, float predictedReward);
    void processExplorationBias(Brain* brain, float curiosity, float novelty);
    void processSensorySurprise(Brain* brain, const SensoryInput& input);
    
private:
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
    
    // Neuromodulation statistics
    float dopamineLevel_ = 0.0f;
    float curiosityLevel_ = 0.0f;
    float noveltyLevel_ = 0.0f;
    float predictionErrorLevel_ = 0.0f;
    float neuromodulationInfluence_ = 1.0f;
    
    // Configuration parameters
    float baseDopamine_ = 0.5f;
    float curiosityGain_ = 1.0f;
    float noveltyThreshold_ = 0.1f;
    float predictionErrorWeight_ = 0.5f;
    
    // Helper methods
    void updateDopamineSignal(Brain* brain, TimestepDuration timestep);
    void updateCuriositySignal(Brain* brain, float novelty, float predictionError, TimestepDuration timestep);
    void updateNoveltySignal(Brain* brain, const SensoryInput& input, TimestepDuration timestep);
    void updatePredictionErrorSignal(Brain* brain, const SensoryInput& input, TimestepDuration timestep);
    void applyNeuromodulationEffects(Brain* brain, TimestepDuration timestep);
    void validateNeuromodulationState(Brain* brain) const;
    
    friend class BrainCore; // For internal access
};

} // namespace nlm