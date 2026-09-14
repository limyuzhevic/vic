#pragma once

#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>
#include <algorithm>

namespace nlm {

// NeuromodulatorManager: Coordinates all neuromodulators and their interactions
// Manages cross-talk, integration, and coordinated modulation of brain activity
class NeuromodulatorManager {
public:
    NeuromodulatorManager();
    ~NeuromodulatorManager();
    
    void initialize(Brain* brain);
    void update(TimestepDuration dt);
    
    // Get neuromodulators
    Dopamine* getDopamine();
    Acetylcholine* getAcetylcholine();
    Norepinephrine* getNorepinephrine();
    Serotonin* getSerotonin();
    Reward* getReward();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Cross-talk mechanisms
    void propagateRewardPrediction(Dopamine* dopamine);
    void enhanceAttentionThroughACh(Acetylcholine* ach, Norepinephrine* ne);
    void regulateMoodSerotonin(Acetylcholine* ach, Serotonin* serotonin);
    void maintainArousalBalance(Norepinephrine* ne, Dopamine* dopamine);
    void coordinateSensoryProcessing(Acetylcholine* ach, Norepinephrine* ne);
    
    // Integrated neuromodulation
    void applyNeuromodulationToBrain();
    void synchronizeNeuromodulatorStates();
    void computeNeuromodulatorInteractions();
    
    // Special neuromodulator interactions
    void processRewardPredictionError(Dopamine* dopamine, Reward* reward, PredictionError* predError);
    void integrateAttentionAndReward(Acetylcholine* ach, Dopamine* dopamine, Reward* reward);
    void balanceArousalAndAttention(Norepinephrine* ne, Acetylcholine* ach);
    void stabilizeMoodAndImpulsivity(Acetylcholine* ach, Serotonin* serotonin);
    
    // Neuromodulator state management
    void resetNeuromodulators();
    void adaptNeuromodulatorParameters(float environmentComplexity);
    void updateNeuromodulatorSynergies();
    
    // Statistics and monitoring
    float getOverallNeuromodulationLevel() const;
    float getNeuromodulatorCoherence() const;
    float getNeuromodulatorBalance() const;
    
    // Neuromodulator interaction matrix (how different neuromodulators affect each other)
    struct NeuromodulatorInteraction {
        float ach_effect_on_da;        // Acetylcholine effect on dopamine
        float ach_effect_on_ne;        // Acetylcholine effect on norepinephrine
        float ach_effect_on_5ht;       // Acetylcholine effect on serotonin
        float ne_effect_on_da;         // Norepinephrine effect on dopamine
        float ne_effect_on_ach;        // Norepinephrine effect on acetylcholine
        float ne_effect_on_5ht;        // Norepinephrine effect on serotonin
        float da_effect_on_ach;        // Dopamine effect on acetylcholine
        float da_effect_on_ne;         // Dopamine effect on norepinephrine
        float da_effect_on_5ht;        // Dopamine effect on serotonin
        float 5ht_effect_on_da;        // Serotonin effect on dopamine
        float 5ht_effect_on_ach;       // Serotonin effect on acetylcholine
        float 5ht_effect_on_ne;        // Serotonin effect on norepinephrine
        
        NeuromodulatorInteraction() : ach_effect_on_da(1.0f), ach_effect_on_ne(1.0f), ach_effect_on_5ht(1.0f)
                                , ne_effect_on_da(1.0f), ne_effect_on_ach(1.0f), ne_effect_on_5ht(1.0f)
                                , da_effect_on_ach(1.0f), da_effect_on_ne(1.0f), da_effect_on_5ht(1.0f)
                                , 5ht_effect_on_da(1.0f), 5ht_effect_on_ach(1.0f), 5ht_effect_on_ne(1.0f) {}
    };
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm