#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Cognition Systems Integration
// Handles neural planner, concept formation, and attentional selection integration

class BrainCognition {
public:
    // Initialize cognition systems
    void initializeCognitionSystems(Brain::Impl& pImpl);
    
    // Update cognition systems
    void updateCognitionSystems(Brain::Impl& pImpl, double timestep);
    
    // Get cognition system statistics
    void logCognitionStats(const Brain::Impl& pImpl) const;
    
private:
    // Helper functions for cognition integration
    void integrateCognitionWithMemory(Brain::Impl& pImpl);
    void updateAttentionSystem(Brain::Impl& pImpl);
    void updateConceptFormation(Brain::Impl& pImpl);
    void updateNeuralPlanner(Brain::Impl& pImpl);
};

} // namespace nlm
