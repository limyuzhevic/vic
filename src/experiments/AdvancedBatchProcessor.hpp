// ============================================================================
// Advanced Batch Processing APIs
// ============================================================================

#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include "Brain.hpp"
#include "UnifiedMemorySystem.hpp"
#include "Config.hpp"

namespace nlm {

// Batch experiment execution result
struct BatchExperimentResult {
    size_t experimentId;
    SimulationStep startStep;
    SimulationStep endStep;
    float finalReward;
    float averageFiringRate;
    size_t totalSpikes;
    size_t memoryUsage;
    bool success;
    std::string errorMessage;
    std::vector<float> rewardHistory;
    std::vector<float> performanceMetrics;
    
    BatchExperimentResult(size_t id) : experimentId(id), success(false), totalSpikes(0), memoryUsage(0) {}
};

// Batch configuration for parallel execution
struct BatchConfig {
    size_t numExperiments;
    size_t parallelThreads;
    SimulationStep duration;
    float baseReward;
    std::vector<std::string> experimentTypes;
    std::vector<std::shared_ptr<Config>> experimentConfigs;
    bool enableMemorySharing;
    bool enableResultAggregation;
    size_t maxMemoryPerExperiment;
    
    BatchConfig()
        : numExperiments(10)
        , parallelThreads(std::thread::hardware_concurrency())
        , duration(1000)
        , baseReward(0.0f)
        , enableMemorySharing(false)
        , enableResultAggregation(true)
        , maxMemoryPerExperiment(1024 * 1024) {} // 1MB per experiment
};

// Advanced batch processor with task queues and resource management
class AdvancedBatchProcessor {
public:
    AdvancedBatchProcessor();
    ~AdvancedBatchProcessor();
    
    // Configure batch processing
    void configure(const BatchConfig& config);
    
    // Execute batch of experiments
    std::vector<BatchExperimentResult> executeBatch(
        std::function<std::unique_ptr<Brain>(size_t, std::shared_ptr<Config>)> experimentFactory,
        const std::vector<size_t>& experimentIds = {});
    
    // Execute single experiment
    BatchExperimentResult executeExperiment(
        size_t experimentId,
        std::function<std::unique_ptr<Brain>(size_t, std::shared_ptr<Config>)> experimentFactory,
        std::shared_ptr<Config> config);
    
    // Get current configuration
    const BatchConfig& getConfig() const { return config_; }
    
    // Get performance statistics
    struct PerformanceStats {
        size_t totalExperiments;
        float averageExecutionTime;
        float successRate;
        size_t totalMemoryUsage;
        size_t peakMemoryUsage;
    };
    
    PerformanceStats getPerformanceStats() const;
    
    // Save batch results to file
    bool saveResults(const std::vector<BatchExperimentResult>& results, const std::string& filepath) const;
    
    // Load batch results from file
    std::vector<BatchExperimentResult> loadResults(const std::string& filepath) const;
    
    // Clear all resources
    void clear();
    
    // Check if processor is available
    bool isAvailable() const { return isInitialized_; }
    
private:
    // Worker thread function
    void workerThread(size_t threadId,
                    std::function<std::unique_ptr<Brain>(size_t, std::shared_ptr<Config>)> experimentFactory,
                    std::mutex& queueMutex,
                    std::vector<std::pair<size_t, std::shared_ptr<Config>>>& experimentQueue,
                    std::vector<BatchExperimentResult>& results,
                    std::condition_variable& resultsAvailable,
                    std::atomic<bool>& stopFlag);
    
    // Execute single experiment in worker thread
    BatchExperimentResult executeExperimentInWorker(
        size_t experimentId,
        std::function<std::unique_ptr<Brain>(size_t, std::shared_ptr<Config>)> experimentFactory,
        std::shared_ptr<Config> config);
    
    // Aggregate results from multiple experiments
    BatchExperimentResult aggregateResults(const std::vector<BatchExperimentResult>& results) const;
    
