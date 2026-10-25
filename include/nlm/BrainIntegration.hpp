// NLM Integration Framework - Enhanced Brain Integration

#pragma once

#include "brain/Brain.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/SemanticMemory.hpp"
#include "memory/ProceduralMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "development/DevelopmentSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "visualisation/VisualizationInterface.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "world/SimpleWorld.hpp"
#include <memory>
#include <map>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class EnhancedCLI;
class ConfigLoader;
class TestFramework;
class IntegrationTester;

// Brain integration manager - central coordinator for all cognitive systems
class BrainIntegrationManager {
public:
    BrainIntegrationManager(std::shared_ptr<Brain> brain);
    ~BrainIntegrationManager();

    // Integration lifecycle
    bool initialize();
    void update(SimulationStep step, Timestamp time);
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);

    // System access
    NeuralWorkingMemory* getWorkingMemory() const;
    NeuralEpisodicMemory* getEpisodicMemory() const;
    NeuralAssociativeMemory* getAssociativeMemory() const;
    PredictionSystem* getPredictionSystem() const;
    Dopamine* getDopamine() const;
    Curiosity* getCuriosity() const;
    Novelty* getNovelty() const;
    PredictionError* getPredictionError() const;
    DevelopmentSystem* getDevelopmentSystem() const;
    ConceptFormation* getConceptFormation() const;
    NeuralPlanner* getNeuralPlanner() const;
    AttentionalSelection* getAttentionalSelection() const;
    VisualizationInterface* getVisualizationInterface() const;
    AgentBrain* getAgentBrain() const;
    AgentBody* getAgentBody() const;

    // Integration control
    void enableMemoryIntegration(bool enable);
    void enablePredictionIntegration(bool enable);
    void enableNeuromodulationIntegration(bool enable);
    void enableDevelopmentIntegration(bool enable);
    void enableCognitionIntegration(bool enable);
    void enableVisualizationIntegration(bool enable);
    void enableAgentIntegration(bool enable);

    // Configuration
    void setMemoryConfig(const MemoryConfig& config);
    void setPredictionConfig(const PredictionConfig& config);
    void setNeuromodulationConfig(const NeuromodulationConfig& config);
    void setDevelopmentConfig(const DevelopmentConfig& config);
    void setCognitionConfig(const CognitionConfig& config);

    // Status and monitoring
    std::string getIntegrationStatus() const;
    PerformanceMetrics getPerformanceMetrics() const;
    IntegrationStats getIntegrationStats() const;

    // Test and validation
    bool validateIntegration() const;
    std::vector<std::string> getIntegrationIssues() const;
    void runIntegrationTests();

    // Export and import
    std::string exportIntegrationState() const;
    bool importIntegrationState(const std::string& stateData);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Connection management
    void establishMemoryConnections();
    void establishPredictionConnections();
    void establishNeuromodulationConnections();
    void establishDevelopmentConnections();
    void establishCognitionConnections();
    void establishVisualizationConnections();
    void establishAgentConnections();

    // Update coordination
    void coordinateMemoryUpdates(SimulationStep step, Timestamp time);
    void coordinatePredictionUpdates(SimulationStep step, Timestamp time);
    void coordinateNeuromodulationUpdates(SimulationStep step, Timestamp time);
    void coordinateDevelopmentUpdates(SimulationStep step, Timestamp time);
    void coordinateCognitionUpdates(SimulationStep step, Timestamp time);
    void coordinateVisualizationUpdates(SimulationStep step, Timestamp time);
    void coordinateAgentUpdates(SimulationStep step, Timestamp time);

    // System configuration
    void configureMemorySystems();
    void configurePredictionSystems();
    void configureNeuromodulationSystems();
    void configureDevelopmentSystems();
    void configureCognitionSystems();
    void configureVisualizationSystems();
    void configureAgentSystems();

    // Error handling and recovery
    void handleIntegrationError(const std::string& error);
    void recoverFromError(const std::string& systemName);
    bool isSystemHealthy(const std::string& systemName) const;

    // Performance optimization
    void optimizeIntegrationPerformance();
    void balanceSystemLoad();
    void minimizeLatency();

    // Logging and monitoring
    void logIntegrationStatus() const;
    void logSystemPerformance() const;
    void logConnectionHealth() const;
};

// Integration configuration structures
struct MemoryConfig {
    bool enableWorkingMemory = true;
    bool enableEpisodicMemory = true;
    bool enableSemanticMemory = true;
    bool enableProceduralMemory = true;
    bool enableAssociativeMemory = true;
    bool enableNeuralMemories = true;
    size_t workingMemoryCapacity = 1000;
    size_t episodicMemoryCapacity = 10000;
    float consolidationRate = 0.1f;
};

struct PredictionConfig {
    bool enablePredictionSystem = true;
    bool enablePredictionError = true;
    bool enableConfidenceTracking = true;
    bool enableErrorBasedLearning = true;
    float predictionUpdateRate = 0.5f;
    float errorLearningRate = 0.1f;
};

