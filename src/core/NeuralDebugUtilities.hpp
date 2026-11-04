#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <unordered_map>
#include <stacktrace>

namespace nlm {

// Neural network analysis utilities
class NeuralNetworkAnalyzer {
public:
    struct ConnectionInfo {
        std::string sourceNeuron;
        std::string targetNeuron;
        double weight;
        double delay;
        std::string type; // "excitatory", "inhibitory", "modulatory"
        double recentActivity;
        bool enabled;
    };
    
    struct NeuronInfo {
        std::string id;
        std::string type;
        double membranePotential;
        double threshold;
        double firingRate;
        size_t spikeCount;
        std::vector<double> recentSpikes;
        std::map<std::string, double> internalState;
        std::vector<std::string> incomingConnections;
        std::vector<std::string> outgoingConnections;
    };
    
    struct Statistics {
        double meanWeight;
        double stddevWeight;
        double minWeight;
        double maxWeight;
        size_t totalConnections;
        std::map<std::string, size_t> connectionTypes;
        double averageFiringRate;
        size_t totalSpikes;
        double connectivity;
    };
    
    NeuralNetworkAnalyzer();
    ~NeuralNetworkAnalyzer();
    
    // Network loading and analysis
    bool loadFromBrain(const std::shared_ptr<Brain>& brain);
    bool loadFromFile(const std::string& filepath);
    bool loadFromNetwork(const std::string& networkData);
    
    // Basic network analysis
    std::vector<ConnectionInfo> getConnections() const;
    std::vector<NeuronInfo> getNeurons() const;
    Statistics computeStatistics() const;
    
    // Network topology analysis
    std::vector<std::vector<std::string>> findStronglyConnectedComponents() const;
    std::vector<std::vector<std::string>> detectCommunities() const;
    std::vector<std::string> findHubNodes(double threshold = 0.8) const;
    std::vector<std::string> findIsolatedNodes() const;
    
    // Connectivity analysis
    double computeConnectivity() const;
    std::map<size_t, size_t> getDegreeDistribution() const;
    double computeClusteringCoefficient() const;
    
    // Activity analysis
    double computeAverageFiringRate() const;
    double computeFiringRate(const std::string& neuronId) const;
    std::vector<double> getFiringRateTimeSeries(size_t steps) const;
    
    // Plasticity analysis
    struct PlasticityAnalysis {
        std::string neuronId;
        double hebbianLearningRate;
        double stdpLTPStrength;
        double stdpLTDStrength;
        double eligibilityTrace;
        double weightChangeRate;
        std::vector<double> recentWeightChanges;
    };
    
    PlasticityAnalysis analyzePlasticity(const std::string& neuronId) const;
    std::vector<PlasticityAnalysis> analyzeAllPlasticity() const;
    
    // Neuromodulation analysis
    struct NeuromodulationAnalysis {
        std::string neuromodulatorId;
        std::string targetNeuronId;
        double concentration;
        double effectStrength;
        double updateFrequency;
        std::vector<double> concentrationHistory;
    };
    
    NeuromodulationAnalysis analyzeNeuromodulation(const std::string& neuromodulatorId) const;
    std::vector<NeuromodulationAnalysis> analyzeAllNeuromodulation() const;
    
    // Network dynamics analysis
    struct DynamicsAnalysis {
        double totalActivity;
        double synchronyLevel;
        double stability;
        double learningRate;
        std::vector<double> frequencySpectrum;
        std::vector<double> autocorrelation;
    };
    
    DynamicsAnalysis analyzeDynamics(const std::vector<double>& timeSeries) const;
    DynamicsAnalysis analyzeNetworkDynamics() const;
    
    // Pattern analysis
    struct PatternAnalysis {
        std::string patternType;
        double patternStrength;
        std::vector<std::string> involvedNeurons;
        std::vector<double> patternWeights;
        double patternStability;
    };
    
    PatternAnalysis detectPattern(const std::string& patternType) const;
    std::vector<PatternAnalysis> detectAllPatterns() const;
    