    // Save configuration to file
    bool saveConfig(const std::string& filepath) const;
    
    // Load configuration from file
    bool loadConfig(const std::string& filepath);
    
    BatchConfig config_;
    bool isInitialized_;
    mutable std::mutex statsMutex_;
    
    // Performance monitoring
    struct {
        size_t totalExperiments;
        float totalExecutionTime;
        size_t successfulExperiments;
        size_t totalMemoryUsage;
        size_t peakMemoryUsage;
    } stats_;
};

// Configuration template management
class ConfigTemplateManager {
public:
    ConfigTemplateManager();
    ~ConfigTemplateManager();
    
    // Save configuration template
    bool saveTemplate(const std::string& name, const Config& config);
    
    // Load configuration template
    bool loadTemplate(const std::string& name, Config& config);
    
    // Delete configuration template
    bool deleteTemplate(const std::string& name);
    
    // List all available templates
    std::vector<std::string> listTemplates() const;
    
    // Create template from existing config
    bool createTemplate(const std::string& name, const std::string& baseTemplate = "");
    
    // Get template description
    std::string getTemplateDescription(const std::string& name) const;
    
    // Export template to file
    bool exportTemplate(const std::string& name, const std::string& filepath) const;
    
    // Import template from file
    bool importTemplate(const std::string& name, const std::string& filepath);
    
    // Validate template
    bool validateTemplate(const std::string& name) const;
    
private:
    struct TemplateInfo {
        std::string name;
        std::string description;
        std::string createdAt;
        size_t version;
        std::string configData;
    };
    
    std::map<std::string, TemplateInfo> templates_;
    std::mutex templatesMutex_;
};

// Real-time performance monitoring
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start monitoring
    void startMonitoring();
    
    // Stop monitoring
    void stopMonitoring();
    
    // Check if monitoring is active
    bool isMonitoring() const { return isMonitoring_; }
    
    // Get current performance metrics
    struct PerformanceMetrics {
        // CPU usage
        float cpuUsage;
        float cpuTemperature;
        
        // Memory usage
        size_t memoryUsage;
        size_t memoryPeak;
        size_t memoryAvailable;
        
        // System load
        float systemLoad;
        
        // Application-specific metrics
        size_t activeNeurons;
        size_t activeSynapses;
        float averageFiringRate;
        float totalSpikes;
        
        // Time metrics
        double updateTime;
        double stepTime;
        double processingTime;
        
        // Timestamp
        Timestamp timestamp;
    };
    
    PerformanceMetrics getMetrics() const;
    
    // Get performance history
    std::vector<PerformanceMetrics> getHistory(size_t maxSamples = 1000) const;
    
    // Set performance sampling rate
    void setSamplingRate(float rate) { samplingRate_ = rate; }
    
    // Enable/disable specific metrics
    void enableMetric(const std::string& metric, bool enable);
    bool isMetricEnabled(const std::string& metric) const;
    
    // Get performance alerts
    std::vector<std::string> getAlerts() const;
    
    // Clear metrics
    void clear();
    
    // Save metrics to file
    bool saveMetrics(const std::string& filepath) const;
    
    // Load metrics from file
    bool loadMetrics(const std::string& filepath);
    
private:
    // Update metrics
    void updateMetrics();
    
    // Collect system metrics
    void collectSystemMetrics();
    
    // Collect application metrics
    void collectApplicationMetrics();
    
    // Check for performance issues
    void checkPerformanceIssues();
    
    // Add alert
    void addAlert(const std::string& message, const std::string& severity = "WARNING");
    
    bool isMonitoring_;
    float samplingRate_;
    mutable std::mutex metricsMutex_;
    
    PerformanceMetrics currentMetrics_;
    std::vector<PerformanceMetrics> metricsHistory_;
    
    // Alert system
    std::vector<std::string> alerts_;
    
    // System monitoring
    Timestamp lastUpdateTime_;
    size_t updateCount_;
};

