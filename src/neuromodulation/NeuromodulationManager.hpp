#pragma once

#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
#include "Reward.hpp"
#include "PredictionError.hpp"
#include "Novelty.hpp"
#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Neuromodulation manager orchestrates all neuromodulatory systems
// Integrates reward, prediction error, novelty, and curiosity signals
// for coordinated learning and exploration
class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Initialize all neuromodulators
    void initialize(class Brain* brain);
    
    // Update all neuromodulators with current brain state
    void update(TimestepDuration dt);
    
    // Get current neuromodulation state
    float getTotalModulation() const;
    float getExplorationDrive() const;
    
    // Process environmental signals
    void processObservation(const Observation& observation,
                           const Observation& previousObservation,
                           float predictedValue);
    
    // Get neuromodulator instances (for direct access)
    Dopamine* getDopamine() const { return dopamine_.get(); }
    Reward* getReward() const { return reward_.get(); }
    PredictionError* getPredictionError() const { return predictionError_.get(); }
    Novelty* getNovelty() const { return novelty_.get(); }
    Curiosity* getCuriosity() const { return curiosity_.get(); }
    
    // Reset all neuromodulators
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
