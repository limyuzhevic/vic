// NLM Advanced Interface Library
// Advanced user interface components for expert users
// Provides CLI, UI, debugging, and monitoring capabilities

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <map>
#include <unordered_map>

namespace nlm {

// Command line interface for advanced operations
class AdvancedCLI {
public:
    enum class Command {
        UNKNOWN,
        HELP,
        VERSION,
        CONFIG,
        CONFIG_VALIDATE,
        CONFIG_SHOW,
        CONFIG_SET,
        CONFIG_EXPORT,
        CONFIG_IMPORT,
        CONFIG_TEMPLATE,
        EXPERIMENT,
        EXPERIMENT_RUN,
        EXPERIMENT_LIST,
        EXPERIMENT_STATUS,
        EXPERIMENT_STOP,
        MONITOR,
        MONITOR_START,
        MONITOR_STOP,
        MONITOR_METRICS,
        DEBUG,
        DEBUG_NEURONS,
        DEBUG_SYNAPSES,
        DEBUG_MEMORY,
        DEBUG_PREDICTION,
        DEBUG_NEUROMODULATION,
        DEBUG_PLASTICITY,
        VISUALIZATION,
        VISUALIZATION_TOGGLE,
        VISUALIZATION_UPDATE,
        VISUALIZATION_EXPORT,
        PERFORMANCE,
        PERFORMANCE_PROFILE,
        PERFORMANCE_STATS,
        PERFORMANCE_NETWORK,
        BATCH,
        BATCH_RUN,
        BATCH_STATUS,
        BATCH_CANCEL,
        CONFIG_SCHEMA,
        CONFIG_VALIDATE_ALL,
        CONFIG_OPTIMIZE,
        CONFIG_BACKUP,
        CONFIG_RESTORE,
        SYSTEM_INFO,
        SYSTEM_LOGS,
        SYSTEM_RESOURCES,
        SYSTEM_NETWORK,
        HELP_COMMAND
    };

    struct CommandArgs {
        Command command;
        std::vector<std::string> args;
        std::unordered_map<std::string, std::string> options;
        bool verbose;
        bool jsonOutput;
        std::string outputFile;
        std::chrono::system_clock::time_point timestamp;
    };

    AdvancedCLI();
    ~AdvancedCLI();

    // Parse command line arguments
    CommandArgs parse(int argc, char** argv);

    // Execute command
    bool execute(const CommandArgs& args, std::ostream& output = std::cout);

    // Get help text for command
    std::string getHelp(Command command) const;

    // Get command from string
    Command getCommand(const std::string& cmd) const;

    // Check if command requires experiment context
    bool requiresExperiment(Command command) const;

    // Check if command is experiment-related
    bool isExperimentCommand(Command command) const;

    // Check if command is visualization-related
    bool isVisualizationCommand(Command command) const;

    // Check if command is debugging-related
    bool isDebugCommand(Command command) const;

    // Check if command is monitoring-related
    bool isMonitoringCommand(Command command) const;

    // Check if command is performance-related
    bool isPerformanceCommand(Command command) const;

    // Check if command is configuration-related
    bool isConfigCommand(Command command) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Interactive UI interface for expert users
class ExpertUI {
public:
    struct UIConfig {
        bool enableNeurons;
        bool enableSynapses;
        bool enableMemory;
        bool enablePrediction;
        bool enableNeuromodulation;
        bool enablePlasticity;
        bool enableVisualization;
        bool enableDebug;
        double refreshRate;
        bool showAdvanced;
        bool interactiveMode;
        std::string layout;
    };

    ExpertUI();
    ~ExpertUI();

    // Initialize UI with configuration
    bool initialize(const UIConfig& config);

    // Start the UI main loop
    void run();

    // Stop the UI
    void stop();

    // Check if UI is active
    bool isActive() const;

    // Update UI state
    void update(double deltaTime);

    // Render UI
    void render();

    // Handle user input
    bool handleInput(const std::string& input);

    // Show help
    void showHelp() const;

    // Show dashboard
    void showDashboard() const;

    // Show neurons panel
    void showNeuronsPanel() const;

    // Show synapses panel
    void showSynapsesPanel() const;

    // Show memory panel
    void showMemoryPanel() const;

    // Show prediction panel
    void showPredictionPanel() const;

    // Show neuromodulation panel
    void showNeuromodulationPanel() const;

    // Show plasticity panel
    void showPlasticityPanel() const;

    // Show debug panel
    void showDebugPanel() const;

    // Show configuration panel
    void showConfigurationPanel() const;

    // Show experiments panel
    void showExperimentsPanel() const;

    // Show performance panel
    void showPerformancePanel() const;

    // Set current brain reference (for inspection)
    void setBrain(std::shared_ptr<class Brain> brain);

    // Set configuration reference (for inspection)
    void setConfig(std::shared_ptr<class Config> config);

    // Set experiment reference (for inspection)
    void setExperiment(std::shared_ptr<class Experiment> experiment);

    // Set performance monitor reference (for inspection)
    void setPerformanceMonitor(std::shared_ptr<class PerformanceMonitor> monitor);

    // Set debug tools reference (for inspection)
    void setDebugTools(std::shared_ptr<class DebugTools> tools);

    // Set visualization interface reference (for inspection)
    void setVisualizationInterface(std::shared_ptr<class VisualizationInterface> viz);

    // Set performance monitor reference (for inspection)
    void setPerformanceMonitor(std::shared_ptr<class PerformanceMonitor> monitor);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration validation and schema management
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();

    // Load validation schema
    bool loadSchema(const std::string& schemaFile);

    // Load validation schema from string
    bool loadSchemaFromString(const std::string& schemaContent);

    // Validate configuration against schema
    std::vector<std::string> validate(const class Config& config) const;

    // Validate specific configuration entries
    std::vector<std::string> validateEntries(const std::vector<std::string>& keys, const class Config& config) const;

    // Get validation errors
    std::vector<std::string> getErrors() const;

    // Clear errors
    void clearErrors();

    // Check if configuration is valid
    bool isValid(const class Config& config) const;

    // Get validation summary
    std::string getValidationSummary(const class Config& config) const;

    // Export validation report
    bool exportValidationReport(const class Config& config, const std::string& outputFile) const;

    // Import validation schema
    bool importValidationSchema(const std::string& schemaFile);

    // Create default validation schema
    bool createDefaultSchema(const std::string& outputFile);

    // Optimize configuration based on schema
    std::unordered_map<std::string, double> optimize(const class Config& config) const;

    // Get suggested values for optimization
    std::unordered_map<std::string, std::string> getOptimizationSuggestions(const class Config& config) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Real-time monitoring and metrics collection
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        // Brain metrics
        uint64_t totalSpikeCount;
        uint64_t firingNeuronCount;
        uint64_t totalSynapseCount;
        uint64_t activeSynapseCount;
        double avgFiringRate;
        double maxFiringRate;
        double avgWeight;
        double weightVariance;

        // Memory metrics
        size_t workingMemoryUsage;
        size_t episodicMemoryUsage;
        size_t semanticMemoryUsage;
        size_t proceduralMemoryUsage;
        uint64_t totalMemoryEvents;

        // Neuromodulation metrics
        double dopamineLevel;
        double curiosityLevel;
        double noveltyLevel;
        double rewardValue;

        // Prediction metrics
        double predictionAccuracy;
        double predictionConfidence;
        uint64_t predictionErrors;
        uint64_t totalPredictions;

        // Plasticity metrics
        uint64_t synapticChanges;
        uint64_t totalPlasticityEvents;
        double avgPlasticityRate;

        // Performance metrics
        double cpuUsage;
        double memoryUsage;
        double networkTraffic;
        uint64_t systemUptime;
        uint64_t simulationSteps;

        // Timing metrics
        double currentStepTime;
        double avgStepTime;
        double maxStepTime;
        std::chrono::system_clock::time_point lastUpdate;
    };

    PerformanceMonitor();
    ~PerformanceMonitor();

    // Start monitoring
    void startMonitoring();

    // Stop monitoring
    void stopMonitoring();

    // Update metrics (should be called each simulation step)
    void updateMetrics(const class Brain& brain);

    // Get current metrics
    PerformanceMetrics getMetrics() const;

    // Start recording metrics to file
    bool startRecording(const std::string& outputFile);

    // Stop recording metrics
    void stopRecording();

    // Export metrics to JSON
    bool exportMetrics(const std::string& outputFile) const;

    // Export metrics to CSV
    bool exportMetricsCSV(const std::string& outputFile) const;

    // Get performance statistics
    std::unordered_map<std::string, double> getStatistics() const;

    // Reset metrics
    void resetMetrics();

    // Set monitoring interval
    void setMonitoringInterval(double interval);

