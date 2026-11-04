#ifndef NLM_ADVANCED_FEATURES_H
#define NLM_ADVANCED_FEATURES_H

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <algorithm>

namespace nlm {

// Advanced configuration profiles
struct AdvancedConfigProfile {
    std::string name;
    std::string description;
    std::map<std::string, std::string> configSettings;
    
    AdvancedConfigProfile(const std::string& n = "", const std::string& d = "")
        : name(n), description(d) {}
};

// Advanced scripting and automation
class AdvancedScripting {
public:
    AdvancedScripting() = default;
    ~AdvancedScripting() = default;
    
    // Execute custom configuration script
    static std::shared_ptr<Config> executeConfigScript(const std::string& script);
    
    // Load advanced configuration from file with variable substitution
    static std::shared_ptr<Config> loadAdvancedConfig(const std::string& filename,
                                                     const std::map<std::string, std::string>& variables = {});
    
    // Apply configuration presets based on use case
    static std::shared_ptr<Config> getPresetConfig(const std::string& useCase);
    
    // Generate configuration for specific testing scenarios
    static std::shared_ptr<Config> generateScenarioConfig(const std::string& scenario);

private:
    static std::string substituteVariables(const std::string& template,
                                           const std::map<std::string, std::string>& variables);
};

// Advanced brain manipulation and control
class AdvancedBrainControl {
public:
    AdvancedBrainControl(std::shared_ptr<Brain> brain) : brain_(brain) {}
    ~AdvancedBrainControl() = default;
    
    // Manipulate neural connectivity patterns
    void applyConnectivityPattern(const std::string& patternType, double strength = 1.0);
    
    // Control plasticity rules dynamically
    void setPlasticityRules(const std::map<std::string, bool>& rules);
    void adjustPlasticityParameters(const std::map<std::string, double>& parameters);
    
    // Manipulate neuromodulation signals
    void applyNeuromodulationPattern(const std::string& pattern, double intensity = 1.0);
    
    // Control developmental processes
    void advanceDevelopment(const std::string& stage);
    void setDevelopmentalSpeed(double speed);
    
    // Advanced memory operations
    void optimizeWorkingMemory(const std::string& strategy = "competitive");
    void replayMemorySequence(const std::vector<SimulationStep>& steps);
    
    // Brain state manipulation
    void modifyNeuralExcitability(const std::string& target, double factor);
    void createNeuralEnsemble(const std::vector<NeuronId>& neuronIds);
    void destroyNeuralEnsemble(const std::vector<NeuronId>& neuronIds);
    
    // Custom learning scenarios
    void createLearningScenario(const std::string& scenario, const std::map<std::string, double>& parameters);

private:
    std::shared_ptr<Brain> brain_;
};

// Advanced agent and world control
class AdvancedAgentControl {
public:
    AdvancedAgentControl(std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world)
        : agent_(agent), world_(world) {}
    ~AdvancedAgentControl() = default;
    
    // Control agent behavior
    void enableAgentSubsystems(const std::vector<std::string>& subsystems);
    void disableAgentSubsystems(const std::vector<std::string>& subsystems);
    void setAgentBehavior(const std::string& behavior, const std::map<std::string, double>& parameters);
    
    // Advanced world manipulation
    void createWorldObjects(const std::vector<WorldObject>& objects);
    void modifyWorldEnvironment(const std::string& environmentType);
    void setWorldDynamicParameters(const std::map<std::string, double>& parameters);
    
    // Advanced agent-world interaction
    void createCustomTask(const std::string& taskDescription);
    void setGoalDrivenBehavior(const std::string& goal, const std::vector<std::string>& subgoals);
    void enableMetaCognition(bool enable);
    
    // Custom sensory processing
    void configureSensoryProcessing(const std::string& modality, const std::map<std::string, double>& parameters);
    void addCustomSensoryInput(const std::string& type, const std::vector<float>& data);
    
    // Advanced motor control
    void enableComplexMotorActions(bool enable);
    void setActionSelectionStrategy(const std::string& strategy);
    void createCustomMotorCommands(const std::vector<Action>& commands);

private:
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
};

// Advanced simulation control
class AdvancedSimulationControl {
public:
    AdvancedSimulationControl(std::shared_ptr<Brain> brain, 
                              std::shared_ptr<AgentBrain> agent = nullptr,
                              std::shared_ptr<SimpleWorld> world = nullptr)
        : brain_(brain), agent_(agent), world_(world) {}
    ~AdvancedSimulationControl() = default;
    
