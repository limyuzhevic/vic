// Advanced API Headers for NLM Python Bindings
// This file provides high-level convenience functions and class abstractions
// for the Neural Learning Machine framework.

#ifndef ADVANCED_API_HPP
#define ADVANCED_API_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <iostream>
#include <functional>
#include <optional>
#include <variant>

// Core NLM includes
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/ExperimentResult.hpp"
#include "../src/experiments/Metrics.hpp"

// Forward declarations
class SimpleAgent;
class SimulationRunner;
class BrainProfiler;
class VisualizationHelper;
class ExperimentManager;

// Type aliases for convenience
typedef std::vector<float> FloatArray;
typedef std::vector<std::vector<float>> FloatMatrix;
typedef std::map<std::string, std::string> StringMap;

// ============================================================================
// High-Level Convenience Functions
// ============================================================================

/**
 * @brief Run a complete simulation with the given configuration
 * @param configPath Path to configuration file
 * @param outputFile Optional output file for results
 * @param steps Number of simulation steps to run
 * @return ExperimentResult containing simulation results
 */
ExperimentResult run_simulation(const std::string& configPath, 
                                 const std::string& outputFile = "",
                                 size_t steps = 1000);

/**
 * @brief Run a single episode for an agent
 * @param brain Shared pointer to brain object
 * @param world Shared pointer to world object
 * @param maxSteps Maximum number of steps for the episode
 * @param render Whether to enable visualization
 * @return AgentAction representing the final action
 */
std::vector<nlm::Action> run_episode(std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   size_t maxSteps = 100,
                                   bool render = false);

/**
 * @brief Run a complete scientific experiment
 * @param experimentConfig Configuration for the experiment
 * @param outputDirectory Directory to save results
 * @param parallel Whether to run in parallel
 * @return ExperimentResult containing experiment data
 */
ExperimentResult run_experiment(const StringMap& experimentConfig,
                                 const std::string& outputDirectory,
                                 bool parallel = false);

/**
 * @brief Create a brain from a JSON configuration file
 * @param configPath Path to JSON configuration file
 * @return Shared pointer to configured brain object
 */
std::shared_ptr<Brain> create_brain_from_config(const std::string& configPath);

/**
 * @brief Create a brain from JSON string
 * @param jsonConfig JSON configuration as string
 * @return Shared pointer to configured brain object
 */
std::shared_ptr<Brain> create_brain_from_json(const std::string& jsonConfig);

/**
 * @brief Save brain state to file
 * @param brain Shared pointer to brain object
 * @param filepath Output file path
 * @param format File format (json, binary, hdf5)
 */
void save_brain_state(std::shared_ptr<Brain> brain,
                     const std::string& filepath,
                     const std::string& format = "json");

/**
 * @brief Create a new world with default configuration
 * @param width World width
 * @param height World height
 * @param numAgents Number of agents to create
 * @return Shared pointer to created world
 */
std::shared_ptr<SimpleWorld> create_world(size_t width = 100,
                                         size_t height = 100,
                                         size_t numAgents = 1);

/**
 * @brief Setup and initialize a world
 * @param world Shared pointer to world object
 * @param config Configuration for world setup
 * @return bool indicating success
 */
bool setup_world(std::shared_ptr<SimpleWorld> world,
                const StringMap& config);

/**
 * @brief Configure world with parameters
 * @param world Shared pointer to world object
 * @param configPath Path to configuration file
 * @return bool indicating success
 */
bool configure_world(std::shared_ptr<SimpleWorld> world,
                    const std::string& configPath);

/**
 * @brief Create a new agent with specified parameters
 * @param brain Shared pointer to brain object
 * @param world Shared pointer to world object
 * @param agentId Unique identifier for the agent
 * @return Shared pointer to created agent
 */
std::shared_ptr<SimpleAgent> create_agent(std::shared_ptr<Brain> brain,
                                         std::shared_ptr<SimpleWorld> world,
                                         const std::string& agentId = "agent_1");

