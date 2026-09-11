#pragma once

#include "Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include <vector>

namespace nlm {

class NeuromodulationSystem {
public:
    NeuromodulationSystem() = default;
    
    void initialize(Brain* brain);
    
    // Dopamine system operations
    void updateDopamine(Brain& brain, TimestepDuration dt);
    void applyDopamineEffects(Brain& brain, float level);
    float getDopamineLevel(Brain& brain) const;
    
    // Curiosity system operations
    void updateCuriosity(Brain& brain, TimestepDuration dt);
    void applyCuriosityEffects(Brain& brain, float level);
    float getCuriosityLevel(Brain& brain) const;
    
    // Prediction error system operations
    void updatePredictionError(Brain& brain, const class SensoryInput& input);
    void applyPredictionErrorEffects(Brain& brain, float error);
    float getPredictionErrorLevel(Brain& brain) const;
    
    // Novelty detection
    void updateNovelty(Brain& brain, TimestepDuration dt);
    void applyNoveltyEffects(Brain& brain, float novelty);
    float getNoveltyLevel(Brain& brain) const;
    
    // Plasticity modulation
    void modulatePlasticity(Brain& brain, float factor);
    float getPlasticityModulationFactor(Brain& brain) const;
    
    // Neuromodulator signal processing
    void applyNeuromodulator(Brain& brain, const Neuromodulator& signal);
    std::vector<Neuromodulator> getActiveSignals(Brain& brain) const;
    
    void logNeuromodulationStatus(Brain& brain) const;
    
private:
    // Dopamine modulation of neural excitability
    void modulateNeuralExcitability(Brain& brain, float dopamineLevel);
    
    // Curiosity-based exploration bias
    void applyExplorationBias(Brain& brain, float curiosityLevel);
    
    // Prediction error-guided learning
    void updateLearningRates(Brain& brain, float predictionError);
    
    // Novelty-based attention
    void modulateAttentionForNovelty(Brain& brain, float noveltyLevel);
    
    // Integration of multiple neuromodulators
    void integrateNeuromodulators(Brain& brain);
    
    // Cross-system modulation
    void crossModulateSystems(Brain& brain);
    
    // Reward prediction and computation
    float computeRewardPrediction(Brain& brain) const;
    
    // Surprise and uncertainty estimation
    float estimateSurprise(Brain& brain) const;
};

} // namespace nlm
