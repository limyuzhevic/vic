# NLM Comprehensive Performance Benchmark Suite
// 
// Advanced benchmarking system for NLM (Neural Learning Machine) providing comprehensive
// performance analysis across all integrated brain systems.
// 
// This benchmark suite goes beyond simple speed testing to provide:
// 1. System integration performance
// 2. Memory system efficiency
// 3. Prediction accuracy and speed
// 4. Cognitive processing performance
// 5. Neuromodulation effectiveness
// 6. Development progression metrics
// 7. Persistence reliability
// 8. Comparative performance analysis
// 9. Stress testing and stability validation
// 10. Long-running behavior analysis
// 
// Designed for researchers, developers, and advanced users who need to understand
// performance characteristics of the integrated NLM brain systems.

#ifndef BENCHMARK_SUITE_HPP
#define BENCHMARK_SUITE_HPP

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "development/DevelopmentSystem.hpp"
#include "performance/CheckpointSystem.hpp"
#include "integration/BrainIntegrationManager.hpp"
#include "core/error_handling/ErrorHandling.hpp"
#include <chrono>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <fstream>

namespace nlm {

// Comprehensive benchmark configuration
struct BenchmarkConfig {
    // Test scenarios
    enum class Scenario {
        BASIC_SIMULATION,      // Basic neural simulation
        MEMORY_INTENSIVE,       // Heavy memory operations
        PREDICTION_FOCUSED,     // Prediction system intensive
        COGNITIVE_INTENSIVE,    // High cognitive load
        NEUROMODULATION_HEAVY, // Neuromodulation intensive
        DEVELOPMENT_INTENSIVE,  // Development progression
        PERSISTENCE_STRESS,     // Persistence under stress
        INTEGRATION_COMPREHENSIVE, // Full integration test
        REALISTIC_BEHAVIOR,    // Realistic agent behavior
        STRESS_TESTING,        // System stress testing
        LONG_RUNNING,          // Long-term stability testing
    };
    
    // Performance parameters
    size_t neuronCount = 1000;
    size_t steps = 5000;
    size_t memoryCapacity = 10000;
    size_t predictionHistory = 1000;
    double timestep = 0.001;
    double simulationTime = 10.0;
    
    // Scenario-specific parameters
    Scenario scenario = Scenario::INTEGRATION_COMPREHENSIVE;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool enableNeuromodulation = true;
    bool enableCognition = true;
    bool enablePrediction = true;
    bool enableMemory = true;
    
    // Performance measurement
    enum class MeasurementType {
        THROUGHPUT,           // Operations per second
        ACCURACY,             // Success rate and correctness
        EFFICIENCY,           // Resource utilization
        LATENCY,             // Response times
        SCALABILITY,          // Performance scaling
        STABILITY,            // Consistency over time
        RELIABILITY,          // Failure rates
        QUALITY,             // Output quality
    };
    
    // Test characteristics
    double durationMultiplier = 1.0;  // Scale test duration
    size_t concurrentTests = 1;       // Number ETH of parallel tests
    bool detailedProfiling = true;    // Detailed performance profiling
    bool includeStressTests = true;   // Include stress scenarios
    bool trackMemoryUsage = true;     // Track memory consumption
    bool trackCPUUsage = true;        // Track CPU utilization
    
    // Configuration validation
    bool validate() const;
    
    // Scenario presets
    static BenchmarkConfig createScenario(Scenario scenario);
    static BenchmarkConfig createMinimal();
    static BenchmarkConfig createStandard();
    static BenchmarkConfig createComprehensive();
    static BenchmarkConfig createStressTest();
    static BenchmarkConfig createLongRunning();
};

// Comprehensive benchmark results
struct BenchmarkResults {
    // Basic performance metrics
    double totalTime;              // Total execution time (seconds)
    double cpuTime;               // CPU time (seconds)
    double wallClockTime;         // Wall clock time (seconds)
    size_t stepsExecuted;         // Number of simulation steps
    size_t neuronsSimulated;      // Number of neurons
    size_t totalSynapses;         // Total synapses
    
    // Performance metrics by system
    struct SystemMetrics {
        double throughput;          // Operations per second
        double efficiency;          // Resource efficiency
        double accuracy;            // Success/correctness rate
        double latency;             // Average response time
        double stability;           // Consistency measure
        double reliability;         // Failure-free operation
        double quality;             // Output quality
        double scalability;         // Performance scaling
        
        // Detailed breakdown by subsystem
        struct MemoryMetrics {
            double workingMemoryEfficiency;
            double episodicMemoryPerformance;
            double memoryRetentionRate;
            double consolidationSpeed;
            double replayEffectiveness;
        } memory;
        
        struct PredictionMetrics {
            double predictionAccuracy;
            double predictionSpeed;
            double predictionErrorRate;
            double confidenceCalibration;
            double multiStepPrediction;
        } prediction;
        
        struct CognitiveMetrics {
            double planningEfficiency;
            double conceptFormationAccuracy;
            double attentionAccuracy;
            double selfModelQuality;
            double socialLearningSpeed;
        } cognition;
        
        struct NeuromodulationMetrics {
            double neuromodulationEffectiveness;
            double rewardLearningSpeed;
            double noveltyDetectionSpeed;
            double curiosityIntegration;
            double predictionErrorUtilization;
        } neuromodulation;
        
        struct DevelopmentMetrics {
            double developmentProgression;
            double maturationSpeed;
            double pruningEfficiency;
            double synaptogenesisRate;
            double learningRateEnhancement;
        } development;
        
        struct IntegrationMetrics {
            double systemConnectivity;
            double integrationLatency;
            double crossSystemPerformance;
            double feedbackLoopEffectiveness;
            double overallIntegrationScore;
        } integration;
    } system;
    
    // Resource usage metrics
    struct ResourceMetrics {
        double peakMemoryUsage;      // MB
        double averageMemoryUsage;  // MB
        double peakCPUUsage;         // %
        double averageCPUUsage;     // %
        double memoryEfficiency;    // MB per operation
        double cpuEfficiency;       // % per operation
        double memoryGrowthRate;    // MB per second
        double cpuGrowthRate;       // % per second
    } resources;
    
    // Time-based metrics
    struct TimeMetrics {
        double avgStepTime;         // ms per step
        double maxStepTime;        // ms
        double minStepTime;        // ms
        double stepTimeStdDev;     // ms
        double timeToFirstSpike;  // ms
        double steadyStateTime;    // ms
        double convergenceTime;   // ms
        std::vector<double> stepTimes; // Detailed step times
    } time;
    
    // Spike and activity metrics
    struct ActivityMetrics {
        double totalSpikes;         // Total spike count
        double avgSpikesPerStep;   // Spikes per step
        double spikeFrequency;     // Hz
        double firingRate;         // Hz
        double excitationInhibitionRatio;
        size_t activeNeurons;      // Currently active neurons
        size_t maxActiveNeurons;   // Peak active neurons
        std::vector<double> firingRates; // Per-neuron firing rates
    } activity;
    
    // Memory and learning metrics
    struct LearningMetrics {
        double memoryRetention;    // Memory retention rate
        double learningCurve;     // Learning progress
        double plasticChange;     // Synaptic change rate
        double adaptationRate;    // Adaptation speed
        double generalization;    // Generalization capability
        double transferLearning;  // Transfer learning ability
        double skillAcquisition;  // Skill acquisition rate
        double expertiseDevelopment; // Expertise development
    } learning;
    
    // Stress test metrics
    struct StressMetrics {
        double stressLevel;         // Current stress level
        double resilience;         // System resilience
        double recoveryTime;       // Recovery from stress
        double stabilityUnderStress; // Performance under stress
        std::vector<double> stressResponses; // Response to different stress types
    } stress;
    
    // Comparative analysis
    struct ComparativeMetrics {
        std::map<std::string, double> baselineComparisons;      // vs baseline
        std::map<std::string, double> scenarioComparisons;     // vs other scenarios
        std::map<std::string, double> historicalComparisons;   // vs previous runs
        double performanceScore;                               // Overall performance score
        double efficiencyScore;                                // Overall efficiency score
        double qualityScore;                                   // Overall quality score
    } comparative;
    
    // Error and reliability metrics
    struct ReliabilityMetrics {
        size_t totalErrors;         // Total errors encountered
        size_t criticalErrors;     // Critical errors
        size_t recoverableErrors;  // Errors that could be recovered from
        double errorRate;          // Errors per operation
        double recoverySuccessRate; // Successful recovery rate
        double systemUptime;       // System availability
        double meanTimeBetweenFailures; // MTBF
    } reliability;
    
    // Configuration used for this benchmark
    BenchmarkConfig config;
    
    // Timestamp
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    // Additional metadata
    std::string benchmarkId;
    std::string description;
    std::string machineInfo;
    
    // Methods
    double getDuration() const;
    double getThroughput() const;
    double getEfficiencyScore() const;
    std::string generateReport() const;
    bool saveResults(const std::string& filename) const;
    bool loadResults(const std::string& filename);
    
    // Comparison operators
    bool operator<(const BenchmarkResults& other) const { return getEfficiencyScore() < other.getEfficiencyScore(); }
    bool operator>(const BenchmarkResults& other) const { return getEfficiencyScore() > other.getEfficiencyScore(); }
    bool operator<=(const BenchmarkResults& other) const { return getEfficiencyScore() <= other.getEfficiencyScore(); }
    bool operator>=(const BenchmarkResults& other) const { return getEfficiencyScore() >= other.getEfficiencyScore(); }
};

// Advanced benchmark suite with comprehensive testing
class BenchmarkSuite {
public:
    BenchmarkSuite(std::shared_ptr<BenchmarkConfig> config);
    ~BenchmarkSuite();
    
    // Run comprehensive benchmark suite
    BenchmarkResults runBenchmarkSuite();
    
    // Run specific scenario
    BenchmarkResults runScenario(BenchmarkConfig::Scenario scenario);
    
    // Run individual subsystem benchmarks
    BenchmarkResults benchmarkMemorySystem();
    BenchmarkResults benchmarkPredictionSystem();
    BenchmarkResults benchmarkCognitiveSystems();
    BenchmarkResults benchmarkNeuromodulationSystems();
    BenchmarkResults benchmarkDevelopmentSystem();
    BenchmarkResults benchmarkPersistence();
    BenchmarkResults benchmarkIntegration();
    
    // Run performance benchmarks
    BenchmarkResults benchmarkThroughput(size_t steps);
    BenchmarkResults benchmarkAccuracy(size_t steps);
    BenchmarkResults benchmarkEfficiency(size_t steps);
    BenchmarkResults benchmarkLatency(size_t steps);
    BenchmarkResults benchmarkScalability(size_t steps);
    BenchmarkResults benchmarkStability(size_t steps);
    
    // Run stress and load benchmarks
    BenchmarkResults benchmarkStressTest(double stressLevel);
    BenchmarkResults benchmarkLoadTest(size_t concurrentAgents);
    BenchmarkResults benchmarkMemoryPressure(size_t memoryPressure);
    BenchmarkResults benchmarkCPULoad(double cpuLoad);
    
    // Run integration benchmarks
    BenchmarkResults benchmarkIntegrationLatency();
    BenchmarkResults benchmarkCrossSystemPerformance();
    BenchmarkResults benchmarkFeedbackLoopEffectiveness();
    BenchmarkResults benchmarkSystemConnectivity();
    
    // Run validation benchmarks
    BenchmarkResults benchmarkMemoryRetention(size_t steps);
    BenchmarkResults benchmarkPredictionAccuracy(size_t steps);
    BenchmarkResults benchmarkLearningCurve(size_t steps);
    BenchmarkResults benchmarkGeneralization(size_t steps);
    BenchmarkResults benchmarkTransferLearning(size_t steps);
    
    // Compare with baseline
    BenchmarkResults benchmarkAgainstBaseline(const std::string& baselineFile);
    BenchmarkResults benchmarkAgainstScenario(BenchmarkConfig::Scenario scenario);
    BenchmarkResults benchmarkHistorical(const std::string& historicalFile);
    
    // Stress testing
    BenchmarkResults stressTestVariability(size_t duration);
    BenchmarkResults stressTestRecovery(size_t steps);
    BenchmarkResults stressTestResourceLimits();
    
    // Long-running tests
    BenchmarkResults longRunningStability(size_t hours);
    BenchmarkResults longRunningLearning(size_t days);
    BenchmarkResults longRunningMemoryRetention(size_t days);
    
    // Detailed profiling
    void enableDetailedProfiling(bool enable);
    void enableMemoryProfiling(bool enable);
    void enableCPUProfiling(bool enable);
    void enableNetworkProfiling(bool enable);
    
    // Configuration
    void setConfig(std::shared_ptr<BenchmarkConfig> config);
    std::shared_ptr<BenchmarkConfig> getConfig() const { return config; }
    
    // State management
    void saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);
    
    // Results management
    void addResult(const BenchmarkResults& result);
    std::vector<BenchmarkResults> getResults() const { return results; }
    BenchmarkResults getBestResult() const;
    BenchmarkResults getWorstResult() const;
    
    // Error handling
    void setErrorHandler(std::shared_ptr<ErrorHandler> handler);
    void setBenchmarkName(const std::string& name);
    std::string getBenchmarkName() const { return benchmarkName; }
    
    // Progress tracking
    void setProgressCallback(std::function<void(double)> callback);
    void reportProgress(double progress) const;
    
    // Report generation
    std::string generateComprehensiveReport() const;
    std::string generatePerformanceReport() const;
    std::string generateComparativeReport() const;
    std::string generateStressReport() const;
    std::string generateLongRunningReport() const;
    
    // Export formats
    bool exportResultsCSV(const std::string& filename) const;
    bool exportResultsJSON(const std::string& filename) const;
    bool exportResultsHTML(const std::string& filename) const;
    bool exportResultsPDF(const std::string& filename) const;
    
    // Analysis
    struct AnalysisResults {
        double meanPerformance;
        double stdDevPerformance;
        double coefficientOfVariation;
        std::chrono::duration<double> meanTime;
        std::chrono::duration<double> timeStdDev;
        std::vector<size_t> outliers;
        bool isConverged; // Statistical significance
    };
    
    AnalysisResults analyzeResults(const std::vector<BenchmarkResults>& results) const;
    
    // Utility functions
    static std::string formatTime(double seconds);
    static std::string formatThroughput(double ops);
    static std::string formatMemory(double bytes);
    static std::string formatPercentage(double value);
    static std::string formatScientific(double value);
    
    // Benchmark presets
    static BenchmarkConfig createSmallScaleBenchmark();
    static BenchmarkConfig createMediumScaleBenchmark();
    static BenchmarkConfig createLargeScaleBenchmark();
    static BenchmarkConfig createResearchScaleBenchmark();
    
    // Scenario-specific configurations
    static BenchmarkConfig createScenarioConfig(BenchmarkConfig::Scenario scenario);
    
    // Validation
    bool validateBenchmarkSetup() const;
    
    // Cleanup
    void cleanup();
    
private:
    // Private implementation details
    void initializeBenchmarkSuite();
    void cleanupBenchmarkSuite();
    
    BenchmarkResults runBasicSimulation(size_t steps);
    BenchmarkResults runMemoryBenchmark(size_t steps);
    BenchmarkResults runPredictionBenchmark(size_t steps);
    BenchmarkResults runCognitiveBenchmark(size_t steps);
    BenchmarkResults runNeuromodulationBenchmark(size_t steps);
    BenchmarkResults runDevelopmentBenchmark(size_t steps);
    BenchmarkResults runPersistenceBenchmark(size_t steps);
    BenchmarkResults runIntegrationBenchmark(size_t steps);
    
    BenchmarkResults runStressTest(double stressLevel);
    BenchmarkResults runLoadTest(size_t concurrentAgents);
    BenchmarkResults runVariabilityTest(size_t duration);
    
    BenchmarkResults runLongRunningTest(size_t hours);
    BenchmarkResults runStabilityTest(size_t steps);
    
    BenchmarkResults runAccuracyTest(size_t steps);
    BenchmarkResults runLearningTest(size_t steps);
    BenchmarkResults runGeneralizationTest(size_t steps);
    
    BenchmarkResults runComparativeTest(const std::string& baselineFile);
    
    void setupBenchmarkEnvironment();
    void teardownBenchmarkEnvironment();
    
    void collectResourceMetrics();
    void collectPerformanceMetrics();
    void collectSystemMetrics();
    
    void startTimer();
    void stopTimer();
    double getElapsedTime() const;
    
    void logBenchmarkProgress(const std::string& message);
    void logBenchmarkResult(const BenchmarkResults& result);
    
    // Member variables
    std::shared_ptr<BenchmarkConfig> config;
    std::vector<BenchmarkResults> results;
    
    std::shared_ptr<ErrorHandler> errorHandler;
    std::function<void(double)> progressCallback;
    
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point currentTime;
    
    std::string benchmarkName;
    
    bool profilingEnabled;
    bool memoryProfilingEnabled;
    bool cpuProfilingEnabled;
    bool networkProfilingEnabled;
    
    // Detailed profiling data
    struct ProfileData {
        std::vector<double> stepTimes;
        std::vector<double> memoryUsage;
        std::vector<double> cpuUsage;
        std::vector<double> networkUsage;
        std::vector<std::string> systemEvents;
    } profileData;
    
    // Resource tracking
    struct ResourceTracker {
        double peakMemory;
        double averageMemory;
        double peakCPU;
        double averageCPU;
        std::chrono::steady_clock::time_point lastSample;
        size_t sampleCount;
    } resourceTracker;
    
    // Benchmark environment
    struct BenchmarkEnvironment {
        std::shared_ptr<Brain> brain;
        std::shared_ptr<World> world;
        std::shared_ptr<AgentBrain> agent;
        std::shared_ptr<BenchmarkSuite> benchmarkSuite;
        
        void setup();
        void cleanup();
        bool isReady() const;
    } environment;
    
    // Constants
    static constexpr double EPSILON = 1e-9;
    static constexpr size_t MAX_SAMPLES = 100000;
    static constexpr double MAX_MEMORY_MB = 1024.0;
    static constexpr double MAX_CPU_PERCENT = 100.0;
};

// Benchmark factory for easy creation
class BenchmarkFactory {
public:
    static std::shared_ptr<BenchmarkSuite> createBenchmark(const std::string& name);
    static std::shared_ptr<BenchmarkSuite> createQuickBenchmark();
    static std::shared_ptr<BenchmarkSuite> createResearchBenchmark();
    static std::shared_ptr<BenchmarkSuite> createProductionBenchmark();
    
    static std::shared_ptr<BenchmarkSuite> createScenarioBenchmark(BenchmarkConfig::Scenario scenario);
    static std::shared_ptr<BenchmarkSuite> createScaleBenchmark(size_t scaleFactor);
    
    static std::vector<BenchmarkResults> runBenchmarkSuite(const std::vector<std::string>& benchmarkNames);
    static BenchmarkResults runBenchmark(const std::string& name);
    
    static void registerBenchmarkType(const std::string& name, std::function<std::shared_ptr<BenchmarkSuite>()> creator);
    static std::vector<std::string> getAvailableBenchmarkTypes();
    
    static BenchmarkConfig createConfiguration(const std::string& configName);
    static std::vector<BenchmarkConfig> getAvailableConfigurations();
}; 

// Global benchmark manager
extern std::shared_ptr<BenchmarkSuite> gBenchmarkSuite;

// Global functions for benchmark management
void initializeBenchmarkSuite(std::shared_ptr<BenchmarkConfig> config);
void shutdownBenchmarkSuite();
std::shared_ptr<BenchmarkSuite> getBenchmarkSuite();
BenchmarkResults runBenchmark(const std::string& name);
BenchmarkResults runBenchmark(BenchmarkConfig::Scenario scenario);
std::vector<BenchmarkResults> runBenchmarkSuite(const std::vector<std::string>& names);
void setBenchmarkProgressCallback(std::function<void(double)> callback);

} // namespace nlm

#endif // BENCHMARK_SUITE_HPP
