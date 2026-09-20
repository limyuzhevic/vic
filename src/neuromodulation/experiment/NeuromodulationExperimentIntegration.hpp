// Neuromodulation system experiment integration
// Moved from Phase4Experiment.cpp - Reward, Curiosity, PredictionError, Novelty systems

#pragma once

#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Neuromodulation Integration Test
// Tests if reward, curiosity, prediction error, and novelty systems work together
class NeuromodulationExperimentIntegration {
public:
    NeuromodulationExperimentIntegration();
    ~NeuromodulationExperimentIntegration();
    
    // Run complete neuromodulation experiment
    void runNeuromodulationTest(Brain* brain, size_t numTrials = 100);
    
    // Test individual neuromodulation systems
    void testRewardSystem(Brain* brain);
    void testCuriositySystem(Brain* brain);
    void testPredictionErrorSystem(Brain* brain);
    void testNoveltySystem(Brain* brain);
    
    // Get neuromodulation metrics
    float getRewardModulationLevel() const { return rewardLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getPredictionErrorLevel() const { return predictionErrorLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    
private:
    void injectNeuromodulatorySignals(Brain* brain, size_t trial);
    void measureNeuromodulationEffects(Brain* brain);
    void verifySystemInteractions(Brain* brain);
    
    // Neuromodulation system pointers
    std::shared_ptr<Reward> reward_;
    std::shared_ptr<Curiosity> curiosity_;
    std::shared_ptr<PredictionError> predictionError_;
    std::shared_ptr<Novelty> novelty_;
    
    // Metrics
    float rewardLevel_ = 0.0f;
    float curiosityLevel_ = 0.0f;
    float predictionErrorLevel_ = 0.0f;
    float noveltyLevel_ = 0.0f;
    std::vector<float> neuromodulationHistory_;
};

} // namespace nlm
