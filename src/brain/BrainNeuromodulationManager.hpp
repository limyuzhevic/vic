#pragma once

#include "Brain.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <memory>
#include <vector>

namespace nlm {

class BrainNeuromodulationManager {
public:
    BrainNeuromodulationManager();
    ~BrainNeuromodulationManager();
    
    // Disable copying
    BrainNeuromodulationManager(const BrainNeuromodulationManager&) = delete;
    BrainNeuromodulationManager& operator=(const BrainNeuromodulationManager&) = delete;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update all neuromodulators
    void updateNeuromodulators(Brain* brain, TimestepDuration timestep);
    
    // Apply neuromodulation to neural excitability
    void applyNeuromodulationToNeurons(Brain* brain, float neuromodulationLevel);
    
    // Apply neuromodulation to plasticity
    void applyNeuromodulationToPlasticity(Brain* brain, float neuromodulationLevel);
    
    // Get neuromodulation system pointers
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