    // Network visualization
    void visualizeNetwork(const std::string& outputFile, const std::string& format = "png",
                         const std::string& layout = "spring") const;
    void visualizeActivity(const std::string& outputFile, const std::string& format = "png",
                          size_t width = 800, size_t height = 600) const;
    void visualizePlasticity(const std::string& outputFile, const std::string& format = "png") const;
    
    // Network comparison
    struct NetworkComparison {
        std::string network1Id;
        std::string network2Id;
        double similarityScore;
        std::vector<std::string> commonConnections;
        std::vector<std::string> differentConnections;
        std::map<std::string, double> structuralDifferences;
    };
    
    NetworkComparison compareWith(const NeuralNetworkAnalyzer& other) const;
    
    // Network evolution
    struct EvolutionAnalysis {
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        double averageWeightChange;
        double plasticityEfficiency;
        std::vector<double> weightEvolution;
        std::vector<double> activityEvolution;
    };
    
    EvolutionAnalysis analyzeEvolution(const std::vector<ConnectionInfo>& previousConnections) const;
    
    // Export and import
    bool exportToGraphML(const std::string& filepath) const;
    bool exportToGEXF(const std::string& filepath) const;
    bool exportToJSON(const std::string& filepath) const;
    
    static NeuralNetworkAnalyzer importFromGraphML(const std::string& filepath);
    static NeuralNetworkAnalyzer importFromGEXF(const std::string& filepath);
    static NeuralNetworkAnalyzer importFromJSON(const std::string& filepath);
    
    // Real-time monitoring
    void startMonitoring(size_t intervalMs = 1000);
    void stopMonitoring();
    bool isMonitoring() const;
    
    struct RealTimeMetrics {
        double cpuUsage;
        double memoryUsage;
        double networkUsage;
        double diskUsage;
        std::chrono::system_clock::time_point timestamp;
        std::vector<double> firingRates;
        std::vector<double> weightChanges;
    };
    
    RealTimeMetrics getRealTimeMetrics() const;
    
    // Performance optimization
    void optimizeConnectivity() const;
    void optimizeWeightDistribution() const;
    void optimizePlasticity() const;
    
    // Debugging
    void enableDebugMode(bool enabled);
    bool isDebugModeEnabled() const;
    
    std::string debug() const;
    
    // Error handling
    bool hasError() const;
    std::string getError() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Brain state monitoring utilities
class BrainStateMonitor {
public:
    BrainStateMonitor();
    ~BrainStateMonitor();
    
    // Monitor brain state
    bool monitor(const std::shared_ptr<Brain>& brain, size_t intervalMs = 100);
    void stopMonitoring();
    bool isMonitoring() const;
    
    // State change detection
    struct StateChange {
        std::chrono::system_clock::time_point timestamp;
        std::string changeType; // "firing", "plasticity", "neuromodulation", "movement"
        std::string neuronId;
        std::string regionId;
        double magnitude;
        std::string description;
    };
    
    std::vector<StateChange> getStateChanges() const;
    std::vector<StateChange> getStateChanges(const std::string& type) const;
    std::vector<StateChange> getStateChanges(const std::string& neuronId) const;
    
    // State analysis
    struct StateAnalysis {
        double averageFiringRate;
        double averagePlasticityRate;
        double averageNeuromodulationEffect;
        size_t totalStateChanges;
        std::map<std::string, size_t> changeTypes;
        std::chrono::system_clock::time_point lastChange;
    };
    
    StateAnalysis analyzeState() const;
    
    // Anomaly detection
    struct Anomaly {
        std::chrono::system_clock::time_point timestamp;
        std::string type; // "high_firing", "abnormal_plasticity", "neuromodulation_spike"
        double severity;
        std::string description;
        std::vector<std::string> affectedNeurons;
    };
    
    std::vector<Anomaly> detectAnomalies() const;
    
    // State visualization
    void visualizeState(const std::string& outputFile, const std::string& format = "png") const;
    void visualizeActivity(const std::string& outputFile, const std::string& format = "png") const;
    void visualizeChanges(const std::string& outputFile, const std::string& format = "png") const;
    
    // State comparison
    struct StateComparison {
        std::chrono::system_clock::time_point time1;
        std::chrono::system_clock::time_point time2;
        double stateSimilarity;
        std::vector<std::string> changedNeurons;
        std::map<std::string, double> differenceMetrics;
    };
    
