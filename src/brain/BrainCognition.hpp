#pragma once

#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include "Brain.hpp"

namespace nlm {

class BrainCognition {
public:
    // Constructor
    BrainCognition(std::shared_ptr<Config> config);
    ~BrainCognition();
    
    // Initialize cognition systems
    void initialize(Brain* brain);
    
    // Update cognition systems
    void update(TimestepDuration timestep);
    
    // Get cognition system accessors
    NeuralPlanner* getPlanner() { return planner.get(); }
    const NeuralPlanner* getPlanner() const { return planner.get(); }
    
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    const ConceptFormation* getConceptFormation() const { return conceptFormation.get(); }
    
    AttentionalSelection* getAttention() { return attention.get(); }
    const AttentionalSelection* getAttention() const { return attention.get(); }
    
    // Apply attention to working memory
    void applyAttentionToWorkingMemory(Brain* brain);
    
    // Process competition among memory traces
    void processMemoryCompetition(Brain* brain);
    
    // Reset cognition state
    void reset();
    
    // Log cognition system status
    void logStatus() const;
    
private:
    // Cognition system components
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // Configuration
    std::shared_ptr<Config> config;
    
    // Helper methods
    void initializeFromConfig();
    void updateConceptualLearning();
    void updateAttentionalSelection();
};

} // namespace nlm