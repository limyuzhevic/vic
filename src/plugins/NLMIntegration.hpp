// NLM Integration APIs for Scripting and Plugins
// Integration points between plugin/scripting systems and NLM core components

#pragma once

#include "UPIP.hpp"
#include "ScriptEngine.hpp"
#include <memory>
#include <functional>
#include <map>
#include <vector>
#include <string>

namespace nlm {
namespace plugins {

// Forward declarations
class Brain;
class MemorySystem;
class Config;
class DevelopmentSystem;
class NeuromodulationSystem;
class PredictionSystem;
namespace visualization {
    class VisualizationEngine;
}

// Brain scripting interface
class BrainScriptingInterface {
public:
    virtual ~BrainScriptingInterface() = default;
    
    // Neuron operations
    virtual json createNeuron(const json& parameters) = 0;
    virtual json getNeuron(const std::string& neuronId) = 0;
    virtual json updateNeuron(const std::string& neuronId, const json& parameters) = 0;
    virtual json deleteNeuron(const std::string& neuronId) = 0;
    virtual json getAllNeurons() = 0;
    
    // Synapse operations
    virtual json createSynapse(const json& parameters) = 0;
    virtual json getSynapse(const std::string& synapseId) = 0;
    virtual json updateSynapse(const std::string& synapseId, const json& parameters) = 0;
    virtual json deleteSynapse(const std::string& synapseId) = 0;
    virtual json getAllSynapses() = 0;
    
    // Neural population operations
    virtual json createNeuralPopulation(const json& parameters) = 0;
    virtual json getNeuralPopulation(const std::string& populationId) = 0;
    virtual json updateNeuralPopulation(const std::string& populationId, const json& parameters) = 0;
    virtual json deleteNeuralPopulation(const std::string& populationId) = 0;
    virtual json getAllNeuralPopulations() = 0;
    
    // Neural region operations
    virtual json createNeuralRegion(const json& parameters) = 0;
    virtual json getNeuralRegion(const std::string& regionId) = 0;
    virtual json updateNeuralRegion(const std::string& regionId, const json& parameters) = 0;
    virtual json deleteNeuralRegion(const std::string& regionId) = 0;
    virtual json getAllNeuralRegions() = 0;
    
    // Brain operations
    virtual json getBrainState() = 0;
    virtual json updateBrainParameters(const json& parameters) = 0;
    virtual json runSimulationStep() = 0;
    virtual json pauseSimulation() = 0;
    virtual json resumeSimulation() = 0;
    virtual json resetSimulation() = 0;
    
    // Neural dynamics operations
    virtual json setNeuralDynamics(const std::string& neuronId, const std::string& dynamicsType, const json& parameters) = 0;
    virtual json getNeuralDynamics(const std::string& neuronId) = 0;
    virtual json updateNeuralDynamics(const std::string& neuronId, const json& parameters) = 0;
    
    // Plasticity operations
    virtual json applyPlasticityRule(const std::string& ruleName, const json& parameters) = 0;
    virtual json getPlasticityRule(const std::string& ruleName) = 0;
    virtual json updatePlasticityRule(const std::string& ruleName, const json& parameters) = 0;
    
    // Development operations
    virtual json startDevelopment() = 0;
    virtual json stopDevelopment() = 0;
    virtual json getDevelopmentStatus() = 0;
    
    // Neuromodulation operations
    virtual json setNeuromodulator(const std::string& neuromodulatorName, double value) = 0;
    virtual json getNeuromodulator(const std::string& neuromodulatorName) = 0;
    virtual json updateNeuromodulator(const std::string& neuromodulatorName, const json& parameters) = 0;
    
    // Prediction operations
    virtual json getPrediction(const std::string& input) = 0;
    virtual json updatePredictionModel(const json& model) = 0;
    virtual json getPredictionConfidence() = 0;
    
    // Brain visualization
    virtual json createVisualizer(const std::string& visualizerType, const json& parameters) = 0;
    virtual json updateVisualizer(const std::string& visualizerId, const json& parameters) = 0;
    virtual json deleteVisualizer(const std::string& visualizerId) = 0;
    virtual json getAllVisualizers() = 0;
    
