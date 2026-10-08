#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;
class Experiment;
class Config;

// Enums for advanced commands
enum class ExperimentType {
    ConnectivityExperiment,
    PlasticityExperiment,
    MemoryExperiment,
    DevelopmentExperiment,
    NeuromodulationExperiment,
    CognitiveExperiment
};

enum class VisualizationFormat {
    JSON,
    CSV,
    Custom,
    Binary
};

enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult
};

enum class SynapseType {
    Excitatory,
    Inhibitory
};

enum class NeuronType {
    Sensory,
    Motor,
    Internal,
    Excitatory,
    Inhibitory,
    Modulatory
};

// Data structures for advanced commands

struct SimulationControlParams {
    float timestep = 0.001f;
    size_t maxSteps = 10000;
    bool eventDriven = true;
    bool parallelProcessing = false;
    size_t resourceLimit = 1000;
    float temperature = 1.0f;
    float pressure = 1.0f;
    std::string optimizationStrategy = "auto";
};

struct NetworkTopologyAnalysis {
    size_t nodeCount = 0;
    size_t edgeCount = 0;
    float averageDegree = 0.0f;
    float clusteringCoefficient = 0.0f;
    float averagePathLength = 0.0f;
    float modularity = 0.0f;
    float smallWorldSigma = 0.0f;
    std::vector<NeuronId> hubs;
    std::vector<NeuronId> bottlenecks;
    std::string analysisTimestamp;
};

struct STDPPatternAnalysis {
    std::vector<float> potentiationPatterns;
    std::vector<float> depressionPatterns;
    float strength = 0.0f;
    float LTPRatio = 0.0f;
    float LTDRatio = 0.0f;
    std::string analysisTimestamp;
};

struct HebbianPatternAnalysis {
    std::vector<float> weightEvolution;
    std::vector<float> correlationPatterns;
    float strength = 0.0f;
    float learningRate = 0.0f;
    float stability = 0.0f;
    std::string analysisTimestamp;
};

struct StructuralPlasticityAnalysis {
    float synaptogenesisRate = 0.0f;
    float pruningRate = 0.0f;
    float structuralPlasticityScore = 0.0f;
    std::vector<NeuronId> newSynapses;
    std::vector<NeuronId> prunedSynapses;
    std::string analysisTimestamp;
};

struct MemorySystemAnalysis {
    float workingMemoryUtil = 0.0f;
    float workingMemoryStability = 0.0f;
    size_t workingMemoryCapacity = 0;
    float episodicMemoryConsolidation = 0.0f;
    float episodicMemoryReplay = 0.0f;
    size_t episodicMemoryCapacity = 0;
    float associativeMemoryPatternStrength = 0.0f;
    float associativeMemoryInterference = 0.0f;
    size_t associativeMemoryCapacity = 0;
    std::string analysisTimestamp;
};

struct ConnectivityPattern {
    enum class Type {
        Random,
        ScaleFree,
        SmallWorld,
        Modular,
        Hierarchical,
        Custom
    };
    
    Type type = Type::Random;
    float density = 0.1f;
    float clustering = 0.3f;
    float pathLength = 2.5f;
    size_t modularity = 3;
    std::unordered_map<NeuronId, std::vector<NeuronId>> customConnections;
};

struct NeuromodulationTuning {
    float dopamineTarget = 1.0f;
    float curiosityTarget = 0.5f;
    float noveltyTarget = 0.3f;
    float predictionErrorTarget = 0.2f;
    float learningRate = 0.01f;
    float adaptationSpeed = 0.1f;
    bool homeostatic = true;
    std::unordered_map<std::string, float> customParameters;
};

struct ExperimentConfig {
    std::string name;
    ExperimentType type;
    std::unordered_map<std::string, std::string> parameters;
    size_t duration = 1000;
    std::string description;
    bool enabled = true;
};

struct ExperimentResult {
    std::string experimentName;
    ExperimentType experimentType;
    bool success = false;
    size_t duration = 0;
    std::unordered_map<std::string, float> metrics;
    std::string errorMessage;
    std::string timestamp;
};

struct VisualizationExportOptions {
    VisualizationFormat format = VisualizationFormat::JSON;
    bool includeNeurons = true;
    bool includeSynapses = true;
    bool includeMembranePotentials = true;
    bool includeSpikes = true;
    std::string outputPath = "./exports";
    float resolution = 1.0f;
    bool highQuality = false;
    std::unordered_map<std::string, std::string> customOptions;
};

struct MonitorConfiguration {
    float samplingRate = 10.0f; // Hz
    size_t bufferSize = 1000;
    bool alertThresholdsEnabled = true;
    std::unordered_map<std::string, float> thresholds;
    bool performanceMonitoring = true;
    bool resourceMonitoring = true;
    std::string outputInterface = "console";
    bool saveToFile = false;
    std::string logFilePath = "./logs/monitor.log";
};

struct DiscoveryParameters {
    size_t minSampleSize = 100;
    float significanceLevel = 0.05f;
    size_t maxIterations = 1000;
    bool enableStatisticalTesting = true;
    std::vector<std::string> propertyTypes;
    float convergenceThreshold = 0.01f;
};

struct SimulationControlParams {
    float timestep = 0.001f;
    size_t maxSteps = 10000;
    bool eventDriven = true;
    bool parallelProcessing = false;
    size_t resourceLimit = 1000;
    float temperature = 1.0f;
    float pressure = 1.0f;
    std::string optimizationStrategy = "auto";
};

struct RollbackOptions {
    bool useCheckpoints = true;
    float rollbackStepPercentage = 0.1f;
    bool validateAfterRollback = true;
    std::string backupLocation = "./backups";
};

struct NetworkTopologyAnalysis {
    size_t nodeCount = 0;
    size_t edgeCount = 0;
    float averageDegree = 0.0f;
    float clusteringCoefficient = 0.0f;
    float averagePathLength = 0.0f;
    float modularity = 0.0f;
    float smallWorldSigma = 0.0f;
    std::vector<NeuronId> hubs;
    std::vector<NeuronId> bottlenecks;
    std::string analysisTimestamp;
};

struct PlasticityPatternAnalysis {
    STDPPatternAnalysis stdpPatterns;
    HebbianPatternAnalysis hebbianPatterns;
    StructuralPlasticityAnalysis structuralPatterns;
    std::vector<NeuronId> plasticityHotspots;
    std::string analysisTimestamp;
};

struct MemorySystemAnalysis {
    float workingMemoryUtil = 0.0f;
    float workingMemoryStability = 0.0f;
    size_t workingMemoryCapacity = 0;
    float episodicMemoryConsolidation = 0.0f;
    float episodicMemoryReplay = 0.0f;
    size_t episodicMemoryCapacity = 0;
    float associativeMemoryPatternStrength = 0.0f;
    float associativeMemoryInterference = 0.0f;
    size_t associativeMemoryCapacity = 0;
    std::string analysisTimestamp;
};

struct EmergentPropertyReport {
    std::string reportId;
    std::string timestamp;
    std::vector<std::string> discoveredProperties;
    std::vector<std::string> emergentBehaviors;
    float confidence = 0.0f;
    std::unordered_map<std::string, float> propertyStrengths;
    std::string explanation;
};

struct BrainSnapshot {
    Timestamp timestamp;
    SimulationStep step;
    std::vector<NeuronState> neuronStates;
    std::vector<SynapseState> synapseStates;
    std::vector<MemoryTrace> memoryTraces;
    float totalEnergy = 0.0f;
    float averageFiringRate = 0.0f;
    std::unordered_map<std::string, float> metrics;
    std::string metadata;
};

struct StateComparisonAnalysis {
    std::string analysisId;
    std::string timestamp;
    std::vector<std::string> comparedStates;
    std::vector<std::string> differences;
    std::vector<std::string> similarities;
    float stabilityScore = 0.0f;
    std::string conclusion;
};

struct MonitoringThresholds {
    float spikeRateThreshold = 100.0f;
    float membranePotentialThreshold = 20.0f;
    float memoryUtilizationThreshold = 0.8f;
    float energyConsumptionThreshold = 1000.0f;
    float performanceThreshold = 0.5f;
    bool enableRealTimeAlerts = true;
    std::string alertOutput = "console";
};

struct MultiScaleParams {
    float coarseTimestep = 0.01f;
    float fineTimestep = 0.001f;
    size_t coarseSteps = 100;
    size_t fineSteps = 1000;
    bool adaptiveTimeStepping = true;
    float accuracyThreshold = 0.01f;
};

struct StressTestConfig {
    enum class StressType {
        HighExcitability,
        HighInhibition,
        NetworkDismantling,
        ResourceStarvation,
        TemperatureStress,
        TemporalPressure
    };
    
    StressType type = StressType::HighExcitability;
    float stressLevel = 1.0f;
    size_t duration = 1000;
    bool measureResilience = true;
    bool measureRecovery = true;
    std::string description;
};

struct StressTestResult {
    std::string testId;
    std::string timestamp;
    bool passed = false;
    float resilienceScore = 0.0f;
    float recoveryTime = 0.0f;
    std::vector<std::string> failureModes;
    std::string analysis;
};

struct EvolutionConfig {
    size_t populationSize = 50;
    float mutationRate = 0.1f;
    float crossoverRate = 0.8f;
    size_t generations = 100;
    float fitnessThreshold = 0.9f;
    std::string selectionMethod = "tournament";
    bool elitism = true;
    std::string fitnessFunction = "performance";
};

struct EvolutionResult {
    std::string evolutionId;
    std::string timestamp;
    bool completed = false;
    size_t bestFitness = 0;
    size_t generations = 0;
    NeuronId bestIndividual = INVALID_NEURON_ID;
    std::vector<float> fitnessHistory;
    std::string evolutionAnalysis;
};

struct RLConfig {
    float learningRate = 0.01f;
    float discountFactor = 0.9f;
    size_t replayBufferSize = 10000;
    size_t batchSize = 32;
    size_t targetUpdateInterval = 100;
    float explorationRate = 0.1f;
    float minExplorationRate = 0.01f;
    size_t explorationDecaySteps = 10000;
    std::string algorithm = "DQN";
    std::string stateSpace = "continuous";
    std::string actionSpace = "discrete";
};

struct ReinforcementLearningResult {
    std::string sessionId;
    std::string timestamp;
    bool successful = false;
    float finalReward = 0.0f;
    size_t episodesCompleted = 0;
    float totalTrainingTime = 0.0f;
    std::vector<float> rewardHistory;
    std::string policyAnalysis;
};

struct MetaCognitiveParams {
    bool selfMonitor = true;
    float selfAwarenessThreshold = 0.5f;
    size_t reflectionInterval = 100;
    bool metaLearning = true;
    float metacognitivePrecision = 0.8f;
    std::string reflectionStrategy = "automatic";
};

struct MetaCognitionResult {
    std::string sessionId;
    std::string timestamp;
    bool engaged = false;
    float metacognitiveScore = 0.0f;
    size_t reflectionsCompleted = 0;
    float learningRate = 0.0f;
    std::string consciousnessReport;
};

struct QuantumParams {
    float coherenceTime = 0.001f;
    float decoherenceRate = 0.01f;
    float entanglementStrength = 0.1f;
    size_t quantumDepth = 5;
    bool simulateMeasurement = true;
    std::string quantumAlgorithm = "variational";
};

struct QuantumNeuralResult {
    std::string sessionId;
    std::string timestamp;
    bool completed = false;
    float quantumCoherence = 0.0f;
    float entanglementEntropy = 0.0f;
    float quantumSpeedup = 1.0f;
    std::string quantumBehavior;
    std::string classicalComparison;
};

struct RobustnessParameters {
    float attackIntensity = 1.0f;
    size_t attackDuration = 100;
    bool measureRecoveryTime = true;
    bool measurePerformanceImpact = true;
    std::string attackType = "random";
    float toleranceThreshold = 0.1f;
};

struct RobustnessAnalysis {
    std::string analysisId;
    std::string timestamp;
    bool robust = false;
    float robustnessScore = 0.0f;
    float recoveryTime = 0.0f;
    float performanceDrop = 0.0f;
    std::vector<std::string> vulnerabilities;
    std::string robustnessAssessment;
};

struct LearningCurveAnalysis {
    std::string analysisId;
    std::string timestamp;
    std::vector<std::pair<size_t, float>> learningPoints;
    float asymptoticPerformance = 0.0f;
    float learningRate = 0.0f;
    float stability = 0.0f;
    std::string learningPattern;
    std::string extrapolation;
};

struct PatternParameters {
    size_t windowSize = 100;
    float correlationThreshold = 0.5f;
    bool enablePatternMining = true;
    std::vector<std::string> patternTypes;
    bool detectAnomalies = true;
};

struct PatternRecognitionResult {
    std::string sessionId;
    std::string timestamp;
    bool successful = false;
    std::vector<std::string> detectedPatterns;
    float confidence = 0.0f;
    std::vector<std::pair<std::string, float>> patternStrengths;
    std::string interpretation;
};

struct CausalParameters {
    size_t interventionWindow = 50;
    float causalStrengthThreshold = 0.3f;
    bool useCounterfactuals = true;
    std::string causalModel = "bayesian";
    bool estimateLongTermEffects = true;
};

struct CausalAnalysisResult {
    std::string sessionId;
    std::string timestamp;
    bool causalLinksFound = false;
    std::vector<std::tuple<std::string, std::string, float>> causalRelations;
    float causalityScore = 0.0f;
    std::string causalModel;
    std::string interventionRecommendations;
};

struct PredictionParameters {
    size_t predictionHorizon = 100;
    bool useNeuralNetwork = true;
    size_t trainingDataSize = 1000;
    float predictionConfidence = 0.8f;
    std::string predictionAlgorithm = "LSTM";
    bool updatePredictions = true;
};

struct PredictiveModelResult {
    std::string modelId;
    std::string timestamp;
    bool accurate = false;
    float accuracyScore = 0.0f;
    std::vector<std::pair<size_t, float>> predictions;
    std::string predictionIntervals;
    std::string modelDiagnostics;
};

struct InformationParameters {
    bool computeTransferEntropy = true;
    bool computeMutualInformation = true;
    size_t informationWindow = 100;
    float informationThreshold = 0.01f;
    std::string informationTheoryModel = "Shannon";
    bool directionalAnalysis = true;
};

struct InformationTheoryResult {
    std::string sessionId;
    std::string timestamp;
    float transferEntropy = 0.0f;
    float mutualInformation = 0.0f;
    float informationFlow = 0.0f;
    std::vector<std::tuple<std::string, std::string, float>> informationChannels;
    std::string informationArchitecture;
};

struct ComplexNetworkParams {
    bool analyzeSmallWorld = true;
    bool analyzeScaleFree = true;
    bool analyzeCommunityStructure = true;
    bool analyzeCentrality = true;
    float resolution = 1.0f;
    std::string networkType = "functional";
    bool weightedAnalysis = true;
};

struct ComplexNetworkResult {
    std::string analysisId;
    std::string timestamp;
    bool successful = false;
    float smallWorldCoefficient = 0.0f;
    float scaleFreeExponent = 0.0f;
    size_t communityCount = 0;
    float averageClustering = 0.0f;
    float averagePathLength = 0.0f;
    std::vector<NeuronId> hubs;
    std::vector<NeuronId> bottlenecks;
    std::string networkClassification;
};

struct DebugConfiguration {
    bool enableBreakpoints = true;
    bool enableWatchVariables = true;
    size_t maxBreakpoints = 100;
    std::string debugOutput = "console";
    bool stepThroughSimulation = false;
    size_t debugStepCount = 10;
    bool logAllActions = true;
    std::string debuggerInterface = "cli";
};

struct PerformanceProfile {
    std::string profileId;
    std::string timestamp;
    float cpuUsage = 0.0f;
    float memoryUsage = 0.0f;
    float simulationSpeed = 0.0f;
    size_t stepsPerSecond = 0;
    float energyConsumption = 0.0f;
    std::vector<std::pair<std::string, float>> performanceMetrics;
    std::string performanceAnalysis;
};

struct MemoryLeakReport {
    std::string reportId;
    std::string timestamp;
    bool leaksFound = false;
    size_t totalLeaks = 0;
    float memoryLeakAmount = 0.0f;
    std::vector<std::string> leakLocations;
    std::string recommendation;
};

struct CodeSwapConfiguration {
    bool enableHotSwapping = true;
    bool backupBeforeSwap = true;
    std::string backupLocation = "./backups";
    bool validateAfterSwap = true;
    std::string compilationFlags = "-O2 -march=native";
    bool enableDebugging = true;
};

struct TestSuiteConfig {
    size_t totalTests = 100;
    float successRateThreshold = 0.95f;
    bool runPerformanceTests = true;
    bool runMemoryTests = true;
    bool runStressTests = true;
    std::string testOutputFormat = "json";
    std::string testReportPath = "./reports";
    bool generateDetailedReports = true;
};

struct TestSuiteResult {
    std::string suiteId;
    std::string timestamp;
    bool allTestsPassed = false;
    size_t totalTests = 0;
    size_t passedTests = 0;
    size_t failedTests = 0;
    float successRate = 0.0f;
    std::vector<std::string> failedTestNames;
    std::string suiteAnalysis;
};

struct LoggingConfiguration {
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    LogLevel minimumLevel = LogLevel::INFO;
    bool enableStructuredLogging = true;
    std::string logFormat = "json";
    size_t maxLogSize = 10000000; // 10MB
    size_t maxLogFiles = 5;
    bool enableConsoleOutput = true;
    bool enableFileOutput = true;
    std::string logFilePath = "./logs/brain.log";
    bool enablePerformanceLogging = true;
    bool enableMemoryLogging = true;
};

struct CLICommandSet {
    std::vector<std::string> basicCommands;
    std::vector<std::string> advancedCommands;
    std::vector<std::string> experimentalCommands;
    std::string commandPrefix = "nlm";
    bool enableTabCompletion = true;
    std::string helpFile = "./help/cli_help.txt";
};

struct WebInterfaceConfig {
    std::string host = "localhost";
    size_t port = 8080;
    bool enableWebSocket = true;
    bool enableHTTP = true;
    std::string apiVersion = "v1";
    bool enableAuthentication = false;
    std::string authenticationMethod = "token";
    bool enableCORS = true;
    std::string staticFilesPath = "./web/static";
    std::string templatePath = "./web/templates";
};

struct VisualizationConfig {
    enum class VisualizationType {
        NetworkGraph,
        ActivityHeatmap,
        SpikeRaster,
        WeightMatrix,
        MemoryStates,
        Neuromodulation,
        Development,
        Custom
    };
    
    VisualizationType type = VisualizationType::NetworkGraph;
    float updateInterval = 1.0f; // seconds
    bool enableRealTime = true;
    size_t windowWidth = 1920;
    size_t windowHeight = 1080;
    bool fullScreen = false;
    std::string backgroundColor = "black";
    bool enableAnnotations = true;
    std::string dataSource = "live";
    bool saveSnapshots = false;
    std::string snapshotPath = "./snapshots";
};

struct ExplorationConfig {
    bool enableInteractiveMode = true;
    bool enableManualControl = true;
    float explorationSpeed = 1.0f;
    bool showAdvancedControls = true;
    bool enableDataExport = true;
    std::string explorationMode = "guided";
    bool enableHints = true;
    std::string helpSystem = "contextual";
    bool enableUndo = true;
    size_t undoSteps = 10;
};

struct ScriptConfig {
    std::string scriptFormat = "python";
    bool enableVariableSubstitution = true;
    bool enableConditionalExecution = true;
    bool enableErrorHandling = true;
    std::string errorHandlingMode = "stop";
    bool enableLogging = true;
    std::string logFile = "./logs/scripts.log";
    bool validateSyntax = true;
    bool enableProgressReporting = true;
};

struct ScriptExecutionResult {
    std::string sessionId;
    std::string timestamp;
    bool successful = false;
    size_t linesExecuted = 0;
    float executionTime = 0.0f;
    std::vector<std::string> outputLines;
    std::vector<std::string> errorLines;
    std::string scriptAnalysis;
};

} // namespace nlm
