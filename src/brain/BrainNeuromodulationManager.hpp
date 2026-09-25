#pragma once

#include "../brain/Brain.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"

namespace nlm {

// BrainNeuromodulationManager manages neuromodulation systems
class BrainNeuromodulationManager {
public:
    BrainNeuromodulationManager() = default;
    ~BrainNeuromodulationManager() = default;
    
    // Initialize neuromodulation systems
    void initialize(Brain* brain);
    
    // Update neuromodulation systems
    void update(Brain* brain, TimestepDuration timestep);
    
    // Apply neuromodulation effects on neurons
    void applyNeuromodulationEffects(Brain* brain);
    
    // Update curiosity based on novelty
    void updateCuriosity(Brain* brain);
    
    // Apply dopamine effects on plasticity
    void applyDopaminePlasticity(Brain* brain);
    
    // Get neuromodulation statistics
    void collectNeuromodulationStats(Brain* brain) const;
};

} // namespace nlm
