#pragma once

#include "BrainMemory.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {
    class Brain;
    
    // Memory systems for information storage and retrieval
    class Memory {
    public:
        Memory();
        ~Memory();
        
        // Initialize memory systems
        void initializeMemory(Brain* brain);
        
        // Update working memory
        void updateWorkingMemory(Timestamp timestep);
        
        // Update episodic memory
        void updateEpisodicMemory(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
        
        // Consolidate memories
        void consolidateMemories(float threshold);
        
        // Store episodic episode
        void storeEpisodicEpisode(Brain* brain, const EpisodicMemoryItem& episode);
        
        // Get working memory instance
        NeuralWorkingMemory* getWorkingMemory();
        
        // Get episodic memory instance
        NeuralEpisodicMemory* getEpisodicMemory();
        
        // Get associative memory instance
        NeuralAssociativeMemory* getAssociativeMemory();
        
        // Get prediction system instance
        PredictionSystem* getPredictionSystem();
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}