/**
 * @brief Enable all neural systems for an agent
 * @param agent Shared pointer to agent object
 * @param enablePlasticity Whether to enable plasticity systems
 * @param enableDevelopment Whether to enable development systems
 * @return bool indicating success
 */
bool enable_systems(std::shared_ptr<SimpleAgent> agent,
                   bool enablePlasticity = true,
                   bool enableDevelopment = true);

/**
 * @brief Setup agent environment with sensors and actuators
 * @param agent Shared pointer to agent object
 * @param sensoryConfig Configuration for sensory systems
 * @param motorConfig Configuration for motor systems
 * @return bool indicating success
 */
bool setup_agent_environment(std::shared_ptr<SimpleAgent> agent,
                            const StringMap& sensoryConfig,
                            const StringMap& motorConfig);

/**
 * @brief Analyze brain activity for patterns and insights
 * @param brain Shared pointer to brain object
 * @param analysisType Type of analysis to perform
 * @param parameters Additional analysis parameters
 * @return std::map<std::string, FloatArray> containing analysis results
 */
std::map<std::string, FloatArray> analyze_brain_activity(
    std::shared_ptr<Brain> brain,
    const std::string& analysisType = "spike_patterns",
    const StringMap& parameters = {});

/**
 * @brief Compute comprehensive statistics for brain data
 * @param brain Shared pointer to brain object
 * @param metrics List of metrics to compute
 * @return std::map<std::string, double> containing computed statistics
 */
std::map<std::string, double> compute_statistics(
    std::shared_ptr<Brain> brain,
    const std::vector<std::string>& metrics = {});

/**
 * @brief Visualize brain state and results
 * @param brain Shared pointer to brain object
 * @param outputFile Output file path for visualization
 * @param renderOptions Rendering options
 * @return bool indicating success
 */
bool visualize_brain_state(std::shared_ptr<Brain> brain,
                          const std::string& outputFile,
                          const StringMap& renderOptions = {});

/**
 * @brief Save experiment results to file
 * @param result ExperimentResult object containing results
 * @param filepath Output file path
 * @param format File format (json, csv, pickle)
 */
void save_experiment_results(const ExperimentResult& result,
                             const std::string& filepath,
                             const std::string& format = "json");

/**
 * @brief Load experiment results from file
 * @param filepath Path to experiment results file
 * @param format File format (json, csv, pickle)
 * @return ExperimentResult loaded from file
 */
ExperimentResult load_experiment_results(const std::string& filepath,
                                         const std::string& format = "json");

/**
 * @brief Load configuration from file
 * @param configPath Path to configuration file
 * @return StringMap containing configuration parameters
 */
StringMap load_config_from_file(const std::string& configPath);

/**
 * @brief Save configuration to JSON file
 * @param config Configuration parameters
 * @param configPath Output file path
 */
void save_config_to_json(const StringMap& config,
                        const std::string& configPath);

// ============================================================================
// High-Level Class Abstractions
// ============================================================================

/**
 * @brief High-level agent interface for common scenarios
 */
class SimpleAgent {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::string agentId_;
    bool isInitialized_ = false;
    
public:
    SimpleAgent(std::shared_ptr<Brain> brain, 
                std::shared_ptr<SimpleWorld> world,
                const std::string& agentId = "agent_1");
    
    ~SimpleAgent() = default;
    
    // Core functionality
    bool initialize();
    void update(float dt = 1.0f);
    void reset();
    
    // Neural operations
    void processInput(const FloatArray& input);
    FloatArray getOutput() const;
    double getReward() const;
    void receiveReward(double reward);
    
    // State management
    bool isActive() const;
    std::string getAgentId() const;
    std::shared_ptr<Brain> getBrain() const;
    std::shared_ptr<SimpleWorld> getWorld() const;
    
    // Configuration and setup
    bool configure(const StringMap& config);
    StringMap getConfig() const;
    
    // Performance monitoring
    double getFiringRate() const;
    size_t getSpikeCount() const;
    FloatArray getNeuralActivity() const;
    
    // Learning and development
    void learnFromExperience(const FloatArray& experience);
    void develop(float dt = 1.0f);
    
    // Serialization
    void saveState(const std::string& filepath) const;
    void loadState(const std::string& filepath);
    
    // Debugging
    std::string getDebugInfo() const;
    void profile(float duration = 1.0f);
};

/**
 * @brief Configurable simulation runner for multiple agents and scenarios
 */
class SimulationRunner {
private:
    std::vector<std::shared_ptr<SimpleAgent>> agents_;
    std::shared_ptr<SimpleWorld> world_;
    size_t numSteps_;
    float dt_;
    bool renderEnabled_;
    std::string outputDirectory_;
    bool isRunning_ = false;
    
public:
    SimulationRunner(size_t numSteps = 1000, float dt = 1.0f);
    
    ~SimulationRunner() = default;
    
    // Agent management
    void addAgent(std::shared_ptr<SimpleAgent> agent);
    void removeAgent(const std::string& agentId);
    std::shared_ptr<SimpleAgent> getAgent(const std::string& agentId) const;
    std::vector<std::shared_ptr<SimpleAgent>> getAllAgents() const;
    
    // World setup
    void setWorld(std::shared_ptr<SimpleWorld> world);
    std::shared_ptr<SimpleWorld> getWorld() const;
    
    // Configuration
    void configure(const StringMap& config);
    StringMap getConfig() const;
    
    // Simulation control
    void start();
    void stop();
    bool isRunning() const;
    void run();
    void step();
    
    // Output and visualization
    void enableRendering(bool enable);
    bool isRenderingEnabled() const;
    void setOutputDirectory(const std::string& directory);
    void saveResults(const std::string& prefix = "simulation");
    
    // Statistics and analysis
    std::map<std::string, double> getStatistics() const;
    FloatMatrix getLearningCurve() const;
    std::map<std::string, FloatArray> getAgentMetrics() const;
    
    // Callbacks
    void setStepCallback(std::function<void(size_t)> callback);
    void setEpisodeCallback(std::function<void(const ExperimentResult&)> callback);
};

/**
 * @brief Advanced brain profiling and analysis
 */
class BrainProfiler {
private:
    std::shared_ptr<Brain> brain_;
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point endTime_;
    bool isProfiling_ = false;
    
public:
    BrainProfiler(std::shared_ptr<Brain> brain);
    
    ~BrainProfiler() = default;
    
    // Profile control
    void start();
    void stop();
    bool isProfiling() const;
    double getElapsedTime() const;
    
    // Analysis methods
    std::map<std::string, FloatArray> analyzeSpikeTiming() const;
    std::map<std::string, FloatArray> analyzeNetworkConnectivity() const;
    std::map<std::string, FloatArray> analyzeMemoryUsage() const;
    std::map<std::string, FloatArray> analyzePlasticityDynamics() const;
    std::map<std::string, FloatArray> analyzeNeuromodulation() const;
    
    // Statistical analysis
    double computeCohesion() const;
    double computeModularity() const;
    double computeEfficiency() const;
    double computeSmallWorldness() const;
    
    // Visualization data
    std::string generateHeatmapData() const;
    std::string generateConnectivityMatrix() const;
    std::string generateActivityTimeSeries() const;
    
    // Export
    void exportProfile(const std::string& filepath) const;
    std::string getProfileSummary() const;
};

/**
 * @brief Helper for visualization and plotting
 */
class VisualizationHelper {
public:
    VisualizationHelper() = default;
    ~VisualizationHelper() = default;
    
    // Plotting functions
    static void plotActivityHeatmap(const FloatMatrix& activity, 
                                    const std::string& filename);
    static void plotLearningCurve(const std::vector<double>& rewards,
                                  const std::string& filename);
    static void plotNetworkTopology(std::shared_ptr<Brain> brain,
                                     const std::string& filename);
    static void plotTimeSeries(const std::vector<std::vector<float>>& data,
                              const std::vector<std::string>& labels,
                              const std::string& filename);
    static void plotComparison(const std::vector<FloatMatrix>& datasets,
                              const std::vector<std::string>& labels,
                              const std::string& filename);
    
    // Data processing
    static FloatMatrix normalizeData(const FloatMatrix& data);
    static FloatMatrix smoothData(const FloatMatrix& data, 
                                 size_t windowSize = 5);
    static std::vector<size_t> findPeaks(const FloatArray& data,
                                         double threshold = 0.5);
    
    // Export formats
    static void exportToCSV(const FloatMatrix& data, 
                            const std::string& filename);
    static void exportToJSON(const std::map<std::string, FloatArray>& data,
                            const std::string& filename);
    static void exportToMatlab(const FloatMatrix& data,
                              const std::string& filename);
    
    // Utilities
    static std::string generateColorMap(const std::string& scheme = "viridis");
    static double interpolate(double x, double x1, double x2,
                              double y1, double y2);
    static FloatArray computeHistogram(const FloatArray& data,
                                       size_t bins = 50);
};

/**
 * @brief Comprehensive experiment management
 */
class ExperimentManager {
private:
    std::vector<ExperimentResult> results_;
    StringMap experimentConfig_;
    std::string resultsDirectory_;
    size_t experimentId_;
    
public:
    ExperimentManager();
    
    ~ExperimentManager() = default;
    
    // Experiment setup
    void configure(const StringMap& config);
    StringMap getConfig() const;
    
    // Experiment execution
    ExperimentResult runExperiment(const std::string& name,
                                   std::function<void(std::shared_ptr<SimpleAgent>)> setup,
                                   std::function<void(std::shared_ptr<SimpleAgent>, float)> step,
                                   std::function<double(std::shared_ptr<SimpleAgent>)> evaluate);
    std::vector<ExperimentResult> runBatch(size_t numExperiments);
    
    // Results management
    void addResult(const ExperimentResult& result);
    std::vector<ExperimentResult> getResults() const;
    ExperimentResult getResult(size_t index) const;
    
    // Analysis and comparison
    std::map<std::string, double> compareExperiments(size_t experiment1, size_t experiment2) const;
    std::map<std::string, FloatArray> analyzeAcrossExperiments(const std::string& metric) const;
    
    // Export and reporting
    void exportAllResults(const std::string& directory = ".");
    void generateReport(const std::string& filename = "experiment_report") const;
    
    // Statistics
    double getMeanPerformance(size_t experimentIndex) const;
    double getVariancePerformance(size_t experimentIndex) const;
    std::map<std::string, double> getGlobalStatistics() const;
};

// ============================================================================
// Advanced Debugging Tools
// ============================================================================

/**
 * @brief Monitor and analyze neuron activity
 */
class NeuronActivityMonitor {
private:
    std::shared_ptr<Brain> brain_;
    std::vector<FloatArray> activityHistory_;
    std::chrono::steady_clock::time_point startTime_;
    
public:
    NeuronActivityMonitor(std::shared_ptr<Brain> brain);
    
    ~NeuronActivityMonitor() = default;
    
    // Monitoring control
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Analysis methods
    FloatArray getActivityStatistics(size_t neuronIndex) const;
    FloatMatrix getActivityTimeSeries(size_t neuronIndex) const;
    std::map<size_t, double> getFiringRates() const;
    std::map<size_t, double> getBursts() const;
    
    // State analysis
    FloatArray computeCoherence() const;
    std::vector<size_t> findSynchronyClusters() const;
    std::map<size_t, FloatArray> getSpatialDistribution() const;
    
    // Export
    void exportActivityData(const std::string& filepath) const;
    std::string getActivitySummary() const;
};

/**
 * @brief Analyze network connectivity
 */
class NetworkConnectivityAnalyzer {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    NetworkConnectivityAnalyzer(std::shared_ptr<Brain> brain);
    
    ~NetworkConnectivityAnalyzer() = default;
    
    // Graph analysis
    FloatMatrix computeAdjacencyMatrix() const;
    double computeClusteringCoefficient() const;
    double computeCharacteristicPathLength() const;
    double computeSmallWorldness() const;
    
    // Topological analysis
    std::vector<size_t> findHubNeurons(double threshold = 0.8) const;
    std::map<size_t, std::vector<size_t>> findCommunities() const;
    FloatArray computeBetweennessCentrality() const;
    FloatArray computeClosenessCentrality() const;
    
    // Dynamic analysis
    FloatMatrix computeTemporalConnectivity(size_t windowSize = 100) const;
    FloatArray computeConnectionStability() const;
    
    // Export
    void exportConnectivityGraph(const std::string& filepath) const;
    std::string getConnectivitySummary() const;
};

/**
 * @brief Profile memory system activity
 */
class MemorySystemProfiler {
private:
    std::shared_ptr<Brain> brain_;
    std::chrono::steady_clock::time_point startTime_;
    
public:
    MemorySystemProfiler(std::shared_ptr<Brain> brain);
    
    ~MemorySystemProfiler() = default;
    
    // Memory analysis
    FloatArray getWorkingMemoryUsage() const;
    FloatArray getEpisodicMemoryUsage() const;
    FloatArray getSemanticMemoryUsage() const;
    FloatArray getProceduralMemoryUsage() const;
    
    // Activity patterns
    std::map<std::string, FloatArray> getMemoryAccessPatterns() const;
    std::map<std::string, FloatArray> getMemoryRetrievalDynamics() const;
    FloatArray computeMemoryStability() const;
    
    // Performance metrics
    double getMemoryRetrievalSpeed() const;
    double getMemoryStorageCapacity() const;
    double getMemoryEfficiency() const;
    
    // Export
    void exportMemoryProfile(const std::string& filepath) const;
    std::string getMemorySummary() const;
};

/**
 * @brief Analyze neuromodulation patterns
 */
class NeuromodulationAnalyzer {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    NeuromodulationAnalyzer(std::shared_ptr<Brain> brain);
    
    ~NeuromodulationAnalyzer() = default;
    
    // Neuromodulator analysis
    FloatArray getDopamineActivity() const;
    FloatArray getCuriosityActivity() const;
    FloatArray getNoveltyActivity() const;
    FloatArray getPredictionErrorActivity() const;
    FloatArray getRewardActivity() const;
    
    // Pattern analysis
    std::map<std::string, FloatArray> getModulationPatterns() const;
    FloatArray computeModulationCorrelation() const;
    FloatArray computeModulationTiming() const;
    
    // Functional analysis
    double getMotivationalDrive() const;
    double getExplorationExploitationBalance() const;
    double getLearningRateEstimate() const;
    
    // Export
    void exportModulationProfile(const std::string& filepath) const;
    std::string getModulationSummary() const;
};

/**
 * @brief Analyze development stages and progress
 */
class DevelopmentStageAnalyzer {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    DevelopmentStageAnalyzer(std::shared_ptr<Brain> brain);
    
    ~DevelopmentStageAnalyzer() = default;
    
    // Stage analysis
    std::string getCurrentStage() const;
    double getStageProgress() const;
    double getDevelopmentalAge() const;
    FloatArray getMaturationIndicators() const;
    
    // Critical period analysis
    bool isInCriticalPeriod() const;
    double getCriticalPeriodProgress() const;
    FloatArray getPlasticityModifiers() const;
    
    // Growth analysis
    FloatArray getBrainGrowthMetrics() const;
    FloatArray getSynaptogenesisRate() const;
    FloatArray getPruningEfficiency() const;
    
    // Export
    void exportDevelopmentProfile(const std::string& filepath) const;
    std::string getDevelopmentSummary() const;
};

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Compare two brain states for similarity
 * @param brain1 First brain to compare
 * @param brain2 Second brain to compare
 * @param metrics Metrics to compare
 * @return std::map<std::string, double> containing comparison results
 */
std::map<std::string, double> compareBrainStates(std::shared_ptr<Brain> brain1,
                                                std::shared_ptr<Brain> brain2,
                                                const std::vector<std::string>& metrics = {});

/**
 * @brief Compute statistical measures for brain data
 * @param data Input data array
 * @param measure Statistical measure to compute
 * @return double computed statistical measure
 */
double computeStatisticalMeasure(const FloatArray& data,
                                 const std::string& measure = "mean");

/**
 * @brief Compute confidence intervals for data
 * @param data Input data array
 * @param confidenceLevel Confidence level (0-1)
 * @return std::pair<double, double> containing confidence interval
 */
std::pair<double, double> computeConfidenceInterval(const FloatArray& data,
                                                   double confidenceLevel = 0.95);

/**
 * @brief Generate simple visualizations
 * @param data Data to visualize
 * @param filename Output file path
 * @param plotType Type of plot (histogram, line, scatter)
 */
void generateVisualization(const FloatArray& data,
                           const std::string& filename,
                           const std::string& plotType = "histogram");

/**
 * @brief Export data for external tools
 * @param data Data to export
 * @param filename Output file path
 * @param format Export format (csv, npy, h5)
 */
void exportDataForExternalTools(const FloatMatrix& data,
                                const std::string& filename,
                                const std::string& format = "csv");

// ============================================================================
// Factory Functions
// ============================================================================

/**
 * @brief Create a brain profiler instance
 * @param brain Brain to profile
 * @return Shared pointer to brain profiler
 */
std::shared_ptr<BrainProfiler> createBrainProfiler(std::shared_ptr<Brain> brain);

/**
 * @brief Create a visualization helper instance
 * @return Shared pointer to visualization helper
 */
std::shared_ptr<VisualizationHelper> createVisualizationHelper();

/**
 * @brief Create an experiment manager instance
 * @return Shared pointer to experiment manager
 */
std::shared_ptr<ExperimentManager> createExperimentManager();

/**
 * @brief Create a neuron activity monitor
 * @param brain Brain to monitor
 * @return Shared pointer to neuron activity monitor
 */
std::shared_ptr<NeuronActivityMonitor> createNeuronActivityMonitor(std::shared_ptr<Brain> brain);

/**
 * @brief Create a network connectivity analyzer
 * @param brain Brain to analyze
 * @return Shared pointer to network connectivity analyzer
 */
std::shared_ptr<NetworkConnectivityAnalyzer> createNetworkConnectivityAnalyzer(std::shared_ptr<Brain> brain);

/**
 * @brief Create a memory system profiler
 * @param brain Brain to profile
 * @return Shared pointer to memory system profiler
 */
std::shared_ptr<MemorySystemProfiler> createMemorySystemProfiler(std::shared_ptr<Brain> brain);

/**
 * @brief Create a neuromodulation analyzer
 * @param brain Brain to analyze
 * @return Shared pointer to neuromodulation analyzer
 */
std::shared_ptr<NeuromodulationAnalyzer> createNeuromodulationAnalyzer(std::shared_ptr<Brain> brain);

/**
 * @brief Create a development stage analyzer
 * @param brain Brain to analyze
 * @return Shared pointer to development stage analyzer
 */
std::shared_ptr<DevelopmentStageAnalyzer> createDevelopmentStageAnalyzer(std::shared_ptr<Brain> brain);

// ============================================================================
// Version and Documentation
// ============================================================================

/**
 * @brief Get advanced API version
 * @return Version string
 */
const std::string& getAdvancedAPIVersion();

/**
 * @brief Get advanced API documentation
 * @return Documentation string
 */
const std::string& getAdvancedAPIDoc();

/**
 * @brief Get usage examples
 * @return Examples string
 */
const std::string& getAdvancedAPIExamples();

#endif // ADVANCED_API_HPP