// Plugin architecture for third-party extensions
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Load plugin from file
    bool loadPlugin(const std::string& filepath);
    
    // Unload plugin
    bool unloadPlugin(const std::string& name);
    
    // Get loaded plugins
    std::vector<std::string> getLoadedPlugins() const;
    
    // Check if plugin is loaded
    bool isPluginLoaded(const std::string& name) const;
    
    // Get plugin information
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::string filepath;
        std::vector<std::string> dependencies;
        bool isEnabled;
        bool isValid;
    };
    
    PluginInfo getPluginInfo(const std::string& name) const;
    
    // Call plugin function
    bool callPluginFunction(const std::string& pluginName, const std::string& functionName,
                           const std::vector<std::any>& args, std::any& result);
    
    // Enable/disable plugin
    bool setPluginEnabled(const std::string& name, bool enabled);
    
    // Get plugin function signatures
    std::vector<std::string> getPluginFunctions(const std::string& name) const;
    
    // Save plugin configuration
    bool savePluginConfig(const std::string& name, const std::string& config) const;
    
    // Load plugin configuration
    bool loadPluginConfig(const std::string& name, std::string& config) const;
    
private:
    // Plugin interface
    class IPlugin {
    public:
        virtual ~IPlugin() = default;
        virtual std::string getName() const = 0;
        virtual std::string getVersion() const = 0;
        virtual std::string getDescription() const = 0;
        virtual bool initialize(const std::map<std::string, std::any>& config) = 0;
        virtual void shutdown() = 0;
        virtual bool isEnabled() const = 0;
        virtual void setEnabled(bool enabled) = 0;
        
        // Plugin functions
        virtual bool callFunction(const std::string& name, const std::vector<std::any>& args, std::any& result) = 0;
        virtual std::vector<std::string> getFunctionNames() const = 0;
    };
    
    struct LoadedPlugin {
        std::string name;
        std::shared_ptr<IPlugin> plugin;
        std::string filepath;
        bool isEnabled;
        Timestamp loadedAt;
    };
    
    std::map<std::string, LoadedPlugin> loadedPlugins_;
    mutable std::mutex pluginsMutex_;
};

// Enhanced visualization tools
class VisualizationTools {
public:
    VisualizationTools();
    ~VisualizationTools();
    