struct NeuromodulationConfig {
    bool enableDopamine = true;
    bool enableCuriosity = true;
    bool enableNovelty = true;
    bool enableRewardPredictionError = true;
    float neuromodulationUpdateRate = 0.3f;
    float explorationBonus = 0.5f;
};

struct DevelopmentConfig {
    bool enableDevelopmentSystem = true;
    bool enableSynaptogenesis = true;
    bool enablePruning = true;
    bool enableMaturation = true;
    bool enableCriticalPeriod = true;
    DevelopmentalStage initialStage = DevelopmentalStage::Initial;
    float developmentRate = 0.01f;
};

struct CognitionConfig {
    bool enableConceptFormation = true;
    bool enableNeuralPlanning = true;
    bool enableAttentionalSelection = true;
    bool enableSelfModel = true;
    bool enableSocialLearning = true;
    float conceptFormationRate = 0.2f;
    float planningHorizon = 10.0f;
};

struct IntegrationStats {
    size_t totalSystems;
    size_t activeSystems;
    size_t connectedSystems;
    double averageConnectionStrength;
    std::map<std::string, double> systemPerformance;
    std::vector<std::string> activeSystemNames;
    std::vector<std::string> connectedSystemNames;
    double integrationHealth;
    size_t totalErrors;
    size_t totalRecoveries;
};

struct PerformanceMetrics {
    double cpuUsage;
    size_t memoryUsage;
    double simulationTime;
    double wallClockTime;
    SimulationStep stepsCompleted;
    float averageFiringRate;
    size_t totalSpikes;
    std::map<std::string, double> systemMetrics;
    std::vector<double> latencyMetrics;
    std::vector<double> throughputMetrics;
};

// Utility functions for integration
namespace IntegrationUtils {
    std::string systemNameToString(BrainSystem system);
    std::string integrationStageToString(IntegrationStage stage);
    double calculateConnectionStrength(const BrainSystem& source, const BrainSystem& target);
    bool validateConnection(const BrainSystem& source, const BrainSystem& target);
    std::vector<std::string> getSystemDependencies(BrainSystem system);
    void logConnection(const std::string& source, const std::string& target, double strength);
    std::string formatConnectionInfo(const std::string& source, const std::string& target, double strength);
}

// Integration event types
enum class IntegrationEventType {
    SystemInitialized,
    SystemConnected,
    SystemUpdated,
    SystemError,
    SystemRecovered,
    IntegrationCompleted,
    IntegrationValidated,
    PerformanceOptimized
};

struct IntegrationEvent {
    IntegrationEventType type;
    std::string systemName;
    std::string message;
    std::map<std::string, std::string> metadata;
    Timestamp timestamp;
    IntegrationEvent(IntegrationEventType t, const std::string& system, const std::string& msg)
        : type(t), systemName(system), message(msg), timestamp(std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::system_clock::now().time_since_epoch()).count()) {}
};

// Event listener interface
class IntegrationEventListener {
public:
    virtual ~IntegrationEventListener() = default;
    virtual void onIntegrationEvent(const IntegrationEvent& event) = 0;
    virtual bool shouldHandleEvent(const IntegrationEvent& event) const = 0;
};

// Event dispatcher for integration events
class IntegrationEventDispatcher {
public:
    static IntegrationEventDispatcher& getInstance();
    void addListener(std::shared_ptr<IntegrationEventListener> listener);
    void removeListener(std::shared_ptr<IntegrationEventListener> listener);
    void dispatch(const IntegrationEvent& event);
    void setEventCallback(std::function<void(const IntegrationEvent&)> callback);

private:
    IntegrationEventDispatcher() = default;
    std::vector<std::shared_ptr<IntegrationEventListener>> listeners;
    std::function<void(const IntegrationEvent&)> eventCallback;
    std::mutex mutex;
};

// Brain system enumeration for integration
enum class BrainSystem {
    CoreNeural,
    WorkingMemory,
    EpisodicMemory,
    SemanticMemory,
    ProceduralMemory,
    AssociativeMemory,
    NeuralWorkingMemory,
    NeuralEpisodicMemory,
    PredictionSystem,
    Dopamine,
    Curiosity,
    Novelty,
    PredictionError,
    DevelopmentSystem,
    ConceptFormation,
    NeuralPlanner,
    AttentionalSelection,
    VisualizationInterface,
    AgentBrain,
    AgentBody,
    SimpleWorld,
    Environment,
    SensoryInput,
    MotorOutput,
    ExperimentRunner
};

// Integration stage for phased activation
enum class IntegrationStage {
    NotInitialized,
    CoreSystemsInitialized,
    MemorySystemsInitialized,
    PredictionSystemsInitialized,
    NeuromodulationSystemsInitialized,
    DevelopmentSystemsInitialized,
    CognitionSystemsInitialized,
    VisualizationSystemsInitialized,
    AgentSystemsInitialized,
    FullyIntegrated,
    ValidationComplete
};

} // namespace nlm
