#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include "Visualizer.hpp"
#include "NetworkVisualizer.hpp"
#include "SpikeVisualizer.hpp"
#include "DataExporter.hpp"
#include "AnimationGenerator.hpp"
#include "WebDashboard.hpp"
#include "HTMLExporter.hpp"

namespace nlm {

class Brain;
class Neuron;
class Synapse;
class SpikeEvent;

/**
 * Visualization experiment system for NLM
 * 
 * Provides comprehensive visualization experiments for:
 * - Neural network visualization experiments
 * - Spike activity analysis experiments
 * - Learning progress visualization experiments
 * - Performance visualization experiments
 * - Comparative visualization experiments
 * - Time-series visualization experiments
 * - Multi-condition experiments
 * - Statistical analysis experiments
 * - Visualization parameter optimization
 * - Experiment reproducibility
 * - Experiment documentation
 * - Experiment validation
 * 
 * Features:
 * - Experiment configuration management
 * - Data collection and analysis
 * - Visualization generation
 * - Result comparison
 * - Statistical analysis
 * - Parameter optimization
 * - Experiment reproducibility
 * - Documentation generation
 * - Error handling
 * - Progress reporting
 * - Resource management
 * 
 * Integration with:
 * - All visualization systems
 * - Data export systems
 * - Experiment management system
 * - Configuration system
 * 
 * @see Visualizer
 * @see NetworkVisualizer
 * @see SpikeVisualizer
 * @see DataExporter
 * @see AnimationGenerator
 * @see WebDashboard
 * @see HTMLExporter
 */
class VisualizationExperiment {
public:
    /**
     * Experiment status enumeration
     */
    enum class ExperimentStatus {
        PENDING,
        RUNNING,
        PAUSED,
        COMPLETED,
        FAILED,
        CANCELLED
    };

    /**
     * Experiment type enumeration
     */
    enum class ExperimentType {
        NETWORK_VISUALIZATION,
        SPIKE_ANALYSIS,
        LEARNING_PROGRESS,
        PERFORMANCE_ANALYSIS,
        COMPARATIVE,
        TIME_SERIES,
        PARAMETER_OPTIMIZATION,
        VALIDATION,
        REPRODUCTION
    };

    /**
     * Experiment configuration
     */
    struct ExperimentConfig {
        std::string name;
        std::string description;
        ExperimentType type = ExperimentType::NETWORK_VISUALIZATION;
        std::string inputDataPath;
        std::string outputPath;
        std::string visualizationType;
        std::string visualizationFormat;
        double duration = 60.0;
        double timeStep = 0.1;
        int frameRate = 30;
        bool enableRealTimeUpdates = false;
        bool generateAnimations = false;
        bool exportResults = true;
        bool generateReports = true;
        bool enableWebDashboard = false;
        int webPort = 8080;
        bool enableStatistics = true;
        bool enableValidation = true;
        double accuracyThreshold = 0.95;
        std::vector<std::string> conditions;
        std::vector<std::string> parameters;
        std::unordered_map<std::string, std::string> metadata;
        std::string createdBy;
        std::chrono::system_clock::time_point createdAt;
        std::unordered_map<std::string, std::string> customSettings;
    };

    /**
     * Experiment result
     */
    struct ExperimentResult {
        std::string experimentId;
        std::string name;
        ExperimentStatus status;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        double totalTime = 0.0;
        bool success = false;
        std::string message;
        std::unordered_map<std::string, std::string> results;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        std::unordered_map<std::string, double> metrics;
        std::string visualizationOutput;
        std::string dataExportPath;
        std::string reportPath;
        bool webDashboardStarted = false;
        int webPort = 0;
        std::string webUrl;
    };

    /**
     * Experiment parameter
     */
    struct ExperimentParameter {
        std::string name;
        std::string type; // string, int, double, bool, enum
        std::string value;
        std::string description;
        std::vector<std::string> allowedValues;
        bool required = false;
        double minValue = std::numeric_limits<double>::lowest();
        double maxValue = std::numeric_limits<double>::max();
        double defaultValue;
    };

    /**
     * Experiment condition
     */
    struct ExperimentCondition {
        std::string name;
        std::string description;
        std::unordered_map<std::string, std::string> parameters;
        std::vector<std::string> dependencies;
        bool enabled = true;
    };

    /**
     * Experiment constructor
     */
    VisualizationExperiment();
    
    /**
     * VisualizationExperiment constructor with configuration
     */
    explicit VisualizationExperiment(const ExperimentConfig& config);
    
    /**
     * VisualizationExperiment destructor
     */
    ~VisualizationExperiment();

    /**
     * Configure experiment
     */
    void configure(const ExperimentConfig& config);
    
    /**
     * Set brain data
     */
    void setBrain(const Brain& brain);
    
    /**
     * Set spike data
     */
    void setSpikeData(const std::vector<SpikeEvent>& spikes);
    
    /**
     * Add experiment parameter
     */
    void addParameter(const ExperimentParameter& parameter);
    
    /**
     * Add experiment condition
     */
    void addCondition(const ExperimentCondition& condition);
    
    /**
     * Add visualization type
     */
    void addVisualizationType(const std::string& type);
    
    /**
     * Start experiment
     */
    bool start();
    
    /**
     * Pause experiment
     */
    void pause();
    
    /**
     * Resume experiment
     */
    void resume();
    
    /**
     * Stop experiment
     */
    void stop();
    
    /**
     * Check if experiment is running
     */
    bool isRunning() const;
    
    /**
     * Check if experiment is completed
     */
    bool isCompleted() const;
    
    /**
     * Run experiment
     */
    ExperimentResult run();
    
    /**
     * Run experiment with custom data
     */
    ExperimentResult run(const Brain& brain, const std::vector<SpikeEvent>& spikes);
    
    /**
     * Get experiment result
     */
    ExperimentResult getResult() const;
    
    /**
     * Get experiment status
     */
    ExperimentStatus getStatus() const;
    
    /**
     * Export experiment results
     */
    bool exportResults(const std::string& outputPath) const;
    
    /**
     * Generate experiment report
     */
    std::string generateReport() const;
    
    /**
     * Validate experiment
     */
    bool validate() const;
    
    /**
     * Get experiment configuration
     */
    ExperimentConfig getConfig() const;
    
    /**
     * Set progress callback
     */
    void setProgressCallback(const std::function<void(double, const std::string&)>& callback);
    
    /**
     * Set result callback
     */
    void setResultCallback(const std::function<void(const ExperimentResult&)>& callback);
    
    /**
     * Save experiment
     */
    bool save(const std::string& filename) const;
    
    /**
     * Load experiment
     */
    static VisualizationExperiment load(const std::string& filename);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Suppress warnings about unused private members
    VisualizationExperiment(const VisualizationExperiment&) = delete;
    VisualizationExperiment& operator=(const VisualizationExperiment&) = delete;
};

} // namespace nlm