    // Generate neural activity visualization
    bool generateNeuralActivityVisualization(
        const Brain& brain,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Generate memory usage visualization
    bool generateMemoryUsageVisualization(
        const UnifiedMemorySystem& memorySystem,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Generate performance metrics visualization
    bool generatePerformanceVisualization(
        const PerformanceMonitor& monitor,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Generate 3D neural network visualization
    bool generate3DNeuralVisualization(
        const Brain& brain,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Generate heat map visualization
    bool generateHeatMapVisualization(
        const std::vector<std::vector<float>>& data,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Generate time series visualization
    bool generateTimeSeriesVisualization(
        const std::vector<Timestamp>& timestamps,
        const std::vector<std::vector<float>>& data,
        const std::vector<std::string>& labels,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Generate connectivity matrix visualization
    bool generateConnectivityMatrixVisualization(
        const Brain& brain,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Generate spike train visualization
    bool generateSpikeTrainVisualization(
        const Brain& brain,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Generate synaptic weight visualization
    bool generateSynapticWeightVisualization(
        const Brain& brain,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Generate population activity visualization
    bool generatePopulationActivityVisualization(
        const Brain& brain,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Generate plasticity visualization
    bool generatePlasticityVisualization(
        const Brain& brain,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Generate neuromodulation visualization
    bool generateNeuromodulationVisualization(
        const Brain& brain,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Set output directory
    void setOutputDirectory(const std::string& directory) { outputDirectory_ = directory; }
    
    // Get output directory
    std::string getOutputDirectory() const { return outputDirectory_; }
    
    // Create output directory if it doesn't exist
    bool ensureOutputDirectory();
    
    // Get supported file formats
    std::vector<std::string> getSupportedFormats() const;
    
    // Check if format is supported
    bool isFormatSupported(const std::string& format) const;
    
    // Get visualization configuration
    struct VisualizationConfig {
        std::string format;
        size_t width;
        size_t height;
        float dpi;
        std::string title;
        std::string xLabel;
        std::string yLabel;
        std::string colorScheme;
        bool showGrid;
        bool showLegend;
        size_t maxDataPoints;
        float lineWidth;
        std::string fontFamily;
        size_t fontSize;
        
        VisualizationConfig()
            : format("PNG")
            , width(1920)
            , height(1080)
            , dpi(300)
            , title("NLM Visualization")
            , xLabel("Time")
            , yLabel("Value")
            , colorScheme("viridis")
            , showGrid(true)
            , showLegend(true)
            , maxDataPoints(10000)
            , lineWidth(2.0f)
            , fontFamily("Arial")
            , fontSize(12) {}
    };
    
private:
    // Generate visualization for specific data type
    bool generateVisualizationForType(
        const std::string& type,
        const std::string& outputFile,
        const VisualizationConfig& config);
    
    // Save data to CSV
    bool saveToCSV(const std::string& filepath, const std::vector<std::vector<float>>& data) const;
    
    // Save data to JSON
    bool saveToJSON(const std::string& filepath, const std::vector<std::vector<float>>& data) const;
    
    // Save data to binary format
    bool saveToBinary(const std::string& filepath, const std::vector<std::vector<float>>& data) const;
    
    std::string outputDirectory_;
    mutable std::mutex visualizationMutex_;
};

} // namespace nlm

// ============================================================================
// Global instances for convenience
// ============================================================================

extern std::unique_ptr<AdvancedBatchProcessor> gBatchProcessor;
extern std::unique_ptr<ConfigTemplateManager> gConfigTemplateManager;
extern std::unique_ptr<PerformanceMonitor> gPerformanceMonitor;
extern std::unique_ptr<PluginManager> gPluginManager;
extern std::unique_ptr<VisualizationTools> gVisualizationTools;

// ============================================================================
// Convenience functions for batch processing
// ============================================================================

inline bool initializeAdvancedFeatures() {
    if (!gBatchProcessor) {
        gBatchProcessor = std::make_unique<AdvancedBatchProcessor>();
    }
    if (!gConfigTemplateManager) {
        gConfigTemplateManager = std::make_unique<ConfigTemplateManager>();
    }
    if (!gPerformanceMonitor) {
        gPerformanceMonitor = std::make_unique<PerformanceMonitor>();
    }
    if (!gPluginManager) {
        gPluginManager = std::make_unique<PluginManager>();
    }
    if (!gVisualizationTools) {
        gVisualizationTools = std::make_unique<VisualizationTools>();
    }
    
    gBatchProcessor->configure(ConfigBuilder::createDefaultConfig().getOr<size_t>("batch_size", 10));
    gPerformanceMonitor->startMonitoring();
    
    return true;
}

inline void shutdownAdvancedFeatures() {
    if (gBatchProcessor) {
        gBatchProcessor->clear();
        gBatchProcessor.reset();
    }
    if (gConfigTemplateManager) {
        gConfigTemplateManager.reset();
    }
    if (gPerformanceMonitor) {
        gPerformanceMonitor->stopMonitoring();
        gPerformanceMonitor.reset();
    }
    if (gPluginManager) {
        gPluginManager.reset();
    }
    if (gVisualizationTools) {
        gVisualizationTools.reset();
    }
}

inline AdvancedBatchProcessor* getBatchProcessor() {
    return gBatchProcessor.get();
}

inline ConfigTemplateManager* getConfigTemplateManager() {
    return gConfigTemplateManager.get();
}

inline PerformanceMonitor* getPerformanceMonitor() {
    return gPerformanceMonitor.get();
}

inline PluginManager* getPluginManager() {
    return gPluginManager.get();
}

inline VisualizationTools* getVisualizationTools() {
    return gVisualizationTools.get();
}

} // namespace nlm