    // Brain analysis
    virtual json analyzeBrainState(const json& analysisType) = 0;
    virtual json performStatisticalAnalysis(const json& parameters) = 0;
    virtual json generateBrainReport() = 0;
    
    // Brain connectivity
    virtual json computeConnectivityMatrix() = 0;
    virtual json analyzeNetworkTopology() = 0;
    virtual json detectCommunityStructure() = 0;
    
    // Brain simulation
    virtual json saveBrainState(const std::string& filepath) = 0;
    virtual json loadBrainState(const std::string& filepath) = 0;
    virtual json checkpointBrainState() = 0;
    virtual json restoreBrainState(const std::string& checkpointId) = 0;
    
    // Brain monitoring
    virtual json getBrainMetrics() = 0;
    virtual json monitorBrainHealth() = 0;
    virtual json getPerformanceStatistics() = 0;
    
    // Brain scripting utilities
    virtual json compileBrainScript(const std::string& script) = 0;
    virtual json executeBrainScript(const std::string& script) = 0;
    virtual json validateBrainScript(const std::string& script) = 0;
};

// Memory scripting interface
class MemoryScriptingInterface {
public:
    virtual ~MemoryScriptingInterface() = default;
    
    // Working memory operations
    virtual json createWorkingMemory(const std::string& name, const json& parameters) = 0;
    virtual json getWorkingMemory(const std::string& memoryId) = 0;
    virtual json updateWorkingMemory(const std::string& memoryId, const json& parameters) = 0;
    virtual json deleteWorkingMemory(const std::string& memoryId) = 0;
    virtual json getAllWorkingMemories() = 0;
    
    // Episodic memory operations
    virtual json encodeExperience(const json& experience) = 0;
    virtual json retrieveExperience(const std::string& memoryId) = 0;
    virtual json replayExperience(const std::string& memoryId) = 0;
    virtual json deleteExperience(const std::string& memoryId) = 0;
    virtual json getAllExperiences() = 0;
    
    // Associative memory operations
    virtual json createAssociation(const std::string& key, const std::string& value) = 0;
    virtual json getAssociation(const std::string& key) = 0;
    virtual json updateAssociation(const std::string& key, const std::string& value) = 0;
    virtual json deleteAssociation(const std::string& key) = 0;
    virtual json getAllAssociations() = 0;
    virtual json findAssociations(const std::string& pattern) = 0;
    
    // Concept formation operations
    virtual json discoverConcept(const json& pattern) = 0;
    virtual json getConcept(const std::string& conceptId) = 0;
    virtual json updateConcept(const std::string& conceptId, const json& parameters) = 0;
    virtual json deleteConcept(const std::string& conceptId) = 0;
    virtual json getAllConcepts() = 0;
    
    // Spatial representation operations
    virtual json encodeLocation(const json& location) = 0;
    virtual json getLocation(const std::string& locationId) = 0;
    virtual json updateLocation(const std::string& locationId, const json& parameters) = 0;
    virtual json deleteLocation(const std::string& locationId) = 0;
    virtual json getAllLocations() = 0;
    
    // Temporal relation operations
    virtual json encodeTemporalRelation(const std::string& beforeId, const std::string& afterId, const json& relation) = 0;
    virtual json getTemporalRelation(const std::string& relationId) = 0;
    virtual json updateTemporalRelation(const std::string& relationId, const json& parameters) = 0;
    virtual json deleteTemporalRelation(const std::string& relationId) = 0;
    virtual json getAllTemporalRelations() = 0;
    
    // Memory management operations
    virtual json optimizeMemoryUsage() = 0;
    virtual json compactMemories() = 0;
    virtual json getMemoryStatistics() = 0;
    virtual json getMemoryUsage(const std::string& memoryId) = 0;
    
    // Memory scripting utilities
    virtual json compileMemoryScript(const std::string& script) = 0;
    virtual json executeMemoryScript(const std::string& script) = 0;
    virtual json validateMemoryScript(const std::string& script) = 0;
};

// Configuration scripting interface
class ConfigurationScriptingInterface {
public:
    virtual ~ConfigurationScriptingInterface() = default;
    
    // Configuration management operations
    virtual json createConfig(const std::string& name, const json& parameters) = 0;
    virtual json getConfig(const std::string& configId) = 0;
    virtual json updateConfig(const std::string& configId, const json& parameters) = 0;
    virtual json deleteConfig(const std::string& configId) = 0;
    virtual json getAllConfigs() = 0;
    
    // Runtime configuration operations
    virtual json applyConfig(const std::string& configId) = 0;
    virtual json getConfigValue(const std::string& key) = 0;
    virtual json setConfigValue(const std::string& key, const json& value) = 0;
    virtual json getConfigSchema(const std::string& configId) = 0;
    
    // Configuration validation
    virtual json validateConfig(const json& config) = 0;
    virtual json getValidationErrors(const json& config) = 0;
    virtual json fixConfig(const json& config) = 0;
    
    // Configuration templates
    virtual json createConfigTemplate(const std::string& name, const json& template) = 0;
    virtual json getConfigTemplate(const std::string& templateName) = 0;
    virtual json updateConfigTemplate(const std::string& templateName, const json& template) = 0;
    virtual json deleteConfigTemplate(const std::string& templateName) = 0;
    virtual json getAllConfigTemplates() = 0;
    
    // Configuration history
    virtual json getConfigHistory(const std::string& configId) = 0;
    virtual json getConfigVersion(const std::string& configId, const std::string& version) = 0;
    virtual json rollbackConfig(const std::string& configId, const std::string& version) = 0;
    
    // Configuration scripting utilities
    virtual json compileConfigScript(const std::string& script) = 0;
    virtual json executeConfigScript(const std::string& script) = 0;
    virtual json validateConfigScript(const std::string& script) = 0;
};

// Simulation control scripting interface
class SimulationControlScriptingInterface {
public:
    virtual ~SimulationControlScriptingInterface() = default;
    
    // Simulation lifecycle operations
    virtual json startSimulation() = 0;
    virtual json stopSimulation() = 0;
    virtual json pauseSimulation() = 0;
    virtual json resumeSimulation() = 0;
    virtual json resetSimulation() = 0;
    
    // Simulation control operations
    virtual json stepSimulation(const json& parameters) = 0;
    virtual json runSimulation(const json& parameters) = 0;
    virtual json getSimulationStatus() = 0;
    virtual json getSimulationTime() = 0;
    
    // Simulation configuration
    virtual json setSimulationParameters(const json& parameters) = 0;
    virtual json getSimulationParameters() = 0;
    virtual json updateSimulationParameters(const json& parameters) = 0;
    
    // Simulation control scripting
    virtual json compileSimulationScript(const std::string& script) = 0;
    virtual json executeSimulationScript(const std::string& script) = 0;
    virtual json validateSimulationScript(const std::string& script) = 0;
};

// Debugging and analysis scripting interface
class DebuggingAnalysisScriptingInterface {
public:
    virtual ~DebuggingAnalysisScriptingInterface() = default;
    
    // Debugging operations
    virtual json setBreakpoint(const std::string& scriptId, int lineNumber) = 0;
    virtual json removeBreakpoint(const std::string& scriptId, int lineNumber) = 0;
    virtual json listBreakpoints(const std::string& scriptId) = 0;
    virtual json debugScript(const std::string& scriptId) = 0;
    virtual json stopDebugging(const std::string& scriptId) = 0;
    
    // Analysis operations
    virtual json analyzeScript(const std::string& scriptId) = 0;
    virtual json profileScript(const std::string& scriptId, const json& parameters) = 0;
    virtual json getScriptMetrics(const std::string& scriptId) = 0;
    virtual json getScriptPerformance(const std::string& scriptId) = 0;
    
    // Debugging analysis scripting
    virtual json compileDebugScript(const std::string& script) = 0;
    virtual json executeDebugScript(const std::string& script) = 0;
    virtual json validateDebugScript(const std::string& script) = 0;
};

// Visualization scripting interface
class VisualizationScriptingInterface {
public:
    virtual ~VisualizationScriptingInterface() = default;
    
    // Visualization operations
    virtual json createVisualizer(const std::string& visualizerType, const json& parameters) = 0;
    virtual json getVisualizer(const std::string& visualizerId) = 0;
    virtual json updateVisualizer(const std::string& visualizerId, const json& parameters) = 0;
    virtual json deleteVisualizer(const std::string& visualizerId) = 0;
    virtual json getAllVisualizers() = 0;
    
    // Visualization control
    virtual json startVisualization(const std::string& visualizerId) = 0;
    virtual json stopVisualization(const std::string& visualizerId) = 0;
    virtual json pauseVisualization(const std::string& visualizerId) = 0;
    virtual json resumeVisualization(const std::string& visualizerId) = 0;
    
    // Visualization scripting
    virtual json compileVisualizationScript(const std::string& script) = 0;
    virtual json executeVisualizationScript(const std::string& script) = 0;
    virtual json validateVisualizationScript(const std::string& script) = 0;
};

// Neural network scripting interface
class NeuralNetworkScriptingInterface {
public:
    virtual ~NeuralNetworkScriptingInterface() = default;
    
    // Neuron/synapse manipulation
    virtual json createNeuron(const json& parameters) = 0;
    virtual json createSynapse(const json& parameters) = 0;
    virtual json connectNeurons(const std::string& sourceId, const std::string& targetId, const json& parameters) = 0;
    virtual json disconnectNeurons(const std::string& sourceId, const std::string& targetId) = 0;
    
    // Neural network operations
    virtual json trainNetwork(const json& trainingData) = 0;
    virtual json evaluateNetwork(const json& testData) = 0;
    virtual json optimizeNetwork(const json& optimizationParameters) = 0;
    
    // Neural scripting
    virtual json compileNeuralScript(const std::string& script) = 0;
    virtual json executeNeuralScript(const std::string& script) = 0;
    virtual json validateNeuralScript(const std::string& script) = 0;
};

// Memory management scripting interface
class MemoryManagementScriptingInterface {
public:
    virtual ~MemoryManagementScriptingInterface() = default;
    
    // Memory pool operations
    virtual json createMemoryPool(const std::string& name, const json& parameters) = 0;
    virtual json getMemoryPool(const std::string& poolId) = 0;
    virtual json updateMemoryPool(const std::string& poolId, const json& parameters) = 0;
    virtual json deleteMemoryPool(const std::string& poolId) = 0;
    virtual json getAllMemoryPools() = 0;
    
    // Caching operations
    virtual json createCache(const std::string& name, const json& parameters) = 0;
    virtual json getCache(const std::string& cacheId) = 0;
    virtual json updateCache(const std::string& cacheId, const json& parameters) = 0;
    virtual json deleteCache(const std::string& cacheId) = 0;
    virtual json getAllCaches() = 0;
    
    // Memory management scripting
    virtual json compileMemoryScript(const std::string& script) = 0;
    virtual json executeMemoryScript(const std::string& script) = 0;
    virtual json validateMemoryScript(const std::string& script) = 0;
};

// Performance optimization scripting interface
class PerformanceOptimizationScriptingInterface {
public:
    virtual ~PerformanceOptimizationScriptingInterface() = default;
    
    // Performance profiling
    virtual json startProfiling(const json& parameters) = 0;
    virtual json stopProfiling() = 0;
    virtual json getProfilingData() = 0;
    virtual json clearProfilingData() = 0;
    
    // Performance analysis
    virtual json analyzePerformance(const json& parameters) = 0;
    virtual json optimizePerformance(const json& parameters) = 0;
    virtual json getPerformanceMetrics() = 0;
    
    // Performance scripting
    virtual json compilePerformanceScript(const std::string& script) = 0;
    virtual json executePerformanceScript(const std::string& script) = 0;
    virtual json validatePerformanceScript(const std::string& script) = 0;
};

// Automated testing and validation interface
class TestingValidationScriptingInterface {
public:
    virtual ~TestingValidationScriptingInterface() = default;
    
