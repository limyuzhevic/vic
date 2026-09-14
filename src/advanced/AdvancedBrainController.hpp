// Advanced Features and Expert Controls for NLM
// This header provides additional functionality for expert users
// including debugging, profiling, advanced configuration, and performance monitoring

#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>

namespace nlm {

// Advanced configuration parameters for expert users
struct AdvancedConfig {
    // Performance tuning
    float simulationSpeedMultiplier = 1.0f;          // Speed up/slow down simulation
    int maxStepCount = 10000;                        // Maximum simulation steps
    bool enablePerformanceProfiling = false;         // Enable performance metrics collection
    bool enableDetailedLogging = false;              // Enable detailed step-by-step logging
    
    // Memory management
    bool enableMemoryTracking = true;                // Track memory usage
    float memoryLimitMB = 512.0f;                    // Memory limit in MB
    bool enableMemoryOptimization = false;            // Optimize memory usage
    
    // Plasticity control
    bool enableDynamicPlasticity = true;             // Dynamic plasticity based on performance
    float plasticityLearningRate = 1.0f;             // Global plasticity learning rate multiplier
    bool enablePlasticityPausing = false;             // Allow pausing plasticity
    float plasticityPauseThreshold = 0.01f;          // Threshold for pausing plasticity
    
    // Debugging and analysis
    bool enableDebugMode = false;                    // Enable debug mode with additional checks
    bool enableSpikeTracing = false;                 // Trace specific neuron spikes
    bool enableSynapseTracking = false;              // Track synapse changes in detail
    bool enableActivityProfiling = false;             // Profile neural activity patterns
    
    // Experimental controls
    bool enableReplayMode = false;                   // Replay recorded episodes
    int replaySpeed = 1;                             // Replay speed (1 = real-time)
    bool enableStimulusControl = false;               // Fine control over input stimuli
    bool enableRewardShaping = false;                // Advanced reward shaping controls
    
    // Visualization and output
    bool enableDetailedOutput = false;               // Enable detailed text output
    bool enableProgressReporting = true;             // Report progress during long simulations
    float outputInterval = 1.0f;                     // Output interval in simulation time
    
    // Validation and safety
    bool enableParameterValidation = true;           // Validate parameters before simulation
    bool enableSafetyChecks = true;                   // Enable safety checks during simulation
    float weightChangeLimit = 0.1f;                  // Maximum allowed weight change per step
    float neuronThresholdLimit = 100.0f;             // Maximum neuron threshold value
    
    AdvancedConfig() = default;
};

// Performance monitoring and metrics
struct PerformanceMetrics {
    double simulationTime;                           // Total simulation time (seconds)
    size_t totalSteps;                               // Total steps completed
    size_t spikeCount;                                // Total spikes generated
    size_t learningEvents;                           // Learning events detected
    size_t plasticityUpdates;                        // Plasticity updates performed
    double cpuUsage;                                 // CPU usage percentage
    double memoryUsageMB;                            // Memory usage in MB
    std::vector<double> stepTimes;                   // Time per step (for performance analysis)
    
    PerformanceMetrics() : simulationTime(0.0), totalSteps(0), spikeCount(0), 
                          learningEvents(0), plasticityUpdates(0), cpuUsage(0.0), 
                          memoryUsageMB(0.0) {}
};

// Debug information for analysis
struct DebugInfo {
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t neuronFiringCount;
    size_t activeSynapseCount;
    std::map<NeuronId, float> neuronPotentials;      // Membrane potentials
    std::map<SynapseId, float> synapseWeights;       // Synapse weights
    std::vector<NeuronId> recentlyActiveNeurons;   // Neurons active in last N steps
    std::vector<SynapseId> recentlyModifiedSynapses; // Synapses modified in last N steps
    
    DebugInfo() : currentStep(0), currentTime(0.0), neuronFiringCount(0), 
                  activeSynapseCount(0) {}
};

// Advanced brain controller with expert features
class AdvancedBrainController {
public:
    explicit AdvancedBrainController(std::shared_ptr<Brain> brain);
    ~AdvancedBrainController();
    
