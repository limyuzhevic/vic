#pragma once

#include "Brain.hpp"
#include "BrainExpert.hpp"
#include "../memory/Memory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <map>

namespace nlm {

// Advanced Memory Expert - Expert-level memory control and customization
class MemoryExpert {
public:
    MemoryExpert(std::shared_ptr<Brain> brain);
    ~MemoryExpert();
    
    // Expert memory control
    void configureConsolidationStrategy(const std::string& strategyName,
                                       const ConsolidationParameters& params);
    void setReplayFrequency(float frequency);
    void enableCustomPruning(const PruningRules& rules);
    void configureMemoryReplay(const ReplayParameters& params);
    
    // Working memory expert control
    void configureWorkingMemory(const WorkingMemoryParameters& params);
    void setMemoryCapacity(size_t capacity);
    void setMemoryDecayRate(float rate);
    void enableMemoryCompetition(bool enable);
    
    // Episodic memory expert control
    void configureEpisodicStorage(float storageCapacity);
    void setConsolidationThreshold(float threshold);
    void enableConsolidation(bool enable);
    
    // Expert performance tuning
    void enableMemoryOptimizations(bool enable);
    void setMemoryPoolSize(size_t size);
    void configureCachingStrategy(const std::string& strategy);
    
    // Memory analysis and monitoring
    void analyzeMemoryUsage() const;
    void profileMemoryPerformance() const;
    void generateMemoryReport() const;
    
    // Expert control access
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    MemoryExpertStats getStats() const;
    
    // State management
    bool saveMemoryState(const std::string& filepath) const;
    bool loadMemoryState(const std::string& filepath);
    
    void enableExpertMode(bool enable);
    bool isExpertModeEnabled() const { return expertMode_; }

private:
    std::shared_ptr<Brain> brain_;
    bool expertMode_;
    
    // Expert memory parameters
    ConsolidationParameters consolidationParams_;
    ReplayParameters replayParams_;
    WorkingMemoryParameters workingMemoryParams_;
    PruningRules pruningRules_;
    
    // Expert statistics
    MemoryExpertStats calculateStats() const;
};

// Advanced Development Expert - Expert-level development system control
class DevelopmentExpert {
public:
    DevelopmentExpert(std::shared_ptr<Brain> brain);
    ~DevelopmentExpert();
    
    // Custom developmental trajectories
    void configureDevelopmentalStages(const std::vector<DevelopmentalStageParams>& stages);
    void setMaturationParameters(const MaturationParameters& params);
    void enableExperienceDependentDevelopment(bool enable);
    
    // Advanced plasticity control
    void configurePlasticityDynamics(const PlasticityDynamics& params);
    void setCriticalPeriodOverride(bool override);
    void setPlasticityModulationParameters(const PlasticityModulation& params);
    
    // Expert development system control
    void enableStructuralPlasticity(bool enable);
    void configureSynaptogenesis(const SynaptogenesisParameters& params);
    void configurePruningParameters(const PruningParameters& params);
    
    // Advanced development algorithms
    void enableHebbianDevelopment(bool enable);
    void enableSTDPDevelopment(bool enable);
    void enableRewardModulatedDevelopment(bool enable);
    
    // Development analysis and monitoring
    void analyzeDevelopmentProgress() const;
    void profileDevelopmentPerformance() const;
    void generateDevelopmentReport() const;
    
    // Expert control access
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    DevelopmentExpertStats getStats() const;
    
    // State management
    bool saveDevelopmentState(const std::string& filepath) const;
    bool loadDevelopmentState(const std::string& filepath);
    
    void enableExpertMode(bool enable);
    bool isExpertModeEnabled() const { return expertMode_; }

private:
    std::shared_ptr<Brain> brain_;
    bool expertMode_;
    
    // Expert development parameters
    std::vector<DevelopmentalStageParams> developmentalStages_;
    MaturationParameters maturationParams_;
    PlasticityDynamics plasticityDynamics_;
    PlasticityModulation plasticityModulation_;
    SynaptogenesisParameters synaptogenesisParams_;
    PruningParameters pruningParams_;
    
    // Expert statistics
    DevelopmentExpertStats calculateStats() const;
};

// Expert Neuromodulation Expert - Advanced neuromodulator control
class NeuromodulationExpert {
public:
    NeuromodulationExpert(std::shared_ptr<Brain> brain);
    ~NeuromodulationExpert();
    