    // Testing operations
    virtual json createTest(const std::string& testName, const json& testData) = 0;
    virtual json runTest(const std::string& testId) = 0;
    virtual json getTestResults(const std::string& testId) = 0;
    virtual json deleteTest(const std::string& testId) = 0;
    virtual json getAllTests() = 0;
    
    // Validation operations
    virtual json validateScript(const std::string& scriptId) = 0;
    virtual json validatePlugin(const std::string& pluginId) = 0;
    virtual json getValidationResults() = 0;
    
    // Testing and validation scripting
    virtual json compileTestScript(const std::string& script) = 0;
    virtual json executeTestScript(const std::string& script) = 0;
    virtual json validateTestScript(const std::string& script) = 0;
};

// Simulation orchestration interface
class SimulationOrchestrationScriptingInterface {
public:
    virtual ~SimulationOrchestrationScriptingInterface() = default;
    
    // Orchestration operations
    virtual json createOrchestration(const std::string& orchestrationName, const json& parameters) = 0;
    virtual json getOrchestration(const std::string& orchestrationId) = 0;
    virtual json updateOrchestration(const std::string& orchestrationId, const json& parameters) = 0;
    virtual json deleteOrchestration(const std::string& orchestrationId) = 0;
    virtual json getAllOrchestrations() = 0;
    
    // Orchestration control
    virtual json startOrchestration(const std::string& orchestrationId) = 0;
    virtual json stopOrchestration(const std::string& orchestrationId) = 0;
    virtual json pauseOrchestration(const std::string& orchestrationId) = 0;
    virtual json resumeOrchestration(const std::string& orchestrationId) = 0;
    
    // Simulation orchestration scripting
    virtual json compileOrchestrationScript(const std::string& script) = 0;
    virtual json executeOrchestrationScript(const std::string& script) = 0;
    virtual json validateOrchestrationScript(const std::string& script) = 0;
};

// Data analysis scripting interface
class DataAnalysisScriptingInterface {
public:
    virtual ~DataAnalysisScriptingInterface() = default;
    
    // Data operations
    virtual json loadData(const std::string& filepath) = 0;
    virtual json saveData(const std::string& filepath, const json& data) = 0;
    virtual json processData(const json& data, const json& parameters) = 0;
    virtual json analyzeData(const json& data, const std::string& analysisType) = 0;
    
    // Data analysis scripting
    virtual json compileAnalysisScript(const std::string& script) = 0;
    virtual json executeAnalysisScript(const std::string& script) = 0;
    virtual json validateAnalysisScript(const std::string& script) = 0;
};

// NLM Integration Manager
class NLMIntegrationManager {
public:
    NLMIntegrationManager(std::shared_ptr<nlm::Brain> brain,
                         std::shared_ptr<nlm::MemorySystem> memorySystem,
                         std::shared_ptr<nlm::Config> config,
                         std::shared_ptr<nlm::DevelopmentSystem> developmentSystem,
                         std::shared_ptr<nlm::NeuromodulationSystem> neuromodulationSystem,
                         std::shared_ptr<nlm::PredictionSystem> predictionSystem,
                         std::shared_ptr<nlm::visualization::VisualizationEngine> visualizer);
    
    // Integration initialization
    bool initialize();
    bool shutdown();
    