    // Check if monitoring is active
    bool isMonitoring() const;

    // Set brain reference (for inspection)
    void setBrain(std::shared_ptr<class Brain> brain);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Debugging and analysis tools
class DebugTools {
public:
    DebugTools();
    ~DebugTools();

    // Enable/disable debugging
    void setDebugEnabled(bool enabled);

    // Check if debugging is enabled
    bool isDebugEnabled() const;

    // Get brain information for debugging
    std::string getBrainInfo(const class Brain& brain) const;

    // Get neuron information for debugging
    std::vector<std::string> getNeuronInfo(const class Brain& brain, int neuronId = -1) const;

    // Get synapse information for debugging
    std::vector<std::string> getSynapseInfo(const class Brain& brain, int synapseId = -1) const;

    // Get memory information for debugging
    std::string getMemoryInfo(const class Brain& brain) const;

    // Get prediction information for debugging
    std::string getPredictionInfo(const class Brain& brain) const;

    // Get neuromodulation information for debugging
    std::string getNeuromodulationInfo(const class Brain& brain) const;

    // Get plasticity information for debugging
    std::string getPlasticityInfo(const class Brain& brain) const;

    // Get configuration information for debugging
    std::string getConfigInfo(const class Config& config) const;

    // Get experiment information for debugging
    std::string getExperimentInfo(const class Experiment& experiment) const;

    // Get network information for debugging
    std::string getNetworkInfo(const class Brain& brain) const;

    // Get connectivity information for debugging
    std::string getConnectivityInfo(const class Brain& brain) const;

    // Get spike information for debugging
    std::string getSpikeInfo(const class Brain& brain, size_t maxSpikes = 100) const;

    // Get weight matrix for debugging
    std::string getWeightMatrix(const class Brain& brain, int regionId = 1) const;

    // Get synaptic current for debugging
    std::string getSynapticCurrents(const class Brain& brain) const;

    // Get neural activity for debugging
    std::string getNeuralActivity(const class Brain& brain) const;

    // Get simulation state for debugging
    std::string getSimulationState(const class Brain& brain) const;

    // Export debugging information to file
    bool exportDebugInfo(const class Brain& brain, const std::string& outputFile) const;

    // Set brain reference (for inspection)
    void setBrain(std::shared_ptr<class Brain> brain);

    // Set configuration reference (for inspection)
    void setConfig(std::shared_ptr<class Config> config);

    // Set experiment reference (for inspection)
    void setExperiment(std::shared_ptr<class Experiment> experiment);

    // Set visualization interface reference (for inspection)
    void setVisualizationInterface(std::shared_ptr<class VisualizationInterface> viz);

    // Set performance monitor reference (for inspection)
    void setPerformanceMonitor(std::shared_ptr<class PerformanceMonitor> monitor);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Batch processing for experiments
class BatchProcessor {
public:
    struct BatchConfig {
        std::string name;
        std::vector<std::string> experimentFiles;
        std::unordered_map<std::string, std::string> configOverrides;
        bool runInParallel;
        uint32_t maxParallelExperiments;
        uint32_t retryCount;
        double timeoutSeconds;
        std::string outputDirectory;
        bool saveResults;
        bool cleanupAfterCompletion;
    };

    struct BatchResult {
        std::string batchId;
        std::string status;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        uint32_t totalExperiments;
        uint32_t completedExperiments;
        uint32_t failedExperiments;
        std::unordered_map<std::string, std::string> experimentResults;
        std::vector<std::string> errorMessages;
    };

    BatchProcessor();
    ~BatchProcessor();

    // Start batch processing
    std::string startBatch(const BatchConfig& config);

    // Stop batch processing
    bool stopBatch(const std::string& batchId);

    // Get batch status
    BatchResult getBatchStatus(const std::string& batchId) const;

    // List all active batches
    std::vector<std::string> listBatches() const;

    // Cancel batch
    bool cancelBatch(const std::string& batchId);

    // Export batch results
    bool exportBatchResults(const std::string& batchId, const std::string& outputFile) const;

    // Get batch results summary
    std::string getBatchResultsSummary(const std::string& batchId) const;

    // Check if batch is running
    bool isBatchRunning(const std::string& batchId) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced configuration management
class AdvancedConfigManager {
public:
    AdvancedConfigManager();
    ~AdvancedConfigManager();

    // Load configuration from file with automatic format detection
    bool loadConfig(const std::string& filePath);