    // Advanced simulation control
    void pauseSimulation(bool pause);
    void fastForward(double factor = 10.0);
    void slowMotion(double factor = 0.1);
    void setTimeWarp(const std::string& warpType, double intensity = 1.0);
    
    // Checkpoint and recovery
    void createCheckpoint(const std::string& name, const std::map<std::string, std::string>& metadata = {});
    bool loadCheckpoint(const std::string& name);
    void restoreFromCheckpoint(const std::string& name);
    
    // Advanced replay and consolidation
    void enableReplayMode(bool enable, const std::string& replayStrategy = "hippocampal");
    void configureReplayParameters(const std::map<std::string, double>& parameters);
    
    // Custom simulation phases
    void createCustomPhase(const std::string& phaseName, 
                          const std::function<void()>& phaseLogic,
                          double duration = 1.0);
    void addSimulationEvent(SimulationStep step, const std::function<void()>& eventLogic);
    
    // Advanced time manipulation
    void setTimeScale(double scale);
    void addTimeEvent(SimulationStep step, const std::string& eventType, const std::map<std::string, double>& data);
    void enableTimeCompression(const std::string& compressionType);
    
    // Advanced state manipulation
    void modifyBrainState(const std::map<std::string, double>& stateChanges);
    void applyGeneticMutation(double mutationRate = 0.01);
    void createEvolutionaryPopulation(size_t populationSize);
    
    // Custom simulation loops
    void createCustomSimulationLoop(const std::function<bool()>& condition,
                                    const std::function<void()>& stepLogic);
    void addObservationCallback(const std::function<void(const SensoryInput&)>& callback);
    void addActionCallback(const std::function<void(const Action&)>& callback);

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    std::map<std::string, std::function<void()>> simulationEvents;
    std::vector<std::pair<SimulationStep, std::function<void()>>> scheduledEvents;
};

// Advanced experiment control
class AdvancedExperimentControl {
public:
    AdvancedExperimentControl() = default;
    ~AdvancedExperimentControl() = default;
    
    // Advanced experiment design
    void createExperiment(const std::string& name, const std::string& hypothesis);
    void addExperimentalCondition(const std::string& conditionName,
                                 const std::map<std::string, std::string>& parameters);
    void addControlCondition(const std::string& conditionName);
    
    // Advanced data collection and analysis
    void configureDataCollection(const std::map<std::string, std::string>& dataTypes);
    void setAnalysisParameters(const std::map<std::string, double>& parameters);
    void addStatisticalAnalysis(const std::string& analysisType);
    
    // Advanced experiment execution
    void runExperiment(const std::string& experimentName, 
                      const std::map<std::string, std::string>& overrides = {});
    void runAdaptiveExperiment(const std::string& experimentName);
    void runHierarchicalExperiment(const std::string& experimentName);
    
    // Advanced results processing
    void applyMachineLearningToResults(const std::string& modelType);
    void generatePredictions(const std::string& predictionModel);
    void performCrossValidation(const std::string& validationType);
    
    // Advanced experiment management
    void saveExperimentResults(const std::string& filename);
    void loadExperimentResults(const std::string& filename);
    void compareExperiments(const std::vector<std::string>& experimentNames);
    void createMetaAnalysis(const std::vector<std::string>& experimentResults);

private:
    std::map<std::string, std::shared_ptr<Config>> experimentConfigs;
    std::map<std::string, std::vector<std::map<std::string, double>>> experimentalData;
};

// Advanced visualization and analysis
class AdvancedVisualization {
public:
    AdvancedVisualization(std::shared_ptr<Brain> brain) : brain_(brain) {}
    ~AdvancedVisualization() = default;
    
    // Advanced neural visualization
    void createNeuralNetworkVisualization(const std::string& outputFormat = "svg");
    void createActivityHeatmap(const std::string& outputFormat = "png");
    void createSpikingPatternVisualization(const std::string& outputFormat = "mp4");
    
    // Advanced memory visualization
    void createMemoryTraceVisualization(const std::string& outputFormat = "pdf");
    void createEpisodicMemoryVisualization(const std::string& outputFormat = "webgl");
    void createWorkingMemoryVisualization(const std::string& outputFormat = "3d");
    
    // Advanced simulation visualization
    void createTimeSeriesVisualization(const std::string& outputFormat = "html");
    void createStateSpaceVisualization(const std::string& outputFormat = "interactive");
    void createMultiScaleVisualization(const std::string& outputFormat = "360");
    
    // Advanced analysis visualization
    void createPlasticityTrajectoryVisualization(const std::string& outputFormat = "3d");
    void createLearningCurveVisualization(const std::string& outputFormat = "interactive");
    void createEnsembleBehaviorVisualization(const std::string& outputFormat = "vr");
    
    // Interactive visualization
    void createInteractiveDashboard(const std::string& port = "8080");
    void enableRealTimeVisualization(bool enable);
    void configureVisualizationSettings(const std::map<std::string, std::string>& settings);

private:
    std::shared_ptr<Brain> brain_;
};

// Advanced debugging and analysis
class AdvancedDebugTools {
public:
    AdvancedDebugTools(std::shared_ptr<Brain> brain) : brain_(brain) {}
    ~AdvancedDebugTools() = default;
    
    // Advanced debugging
    void enableDebugMode(const std::string& debugLevel = "full");
    void setDebugFilter(const std::string& filterExpression);
    void addDebugCallback(const std::function<void(const std::string&)>& callback);
    
    // Advanced analysis
    void performNeuralStabilityAnalysis();
    void analyzeNetworkTopology(const std::string& analysisType = "eigenvector");
    void detectCriticalNetworkNodes();
    void identifyBottleneckAreas();
    
    // Performance analysis
    void analyzeMemoryUsagePatterns();
    void analyzeComputationalComplexity();
    void identifyPerformanceBottlenecks();
    void profileStepExecutionTimes();
    
    // Advanced diagnostics
    void runConnectivityDiagnostics();
    void testPlasticityMechanisms();
    void verifyNeuromodulationIntegration();
    void validateMemorySystems();
    
    // Advanced error analysis
    void analyzeErrorPatterns(const std::string& errorType = "all");
    void identifySystemFailures();
    void predictSystemInstability();
    void suggestImprovements();

private:
    std::shared_ptr<Brain> brain_;
    std::vector<std::function<void(const std::string&)>> debugCallbacks;
};

// Advanced utility functions
class AdvancedUtils {
public:
    AdvancedUtils() = default;
    ~AdvancedUtils() = default;
    
    // Configuration utilities
    static std::string generateConfigHash(const std::shared_ptr<Config>& config);
    static bool validateConfigAgainstSchema(const std::shared_ptr<Config>& config,
                                           const std::string& schemaFile);
    static std::shared_ptr<Config> mergeConfigs(const std::vector<std::shared_ptr<Config>>& configs);
    
    // Data utilities
    static std::string serializeData(const std::map<std::string, double>& data);
    static std::map<std::string, double> deserializeData(const std::string& data);
    static std::vector<double> calculateStatistics(const std::vector<double>& data);
    
    // File utilities
    static bool backupFile(const std::string& source, const std::string& destination);
    static bool compareFiles(const std::string& file1, const std::string& file2);
    static std::string getFileChecksum(const std::string& filename);
    
    // Network utilities
    static bool checkNetworkConnectivity(const std::string& host, int port = 80);
    static std::string getLocalIPAddress();
    static std::vector<std::string> getNetworkInterfaces();
    
    // System utilities
    static double getSystemLoadAverage();
    static size_t getAvailableMemory();
    static std::string getSystemInfo();
    
    // Timing utilities
    static std::chrono::system_clock::time_point getSystemTime();
    static double timeSince(const std::chrono::system_clock::time_point& startTime);
    static std::string formatTime(double seconds);
    
    // String utilities
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Math utilities
    static double clamp(double value, double min, double max);
    static double smoothstep(double x, double edge0, double edge1);
    static std::vector<double> normalize(const std::vector<double>& values);
    static double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    
    // Validation utilities
    static bool isValidConfigKey(const std::string& key);
    static bool isValidNeuronId(const NeuronId& id);
    static bool isValidSynapseId(const SynapseId& id);
    
    // Configuration utilities
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createMinimalConfig();
    static std::shared_ptr<Config> createExpertConfig();
    
    // Advanced configuration utilities
    static std::map<std::string, std::string> inferConfigFromData(const std::vector<std::vector<double>>& data);
    static std::shared_ptr<Config> optimizeConfigForPerformance(const std::shared_ptr<Config>& config);
    static std::shared_ptr<Config> optimizeConfigForMemory(const std::shared_ptr<Config>& config);
    static std::shared_ptr<Config> optimizeConfigForAccuracy(const std::shared_ptr<Config>& config);
    
    // Analysis utilities
    static std::string analyzeConfig(const std::shared_ptr<Config>& config);
    static double evaluateConfig(const std::shared_ptr<Config>& config, 
                                const std::map<std::string, double>& metrics);
    static std::vector<std::string> suggestConfigImprovements(const std::shared_ptr<Config>& config);
};

} // namespace nlm

#endif // NLM_ADVANCED_FEATURES_H
