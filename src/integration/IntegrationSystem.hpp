// Comprehensive Integration Framework for NLM Phase 6
// Connects memory, prediction, neuromodulation, and development systems

#pragma once

#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../agent/AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <chrono>

namespace nlm {

// Integration events that can affect multiple systems
struct IntegrationEvent {
    enum class Type {
        PredictionError,
        MemoryUpdate,
        NeuromodulationChange,
        DevelopmentStageChange,
        RewardReceived,
        NoveltyDetected,
        ActionExecuted,
        WorldInteraction,
        MemoryReplay,
        Consolidation
    };
    
    Type type;
    float timestamp;
    SimulationStep step;
    std::unordered_map<std::string, float> parameters;
    std::string metadata;
};

// Integration observer interface
class IntegrationObserver {
public:
    virtual ~IntegrationObserver() = default;
    virtual void onIntegrationEvent(const IntegrationEvent& event) = 0;
};

// Central integration system for Phase 6
class IntegrationSystem {
public:
    IntegrationSystem(std::shared_ptr<Brain> brain,
                     std::shared_ptr<AgentBrain> agent,
                     std::shared_ptr<WorkingMemory> workingMem,
                     std::shared_ptr<EpisodicMemory> episodicMem,
                     std::shared_ptr<SemanticMemory> semanticMem,
                     std::shared_ptr<ProceduralMemory> proceduralMem,
                     std::shared_ptr<AssociativeMemory> associativeMem,
                     std::shared_ptr<PredictionSystem> predictionSys,
                     std::shared_ptr<Neuromodulator> neuromodulator,
                     std::shared_ptr<DevelopmentSystem> developmentSys);
    
    ~IntegrationSystem();
    
    // Main integration loop
    void integrateStep(SimulationStep step, TimestepDuration dt);
    
    // Connect systems to each other
    void establishSystemConnections();
    
    // Memory-Prediction integration
    void integrateMemoryPrediction();
    void onPredictionError(float error, float confidence);
    void onMemoryUpdate(const std::string& memoryType, const std::string& operation);
    
    // Neuromodulation-Memory integration
    void integrateNeuromodulationMemory();
    void onNeuromodulationChange(const Neuromodulator::State& state);
    void enhanceMemoryConsolidation(float dopamineLevel, float noveltyLevel);
    
    // Development-System integration
    void integrateDevelopmentSystem();
    void onDevelopmentStageChange(DevelopmentalStage stage);
    void adaptMemoryForDevelopment();
    
    // Agent-Brain-World integration
    void integrateAgentWorld();
    void onActionExecuted(const MotorCommand& action);
    void onWorldInteraction(const WorldInteraction& interaction);
    
    // Persistence and replay
    void integratePersistence();
    void saveIntegrationState(const std::string& filename);
    void loadIntegrationState(const std::string& filename);
    void triggerMemoryReplay();
    
    // Event handling
    void registerObserver(IntegrationObserver* observer);
    void unregisterObserver(IntegrationObserver* observer);
    void notifyObservers(const IntegrationEvent& event);
    
    // Configuration
    void configureMemoryPredictionIntegration(bool enable);
    void configureNeuromodulationMemoryIntegration(bool enable);
    void configureReplayMechanism(bool enable);
    void configurePersistence(bool enable);
    
    // Statistics and monitoring
    struct IntegrationStats {
        size_t predictionMemoryIntegrations;
        size_t neuromodulationMemoryIntegrations;
        size_t memoryReplayEvents;
        size_t consolidationEvents;
        float averagePredictionError;
        float memoryRetentionRate;
        size_t activeConnections;
        double totalIntegrationTime;
    };
    
    IntegrationStats getStats() const { return stats_; }
    
    // Validation
    bool validateIntegration() const;
    std::string getIntegrationReport() const;
    
    // Phase 6 specific features
    void enablePhase6Features();
    bool arePhase6FeaturesEnabled() const { return phase6Enabled_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    IntegrationStats stats_;
    bool phase6Enabled_;
    bool memoryPredictionEnabled_;
    bool neuromodulationMemoryEnabled_;
    bool replayEnabled_;
    bool persistenceEnabled_;
    
    std::vector<IntegrationObserver*> observers_;
    std::chrono::steady_clock::time_point lastStepTime_;
};

// Factory function for creating integration system
std::unique_ptr<IntegrationSystem> createIntegrationSystem(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<AgentBrain> agent,
    std::shared_ptr<WorkingMemory> workingMem,
    std::shared_ptr<EpisodicMemory> episodicMem,
    std::shared_ptr<SemanticMemory> semanticMem,
    std::shared_ptr<ProceduralMemory> proceduralMem,
    std::shared_ptr<AssociativeMemory> associativeMem,
    std::shared_ptr<PredictionSystem> predictionSys,
    std::shared_ptr<Neuromodulator> neuromodulator,
    std::shared_ptr<DevelopmentSystem> developmentSys);

// Memory-Prediction integration utilities
namespace MemoryPredictionIntegration {
    // Create prediction-based memory updates
    void predictAndUpdateMemory(const SensoryInput& currentState,
                               std::shared_ptr<PredictionSystem> prediction,
                               std::shared_ptr<EpisodicMemory> episodic);
    
    // Use prediction error to enhance working memory
    void updateWorkingMemoryWithPredictionError(std::shared_ptr<WorkingMemory> workingMem,
                                               float predictionError,
                                               float confidence);
    
    // Generate memory updates based on prediction confidence
    void enhanceMemoryBasedOnPrediction(std::shared_ptr<SemanticMemory> semantic,
                                       std::shared_ptr<AssociativeMemory> associative,
                                       float predictionError,
                                       float confidence);
}

// Neuromodulation-Memory integration utilities  
namespace NeuromodulationMemoryIntegration {
    // Enhance memory consolidation based on neuromodulator states
    void consolidateMemoryWithNeuromodulation(std::shared_ptr<EpisodicMemory> episodic,
                                             std::shared_ptr<WorkingMemory> workingMem,
                                             const Neuromodulator::State& neuromodState);
    
    // Use dopamine to strengthen important memories
    void dopamineEnhancedMemoryStrengthening(std::shared_ptr<EpisodicMemory> episodic,
                                            float dopamineLevel,
                                            float predictionError);
    
    // Use novelty to focus memory encoding
    void noveltyDrivenMemoryEncoding(std::shared_ptr<WorkingMemory> workingMem,
                                    float noveltyLevel);
}

// Development-System integration utilities
namespace DevelopmentSystemIntegration {
    // Adapt memory systems for developmental stage
    void adaptMemorySystemsForDevelopment(std::shared_ptr<WorkingMemory> workingMem,
                                         std::shared_ptr<EpisodicMemory> episodic,
                                         DevelopmentalStage stage);
    
    // Integrate developmental plasticity with memory consolidation
    void integrateDevelopmentPlasticity(std::shared_ptr<AssociativeMemory> associative,
                                        float plasticityModifier,
                                        DevelopmentalStage stage);
}

// Persistence utilities
namespace PersistenceIntegration {
    // Save integration state
    void saveIntegrationState(const IntegrationSystem& integration, const std::string& filename);
    
    // Load integration state  
    void loadIntegrationState(IntegrationSystem& integration, const std::string& filename);
    
    // Export memory state for persistence
    void exportMemoryState(const IntegrationSystem& integration, const std::string& filename);
    
    // Import memory state
    void importMemoryState(IntegrationSystem& integration, const std::string& filename);
}

} // namespace nlm
