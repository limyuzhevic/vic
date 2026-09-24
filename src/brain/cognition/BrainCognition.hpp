#pragma once

#include "BrainCognition.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {
    class Brain;
    
    // Cognition systems for thought and decision processes
    class Cognition {
    public:
        Cognition();
        ~Cognition();
        
        // Initialize cognition systems
        void initializeCognition(Brain* brain);
        
        // Update attention system
        void updateAttention(Brain* brain, Timestamp timestep);
        
        // Update concept formation
        void updateConceptFormation(Brain* brain);
        
        // Process attention competition
        void processAttentionCompetition(const std::vector<NeuronId>& neurons);
        
        // Plan actions
        void planActions(Brain* brain);
        
        // Get cognitive components
        NeuralPlanner* getPlanner();
        ConceptFormation* getConceptFormation();
        AttentionalSelection* getAttention();
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}
