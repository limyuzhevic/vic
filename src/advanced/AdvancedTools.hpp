#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <queue>
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../brain_analysis/BrainAnalysisTools.hpp"
#include "../brain_management/BrainController.hpp"
#include "../configuration_management/AdvancedConfigManager.hpp"

namespace nlm {

// Advanced Performance Profiler - Comprehensive performance analysis and optimization
class AdvancedPerformanceProfiler {
public:
    AdvancedPerformanceProfiler(std::shared_ptr<Logger> logger);
    ~AdvancedPerformanceProfiler();
    
    // Profiling setup and control
    void startProfiling(const std::string& profileName, 
                       const std::map<std::string, std::string>& tags = {});
    void stopProfiling();
    bool isProfiling() const;
    std::string getCurrentProfile() const;
    
    // Time profiling with detailed metrics
    struct TimeMeasurement {
        std::string operationName;
        double duration;  // in seconds
        std::map<std::string, std::string> parameters;
        double cpuUsage;
        double memoryUsage;
        Timestamp timestamp;
        size_t threadId;
    };
    
    void startTimer(const std::string& timerName, 
                   const std::map<std::string, std::string>& params = {});
    double stopTimer(const std::string& timerName);
    double getTimerValue(const std::string& timerName) const;
    void resetTimer(const std::string& timerName);
    std::vector<TimeMeasurement> getTimerHistory(const std::string& timerName) const;
    std::map<std::string, std::vector<TimeMeasurement>> getAllTimers() const;
    void resetAllTimers();
    
    // Memory profiling
    struct MemorySnapshot {
        double totalMemory;      // in MB
        double usedMemory;       // in MB
        double availableMemory;  // in MB
        std::map<std::string, double> regionMemory;
        Timestamp timestamp;
    };
    
    void enableMemoryProfiling(bool enable);
    bool isMemoryProfilingEnabled() const;
    MemorySnapshot takeMemorySnapshot();
    std::vector<MemorySnapshot> getMemoryHistory(size_t maxSamples = 1000) const;
    double getCurrentMemoryUsage() const;
    double getPeakMemoryUsage() const;
    double getMemoryGrowthRate() const;
    
    // CPU and performance core profiling
    struct CPUPerformance {
        double overallLoad;
        std::map<std::string, double> coreLoads;
        std::map<std::string, double> processLoads;
        Timestamp timestamp;
    };
    
    CPUPerformance getCurrentCPUPerformance() const;
    std::vector<CPUPerformance> getCPUMetrics(size_t samples = 100) const;
    double getAverageCPULoad() const;
    double getCPUCapacityUtilization() const;
    
    // Spike and neural event profiling
    struct SpikeEvent {
        NeuronId neuronId;
        SimulationStep step;
        Timestamp timestamp;
        float membranePotential;
        double processingTime;  // for this spike
        size_t threadId;
    };
    
    void enableSpikeProfiling(bool enable);
    bool isSpikeProfilingEnabled() const;
    void recordSpike(const SpikeEvent& spike);
    std::vector<SpikeEvent> getSpikeHistory(SimulationStep fromStep = 0) const;
    size_t getTotalSpikeCount() const;
    size_t getSpikeCountAtStep(SimulationStep step) const;
    double getAverageSpikesPerStep() const;
    float getAverageFiringRate() const;
    
    // Plasticity and synaptic change profiling
    struct PlasticityEvent {
        std::string eventType;  // "LTP", "LTD", "reward_modulation", etc.
        SynapseId synapseId;
        SynapticWeight delta;
        float preVoltage;
        float postVoltage;
        SimulationStep step;
        Timestamp timestamp;
        double processingTime;
    };
    
    void enablePlasticityProfiling(bool enable);
    bool isPlasticityProfilingEnabled() const;
    void recordPlasticityEvent(const PlasticityEvent& event);
    std::vector<PlasticityEvent> getPlasticityHistory(SimulationStep fromStep = 0) const;
    double getTotalPlasticityChange() const;
    double getAveragePlasticityPerStep() const;
    std::map<std::string, double> getPlasticityStatistics() const;
    
    // Brain operation profiling
    struct OperationProfile {
        std::string operationName;
        double totalDuration;
        size_t callCount;
        double averageDuration;
        double minDuration;
        double maxDuration;
        std::map<std::string, std::string> parameters;
        std::map<std::string, double> metrics;
    };
    
    void profileBrainOperation(const std::string& operationName,
                              std::function<void()> operation,
                              const std::map<std::string, std::string>& parameters = {});
    OperationProfile getOperationProfile(const std::string& operationName) const;
    std::vector<std::string> getAllOperationProfiles() const;
    std::map<std::string, OperationProfile> getAllOperationDetails() const;
    void clearOperationProfiles();
    
    // Performance analysis
    struct PerformanceAnalysis {
        std::map<std::string, double> bottlenecks;
        std::map<std::string, double> performanceScores;
        std::vector<std::string> recommendations;
        double overallHealthScore;
        std::vector<std::string> issues;
    };
    
    PerformanceAnalysis analyzePerformance(const std::string& profileName = "default") const;
    std::vector<std::string> generateOptimizationRecommendations() const;
    double calculatePerformanceScore() const;
    std::map<std::string, double> estimateOptimizationGain(const std::string& bottleneck) const;
    
    // Bottleneck detection
    std::vector<std::string> detectMemoryBottlenecks() const;
    std::vector<std::string> detectCPU bottlenecks() const;
    std::vector<std::string> detectNetworkBottlenecks() const;
    std::vector<std::string> detectI/Obottlenecks() const;
    
    // Profiling data export and import
    bool exportProfiles(const std::string& filepath) const;
    bool importProfiles(const std::string& filepath);
    std::string generateProfileReport() const;
    std::string generateDetailedReport(const std::string& profileName = "default") const;
    
    // Performance tuning and presets
    void autoTuneParameters();
    void applyPerformancePreset(const std::string& presetName);
    bool hasPerformancePreset(const std::string& presetName) const;
    std::vector<std::string> getAvailablePresets() const;
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enable, double sampleInterval = 0.1);
    bool isRealTimeMonitoringEnabled() const;
    std::map<std::string, double> getRealTimeMetrics() const;
    std::map<std::string, std::vector<double>> getTimeSeriesData(const std::string& metric, 
                                                                 size_t samples = 100) const;
    
    // Statistical analysis
    double calculateMovingAverage(const std::string& metricName, 
                                  size_t windowSize = 100) const;
    double calculateStandardDeviation(const std::string& metricName, 
                                     size_t samples = 100) const;
    double calculatePercentile(const std::string& metricName, 
                               double percentile, size_t samples = 100) const;
    
    // Pattern detection and anomaly detection
    std::vector<std::string> detectAnomalies(const std::string& profileName = "default") const;
    bool isAnomaly(const std::string& metricName, double value, 
                   double threshold = 2.0) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced Memory Manager - Comprehensive memory management with episodic support
class AdvancedMemoryManager {
public:
    AdvancedMemoryManager(std::shared_ptr<Logger> logger);
    ~AdvancedMemoryManager();
    
    // Episodic memory management
    struct EpisodicMemoryEntry {
        SimulationStep step;
        Timestamp timestamp;
        std::string description;
        std::vector<NeuronId> involvedNeurons;
        std::vector<float> neuronStates;
        std::map<std::string, ConfigValue> metadata;
        float reward;
        float predictionError;
        std::string context;
        std::vector<std::string> tags;
    };
    
    void recordExperience(const std::string& description,
                         const std::vector<NeuronId>& involvedNeurons,
                         const std::vector<float>& neuronStates,
                         float reward,
                         float predictionError,
                         const std::string& context = "",
                         const std::vector<std::string>& tags = {});
    
    void replayExperience(size_t episodeIndex, float replayStrength = 1.0f);
    void replayRecentEpisodes(size_t count = 10, float strength = 0.5f);
    void replaySimilarEpisodes(const std::vector<float>& targetState,
                               size_t maxEpisodes = 5,
                               float strength = 0.7f);
    void replayBasedOnReward(float minReward, size_t maxEpisodes = 10);
    void replayBasedOnNovelty(float minNovelty, size_t maxEpisodes = 10);
    
    // Memory consolidation and optimization
    void consolidateMemory(float threshold = 0.7f);
    void consolidateByImportance();
    void consolidateByRecency(float recencyWeight = 0.3f);
    void optimizeMemoryLayout();
    void compressMemories();
    void pruneMemories(size_t maxEntries);
    
    // Memory analysis and insights
    std::vector<EpisodicMemoryEntry> searchMemories(const std::string& keyword) const;
    std::vector<EpisodicMemoryEntry> searchMemoriesByTag(const std::string& tag) const;
    std::vector<EpisodicMemoryEntry> searchMemoriesByContext(
        const std::string& context) const;
    
    std::vector<EpisodicMemoryEntry> getRecentMemories(size_t count = 10) const;
    std::vector<EpisodicMemoryEntry> getMemoriesWithReward(float minReward) const;
    std::vector<EpisodicMemoryEntry> getMemoriesWithPredictionError(float threshold) const;
    
    // Memory statistics and metrics
    size_t getEpisodeCount() const;
    float getMemoryCapacityUtilization() const;
    float getReplayEffectiveness() const;
    std::vector<std::string> getMemoryHealthIssues() const;
    
    // Memory export and import
    bool exportMemories(const std::string& filepath) const;
    bool importMemories(const std::string& filepath);
    void backupMemories(const std::string& filepath) const;
    void restoreMemories(const std::string& filepath);
    void exportMemoriesAsText(const std::string& filepath) const;
    bool importMemoriesFromText(const std::string& filepath);
    
    // Memory quality metrics
    double calculateMemoryCoherence() const;
    double calculateMemoryStability() const;
    double calculateMemoryEfficiency() const;
    std::vector<float> analyzeMemoryPatterns() const;
    std::map<std::string, double> getMemoryQualityMetrics() const;
    
    // Memory management controls
    void adjustReplayParameters(float consolidationRate, float noveltyBoost);
    void setReplayBudget(size_t maxEpisodesPerStep);
    size_t getReplayBudget() const;
    void enableReplay(bool enable);
    bool isReplayEnabled() const;
    void setConsolidationThreshold(float threshold);
    float getConsolidationThreshold() const;
    
    // Memory replay analysis
    std::vector<std::pair<size_t, float>> getReplayEffects(size_t episodeIndex) const;
    std::map<size_t, float> analyzeReplayImpact() const;
    float estimateMemoryValue(size_t episodeIndex) const;
    
    // Memory compression and storage optimization
    void enableCompression(bool enable);
    bool isCompressionEnabled() const;
    void setCompressionLevel(int level);
    int getCompressionLevel() const;
    size_t getCompressedMemorySize() const;
    size_t getOriginalMemorySize() const;
    double getCompressionRatio() const;
    
    // Memory encryption and security
    void enableEncryption(bool enable);
    bool isEncryptionEnabled() const;
    void setEncryptionKey(const std::string& key);
    bool encryptMemories();
    bool decryptMemories();
    bool isMemoriesEncrypted() const;
    
    // Advanced memory operations
    void mergeSimilarMemories(float similarityThreshold);
    void deduplicateMemories();
    void archiveOldMemories(size_t maxAgeSteps);
    void reorganizeMemoryIndex();
    
    // Memory monitoring
    void enableMemoryMonitoring(bool enable);
    bool isMemoryMonitoringEnabled() const;
    std::map<std::string, double> getMemoryMetrics() const;
    std::map<std::string, std::vector<double>> getMemoryTimeSeries(const std::string& metric,
                                                                    size_t samples = 100) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced Toolkit - Master container for all advanced utilities
class AdvancedToolkit {
public:
    AdvancedToolkit(std::shared_ptr<Brain> brain = nullptr,
                   std::shared_ptr<Environment> environment = nullptr,
                   std::shared_ptr<Logger> logger = nullptr);
    ~AdvancedToolkit();
    
    // Component access
    BrainStateAnalyzer* getBrainAnalyzer() const;
    BrainController* getBrainController() const;
    AdvancedConfigManager* getConfigManager() const;
    AdvancedPerformanceProfiler* getPerformanceProfiler() const;
    AdvancedSimulationController* getSimulationController() const;
    AdvancedMemoryManager* getMemoryManager() const;
    
    // Initialization and configuration
    bool initialize();
    bool initializeWithConfig(const std::map<std::string, ConfigValue>& config);
    bool isInitialized() const;
    std::string getInitializationStatus() const;
    
    // Configuration and setup
    void configureBrain(std::shared_ptr<Brain> brain);
    void configureEnvironment(std::shared_ptr<Environment> environment);
    void configureLogger(std::shared_ptr<Logger> logger);
    std::shared_ptr<Brain> getBrain() const;
    std::shared_ptr<Environment> getEnvironment() const;
    std::shared_ptr<Logger> getLogger() const;
    
    // Component management
    bool hasComponent(const std::string& componentName) const;
    std::vector<std::string> getAvailableComponents() const;
    bool enableComponent(const std::string& componentName);
    bool disableComponent(const std::string& componentName);
    bool isComponentEnabled(const std::string& componentName) const;
    
    // System operations
    void reset();
    void shutdown();
    void emergencyStop();
    std::string getStatusReport() const;
    
    // Analysis and reporting
    std::string generateSystemReport() const;
    std::map<std::string, double> getSystemMetrics() const;
    std::vector<std::string> getSystemWarnings() const;
    std::vector<std::string> getSystemErrors() const;
    
    // Batch operations
    void executeBatch(const std::vector<std::function<void()>>& operations);
    size_t getBatchSize() const;
    void setBatchSize(size_t size);
    std::vector<std::string> getBatchResults() const;
    void clearBatchResults();
    
    // Profiling and recording
    bool startRecording(const std::string& filepath);
    bool stopRecording();
    bool startPlayback(const std::string& filepath);
    bool stopPlayback();
    bool isRecording() const;
    bool isPlaying() const;
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enable, double interval = 1.0);
    bool isRealTimeMonitoringEnabled() const;
    std::map<std::string, double> getRealTimeMetrics() const;
    
    // Analysis and optimization
    std::string analyzeSystemPerformance() const;
    std::vector<std::string> getOptimizationRecommendations() const;
    double calculateSystemHealthScore() const;
    
    // Export and import
    bool exportConfiguration(const std::string& filepath) const;
    bool importConfiguration(const std::string& filepath);
    bool exportReports(const std::string& directory) const;
    bool importReports(const std::string& directory);
    
    // Utility functions
    void wait(double seconds) const;
    void printStatus() const;
    void printHelp(const std::string& topic = "") const;
    
    // Singleton pattern support
    static AdvancedToolkit& getInstance();
    static void initializeSingleton(std::shared_ptr<Brain> brain = nullptr,
                                   std::shared_ptr<Environment> environment = nullptr,
                                   std::shared_ptr<Logger> logger = nullptr);
    static void shutdownSingleton();
    static bool isSingletonInitialized();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Factory function for creating AdvancedSimulationController
std::shared_ptr<AdvancedSimulationController> createAdvancedSimulationController(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Environment> environment,
    std::shared_ptr<Logger> logger);

// Factory function for creating AdvancedMemoryManager
std::shared_ptr<AdvancedMemoryManager> createAdvancedMemoryManager(
    std::shared_ptr<Logger> logger);

// Factory function for creating AdvancedPerformanceProfiler
std::shared_ptr<AdvancedPerformanceProfiler> createAdvancedPerformanceProfiler(
    std::shared_ptr<Logger> logger);

// Factory function for creating AdvancedConfigManager
std::shared_ptr<AdvancedConfigManager> createAdvancedConfigManager(
    std::shared_ptr<Logger> logger);

// Factory function for creating BrainController
std::shared_ptr<BrainController> createBrainController(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Logger> logger);

// Factory function for creating BrainStateAnalyzer
std::shared_ptr<BrainStateAnalyzer> createBrainStateAnalyzer(
    std::shared_ptr<Logger> logger);

// Composite factory function for creating complete AdvancedToolkit
std::shared_ptr<AdvancedToolkit> createCompleteAdvancedToolkit(
    std::shared_ptr<Brain> brain = nullptr,
    std::shared_ptr<Environment> environment = nullptr,
    std::shared_ptr<Logger> logger = nullptr,
    const std::string& profile = "default");

} // namespace nlm