    // Configuration management
    void setAdvancedConfig(const AdvancedConfig& config);
    AdvancedConfig getAdvancedConfig() const;
    
    // Performance monitoring
    void startPerformanceMonitoring();
    PerformanceMetrics getPerformanceMetrics() const;
    double getAverageStepTime() const;
    void printPerformanceReport() const;
    
    // Debug and analysis
    void enableDebugMode(bool enable);
    DebugInfo getDebugInfo() const;
    void traceNeuronActivity(NeuronId neuronId, int historySteps = 100);
    void traceSynapseChanges(SynapseId synapseId, int historySteps = 100);
    
    // Advanced plasticity control
    void enableDynamicPlasticity(bool enable);
    void setGlobalPlasticityRate(float rate);
    void pausePlasticity(bool pause);
    float getCurrentPlasticityLevel() const;
    
    // Memory management
    void enableMemoryOptimization(bool enable);
    void setMemoryLimitMB(float limitMB);
    size_t getCurrentMemoryUsageMB() const;
    void triggerMemoryCleanup();
    
    // Experimental controls
    void enableReplayMode(bool enable);
    void setReplaySpeed(int speed);
    void recordEpisode();
    void replayEpisode(size_t episodeIndex);
    size_t getRecordedEpisodeCount() const;
    
    // Advanced simulation control
    void setSimulationSpeed(float multiplier);
    void setMaxSteps(int maxSteps);
    void setOutputInterval(float interval);
    bool shouldContinueSimulation() const;
    
    // Export/import functionality
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    bool exportMetrics(const std::string& filepath) const;
    bool importAdvancedConfig(const std::string& filepath);
    
    // Expert neural control
    void injectCurrentToSpecificNeuron(NeuronId neuronId, MembranePotential current);
    void setNeuronParameters(NeuronId neuronId, float threshold, float restingPotential, float leakConductance);
    void setSynapseParameters(SynapseId synapseId, float weight, float delay, SynapseType type);
    void enableSynapsePlasticity(SynapseId synapseId, bool hebbian, bool stdp, bool rewardModulated);
    
    // Analysis and statistics
    float calculateExcitatoryInhibitoryRatio() const;
    float calculateNetworkEfficiency() const;
    float calculateLearningRate() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count = 10) const;
    std::vector<SynapseId> getMostModifiedSynapses(size_t count = 10) const;
    
    // Validation and safety
    bool validateConfiguration() const;
    bool performSafetyCheck() const;
    std::vector<std::string> getSafetyIssues() const;
    void applySafetyCorrections();
    
    // Command execution for expert users
    bool executeAdvancedCommand(const std::string& command, const std::vector<std::string>& args);
    void printAdvancedHelp() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced experiment runner with expert controls
class AdvancedExperimentRunner {
public:
    AdvancedExperimentRunner();
    ~AdvancedExperimentRunner();
    
    // Experiment management
    void addExperiment(const std::string& name, std::function<void(std::shared_ptr<Brain>)> experiment);
    void runExperiment(const std::string& name, std::shared_ptr<Brain> brain);
    void runAllExperiments(std::shared_ptr<Brain> brain);
    
    // Parameter optimization
    void enableParameterOptimization(bool enable);
    void setOptimizationGoal(const std::string& goal); // e.g., "maximize_learning", "stability", "speed"
    void runParameterSearch(std::shared_ptr<Brain> brain, int generations = 100);
    
    // Statistical analysis
    struct ExperimentResults {
        std::string name;
        double duration;
        size_t steps;
        size_t spikes;
        float learningRate;
        float finalWeightChange;
        std::vector<double> performanceMetrics;
        std::map<std::string, double> parameters;
    };
    
    std::vector<ExperimentResults> getExperimentResults() const;
    void printResultsSummary() const;
    void saveResults(const std::string& filepath) const;
    
    // Advanced visualization (text-based for CLI)
    void enableTextVisualization(bool enable);
    void printNetworkVisualization(std::shared_ptr<Brain> brain) const;
    void printActivityHeatmap(std::shared_ptr<Brain> brain) const;
    void printPlasticityMap(std::shared_ptr<Brain> brain) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Expert-level command processor
class ExpertCommandProcessor {
public:
    ExpertCommandProcessor();
    ~ExpertCommandProcessor();
    
