#pragma once

// BrainCognitionIntegration.hpp - Neural planner, concept formation, attention integration
// Contains cognition system integration functionality

#include "BrainCore.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class BrainCore;

// Brain Cognition Integration Implementation
class BrainCognitionIntegration {
public:
    struct Impl;
    Impl* pImpl;
    
    // Create brain cognition integration with reference to core brain
    explicit BrainCognitionIntegration(BrainCore* core);
    
    ~BrainCognitionIntegration();
    
    // Disable copying, enable moving
    BrainCognitionIntegration(const BrainCognitionIntegration&) = delete;
    BrainCognitionIntegration& operator=(const BrainCognitionIntegration&) = delete;
    BrainCognitionIntegration(BrainCognitionIntegration&&) noexcept;
    BrainCognitionIntegration& operator=(BrainCognitionIntegration&&) noexcept;
    
    // Initialize cognition systems
    bool initialize();
    
    // Update cognition systems
    void update(TimestepDuration dt);
    
    // Process competition between neural traces
    void processCompetition(const std::vector<NeuronId>& competitors);
    
    // Get cognition system references
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Clear cognition state
    void clear();
    
    // Get statistics
    size_t getPlanningIterations() const;
    size_t getConceptFormations() const;
    size_t getAttentionSelections() const;
    
private:
    // Internal helper methods
    void updatePlanner();
    void updateConceptFormation();
    void updateAttention();
    void updatePlanningMetrics();
    
    // Utility methods
    void calculateCognitionStats() const;
    
    struct Impl {
        BrainCore* brainCore;
        
        // Cognition system references
        NeuralPlanner* planner;
        ConceptFormation* conceptFormation;
        AttentionalSelection* attention;
        
        // Cognition state
        std::vector<NeuronId> activePlan;
        std::vector<uint32_t> competitionHistory;
        
        // Statistics
        size_t planningIterations;
        size_t conceptFormations;
        size_t attentionSelections;
        
        Impl(BrainCore* core);
    };
};

} // namespace nlm

