// NeuromodulationSystem - Handles dopamine, curiosity, novelty, and prediction error
// Extracted from Brain::step() method
#pragma once

#include "../brain/Brain.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <memory>

namespace nlm {

class NeuromodulationSystem {
public:
    NeuromodulationSystem(std::shared_ptr<Brain> brain);
    ~NeuromodulationSystem();
    
    // Initialize the neuromodulation system
    bool initialize();
    
    // Process one timestep of neuromodulation operations
    void step(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep);
    
    // Get neuromodulation system pointers
    Dopamine* getDopamine() const { return dopamine_.get(); }
    Curiosity* getCuriosity() const { return curiosity_.get(); }
    Novelty* getNovelty() const { return novelty_.get(); }
    PredictionError* getPredictionError() const { return predictionError_.get(); }
    
    // Apply reward-based neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<PredictionError> predictionError_;
    std::unique_ptr<Novelty> novelty_;
    
    // State tracking
    float dopamineLevel_;
    
    // Helper methods
    void updateDopamineSystem(TimestepDuration timestep);
    void updateCuriositySystem(TimestepDuration timestep);
    void updateNoveltySystem(TimestepDuration timestep);
    void updatePredictionErrorSystem(TimestepDuration timestep);
    void applyNeuromodulationToNeurons();
};

} // namespace nlm
