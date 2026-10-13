#pragma once

#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "BrainInitializer.hpp"
#include <memory>

namespace nlm {

class Brain;
class Neuron;

class BrainNeuromodulatorIntegrator {
public:
    explicit BrainNeuromodulatorIntegrator(std::shared_ptr<Config> config);
    ~BrainNeuromodulatorIntegrator() = default;
    
    // Initialize neuromodulation systems
    void initializeNovelty(Brain* brain);
    void initializeCuriosity(Brain* brain);
    void initializeDopamine(Brain* brain);
    void initializePredictionError(Brain* brain);
    
    // Update neuromodulation systems
    void updateNovelty(Brain* brain, TimestepDuration dt);
    void updateCuriosity(Brain* brain, TimestepDuration dt);
    void updateDopamine(Brain* brain, TimestepDuration dt);
    void updatePredictionError(Brain* brain, SimulationStep step, Timestamp time);
    
    // Neuromodulation effects
    void applyDopamineEffects(Brain* brain, float dopamineLevel);
    void applyCuriosityEffects(Brain* brain, float curiosityLevel);
    void applyNoveltyEffects(Brain* brain, float noveltyLevel);
    void applyPredictionErrorEffects(Brain* brain, float predictionError);
    
    // Neuromodulator access
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }
    
    // Configuration
    void configureDopamine(Brain* brain, float scale, float baseline);
    void configureCuriosity(Brain* brain, float explorationRate, bool curiosityDriven);
    void configureNovelty(Brain* brain, float sensitivity, float threshold);
    
    // Plasticity modulation
    float calculatePlasticityModulation(Brain* brain);
    void applyModulatedPlasticity(Brain* brain, float plasticityMod);
    
    // State management
    void resetNeuromodulation(Brain* brain);
    void resetDopamine(Brain* brain);
    void resetCuriosity(Brain* brain);
    
    // Advanced neuromodulation features
    void enableEmotionSystem(Brain* brain, bool enabled);
    void enableAffectiveLearning(Brain* brain, bool enabled);
    void enableSocialAffection(Brain* brain, bool enabled);

private:
    // Neuromodulator instances
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Configuration parameters
    bool emotionSystemEnabled;
    bool affectiveLearningEnabled;
    bool socialAffectionEnabled;
    float dopamineScale;
    float dopamineBaseline;
    float curiosityExplorationRate;
    bool curiosityDriven;
    float noveltySensitivity;
    float noveltyThreshold;
    
    // Helper methods
    void setupNeuromodulationConnections(Brain* brain);
    void integrateWithPlasticitySystems(Brain* brain);
    void integrateWithMemorySystems(Brain* brain);
    void integrateWithDevelopment(Brain* brain);
    
    // Effect application
    void applyDopamineToNeuralExcitability(Brain* brain, float dopamineLevel);
    void applyDopamineToPlasticity(Brain* brain, float dopamineLevel);
    void applyCuriosityToExploration(Brain* brain, float curiosityLevel);
    void applyNoveltyToLearning(Brain* brain, float noveltyLevel);
    void applyPredictionErrorToLearning(Brain* brain, float predictionError);
    
    // Advanced features
    void enableEmotionSystem(Brain* brain);
    void enableAffectiveLearning(Brain* brain);
    void enableSocialAffection(Brain* brain);
    
    // Integration helper methods
    void updateBrainStateFromNeuromodulation(Brain* brain);
    void applyNeuromodulationToDevelopment(Brain* brain);
    void applyNeuromodulationToCognition(Brain* brain);
};