#pragma once

#include "../brain/Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"

namespace nlm {

// BrainCognitionManager manages cognition systems
class BrainCognitionManager {
public:
    BrainCognitionManager() = default;
    ~BrainCognitionManager() = default;
    
    // Initialize cognition systems
    void initialize(Brain* brain);
    
    // Update cognition systems
    void update(Brain* brain, TimestepDuration timestep);
    
    // Process competition in attention system
    void processAttentionCompetition(Brain* brain);
    
    // Get cognition statistics
    void collectCognitionStats(Brain* brain) const;
};

} // namespace nlm
