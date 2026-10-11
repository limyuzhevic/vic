#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Neuromodulation update
    void update(TimestepDuration dt);
    
    // Dopamine effects on neural excitability
    void updateDopamine(TimestepDuration dt);
    float getDopamineLevel() const { return dopamineLevel; }
    float getPlasticityFactor() const { return plasticityFactor; }
    
    // Curiosity and novelty updates
    void updateCuriosity(TimestepDuration dt);
    void updateNovelty(TimestepDuration dt);
    
    // Neuromodulation factor calculation
    void calculateNeuromodulationFactor();
    
    // Global neuromodulation effects on neurons
    void applyGlobalNeuromodulation(float factor);
    
    // Integration with neuromodulation systems
    void setDopamine(Dopamine* dopamine) { this->dopamine = dopamine; }
    void setCuriosity(Curiosity* curiosity) { this->curiosity = curiosity; }
    void setNovelty(Novelty* novelty) { this->novelty = novelty; }
    void setPredictionError(PredictionError* predictionError) { 
        this->predictionError = predictionError; 
    }
    
    // Statistics
    float getCurrentDopamineLevel() const { return currentDopamineLevel; }
    float getCurrentCuriosityLevel() const { return currentCuriosityLevel; }
    float getCurrentNoveltyLevel() const { return currentNoveltyLevel; }
    
private:
    // Neuromodulator instances
    Dopamine* dopamine;
    Curiosity* curiosity;
    Novelty* novelty;
    PredictionError* predictionError;
    
    // Current neuromodulation levels
    float dopamineLevel;
    float currentDopamineLevel;
    float currentCuriosityLevel;
    float currentNoveltyLevel;
    
    // Neuromodulation state
    float plasticityFactor;
};

} // namespace nlm
