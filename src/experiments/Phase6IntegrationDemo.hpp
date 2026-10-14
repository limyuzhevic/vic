# NLM Phase 6: Complete Integration Brain Demo
// 
// This demo implements the actual integration that Phase 6 was supposed to achieve:
// - All cognitive systems are instantiated and connected
// - Memory systems work as part of the brain loop
// - Prediction integrates with action selection
// - Neuromodulation coordinates learning and memory
// - Development affects all brain functions
// - Checkpoint and replay work properly
//

#pragma once

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "development/DevelopmentSystem.hpp"

#include <string>
#include <memory>

using namespace nlm;

/**
 * Advanced demonstration of Phase 6 integration
 * This shows the brain loop that actually works, not the disconnected stubs
 */
class Phase6IntegrationDemo {
public:
    Phase6IntegrationDemo();
    ~Phase6IntegrationDemo();
    
    /**
     * Run the complete integration demonstration
     */
    void run();
    
private:
    /**
     * Configure settings for complete integration
     */
    void configureForIntegration(std::shared_ptr<Config> config);
    
    /**
     * Run the main brain loop with comprehensive monitoring
     */
    void runBrainLoop(size_t maxSteps);
    
    /**
     * Update working memory with current experience
     */
    void updateMemorySystems(size_t step, const MotorCommand& action, float reward, const SensoryPercept& percept);
    
    /**
     * Update prediction system
     */
    void updatePredictionSystem(size_t step, const MotorCommand& action, float reward);
    
    /**
     * Apply neuromodulation signals
     */
    void applyNeuromodulation(size_t step, float reward);
    
    /**
     * Test complete system integration
     */
    void testSystemIntegration();
    
    /**
     * Test persistence (checkpoint/save-load)
     */
    void testPersistence();
    
    /**
     * Test replay system
     */
    void testReplaySystem();
    
    /**
     * Analyze final state
     */
    void analyzeFinalState();
    
    /**
     * Convert developmental stage to string
     */
    std::string getDevelopmentalStageString(DevelopmentalStage stage);
    
    // Member variables
    Logger* logger;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
};