    StateComparison compareStates(const std::chrono::system_clock::time_point& time1,
                                 const std::chrono::system_clock::time_point& time2) const;
    
    // Export state
    bool exportState(const std::string& filepath) const;
    bool importState(const std::string& filepath);
    
    // State validation
    bool validateState() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Control
    void resetStateMonitor();
    void pauseMonitoring();
    void resumeMonitoring();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural plasticity analysis utilities
class PlasticityAnalyzer {
public:
    PlasticityAnalyzer();
    ~PlasticityAnalyzer();
    
    // Plasticity rules analysis
    struct PlasticityRule {
        std::string ruleId;
        std::string ruleType; // "hebbian", "stdp", "reward_modulated"
        std::string targetNeuronId;
        std::string targetSynapseId;
        double learningRate;
        double eligibilityTrace;
        double weightChange;
        std::chrono::system_clock::time_point lastApplied;
        std::vector<double> recentChanges;
    };
    
    std::vector<PlasticityRule> getPlasticityRules() const;
    PlasticityRule getPlasticityRule(const std::string& ruleId) const;
    
    // STDP analysis
    struct STDPData {
        std::chrono::system_clock::time_point preSpikeTime;
        std::chrono::system_clock::time_point postSpikeTime;
        double weightChange;
        std::string synapseId;
        std::string preNeuronId;
        std::string postNeuronId;
    };
    
    std::vector<STDPData> getSTDPData() const;
    double computeSTDPEffect(const STDPData& stdp) const;
    
    // Hebbian analysis
    struct HebbianData {
        std::string neuronId1;
        std::string neuronId2;
        double correlation;
        double weightChange;
        std::chrono::system_clock::time_point lastCorrelation;
    };
    
    std::vector<HebbianData> getHebbianData() const;
    
    // Reward-modulated plasticity analysis
    struct RewardModulatedData {
        std::string neuronId;
        double rewardPredictionError;
        double dopamineLevel;
        double weightChange;
        std::chrono::system_clock::time_point rewardTime;
    };
    
    std::vector<RewardModulatedData> getRewardModulatedData() const;
    
    // Plasticity efficiency
    struct PlasticityEfficiency {
        double learningRate;
        double stability;
        double adaptability;
        double specificity;
        double generalization;
    };
    
    PlasticityEfficiency computePlasticityEfficiency() const;
    
    // Plasticity optimization
    void optimizePlasticityRules() const;
    void balancePlasticity() const;
    
    // Plasticity visualization
    void visualizePlasticityRules(const std::string& outputFile, const std::string& format = "png") const;
    void visualizeSTDP(const std::string& outputFile, const std::string& format = "png") const;
    void visualizeHebbian(const std::string& outputFile, const std::string& format = "png") const;
    
    // Plasticity comparison
    struct PlasticityComparison {
        std::string rule1Id;
        std::string rule2Id;
        double similarity;
        std::map<std::string, double> parameterDifferences;
        std::string recommendation;
    };
    
    PlasticityComparison compareRules(const std::string& rule1Id, const std::string& rule2Id) const;
    
    // Plasticity validation
    bool validatePlasticityRules() const;
    std::vector<std::string> getPlasticityValidationErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neuromodulation analysis utilities
class NeuromodulationAnalyzer {
public:
    NeuromodulationAnalyzer();
    ~NeuromodulationAnalyzer();
    
    // Neuromodulator data
    struct NeuromodulatorData {
        std::string id;
        std::string type; // "dopamine", "serotonin", "acetylcholine"
        double baselineConcentration;
        double currentConcentration;
        double effectStrength;
        std::chrono::system_clock::time_point lastUpdate;
        std::vector<double> concentrationHistory;
    };
    
    std::vector<NeuromodulatorData> getNeuromodulators() const;
    NeuromodulatorData getNeuromodulator(const std::string& id) const;
    
    // Target analysis
    struct TargetAnalysis {
        std::string neuromodulatorId;
        std::string targetNeuronId;
        double effect;
        double sensitivity;
        std::chrono::system_clock::time_point lastEffect;
    };
    
    std::vector<TargetAnalysis> getTargetAnalysis(const std::string& neuromodulatorId) const;
    
