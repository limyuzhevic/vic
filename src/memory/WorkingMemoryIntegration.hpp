#pragma once

#include "EnhancedWorkingMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Integration system for coordinating working memory with other cognitive systems
class WorkingMemoryIntegration {
public:
    WorkingMemoryIntegration();
    ~WorkingMemoryIntegration();
    
    // Initialize the integration system
    void initialize(Brain* brain,
                    NeuralEpisodicMemory* episodicMemory,
                    NeuralAssociativeMemory* associativeMemory,
                    PredictionSystem* predictionSystem,
                    AttentionalSelection* attentionSystem,
                    DevelopmentSystem* developmentSystem,
                    Dopamine* dopamine,
                    Curiosity* curiosity,
                    Novelty* novelty,
                    PredictionError* predictionError);
    
    // Synchronize working memory with episodic memory
    void synchronizeWithEpisodicMemory(EnhancedWorkingMemory& workingMemory,
                                       const std::vector<float>& currentPattern);
    
    // Synchronize working memory with associative memory
    void synchronizeWithAssociativeMemory(EnhancedWorkingMemory& workingMemory,
                                          const std::vector<float>& currentPattern);
    
    // Integrate working memory with prediction system
    void integrateWithPredictionSystem(EnhancedWorkingMemory& workingMemory,
                                       const std::vector<float>& predictedPattern,
                                       float predictionError);
    
    // Coordinate with attentional selection
    void coordinateWithAttention(EnhancedWorkingMemory& workingMemory,
                                const std::vector<NeuronId>& attendedNeurons);
    
    // Integrate with neuromodulation systems
    void integrateNeuromodulation(EnhancedWorkingMemory& workingMemory,
                                   const class Neuromodulator& signal);
    
    // Coordinate with development system
    void coordinateWithDevelopment(EnhancedWorkingMemory& workingMemory,
                                   DevelopmentalStage developmentalStage);
    
    // Process feedback from prediction error
    void processPredictionError(EnhancedWorkingMemory& workingMemory,
                                float errorSignal,
                                const std::vector<float>& expectedPattern,
                                const std::vector<float>& actualPattern);
    
    // Process curiosity signals
    void processCuriosity(EnhancedWorkingMemory& workingMemory,
                          float curiosityLevel,
                          const std::vector<float>& novelPattern);
    
    // Process novelty signals
    void processNovelty(EnhancedWorkingMemory& workingMemory,
                        float noveltySignal,
                        const std::vector<float>& surprisingPattern);
    
    // Get integration statistics
    struct IntegrationStats {
        size_t episodicMemorySyncs;
        size_t associativeMemorySyncs;
        size_t predictionIntegrations;
        size_t attentionalCoordinations;
        size_t neuromodulationIntegrations;
        size_t developmentCoordinations;
        size_t predictionErrorProcessings;
        size_t curiosityProcessings;
        size_t noveltyProcessings;
        float averageIntegrationTime;
        std::chrono::nanoseconds totalSyncTime;
    };
    
    IntegrationStats getStats() const { return stats_; }
    
    // Reset statistics
    void resetStats() {
        stats_ = IntegrationStats();
    }
    
    // Check if integration is complete (all systems connected)
    bool isInitialized() const {
        return brain_ && episodicMemory_ && associativeMemory_ &&
               predictionSystem_ && attentionSystem_ && developmentSystem_ &&
               dopamine_ && curiosity_ && novelty_ && predictionError_;
    }
    
    // Get the brain instance
    Brain* getBrain() const { return brain_; }
    
    // Set the brain instance
    void setBrain(Brain* brain) { brain_ = brain; }

private:
    // Core systems
    Brain* brain_;
    NeuralEpisodicMemory* episodicMemory_;
    NeuralAssociativeMemory* associativeMemory_;
    PredictionSystem* predictionSystem_;
    AttentionalSelection* attentionSystem_;
    DevelopmentSystem* developmentSystem_;
    Dopamine* dopamine_;
    Curiosity* curiosity_;
    Novelty* novelty_;
    PredictionError* predictionError_;
    
    // Integration statistics
    IntegrationStats stats_;
    
    // Configuration
    bool enableEpisodicSync_;
    bool enableAssociativeSync_;
    bool enablePredictionIntegration_;
    bool enableAttentionCoordination_;
    bool enableNeuromodulationIntegration_;
    bool enableDevelopmentCoordination_;
    bool enableLearningDrivenIntegration_;
    
    // Timing and thresholds
    size_t syncInterval_;
    float predictionErrorThreshold_;
    float curiosityThreshold_;
    float noveltyThreshold_;
    float attentionGainThreshold_;
    float neuromodulationThreshold_;
    
    // Pattern matching utilities
    std::vector<float> normalizePattern(const std::vector<float>& pattern) const;
    float computePatternDistance(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;
    
    // Signal processing utilities
    float applyAttentionFilter(float signal, const std::vector<NeuronId>& attendedNeurons) const;
    float applyNeuromodulation(float signal, const class Neuromodulator& modulator) const;
    
    // Synchronization helpers
    void broadcastToEpisodicMemory(const std::vector<float>& pattern);
    void broadcastToAssociativeMemory(const std::vector<float>& pattern);
    void broadcastToPredictionSystem(const std::vector<float>& pattern);
    void broadcastToAttentionSystem(const std::vector<NeuronId>& neurons);
    void broadcastToDevelopmentSystem(DevelopmentalStage stage);
    
    // Learning integration helpers
    void processLearningSignal(float signal, EnhancedWorkingMemory& workingMemory);
    void processErrorSignal(float error, EnhancedWorkingMemory& workingMemory);
};

} // namespace nlm