#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Neuromodulation Systems Integration
// Handles dopamine, curiosity, novelty, and prediction error integration

class BrainNeuromodulation {
public:
    // Initialize neuromodulation systems
    void initializeNeuromodulationSystems(Brain::Impl& pImpl);
    
    // Update neuromodulation systems
    void updateNeuromodulationSystems(Brain::Impl& pImpl, double timestep);
    
    // Get neuromodulation system statistics
    void logNeuromodulationStats(const Brain::Impl& pImpl) const;
    
private:
    // Helper functions for neuromodulation integration
    void applyNeuromodulationToNeurons(Brain::Impl& pImpl);
    void updateDopamineEffects(Brain::Impl& pImpl);
    void updateCuriosityEffects(Brain::Impl& pImpl);
    void updateNoveltyEffects(Brain::Impl& pImpl);
    void updatePredictionErrorEffects(Brain::Impl& pImpl);
};

} // namespace nlm