    // Command registration
    void registerCommand(const std::string& name, const std::string& description, 
                         std::function<bool(const std::vector<std::string>&)> handler,
                         const std::string& helpText = "");
    
    // Command execution
    bool executeCommand(const std::string& commandLine, std::shared_ptr<Brain> brain);
    
    // Command completion and help
    std::vector<std::string> getCommandCompletions(const std::string& partialCommand) const;
    void printCommandHelp(const std::string& command) const;
    void printAllCommandHelp() const;
    
    // Advanced command parsing
    struct ParsedCommand {
        std::string command;
        std::vector<std::string> args;
        std::map<std::string, std::string> flags;
        bool isAdvanced;
    };
    
    ParsedCommand parseCommand(const std::string& commandLine) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory profiler for expert users
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Memory tracking
    void startTracking();
    void stopTracking();
    bool isTracking() const;
    
    // Memory allocation tracking
    struct MemoryAllocation {
        size_t size;
        std::string type;
        SimulationStep allocationStep;
        Timestamp allocationTime;
        void* pointer;
    };
    
    void trackAllocation(size_t size, const std::string& type, SimulationStep step, Timestamp time, void* pointer);
    void trackDeallocation(void* pointer);
    std::vector<MemoryAllocation> getMemoryAllocations() const;
    
    // Memory leak detection
    bool detectMemoryLeaks() const;
    std::vector<void*> getMemoryLeaks() const;
    
    // Memory usage analysis
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    std::map<std::string, size_t> getMemoryUsageByType() const;
    
    // Memory optimization suggestions
    std::vector<std::string> getOptimizationSuggestions() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance analyzer for expert users
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    // Performance measurement
    void startMeasurement();
    void stopMeasurement();
    bool isMeasuring() const;
    
    // Step timing analysis
    void recordStepTime(SimulationStep step, double duration);
    double getAverageStepTime() const;
    double getStepTimeVariance() const;
    double getStepTimeMin() const;
    double getStepTimeMax() const;
    
    // Activity profiling
    void recordActivity(SimulationStep step, size_t spikeCount, size_t activeNeurons);
    size_t getAverageSpikesPerStep() const;
    size_t getAverageActiveNeuronsPerStep() const;
    
    // Bottleneck analysis
    std::vector<SimulationStep> findBottleneckSteps(double threshold = 0.05) const;
    std::vector<SimulationStep> findSlowdownSteps(double threshold = 0.02) const;
    
    // Performance optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    
    // Report generation
    void generatePerformanceReport(const std::string& filepath) const;
    void printPerformanceReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced visualization for CLI
class AdvancedVisualizer {
public:
    AdvancedVisualizer();
    ~AdvancedVisualizer();
    
    // Network visualization
    void printNetworkTopology(std::shared_ptr<Brain> brain) const;
    void printActivityBars(std::shared_ptr<Brain> brain, size_t maxNeurons = 50) const;
    void printConnectionMatrix(std::shared_ptr<Brain> brain, size_t maxNeurons = 20) const;
    
    // Temporal visualization
    void printSpikeRaster(std::shared_ptr<Brain> brain, size_t stepsBack = 100) const;
    void printWeightEvolution(std::shared_ptr<Brain> brain, size_t synapseId = 0) const;
    void printActivityTimeline(std::shared_ptr<Brain> brain, size_t duration = 1000) const;
    
    // Statistical visualization
    void printHistogram(const std::vector<double>& data, const std::string& title, 
                       const std::string& axisLabel) const;
    void printScatterPlot(const std::vector<std::pair<double, double>>& points,
                         const std::string& xLabel, const std::string& yLabel) const;
    void printTimeSeries(const std::vector<double>& values, const std::string& title,
                       double samplingInterval = 1.0) const;
    
    // Progress and status visualization
    void printProgressBar(double progress, const std::string& label = "Progress") const;
    void printStatusLine(const std::string& status, const std::string& detail = "") const;
    void printPerformanceMetrics(const PerformanceMetrics& metrics) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm