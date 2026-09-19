#pragma once

#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/NewNovelty.hpp"
#include "../core/Logger/Logger.hpp"
#include "Brain.hpp"

namespace nlm {

class BrainNeuromod {
public:
    // Constructor
    BrainNeuromod(std::shared_ptr<Config> config);
    ~BrainNeuromod();
    
    // Initialize neuromodulation systems
    void initialize(Brain* brain);
    
    // Update neuromodulation systems
    void update(TimestepDuration timestep);
    
    // Apply neuromodulatory effects on neural excitability
    void applyToNeuralExcitability(Brain* brain);
    
    // Get neuromodulation system accessors
    Dopamine* getDopamine() { return dopamine.get(); }
    const Dopamine* getDopamine() const { return dopamine.get(); }
    
    Curiosity* getCuriosity() { return curiosity.get(); }
    const Curiosity* getCuriosity() const { return curiosity.get(); }
    
    PredictionError* getPredictionError() { return predictionError.get(); }
    const PredictionError* getPredictionError() const { return predictionError.get(); }
    
    Novelty* getNovelty() { return novelty.get(); }
    const Novelty* getNovelty() const { return novelty.get(); }
    
    // Get neuromodulation levels
    float getDopamineLevel() const { return dopamineLevel; }
    float getCuriosityLevel() const { return curiosityLevel; }
    float getNoveltyLevel() const { return noveltyLevel; }
    
    // Set neuromodulation levels
    void setDopamineLevel(float level) { dopamineLevel = level; }
    void setCuriosityLevel(float level) { curiosityLevel = level; }
    void setNoveltyLevel(float level) { noveltyLevel = level; }
    
    // Get neuromodulation factors for plasticity
    float getDopaminePlasticityFactor() const { return dopaminePlasticityFactor; }
    float getCuriosityModulationFactor() const { return curiosityModulationFactor; }
    
    // Update neuromodulation factors
    void updateModulationFactors();
    
    // Reset neuromodulation state
    void reset();
    
    // Log neuromodulation system status
    void logStatus() const;
    
private:
    // Neuromodulation system components
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Configuration
    std::shared_ptr<Config> config;
    
    // Neuromodulation levels
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    
    // Neuromodulation factors
    float dopaminePlasticityFactor;
    float curiosityModulationFactor;
    
    // Helper methods
    void initializeFromConfig();
    void applyDopamineEffects(Brain* brain);
    void applyCuriosityEffects();
    void applyNoveltyEffects();
    void applyPredictionErrorEffects();
};

} // namespace nlm