    // Advanced neuromodulator control
    void configureNeuromodulatorDynamics(const NeuromodulatorDynamics& dynamics);
    void setNeuromodulatorLevels(const std::map<std::string, float>& levels);
    void addNeuromodulatorLevels(const std::map<std::string, float>& deltas);
    
    // Custom neuromodulator algorithms
    void configureCustomNeuromodulation(const std::string& name,
                                        std::function<float(const NeuromodulatorState&)> algorithm);
    void enableCrossSystemCoupling(const CrossSystemCoupling& coupling);
    void configureFeedbackLoops(const FeedbackLoopConfig& config);
    
    // Expert neuromodulation systems
    void configureCuriositySystem(const CuriositySystemParams& params);
    void configureNoveltyDetection(const NoveltyDetectionParams& params);
    void configureRewardPrediction(const RewardPredictionParams& params);
    
    // Advanced neuromodulation algorithms
    void enableDynamicNeuromodulation(bool enable);
    void configureNeuromodulatorInteractions(const NeuromodulatorInteractions& interactions);
    void enableNeuromodulatorLearning(bool enable);
    
    // Neuromodulation analysis and monitoring
    void analyzeNeuromodulationActivity() const;
    void profileNeuromodulationPerformance() const;
    void generateNeuromodulationReport() const;
    
    // Expert control access
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    NeuromodulationExpertStats getStats() const;
    
    // State management
    bool saveNeuromodulationState(const std::string& filepath) const;
    bool loadNeuromodulationState(const std::string& filepath);
    
    void enableExpertMode(bool enable);
    bool isExpertModeEnabled() const { return expertMode_; }

private:
    std::shared_ptr<Brain> brain_;
    bool expertMode_;
    
    // Expert neuromodulation parameters
    NeuromodulatorDynamics neuromodulatorDynamics_;
    CrossSystemCoupling crossSystemCoupling_;
    FeedbackLoopConfig feedbackLoops_;
    CuriositySystemParams curiosityParams_;
    NoveltyDetectionParams noveltyParams_;
    RewardPredictionParams rewardParams_;
    NeuromodulatorInteractions interactions_;
    
    // Expert statistics
    NeuromodulationExpertStats calculateStats() const;
};

// Advanced Performance Expert - Expert-level performance tuning
class PerformanceExpert {
public:
    PerformanceExpert(std::shared_ptr<Brain> brain);
    ~PerformanceExpert();
    
    // Performance optimization
    void configureParallelProcessing(const ParallelProcessingConfig& config);
    void setMemoryPoolLimits(const MemoryPoolLimits& limits);
    void enableSIMD(bool enable);
    void configureHardwareAccelerations(const HardwareAccelerations& accel);
    
    // Advanced performance tuning
    void configureMemoryManagementStrategy(const std::string& strategy);
    void setGarbageCollectionParameters(const GarbageCollectionParams& params);
    void configureResourceAllocationStrategy(const ResourceAllocationStrategy& strategy);
    
    // Performance monitoring and analysis
    void enableRealTimeMonitoring(bool enable);
    void setPerformanceThresholds(const PerformanceThresholds& thresholds);
    void configurePerformanceLogging(const PerformanceLoggingConfig& config);
    
    // Performance analysis and optimization
    void analyzePerformanceBottlenecks() const;
    void profilePerformanceCache() const;
    void generatePerformanceReport() const;
    
    // Expert control access
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    PerformanceExpertStats getStats() const;
    
    // State management
    bool savePerformanceState(const std::string& filepath) const;
    bool loadPerformanceState(const std::string& filepath);
    
    void enableExpertMode(bool enable);
    bool isExpertModeEnabled() const { return expertMode_; }

private:
    std::shared_ptr<Brain> brain_;
    bool expertMode_;
    
    // Expert performance parameters
    ParallelProcessingConfig parallelConfig_;
    MemoryPoolLimits memoryLimits_;
    HardwareAccelerations hardwareAccel_;
    std::string memoryManagementStrategy_;
    GarbageCollectionParams gcParams_;
    ResourceAllocationStrategy resourceStrategy_;
    PerformanceThresholds performanceThresholds_;
    PerformanceLoggingConfig loggingConfig_;
    
    // Expert statistics
    PerformanceExpertStats calculateStats() const;
};

// Advanced Integration Expert - Expert-level system integration
class IntegrationExpert {
public:
    IntegrationExpert(std::shared_ptr<Brain> brain);
    ~IntegrationExpert();
    
    // Plugin architecture
    void enablePluginSystem(bool enable);
    void registerPlugin(const std::string& name, std::unique_ptr<PluginInterface> plugin);
    void enablePlugin(const std::string& name, bool enable);
    
    // External data integration
    void configureSensorInterface(const SensorInterfaceConfig& config);
    void configureExternalDataSources(const std::vector<DataSourceConfig>& sources);
    void setDataPreprocessingPipeline(const DataPreprocessingPipeline& pipeline);
    
    // Advanced simulation environments
    void configureCustomEnvironment(const EnvironmentConfig& config);
    void enableSimulationOptimizations(bool enable);
    void configureSimulationParameters(const SimulationParameters& params);
    
    // Integration analysis and monitoring
    void analyzeIntegrationPerformance() const;
    void profileIntegrationLatency() const;
    void generateIntegrationReport() const;
    
    // Expert control access
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    IntegrationExpertStats getStats() const;
    
    // State management
    bool saveIntegrationState(const std::string& filepath) const;
    bool loadIntegrationState(const std::string& filepath);
    
    void enableExpertMode(bool enable);
    bool isExpertModeEnabled() const { return expertMode_; }

private:
    std::shared_ptr<Brain> brain_;
    bool expertMode_;
    
    // Expert integration parameters
    bool pluginSystemEnabled_;
    std::map<std::string, std::unique_ptr<PluginInterface>> plugins_;
    SensorInterfaceConfig sensorConfig_;
    std::vector<DataSourceConfig> dataSources_;
    DataPreprocessingPipeline preprocessingPipeline_;
    EnvironmentConfig environmentConfig_;
    SimulationParameters simulationParams_;
    
    // Expert statistics
    IntegrationExpertStats calculateStats() const;
};

// Expert Scripting Expert - Advanced scripting and automation
class ScriptingExpert {
public:
    ScriptingExpert(std::shared_ptr<Brain> brain);
    ~ScriptingExpert();
    
    // Advanced scripting API
    void enableScripting(bool enable);
    void configureScriptingEngine(const ScriptingEngineConfig& config);
    void setScriptingLanguage(const std::string& language);
    void configureScriptingAPI(const ScriptingAPIConfig& api);
    
    // Custom simulation protocols
    void registerSimulationProtocol(const std::string& name,
                                   const SimulationProtocol& protocol);
    void enableSimulationProtocol(const std::string& name, bool enable);
    void configureProtocolParameters(const std::string& name,
                                    const ProtocolParameters& params);
    
    // Batch processing optimization
    void configureBatchProcessing(const BatchProcessingConfig& config);
    void enableBatchMode(bool enable);
    void configureCheckpointing(const CheckpointConfig& config);
    
    // Advanced automation
    void enableReproducibility(bool enable);
    void configureExperimentFramework(const ExperimentFrameworkConfig& config);
    void configureAdvancedVisualization(const VisualizationConfig& config);
    
    // Scripting analysis and monitoring
    void analyzeScriptPerformance() const;
    void profileScriptExecution() const;
    void generateScriptReport() const;
    
    // Expert control access
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    ScriptingExpertStats getStats() const;
    
    // State management
    bool saveScriptingState(const std::string& filepath) const;
    bool loadScriptingState(const std::string& filepath);
    
    void enableExpertMode(bool enable);
    bool isExpertModeEnabled() const { return expertMode_; }

private:
    std::shared_ptr<Brain> brain_;
    bool expertMode_;
    
    // Expert scripting parameters
    bool scriptingEnabled_;
    ScriptingEngineConfig engineConfig_;
    std::string scriptingLanguage_;
    ScriptingAPIConfig apiConfig_;
    std::map<std::string, SimulationProtocol> protocols_;
    BatchProcessingConfig batchConfig_;
    CheckpointConfig checkpointConfig_;
    bool reproducibilityEnabled_;
    ExperimentFrameworkConfig experimentConfig_;
    VisualizationConfig visualizationConfig_;
    