    // Integration point access
    std::shared_ptr<BrainScriptingInterface> getBrainScriptingInterface() const { return brainScripting_; }
    std::shared_ptr<MemoryScriptingInterface> getMemoryScriptingInterface() const { return memoryScripting_; }
    std::shared_ptr<ConfigurationScriptingInterface> getConfigurationScriptingInterface() const { return configScripting_; }
    std::shared_ptr<SimulationControlScriptingInterface> getSimulationControlScriptingInterface() const { return simulationControlScripting_; }
    std::shared_ptr<DebuggingAnalysisScriptingInterface> getDebuggingAnalysisScriptingInterface() const { return debuggingAnalysisScripting_; }
    std::shared_ptr<VisualizationScriptingInterface> getVisualizationScriptingInterface() const { return visualizationScripting_; }
    std::shared_ptr<NeuralNetworkScriptingInterface> getNeuralNetworkScriptingInterface() const { return neuralNetworkScripting_; }
    std::shared_ptr<MemoryManagementScriptingInterface> getMemoryManagementScriptingInterface() const { return memoryManagementScripting_; }
    std::shared_ptr<PerformanceOptimizationScriptingInterface> getPerformanceOptimizationScriptingInterface() const { return performanceOptimizationScripting_; }
    std::shared_ptr<TestingValidationScriptingInterface> getTestingValidationScriptingInterface() const { return testingValidationScripting_; }
    std::shared_ptr<SimulationOrchestrationScriptingInterface> getSimulationOrchestrationScriptingInterface() const { return simulationOrchestrationScripting_; }
    std::shared_ptr<DataAnalysisScriptingInterface> getDataAnalysisScriptingInterface() const { return dataAnalysisScripting_; }
    
    // Plugin integration
    bool registerPlugin(std::shared_ptr<IPlugin> plugin);
    bool unregisterPlugin(const std::string& pluginId);
    std::vector<std::string> getRegisteredPlugins() const;
    
    // Script integration
    ScriptResult executeScript(const std::string& script, const ScriptContext& context);
    bool registerScriptEngine(const std::string& language, std::shared_ptr<IScriptEngine> engine);
    std::shared_ptr<IScriptEngine> getScriptEngine(const std::string& language) const;
    
    // Integration management
    bool enableIntegrationPoint(const std::string& point, bool enabled);
    bool isIntegrationPointEnabled(const std::string& point) const;
    json getIntegrationStatus() const;
    
    // NLM system access
    std::shared_ptr<nlm::Brain> getBrain() const { return brain_; }
    std::shared_ptr<nlm::MemorySystem> getMemorySystem() const { return memorySystem_; }
    std::shared_ptr<nlm::Config> getConfig() const { return config_; }
    std::shared_ptr<nlm::DevelopmentSystem> getDevelopmentSystem() const { return developmentSystem_; }
    std::shared_ptr<nlm::NeuromodulationSystem> getNeuromodulationSystem() const { return neuromodulationSystem_; }
    std::shared_ptr<nlm::PredictionSystem> getPredictionSystem() const { return predictionSystem_; }
    std::shared_ptr<nlm::visualization::VisualizationEngine> getVisualizer() const { return visualizer_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::MemorySystem> memorySystem_;
    std::shared_ptr<nlm::Config> config_;
    std::shared_ptr<nlm::DevelopmentSystem> developmentSystem_;
    std::shared_ptr<nlm::NeuromodulationSystem> neuromodulationSystem_;
    std::shared_ptr<nlm::PredictionSystem> predictionSystem_;
    std::shared_ptr<nlm::visualization::VisualizationEngine> visualizer_;
    
    std::shared_ptr<BrainScriptingInterface> brainScripting_;
    std::shared_ptr<MemoryScriptingInterface> memoryScripting_;
    std::shared_ptr<ConfigurationScriptingInterface> configScripting_;
    std::shared_ptr<SimulationControlScriptingInterface> simulationControlScripting_;
    std::shared_ptr<DebuggingAnalysisScriptingInterface> debuggingAnalysisScripting_;
    std::shared_ptr<VisualizationScriptingInterface> visualizationScripting_;
    std::shared_ptr<NeuralNetworkScriptingInterface> neuralNetworkScripting_;
    std::shared_ptr<MemoryManagementScriptingInterface> memoryManagementScripting_;
    std::shared_ptr<PerformanceOptimizationScriptingInterface> performanceOptimizationScripting_;
    std::shared_ptr<TestingValidationScriptingInterface> testingValidationScripting_;
    std::shared_ptr<SimulationOrchestrationScriptingInterface> simulationOrchestrationScripting_;
    std::shared_ptr<DataAnalysisScriptingInterface> dataAnalysisScripting_;
};

} // namespace plugins
} // namespace nlm