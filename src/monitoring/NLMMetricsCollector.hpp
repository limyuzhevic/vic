#pragma once

#include "MetricRegistry.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

class Brain;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

class NLMMetricsCollector {
public:
    NLMMetricsCollector(std::shared_ptr<MetricRegistry> registry,
                       std::shared_ptr<Config> config);
    ~NLMMetricsCollector() = default;
    
    // Initialize the NLM metrics collector
    bool initialize();
    
    // Register all built-in NLM metrics
    void registerBuiltinMetrics();
    
    // Register metrics for specific components
    void registerBrainMetrics(std::shared_ptr<Brain> brain);
    void registerMemoryMetrics(std::shared_ptr<NeuralWorkingMemory> workingMemory,
                               std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                               std::shared_ptr<NeuralAssociativeMemory> associativeMemory);
    void registerPredictionMetrics(std::shared_ptr<PredictionSystem> predictionSystem);
    void registerCognitionMetrics(std::shared_ptr<NeuralPlanner> planner,
                                  std::shared_ptr<ConceptFormation> conceptFormation,
                                  std::shared_ptr<AttentionalSelection> attention);
    void registerDevelopmentMetrics(std::shared_ptr<DevelopmentSystem> developmentSystem);
    void registerNeuromodulationMetrics(std::shared_ptr<Dopamine> dopamine,
                                        std::shared_ptr<Curiosity> curiosity,
                                        std::shared_ptr<Novelty> novelty,
                                        std::shared_ptr<PredictionError> predictionError);
    
    // Register custom NLM metrics
    void registerCustomMetrics();
    
    // Get registry reference
    std::shared_ptr<MetricRegistry> getRegistry() const { return registry_; }
    
    // Check if collector is initialized
    bool isInitialized() const { return isInitialized_; }
    
    // Update all metrics (collect current values)
    void updateMetrics();
    
    // Get metric value by name
    double getMetricValue(const std::string& name) const;
    
    // Get all metric values
    std::vector<MetricData> getAllMetricValues() const;
    
    // Set up integration with Brain and its components
    void setupBrainIntegration(std::shared_ptr<Brain> brain);
    
    // Clean up integration
    void cleanup() {
        isInitialized_ = false;
        brain_ = nullptr;
        workingMemory_ = nullptr;
        episodicMemory_ = nullptr;
        associativeMemory_ = nullptr;
        predictionSystem_ = nullptr;
        planner_ = nullptr;
        conceptFormation_ = nullptr;
        attention_ = nullptr;
        developmentSystem_ = nullptr;
        dopamine_ = nullptr;
        curiosity_ = nullptr;
        novelty_ = nullptr;
        predictionError_ = nullptr;
    }
    
private:
    // Built-in metric collection functions
    double collectTotalSpikeCount() const;
    double collectFiringNeuronCount() const;
    double collectAverageFiringRate() const;
    double collectExcitationInhibitionRatio() const;
    double collectTotalNeuronCount() const;
    double collectTotalSynapseCount() const;
    double collectActiveNeuronCount() const;
    double collectPendingSpikeCount() const;
    
    // Memory metrics
    double collectWorkingMemoryActiveTraces() const;
    double collectEpisodicMemoryEpisodeCount() const;
    double collectAssociativeMemoryActiveAssociations() const;
    
    // Neuromodulation metrics
    double collectDopamineLevel() const;
    double collectCuriosityLevel() const;
    double collectNoveltyLevel() const;
    double collectPredictionError() const;
    
    // Cognition metrics
    double collectPlanningActivity() const;
    double collectConceptActivation() const;
    double collectAttentionFocus() const;
    
    // Development metrics
    double collectDevelopmentalStage() const;
    double collectPlasticityRate() const;
    
    // System metrics
    double collectSystemCPUUsage() const;
    double collectSystemMemoryUsage() const;
    double collectSystemProcessTime() const;
    
    // Behavior metrics
    double collectExplorationRate() const;
    double collectExploitationRate() const;
    double collectAdaptationRate() const;
    
    // Spike pattern metrics
    double collectThetaGamma coupling() const;
    double collectBurstRate() const;
    double collectSynchronyLevel() const;
    
    // Memory efficiency metrics
    double collectMemoryConsolidationRate() const;
    double collectMemoryReplayFrequency() const;
    double collectMemoryEncodingAccuracy() const;
    
    // Prediction metrics
    double collectPredictionAccuracy() const;
    double collectPredictionLatency() const;
    double collectPredictionErrorVariance() const;
    
    std::shared_ptr<MetricRegistry> registry_;
    std::shared_ptr<Config> config_;
    
    // Integration pointers
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuralWorkingMemory> workingMemory_;
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::shared_ptr<NeuralAssociativeMemory> associativeMemory_;
    std::shared_ptr<PredictionSystem> predictionSystem_;
    std::shared_ptr<NeuralPlanner> planner_;
    std::shared_ptr<ConceptFormation> conceptFormation_;
    std::shared_ptr<AttentionalSelection> attention_;
    std::shared_ptr<DevelopmentSystem> developmentSystem_;
    std::shared_ptr<Dopamine> dopamine_;
    std::shared_ptr<Curiosity> curiosity_;
    std::shared_ptr<Novelty> novelty_;
    std::shared_ptr<PredictionError> predictionError_;
    
    bool isInitialized_;
    
    // Performance timing
    std::chrono::steady_clock::time_point lastUpdateTime_;
    double updateInterval_;
};

} // namespace nlm