    // Expert statistics
    ScriptingExpertStats calculateStats() const;
};

// Expert Research Expert - Scientific research features
class ResearchExpert {
public:
    ResearchExpert(std::shared_ptr<Brain> brain);
    ~ResearchExpert();
    
    // Hypothesis testing frameworks
    void configureHypothesisTesting(const HypothesisTestingConfig& config);
    void enableStatisticalTesting(bool enable);
    void configureStatisticalAnalysis(const StatisticalAnalysisConfig& config);
    
    // Advanced research tools
    void configureDataExport(const DataExportConfig& config);
    void configureAnalysisTools(const AnalysisToolsConfig& config);
    void enableReproducibleResearch(bool enable);
    
    // Collaborative research tools
    void configureCollaborationTools(const CollaborationConfig& config);
    void enableResearchSharing(bool enable);
    void configureVersionControl(const VersionControlConfig& config);
    
    // Research analysis and monitoring
    void analyzeResearchData() const;
    void profileResearchPerformance() const;
    void generateResearchReport() const;
    
    // Expert control access
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    ResearchExpertStats getStats() const;
    
    // State management
    bool saveResearchState(const std::string& filepath) const;
    bool loadResearchState(const std::string& filepath);
    
    void enableExpertMode(bool enable);
    bool isExpertModeEnabled() const { return expertMode_; }

private:
    std::shared_ptr<Brain> brain_;
    bool expertMode_;
    
    // Expert research parameters
    HypothesisTestingConfig hypothesisConfig_;
    StatisticalAnalysisConfig statisticalConfig_;
    DataExportConfig dataExportConfig_;
    AnalysisToolsConfig analysisConfig_;
    CollaborationConfig collaborationConfig_;
    VersionControlConfig versionConfig_;
    bool reproducibleResearch_;
    
    // Expert statistics
    ResearchExpertStats calculateStats() const;
};

// Expert System Manager - Manages all expert features
class ExpertSystemManager {
public:
    ExpertSystemManager(std::shared_ptr<Brain> brain);
    ~ExpertSystemManager();
    
    // Initialize all expert systems
    bool initialize();
    
    // Enable/disable expert mode
    void enableExpertMode(bool enable);
    bool isExpertModeEnabled() const { return expertMode_; }
    
    // Get expert subsystem access
    std::shared_ptr<MemoryExpert> getMemoryExpert() { return memoryExpert_; }
    std::shared_ptr<DevelopmentExpert> getDevelopmentExpert() { return developmentExpert_; }
    std::shared_ptr<NeuromodulationExpert> getNeuromodulationExpert() { return neuromodulationExpert_; }
    std::shared_ptr<PerformanceExpert> getPerformanceExpert() { return performanceExpert_; }
    std::shared_ptr<IntegrationExpert> getIntegrationExpert() { return integrationExpert_; }
    std::shared_ptr<ScriptingExpert> getScriptingExpert() { return scriptingExpert_; }
    std::shared_ptr<ResearchExpert> getResearchExpert() { return researchExpert_; }
    std::shared_ptr<BrainExpert> getBrainExpert() { return brainExpert_; }
    
    // System analysis and monitoring
    void analyzeSystemPerformance() const;
    void profileSystemResources() const;
    void generateSystemReport() const;
    
    // System state management
    bool saveSystemState(const std::string& filepath) const;
    bool loadSystemState(const std::string& filepath);
    
    // Expert system statistics
    ExpertSystemStats getStats() const;
    
private:
    std::shared_ptr<Brain> brain_;
    bool expertMode_;
    
    // Expert subsystems
    std::shared_ptr<MemoryExpert> memoryExpert_;
    std::shared_ptr<DevelopmentExpert> developmentExpert_;
    std::shared_ptr<NeuromodulationExpert> neuromodulationExpert_;
    std::shared_ptr<PerformanceExpert> performanceExpert_;
    std::shared_ptr<IntegrationExpert> integrationExpert_;
    std::shared_ptr<ScriptingExpert> scriptingExpert_;
    std::shared_ptr<ResearchExpert> researchExpert_;
    std::shared_ptr<BrainExpert> brainExpert_;
    
    // Expert system statistics
    ExpertSystemStats calculateStats() const;
};

} // namespace nlm