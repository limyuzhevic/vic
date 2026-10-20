#pragma once

#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "ISubsystem.hpp"
#include <memory>

namespace nlm {

class NeuromodulationSystem : public ISubsystem {
public:
    NeuromodulationSystem();
    ~NeuromodulationSystem() override;
    
    // ISubsystem interface
    bool initialize(Brain* brain) override;
    void update(float dt) override;
    void reset() override;
    void logStatus() const override;
    
    // Neuromodulator access
    Dopamine* getDopamine() const;
    Curiosity* getCuriosity() const;
    PredictionError* getPredictionError() const;
    Novelty* getNovelty() const;
    
    // Neuromodulation coordination
    void updateAllNeuromodulators(float dt);
    void applyNeuromodulationToBrain();
    void updateRewardPrediction();
    void updateNoveltyDetection();
    
    // Neuromodulator statistics
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorMagnitude() const;
    
    // Neuromodulation effects
    void applyDopamineEffectsToExcitability();
    void applyDopamineEffectsToPlasticity();
    void applyCuriosityEffectsToExploration();
    void applyNoveltyEffectsToLearning();
    
    // Reward and learning
    void recordReward(float reward);
    void recordOutcome(float outcome);
    void calculatePredictionError(float predicted, float actual);
    void updateValueFunction(float predictionError);
    
    // State management
    void resetNeuromodulationState();
    void synchronizeNeuromodulationWithEnvironment();
    
    // Configuration
    void setLearningRate(float rate);
    void setExplorationRate(float rate);
    void setRewardSensitivity(float sensitivity);
    void setPredictionErrorThreshold(float threshold);
    
    // Statistics
    size_t getRewardCount() const;
    size_t getPredictionErrorCount() const;
    size_t getNoveltyEvents() const;
    float getAveragePredictionError() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