    // Neuromodulation effects
    struct NeuromodulationEffect {
        std::string neuromodulatorId;
        std::string targetId;
        double magnitude;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, double> parameters;
    };
    
    std::vector<NeuromodulationEffect> getNeuromodulationEffects() const;
    
    // Neuromodulation dynamics
    struct NeuromodulationDynamics {
        double productionRate;
        double degradationRate;
        double uptakeRate;
        double releaseRate;
        double baseline;
        double peak;
        double trough;
    };
    
    NeuromodulationDynamics analyzeDynamics(const std::string& neuromodulatorId) const;
    
    // Neuromodulation optimization
    void optimizeNeuromodulation() const;
    void balanceNeuromodulation() const;
    
    // Neuromodulation visualization
    void visualizeNeuromodulators(const std::string& outputFile, const std::string& format = "png") const;
    void visualizeEffects(const std::string& outputFile, const std::string& format = "png") const;
    void visualizeDynamics(const std::string& outputFile, const std::string& format = "png") const;
    
    // Neuromodulation comparison
    struct NeuromodulationComparison {
        std::string neuromod1Id;
        std::string neuromod2Id;
        double similarity;
        std::map<std::string, double> parameterDifferences;
        std::string recommendation;
    };
    
    NeuromodulationComparison compareNeuromodulators(const std::string& neuromod1Id, const std::string& neuromod2Id) const;
    
    // Neuromodulation validation
    bool validateNeuromodulators() const;
    std::vector<std::string> getNeuromodulationValidationErrors() const;
    
    // Neuromodulation control
    void resetNeuromodulators() const;
    void pauseNeuromodulation() const;
    void resumeNeuromulation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural network debugging utilities
class NeuralDebugUtilities {
public:
    NeuralDebugUtilities();
    ~NeuralDebugUtilities();
    
    // Debug mode control
    void enableDebugMode(bool enabled);
    bool isDebugModeEnabled() const;
    
    // Debug information
    struct DebugInfo {
        std::chrono::system_clock::time_point timestamp;
        std::string sessionId;
        std::string debugLevel; // "basic", "detailed", "comprehensive"
        std::map<std::string, std::string> systemInfo;
        std::vector<std::string> activeDebugCommands;
        bool performanceProfilingEnabled;
        bool memoryTrackingEnabled;
        bool networkTracingEnabled;
    };
    
    DebugInfo getDebugInfo() const;
    
    // Debug commands
    std::string executeDebugCommand(const std::string& command);
    bool registerDebugCommand(const std::string& name, const std::string& description,
                             const std::function<std::string(const std::vector<std::string>&)>& handler);
    
    // Debug logging
    void setDebugLogLevel(EnhancedLogLevel level);
    EnhancedLogLevel getDebugLogLevel() const;
    
    // Debug session management
    void startDebugSession();
    void endDebugSession();
    bool isDebugSessionActive() const;
    
    // Real-time debugging
    void startRealTimeDebug(size_t intervalMs = 500);
    void stopRealTimeDebug();
    bool isRealTimeDebugEnabled() const;
    
    // Debug visualization
    void visualizeDebugInfo(const std::string& outputFile, const std::string& format = "png") const;
    void visualizeNetworkState(const std::string& outputFile, const std::string& format = "png") const;
    void visualizeActivity(const std::string& outputFile, const std::string& format = "png") const;
    
    // Debug export
    bool exportDebugInfo(const std::string& filepath) const;
    bool importDebugInfo(const std::string& filepath);
    
    // Debug control
    void resetDebugSession();
    void pauseDebugging();
    void resumeDebugging();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural network testing utilities
class NeuralTestUtilities {
public:
    NeuralTestUtilities();
    ~NeuralTestUtilities();
    
    // Test brain functionality
    bool runBasicConnectivityTest(const std::shared_ptr<Brain>& brain) const;
    bool runPlasticityTest(const std::shared_ptr<Brain>& brain) const;
    bool runNeuromodulationTest(const std::shared_ptr<Brain>& brain) const;
    bool runMemoryTest(const std::shared_ptr<Brain>& brain) const;
    
