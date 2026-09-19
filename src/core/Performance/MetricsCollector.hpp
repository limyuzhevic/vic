#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <memory>
#include <fstream>
#include <iostream>
#include "../core/Types/Types.hpp"
#include "../core/Errors/Error.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"

namespace nlm {

/**
 * @brief Metric data collection and analysis for NLM simulations
 *
 * This class collects comprehensive metrics during neural simulation runs,
 * including performance metrics, learning progress, and system state information.
 * It provides both real-time monitoring and post-simulation analysis capabilities.
 */
class MetricsCollector {
public:
    // Metric types for classification
    enum class MetricType {
        PERFORMANCE,        // Raw performance measurements
        LEARNING,           // Learning-related metrics
        STABILITY,          // System stability indicators
        EFFICIENCY,         // Resource utilization metrics
        BEHAVIORAL,         // Agent behavior patterns
        NETWORK_STATISTICS, // Neural network properties
        ENERGY_CONSUMPTION, // Energy usage metrics
        CUSTOM              // User-defined metrics
    };

    // Metric snapshot at a point in time
    struct MetricSnapshot {
        SimulationStep step;
        Timestamp timestamp;
        double elapsedTime;  // Wall-clock time
        
        // Performance metrics
        double cpuUsage;     // CPU utilization percentage
        double memoryUsage;  // Memory usage in MB
        double simulationSpeed; // Steps per second
        
        // Learning metrics
        float learningRate;  // Synaptic change rate
        float predictionError; // Prediction error magnitude
        float noveltyLevel;   // Novelty detection level
        float dopamineLevel;  // Neuromodulation level
        
        // Network metrics
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        float excitationInhibitionRatio;
        float totalSpikeCount;
        
        // Behavioral metrics
        float rewardReceived;
        float cumulativeReward;
        size_t actionCount;
        std::map<MotorCommand, size_t> actionFrequency;
        
        // Development metrics
        float plasticityModifier;
        DevelopmentalStage developmentalStage;
        double developmentalAge;
        
        // Memory metrics
        size_t workingMemoryCapacity;
        size_t episodicMemoryCount;
        float memoryConsolidationRate;
        
        // Environment interaction
        float energyConsumed;
        float healthLevel;
        bool environmentChanged;
        
        // Additional custom metrics
        std::map<std::string, float> customMetrics;
    };

    // Configuration for metrics collection
    struct MetricsConfig {
        bool enablePerformanceMetrics = true;
        bool enableLearningMetrics = true;
        bool enableNetworkMetrics = true;
        bool enableBehavioralMetrics = true;
        bool enableDevelopmentMetrics = true;
        bool enableMemoryMetrics = true;
        bool enableEnergyMetrics = true;
        
        // Sampling configuration
        size_t snapshotInterval = 100;      // Collect snapshot every N steps
        size_t maxHistorySize = 10000;      // Maximum number of snapshots to keep
        bool continuousRecording = true;    // Whether to record continuously or on-demand
        
        // Export configuration
        bool exportToCSV = false;
        bool exportToJSON = true;
        bool exportToSQLite = false;
        std::string exportFilename;
        std::string exportDirectory;
        
        // Filtering configuration
        SimulationStep startStep = 0;      // Start recording from this step
        SimulationStep endStep = 999999;   // Stop recording at this step
        float minLearningRate = 0.0f;      // Minimum learning rate threshold
        float minNoveltyLevel = 0.0f;      // Minimum novelty level threshold
    };

    // Results from metrics analysis
    struct MetricsAnalysis {
        // Performance summary
        double averageSimulationSpeed;
        double minSimulationSpeed;
        double maxSimulationSpeed;
        double memoryUsageAverage;
        
        // Learning progress
        float initialLearningRate;
        float finalLearningRate;
        float maxLearningRate;
        float totalLearningProgress;
        
        // Behavioral patterns
        float totalReward;
        float averageRewardPerStep;
        size_t maxActionsPerStep;
        float rewardVariance;
        
        // Network stability
        float stabilityIndex; // Measure of network stability
        float burstingActivity; // Measure of coordinated activity
        size_t activeNeuronVariability;
        
        // Development summary
        double totalDevelopmentTime;
        size_t stageTransisions;
        std::map<DevelopmentalStage, size_t> stageDurations;
        
        // Recommendations and insights
        std::vector<std::string> insights;
        std::vector<std::string> recommendations;
    };

    // Progress callback type for real-time monitoring
    using ProgressCallback = std::function<void(const MetricSnapshot&, size_t, size_t)>;

    MetricsCollector() = default;
    explicit MetricsCollector(const MetricsConfig& config);
    
    // Configuration and setup
    void configure(const MetricsConfig& config);
    const MetricsConfig& getConfig() const { return config_; }
    
    // Core recording functionality
    void recordSnapshot(const Brain& brain, const AgentBrain& agent, 
                       const SimpleWorld& world, SimulationStep step,
                       Timestamp timestamp, double wallClockTime);
    
    // Data access
    const std::vector<MetricSnapshot>& getSnapshots() const { return snapshots_; }
    size_t getSnapshotCount() const { return snapshots_.size(); }
    const MetricSnapshot* getSnapshot(SimulationStep step) const;
    
    // Real-time monitoring
    void setProgressCallback(ProgressCallback callback) { progressCallback_ = callback; }
    void enableRealTimeMonitoring(bool enabled) { realTimeMonitoring_ = enabled; }
    
    // Analysis methods
    MetricsAnalysis analyze() const;
    std::vector<MetricSnapshot> filterSnapshots(const std::function<bool(const MetricSnapshot&)>& predicate) const;
    
    // Export functionality
    bool exportToCSV(const std::string& filename) const;
    bool exportToJSON(const std::string& filename) const;
    bool exportToSQLite(const std::string& filename) const;
    
    // Utility methods
    double calculateRunningAverage(size_t numSamples = 100) const;
    double calculatePerformanceScore() const;
    std::string generateReport() const;
    
    // Control
    void clear() { snapshots_.clear(); }
    void reset() { snapshots_.clear(); progressCallback_ = nullptr; }
    void pause() { recordingPaused_ = true; }
    void resume() { recordingPaused_ = false; }
    bool isPaused() const { return recordingPaused_; }
    bool isRecording() const { return !snapshots_.empty(); }

private:
    MetricsConfig config_;
    std::vector<MetricSnapshot> snapshots_;
    ProgressCallback progressCallback_;
    bool realTimeMonitoring_ = false;
    bool recordingPaused_ = false;
    
    // Helper methods
    void updateProgress(SimulationStep step, size_t totalSteps) const;
    void validateConfig() const;
    void normalizeMetricValues(MetricSnapshot& snapshot) const;
    static std::string generateTimestamp() { return ""; } // TODO: implement
};

} // namespace nlm