    // Save configuration to file
    bool saveConfig(const std::string& filePath, ConfigFormat format = ConfigFormat::Unknown) const;

    // Export configuration to JSON
    bool exportToJSON(const std::string& outputFile) const;

    // Export configuration to YAML
    bool exportToYAML(const std::string& outputFile) const;

    // Export configuration to INI format
    bool exportToINI(const std::string& outputFile) const;

    // Import configuration from JSON
    bool importFromJSON(const std::string& inputFile);

    // Import configuration from YAML
    bool importFromYAML(const std::string& inputFile);

    // Import configuration from INI
    bool importFromINI(const std::string& inputFile);

    // Create configuration template
    bool createTemplate(const std::string& outputFile, const std::string& description = "") const;

    // Apply configuration template
    bool applyTemplate(const std::string& templateFile);

    // Validate configuration against schema
    bool validateConfiguration(const std::string& schemaFile) const;

    // Optimize configuration
    std::unordered_map<std::string, double> optimizeConfiguration() const;

    // Backup configuration
    bool backupConfiguration(const std::string& backupDirectory) const;

    // Restore configuration from backup
    bool restoreConfiguration(const std::string& backupFile);

    // Get configuration history
    std::vector<std::chrono::system_clock::time_point> getConfigurationHistory() const;

    // Get configuration diff
    std::string getConfigurationDiff(const std::string& backupFile) const;

    // Set configuration reference (for inspection)
    void setConfig(std::shared_ptr<class Config> config);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Real-time visualization and monitoring interface
class RealTimeMonitor {
public:
    RealTimeMonitor();
    ~RealTimeMonitor();

    // Start real-time monitoring
    bool startMonitoring(double updateInterval = 0.1);

    // Stop real-time monitoring
    void stopMonitoring();

    // Update real-time data
    void update(const class Brain& brain, const class Config& config,
                const class Experiment& experiment, const class PerformanceMonitor& perf);

    // Render real-time dashboard
    void renderDashboard();

    // Render neural activity visualization
    void renderNeuralActivity(const class Brain& brain);

    // Render memory visualization
    void renderMemory(const class Brain& brain);

    // Render prediction visualization
    void renderPrediction(const class Brain& brain);

    // Render neuromodulation visualization
    void renderNeuromodulation(const class Brain& brain);

    // Render plasticity visualization
    void renderPlasticity(const class Brain& brain);

    // Render configuration visualization
    void renderConfiguration(const class Config& config);

    // Render experiment visualization
    void renderExperiment(const class Experiment& experiment);

    // Render performance visualization
    void renderPerformance(const class PerformanceMonitor& perf);

    // Export real-time data to file
    bool exportData(const std::string& outputFile) const;

    // Export visualization to file
    bool exportVisualization(const std::string& outputFile) const;

    // Set brain reference (for inspection)
    void setBrain(std::shared_ptr<class Brain> brain);

    // Set configuration reference (for inspection)
    void setConfig(std::shared_ptr<class Config> config);

    // Set experiment reference (for inspection)
    void setExperiment(std::shared_ptr<class Experiment> experiment);

    // Set performance monitor reference (for inspection)
    void setPerformanceMonitor(std::shared_ptr<class PerformanceMonitor> monitor);

    // Set visualization interface reference (for inspection)
    void setVisualizationInterface(std::shared_ptr<class VisualizationInterface> viz);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Factory for creating advanced interface components
class InterfaceFactory {
public:
    InterfaceFactory();
    ~InterfaceFactory();

    // Create advanced CLI instance
    static std::unique_ptr<AdvancedCLI> createAdvancedCLI();

    // Create expert UI instance
    static std::unique_ptr<ExpertUI> createExpertUI();

    // Create config validator instance
    static std::unique_ptr<ConfigValidator> createConfigValidator();

    // Create performance monitor instance
    static std::unique_ptr<PerformanceMonitor> createPerformanceMonitor();

    // Create debug tools instance
    static std::unique_ptr<DebugTools> createDebugTools();

    // Create batch processor instance
    static std::unique_ptr<BatchProcessor> createBatchProcessor();

    // Create advanced config manager instance
    static std::unique_ptr<AdvancedConfigManager> createAdvancedConfigManager();

    // Create real-time monitor instance
    static std::unique_ptr<RealTimeMonitor> createRealTimeMonitor();

    // Initialize all interface components
    static bool initializeAll();

    // Cleanup all interface components
    static void cleanupAll();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm