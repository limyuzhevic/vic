#pragma once

/**
 * Phase6IntegratedExperiment - Phase 6 final integration test
 * 
 * This experiment demonstrates that the integrated brain systems work together:
 * - Memory systems are connected to neural processing
 * - Neuromodulation affects plasticity and neural dynamics
 * - Development affects plasticity rates
 * - Prediction system is integrated
 * - Replay and consolidation are functional
 * - Checkpoint save/load works
 * 
 * The experiment runs a complete lifetime simulation and verifies
 * that all systems interact properly.
 * 
 * Advanced features:
 * - Comprehensive integration verification
 * - Detailed metrics collection
 * - Checkpoint/restore functionality
 * - Replay and consolidation testing
 * - Performance benchmarking
 * - Error handling and recovery
 * - Visualization support
 * - Configuration management
 * - Progress tracking
 * - Error reporting and recovery
 * - Result analysis and reporting
 */

#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <sstream>

namespace nlm {

/**
 * Enhanced integration test result with detailed metrics
 */
struct Phase6IntegrationResult {
    // System integration status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
    
    // Performance metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    // Efficiency metrics
    uint64_t totalSpikes;
    uint64_t activeNeurons;
    double simulationEfficiency;
    double energyEfficiency;
    
    // Timestamps
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double totalWallClockTime;
    
    // Additional metadata
    std::string brainType;
    std::string simulationMode;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    Phase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , avgSynapticWeight(0.0f)
        , memoryEpisodesStored(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , totalSpikes(0)
        , activeNeurons(0)
        , simulationEfficiency(0.0)
        , energyEfficiency(0.0)
        , totalWallClockTime(0.0) {}
};

/**
 * Comprehensive configuration for Phase 6 integration experiment
 * 
 * Provides extensive configuration options for:
 * - Network parameters (neurons, regions, connectivity)
 * - Simulation parameters (timestep, max steps)
 * - System enable/disable flags
 * - Performance parameters (checkpointing, replay)
 * - Advanced features (visualization, profiling)
 * - Configuration metadata (description, seed)
 */
struct Phase6Config {
    // Network parameters
    uint64_t neuronCount;
    uint64_t regionCount;
    float connectionProbability;
    
    // Simulation parameters
    uint64_t maxSteps;
    double simulationTimestep;
    
    // System enable flags
    bool enableMemorySystems;
    bool enableNeuromodulation;
    bool enablePrediction;
    bool enableDevelopment;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableVisualization;
    
    // Performance parameters
    std::string checkpointPath;
    uint64_t maxMemoryEpisodes;
    double noveltyThreshold;
    double curiosityThreshold;
    
    // Configuration metadata
    std::string configDescription;
    uint32_t randomSeed;
    
    Phase6Config()
        : neuronCount(1000)
        , regionCount(2)
        , connectionProbability(0.1f)
        , maxSteps(2000)
        , simulationTimestep(0.001)
        , enableMemorySystems(true)
        , enableNeuromodulation(true)
        , enablePrediction(true)
        , enableDevelopment(true)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableVisualization(false)
        , maxMemoryEpisodes(100)
        , noveltyThreshold(0.5f)
        , curiosityThreshold(0.3f)
        , configDescription("Phase 6 integration configuration")
        , randomSeed(42) {}
};

/**
 * Advanced integration test with comprehensive verification
 * 
 * Provides sophisticated testing capabilities for:
 * - Component integration verification
 * - Performance benchmarking
 * - Error handling and recovery
 * - Visualization and reporting
 * - Configuration management
 * - Progress tracking
 * - Result analysis
 * - Advanced testing modes
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test
     * 
     * Executes the full Phase 6 integration experiment with:
     * - Network initialization and configuration
     * - Component integration verification
     * - Simulation execution
     * - Metrics collection
     * - Result analysis
     * 
     * @param config Configuration for the integration test
     * @return Detailed results of the integration test
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Perform simple integration verification
     * 
     * Runs basic integration checks to ensure:
     * - All required systems are present
     * - Memory systems are integrated
     * - Neuromodulation systems are functional
     * - Basic connectivity is working
     * 
     * @return True if integration is successful, false otherwise
     */
    bool verifyIntegration() const;
    
    /**
     * Test memory system integration
     * 
     * Verifies that:
     * - Working memory is operational
     * - Episodic memory is functional
     * - Semantic memory is accessible
     * - Procedural memory is integrated
     * 
     * @return True if memory integration is successful
     */
    bool testMemoryIntegration() const;
    
    /**
     * Test neuromodulation system integration
     * 
     * Verifies that:
     * - Dopamine system is operational
     * - Curiosity system is functional
     * - Novelty detection works
     * - Prediction error signaling is active
     * 
     * @return True if neuromodulation integration is successful
     */
    bool testNeuromodulationIntegration() const;
    
    /**
     * Test prediction system integration
     * 
     * Verifies that:
     * - Prediction system is operational
     * - Neural prediction models work
     * - Error signals are generated
     * - Adaptation mechanisms are functional
     * 
     * @return True if prediction integration is successful
     */
    bool testPredictionIntegration() const;
    
    /**
     * Test development system integration
     * 
     * Verifies that:
     * - Development stages are supported
     * - Plasticity is modulated by development
     * - Structural plasticity works
     * - Maturation processes function
     * 
     * @return True if development integration is successful
     */
    bool testDevelopmentIntegration() const;
    
    /**
     * Test checkpoint save/load functionality
     * 
     * Verifies that:
     * - Checkpoint saving works
     * - Checkpoint loading works
     * - State restoration is functional
     * - Data persistence works correctly
     * 
     * @return True if checkpoint integration is successful
     */
    bool testCheckpointing() const;
    
    /**
     * Test replay and consolidation functionality
     * 
     * Verifies that:
     * - Replay mechanisms work
     * - Consolidation processes function
     * - Memory replay is functional
     * - Sleep/rest cycle integration works
     * 
     * @return True if replay integration is successful
     */
    bool testReplay() const;
    
    /**
     * Load configuration from file
     * 
     * Loads Phase 6 configuration from a JSON file.
     * 
     * @param filepath Path to configuration file
     * @return True if configuration is loaded successfully
     */
    bool loadConfigFromFile(const std::string& filepath);
    
    /**
     * Save configuration to file
     * 
     * Saves Phase 6 configuration to a JSON file.
     * 
     * @param filepath Path to save configuration file
     * @return True if configuration is saved successfully
     */
    bool saveConfigToFile(const std::string& filepath) const;
    
    /**
     * Validate configuration
     * 
     * Validates that the configuration is consistent and complete.
     * 
     * @param config Configuration to validate
     * @return True if configuration is valid
     */
    bool validateConfig(const Phase6Config& config) const;
    
    /**
     * Run performance benchmark
     * 
     * Benchmarks the performance of the integration test.
     * 
     * @param config Configuration for benchmarking
     * @return Performance results
     */
    double benchmarkPerformance(const Phase6Config& config) const;
    
    /**
     * Run performance tests
     * 
     * Runs comprehensive performance tests including:
     * - Memory usage measurement
     * - CPU usage measurement
     * - Network performance testing
     * - I/O performance testing
     */
    void runPerformanceTests() const;
    
    /**
     * Create visualization
     * 
     * Creates visualization of the integration test results.
     * 
     * @param result Integration test results
     * @return True if visualization is created successfully
     */
    bool createVisualization(const Phase6IntegrationResult& result) const;
    
    /**
     * Export results to file
     * 
     * Exports integration test results to various formats.
     * 
     * @param result Integration test results
     * @param format Export format (json, csv, xml, txt)
     * @return True if export is successful
     */
    bool exportResults(const Phase6IntegrationResult& result, const std::string& format) const;
    
    /**
     * Create default configuration
     * 
     * Creates a default Phase 6 configuration with sensible defaults.
     * 
     * @return Default configuration
     */
    static Phase6Config createDefaultConfig();
    
    /**
     * Create minimal configuration
     * 
     * Creates a minimal Phase 6 configuration for quick testing.
     * 
     * @return Minimal configuration
     */
    static Phase6Config createMinimalConfig();
    
    /**
     * Create maximal configuration
     * 
     * Creates a maximal Phase 6 configuration for comprehensive testing.
     * 
     * @return Maximal configuration
     */
    static Phase6Config createMaximalConfig();
    
    /**
     * Analyze results
     * 
     * Performs comprehensive analysis of integration test results.
     * 
     * @param result Integration test results
     */
    static void analyzeResults(const Phase6IntegrationResult& result);
    
    /**
     * Generate report
     * 
     * Generates a comprehensive report of integration test results.
     * 
     * @param result Integration test results
     * @return Generated report as string
     */
    static std::string generateReport(const Phase6IntegrationResult& result);
    
    /**
     * Save report to file
     * 
     * Saves the generated report to a file.
     * 
     * @param result Integration test results
     * @param filename Output filename
     * @return True if report is saved successfully
     */
    static bool saveReport(const Phase6IntegrationResult& result, const std::string& filename);
    
    /**
     * Get experiment component
     * 
     * @return Shared pointer to experiment component
     */
    std::shared_ptr<Experiment> getExperiment() const { return experiment_; }
    
    /**
     * Get configuration
     * 
     * @return Current configuration
     */
    const Phase6Config& getConfig() const { return config_; }
    
    /**
     * Set configuration
     * 
     * @param config New configuration
     */
    void setConfig(const Phase6Config& config) { config_ = config; }
    
    /**
     * Get version
     * 
     * @return Version string
     */
    static std::string getVersion() { return "6.0.0"; }
    
    /**
     * Get description
     * 
     * @return Description string
     */
    static std::string getDescription() { return "Phase 6 Final Integration Test Framework"; }
    
    /**
     * Validate configuration
     * 
     * Validates that the configuration is consistent and complete.
     * 
     * @param config Configuration to validate
     * @param errors Output parameter for error messages
     * @return True if configuration is valid
     */
    static bool validateConfig(const Phase6Config& config, std::vector<std::string>& errors);
    
    /**
     * Run specific test mode
     * 
     * @param mode Test mode to run
     * @param config Configuration for the test
     * @return True if test is successful
     */
    bool runTestMode(TestMode mode, const Phase6Config& config);
    
    /**
     * Run test suite
     * 
     * Runs multiple tests with different configurations.
     * 
     * @param configs List of configurations to test
     * @return List of test results
     */
    std::vector<Phase6IntegrationResult> runTestSuite(const std::vector<Phase6Config>& configs) const;
    
    /**
     * Get test mode enumeration
     */
    enum class TestMode {
        Connectivity,
        Plasticity,
        STDP,
        Memory,
        Neuromodulation,
        Prediction,
        Development,
        Checkpointing,
        Replay,
        All
    };
    
    /**
     * Set error callback
     */
    using ErrorCallback = std::function<void(const std::string&, LogLevel)>;
    void setErrorCallback(ErrorCallback callback) { errorCallback_ = callback; }
    
    /**
     * Set progress callback
     */
    using ProgressCallback = std::function<void(double, const std::string&)>;
    void setProgressCallback(ProgressCallback callback) { progressCallback_ = callback; }
    
    /**
     * Update progress
     */
    void updateProgress(double progress, const std::string& message) const;
    
    /**
     * Report error
     */
    void reportError(const std::string& error, LogLevel level = LogLevel::Error) const;
    
    /**
     * Handle error
     */
    bool handleError(const std::string& error) const;
    
    /**
     * Log error
     */
    void logError(const std::string& error, LogLevel level = LogLevel::Error) const;
    
private:
    // Component integration points
    std::shared_ptr<Experiment> experiment_;
    Phase6Config config_;
    
    // Callbacks for error and progress reporting
    ErrorCallback errorCallback_;
    ProgressCallback progressCallback_;
    
    // Internal helper methods
    void initializeComponents();
    void cleanupComponents();
    
    // Integration verification helpers
    bool verifyMemoryIntegration(const Phase6Config& config) const;
    bool verifyNeuromodulationIntegration(const Phase6Config& config) const;
    bool verifyPredictionIntegration(const Phase6Config& config) const;
    bool verifyDevelopmentIntegration(const Phase6Config& config) const;
    
    // Performance measurement helpers
    void measureMemoryUsage() const;
    void measureCPUUsage() const;
    void measureNetworkPerformance() const;
    
    // Result analysis helpers
    void analyzeIntegrationScore(const Phase6IntegrationResult& result) const;
    void analyzeEfficiencyMetrics(const Phase6IntegrationResult& result) const;
    void generateIntegrationReport(const Phase6IntegrationResult& result) const;
    
    // Configuration validation helpers
    static bool validateNetworkParameters(const Phase6Config& config, std::vector<std::string>& errors);
    static bool validateSimulationParameters(const Phase6Config& config, std::vector<std::string>& errors);
    static bool validateSystemParameters(const Phase6Config& config, std::vector<std::string>& errors);
    
    // File I/O helpers
    static bool saveJsonResults(const Phase6IntegrationResult& result, const std::string& filename);
    static bool saveCsvResults(const Phase6IntegrationResult& result, const std::string& filename);
    static bool saveTextResults(const Phase6IntegrationResult& result, const std::string& filename);
    
    // Visualization helpers
    static bool createTextVisualization(const Phase6IntegrationResult& result, const std::string& filename);
    static bool createHtmlVisualization(const Phase6IntegrationResult& result, const std::string& filename);
    
    // Random testing helpers
    static Phase6Config generateRandomConfig();
    static Phase6Config generateConfigForPerformanceTest();
    
    // Comparison testing helpers
    static bool compareResults(const Phase6IntegrationResult& result1, const Phase6IntegrationResult& result2);
    static Phase6IntegrationResult averageResults(const std::vector<Phase6IntegrationResult>& results);
};