    // Performance tests
    struct PerformanceTest {
        std::string testName;
        size_t neuronCount;
        size_t synapseCount;
        double expectedPerformance;
        double actualPerformance;
        double efficiency;
        std::chrono::duration<double> executionTime;
    };
    
    PerformanceTest runPerformanceTest(const std::shared_ptr<Brain>& brain) const;
    
    // Stress tests
    struct StressTest {
        std::string testName;
        size_t neuronCount;
        double stressLevel;
        double stability;
        std::chrono::duration<double> executionTime;
    };
    
    StressTest runStressTest(const std::shared_ptr<Brain>& brain, double stressLevel) const;
    
    // Accuracy tests
    struct AccuracyTest {
        std::string testName;
        double expectedOutput;
        double actualOutput;
        double accuracy;
        std::string errorMessage;
    };
    
    AccuracyTest runAccuracyTest(const std::shared_ptr<Brain>& brain, const std::string& testScenario) const;
    
    // Test suites
    struct TestSuite {
        std::string name;
        std::vector<PerformanceTest> performanceTests;
        std::vector<StressTest> stressTests;
        std::vector<AccuracyTest> accuracyTests;
        double overallScore;
        bool allPassed;
    };
    
    TestSuite runTestSuite(const std::shared_ptr<Brain>& brain) const;
    
    // Test validation
    bool validateTests() const;
    std::vector<std::string> getTestValidationErrors() const;
    
    // Test export
    bool exportTestResults(const std::string& filepath) const;
    bool importTestResults(const std::string& filepath);
    
    // Test analysis
    struct TestAnalysis {
        double averagePerformance;
        double averageAccuracy;
        double averageStability;
        std::map<std::string, double> testScores;
        std::string overallAssessment;
    };
    
    TestAnalysis analyzeTestResults(const TestSuite& testSuite) const;
    
    // Test comparison
    struct TestComparison {
        std::string test1Id;
        std::string test2Id;
        double similarity;
        std::map<std::string, double> parameterDifferences;
        std::string recommendation;
    };
    
    TestComparison compareTests(const std::string& test1Id, const std::string& test2Id) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural network validation utilities
class NeuralValidator {
public:
    NeuralValidator();
    ~NeuralValidator();
    
    // Network validation
    struct ValidationResult {
        std::string testName;
        bool passed;
        std::string errorMessage;
        std::chrono::duration<double> executionTime;
    };
    
    ValidationResult validateNetwork(const std::shared_ptr<Brain>& brain) const;
    std::vector<ValidationResult> validateAll(const std::shared_ptr<Brain>& brain) const;
    
    // Topology validation
    ValidationResult validateTopology(const std::shared_ptr<Brain>& brain) const;
    ValidationResult validateConnectivity(const std::shared_ptr<Brain>& brain) const;
    ValidationResult validatePlasticity(const std::shared_ptr<Brain>& brain) const;
    
    // Activity validation
    ValidationResult validateActivity(const std::shared_ptr<Brain>& brain) const;
    ValidationResult validateFiringRates(const std::shared_ptr<Brain>& brain) const;
    ValidationResult validateSynapticActivity(const std::shared_ptr<Brain>& brain) const;
    
    // Resource validation
    ValidationResult validateResources(const std::shared_ptr<Brain>& brain) const;
    ValidationResult validateMemory(const std::shared_ptr<Brain>& brain) const;
    ValidationResult validatePerformance(const std::shared_ptr<Brain>& brain) const;
    
    // Consistency validation
    ValidationResult validateConsistency(const std::shared_ptr<Brain>& brain) const;
    ValidationResult validateTemporalConsistency(const std::shared_ptr<Brain>& brain) const;
    ValidationResult validateSpatialConsistency(const std::shared_ptr<Brain>& brain) const;
    
    // Validation summary
    struct ValidationSummary {
        size_t totalTests;
        size_t passedTests;
        size_t failedTests;
        double successRate;
        std::chrono::duration<double> totalExecutionTime;
        std::vector<std::string> failedTestNames;
        std::string overallStatus;
    };
    
    ValidationSummary getValidationSummary(const std::vector<ValidationResult>& results) const;
    
    // Validation export
    bool exportValidationResults(const std::vector<ValidationResult>& results, const std::string& filepath) const;
    bool importValidationResults(std::vector<ValidationResult>& results, const std::string& filepath);
    
    // Validation comparison
    struct ValidationComparison {
        std::string brain1Id;
        std::string brain2Id;
        double similarity;
        std::map<std::string, double> parameterDifferences;
        std::string recommendation;
    };
    
    ValidationComparison compareValidation(const std::vector<ValidationResult>& results1,
                                          const std::vector<ValidationResult>& results2) const;
    
    // Validation optimization
    void optimizeForValidation(const std::shared_ptr<Brain>& brain) const;
    void improveValidationAccuracy(const std::shared_ptr<Brain>& brain) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural network export/import utilities
class NeuralExportImport {
public:
    NeuralExportImport();
    ~NeuralExportImport();
    
    // Brain export
    bool exportBrain(const std::shared_ptr<Brain>& brain, const std::string& filepath) const;
    bool exportBrainState(const std::shared_ptr<Brain>& brain, const std::string& filepath) const;
    bool exportBrainNetwork(const std::shared_ptr<Brain>& brain, const std::string& filepath) const;
    
    // Brain import
    std::shared_ptr<Brain> importBrain(const std::string& filepath);
    bool importBrainState(std::shared_ptr<Brain>& brain, const std::string& filepath);
    bool importBrainNetwork(std::shared_ptr<Brain>& brain, const std::string& filepath);
    
    // Network export
    bool exportNetwork(const NeuralNetworkAnalyzer& analyzer, const std::string& filepath) const;
    bool exportNetworkGraph(const NeuralNetworkAnalyzer& analyzer, const std::string& filepath) const;
    bool exportNetworkData(const NeuralNetworkAnalyzer& analyzer, const std::string& filepath) const;
    
    // Network import
    NeuralNetworkAnalyzer importNetwork(const std::string& filepath);
    NeuralNetworkAnalyzer importNetworkGraph(const std::string& filepath);
    NeuralNetworkAnalyzer importNetworkData(const std::string& filepath);
    
    // Session export/import
    bool exportSession(const std::string& sessionName, const std::string& filepath) const;
    bool importSession(std::string& sessionName, const std::string& filepath);
    
    // Validation export/import
    bool exportValidation(const NeuralValidator::ValidationSummary& summary, const std::string& filepath) const;
    bool importValidation(NeuralValidator::ValidationSummary& summary, const std::string& filepath);
    
    // Test export/import
    bool exportTestResults(const NeuralTestUtilities::TestSuite& testSuite, const std::string& filepath) const;
    bool importTestResults(NeuralTestUtilities::TestSuite& testSuite, const std::string& filepath);
    
    // Backup and restore
    bool createBackup(const std::string& sourceFile, const std::string& backupFile) const;
    bool restoreBackup(const std::string& backupFile, const std::string& targetFile) const;
    
    // Archive management
    bool createArchive(const std::vector<std::string>& files, const std::string& archiveFile) const;
    bool extractArchive(const std::string& archiveFile, const std::string& outputDir) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural network utilities factory
class NeuralUtilitiesFactory {
public:
    NeuralUtilitiesFactory();
    ~NeuralUtilitiesFactory();
    
    // Create analyzers
    static std::unique_ptr<NeuralNetworkAnalyzer> createNetworkAnalyzer();
    static std::unique_ptr<BrainStateMonitor> createBrainStateMonitor();
    static std::unique_ptr<PlasticityAnalyzer> createPlasticityAnalyzer();
    static std::unique_ptr<NeuromodulationAnalyzer> createNeuromodulationAnalyzer();
    static std::unique_ptr<NeuralDebugUtilities> createNeuralDebugUtilities();
    static std::unique_ptr<NeuralTestUtilities> createNeuralTestUtilities();
    static std::unique_ptr<NeuralValidator> createNeuralValidator();
    static std::unique_ptr<NeuralExportImport> createNeuralExportImport();
    
    // Configuration
    void setDefaultConfig(const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getDefaultConfig() const;
    
    // Resource management
    void cleanup();
    size_t getResourceUsage() const;
    
    // Version control
    std::string getVersion() const;
    void setVersion(const std::string& version);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm