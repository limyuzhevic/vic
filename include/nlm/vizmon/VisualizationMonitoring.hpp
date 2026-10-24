#ifndef NLM_VISUALIZATION_MONITORING_HPP
#define NLM_VISUALIZATION_MONITORING_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <any>

namespace nlm {
namespace vizmon {

// Visualization types
enum class VisualizationType {
    Console,
    Web,
    Graph,
    Network,
    Timeline,
    Heatmap,
    3D,
    Custom
};

// Monitoring types
enum class MonitoringType {
    System,
    Simulation,
    Neural,
    Memory,
    Performance,
    Network,
    Custom
};

// Visualization data
struct VisualizationData {
    std::string title;
    std::string type;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::any> data;
    std::map<std::string, std::string> metadata;
    
    VisualizationData() {
        timestamp = std::chrono::system_clock::now();
    }
};

// Monitoring data
struct MonitoringData {
    std::chrono::system_clock::time_point timestamp;
    std::string metricName;
    double value;
    std::map<std::string, std::string> tags;
    std::map<std::string, std::any> context;
    
    MonitoringData() : value(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    MonitoringData(double val, const std::string& name) : value(val), metricName(name) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Visualization interface
class Visualization {
public:
    virtual ~Visualization() = default;
    
    virtual std::string getName() const = 0;
    virtual VisualizationType getType() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getAuthor() const = 0;
    
    // Visualization lifecycle
    virtual bool initialize(const std::map<std::string, std::any>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Visualization functionality
    virtual bool canRender(const std::string& dataType) const = 0;
    virtual std::any render(const std::string& dataType, const VisualizationData& data) = 0;
    virtual bool update(const VisualizationData& data) = 0;
    virtual bool save(const std::string& filepath) const = 0;
    
    // Visualization configuration
    virtual bool configure(const std::map<std::string, std::any>& config) = 0;
    virtual std::map<std::string, std::any> getConfig() const = 0;
};

// Console visualization
class ConsoleVisualization : public Visualization {
public:
    ConsoleVisualization();
    ~ConsoleVisualization() override = default;
    
    std::string getName() const override { return "Console Visualization"; }
    VisualizationType getType() const override { return VisualizationType::Console; }
    std::string getDescription() const override { 
        return "Console-based visualization for text-based displays"; 
    }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getAuthor() const override { return "NLM Team"; }
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isInitialized() const override;
    
    bool canRender(const std::string& dataType) const override;
    std::any render(const std::string& dataType, const VisualizationData& data) override;
    bool update(const VisualizationData& data) override;
    bool save(const std::string& filepath) const override;
    
    bool configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfig() const override;
    
    // Console-specific functionality
    void setOutputWidth(size_t width);
    void setOutputHeight(size_t height);
    void setRefreshInterval(std::chrono::milliseconds interval);
    void setColorScheme(const std::string& scheme);
    
private:
    bool initialized_;
    size_t outputWidth_;
    size_t outputHeight_;
    std::chrono::milliseconds refreshInterval_;
    std::string colorScheme_;
    std::vector<VisualizationData> recentData_;
};

// Web visualization
class WebVisualization : public Visualization {
public:
    WebVisualization();
    ~WebVisualization() override = default;
    
    std::string getName() const override { return "Web Visualization"; }
    VisualizationType getType() const override { return VisualizationType::Web; }
    std::string getDescription() const override { 
        return "Web-based visualization with HTML/JS frontend"; 
    }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getAuthor() const override { return "NLM Team"; }
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isInitialized() const override;
    
    bool canRender(const std::string& dataType) const override;
    std::any render(const std::string& dataType, const VisualizationData& data) override;
    bool update(const VisualizationData& data) override;
    bool save(const std::string& filepath) const override;
    
    bool configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfig() const override;
    
    // Web-specific functionality
    void setWebPort(int port);
    void setWebHost(const std::string& host);
    void setAutoRefresh(bool enabled);
    void setStaticFiles(const std::string& directory);
    void setServerAddress(const std::string& address);
    
private:
    bool initialized_;
    int webPort_;
    std::string webHost_;
    bool autoRefresh_;
    std::string staticFiles_;
    std::string serverAddress_;
    std::thread webServerThread_;
    bool serverRunning_;
};

// Graph visualization
class GraphVisualization : public Visualization {
public:
    GraphVisualization();
    ~GraphVisualization() override = default;
    
    std::string getName() const override { return "Graph Visualization"; }
    VisualizationType getType() const override { return VisualizationType::Graph; }
    std::string getDescription() const override { 
        return "Graph-based visualization for network and data relationships"; 
    }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getAuthor() const override { return "NLM Team"; }
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isInitialized() const override;
    
    bool canRender(const std::string& dataType) const override;
    std::any render(const std::string& dataType, const VisualizationData& data) override;
    bool update(const VisualizationData& data) override;
    bool save(const std::string& filepath) const override;
    
    bool configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfig() const override;
    
    // Graph-specific functionality
    void setGraphType(const std::string& type);
    void setGraphStyle(const std::string& style);
    void setLayoutAlgorithm(const std::string& algorithm);
    void setNodeColorMapping(const std::map<std::string, std::string>& mapping);
    void setEdgeWidthMapping(const std::map<std::string, double>& mapping);
    
private:
    bool initialized_;
    std::string graphType_;
    std::string graphStyle_;
    std::string layoutAlgorithm_;
    std::map<std::string, std::string> nodeColorMapping_;
    std::map<std::string, double> edgeWidthMapping_;
    std::vector<VisualizationData> graphData_;
};

// Timeline visualization
class TimelineVisualization : public Visualization {
public:
    TimelineVisualization();
    ~TimelineVisualization() override = default;
    
    std::string getName() const override { return "Timeline Visualization"; }
    VisualizationType getType() const override { return VisualizationType::Timeline; }
    std::string getDescription() const override { 
        return "Timeline-based visualization for temporal data"; 
    }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getAuthor() const override { return "NLM Team"; }
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isInitialized() const override;
    
    bool canRender(const std::string& dataType) const override;
    std::any render(const std::string& dataType, const VisualizationData& data) override;
    bool update(const VisualizationData& data) override;
    bool save(const std::string& filepath) const override;
    
    bool configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfig() const override;
    
    // Timeline-specific functionality
    void setTimeRange(double start, double end);
    void setTimeAxisLabel(const std::string& label);
    void setDataSeries(const std::vector<std::string>& series);
    void setAggregationMethod(const std::string& method);
    
private:
    bool initialized_;
    double timeStart_;
    double timeEnd_;
    std::string timeAxisLabel_;
    std::vector<std::string> dataSeries_;
    std::string aggregationMethod_;
    std::vector<VisualizationData> timelineData_;
};

// Monitoring interface
class Monitoring {
public:
    virtual ~Monitoring() = default;
    
    virtual std::string getName() const = 0;
    virtual MonitoringType getType() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getAuthor() const = 0;
    
    // Monitoring lifecycle
    virtual bool initialize(const std::map<std::string, std::any>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Monitoring functionality
    virtual bool canMonitor(const std::string& metricName) const = 0;
    virtual std::any collectData(const std::string& metricName,
                               const std::map<std::string, std::any>& parameters) = 0;
    virtual bool startMonitoring(const std::map<std::string, std::any>& config) = 0;
    virtual bool stopMonitoring() = 0;
    virtual bool isMonitoring() const = 0;
    
    // Monitoring configuration
    virtual bool configure(const std::map<std::string, std::any>& config) = 0;
    virtual std::map<std::string, std::any> getConfig() const = 0;
};

// System monitor
class SystemMonitor : public Monitoring {
public:
    SystemMonitor();
    ~SystemMonitor() override = default;
    
    std::string getName() const override { return "System Monitor"; }
    MonitoringType getType() const override { return MonitoringType::System; }
    std::string getDescription() const override { 
        return "System resource monitoring (CPU, memory, disk, network)"; 
    }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getAuthor() const override { return "NLM Team"; }
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isInitialized() const override;
    
    bool canMonitor(const std::string& metricName) const override;
    std::any collectData(const std::string& metricName,
                        const std::map<std::string, std::any>& parameters) override;
    bool startMonitoring(const std::map<std::string, std::any>& config) override;
    bool stopMonitoring() override;
    bool isMonitoring() const override;
    
    bool configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfig() const override;
    
    // System-specific functionality
    void setMonitoringInterval(std::chrono::milliseconds interval);
    void setMetrics(const std::vector<std::string>& metrics);
    void setAlertThresholds(const std::map<std::string, double>& thresholds);
    void setNotificationCallback(std::function<void(const MonitoringData&)> callback);
    
private:
    bool initialized_;
    std::chrono::milliseconds monitoringInterval_;
    std::vector<std::string> metrics_;
    std::map<std::string, double> alertThresholds_;
    std::function<void(const MonitoringData&)> notificationCallback_;
    std::thread monitoringThread_;
    bool monitoringEnabled_;
    std::vector<MonitoringData> recentData_;
};

// Simulation monitor
class SimulationMonitor : public Monitoring {
public:
    SimulationMonitor();
    ~SimulationMonitor() override = default;
    
    std::string getName() const override { return "Simulation Monitor"; }
    MonitoringType getType() const override { return MonitoringType::Simulation; }
    std::string getDescription() const override { 
        return "Neural simulation monitoring and statistics collection"; 
    }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getAuthor() const override { return "NLM Team"; }
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isInitialized() const override;
    
    bool canMonitor(const std::string& metricName) const override;
    std::any collectData(const std::string& metricName,
                        const std::map<std::string, std::any>& parameters) override;
    bool startMonitoring(const std::map<std::string, std::any>& config) override;
    bool stopMonitoring() override;
    bool isMonitoring() const override;
    
    bool configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfig() const override;
    
    // Simulation-specific functionality
    void setNeuronMetrics(const std::vector<std::string>& metrics);
    void setSynapseMetrics(const std::vector<std::string>& metrics);
    void setMemoryMetrics(const std::vector<std::string>& metrics);
    void setPlasticityMetrics(const std::vector<std::string>& metrics);
    
private:
    bool initialized_;
    std::vector<std::string> neuronMetrics_;
    std::vector<std::string> synapseMetrics_;
    std::vector<std::string> memoryMetrics_;
    std::vector<std::string> plasticityMetrics_;
    std::thread monitoringThread_;
    bool monitoringEnabled_;
    std::vector<MonitoringData> recentData_;
};

// Real-time monitoring manager
class RealTimeMonitor {
public:
    RealTimeMonitor();
    ~RealTimeMonitor() = default;
    
    // Monitoring management
    bool addMonitor(std::unique_ptr<Monitoring> monitor);
    bool removeMonitor(const std::string& name);
    bool hasMonitor(const std::string& name) const;
    
    // Monitoring control
    bool startMonitoring(const std::map<std::string, std::any>& config);
    bool stopMonitoring();
    bool pauseMonitoring();
    bool resumeMonitoring();
    bool isMonitoring() const;
    
    // Monitoring data collection
    std::vector<MonitoringData> collectAllData() const;
    std::vector<MonitoringData> collectData(const std::string& metricName) const;
    MonitoringData collectData(const std::string& metricName,
                               const std::map<std::string, std::any>& parameters) const;
    
    // Monitoring visualization
    std::any createVisualization(const std::string& visualizationName,
                                const std::vector<MonitoringData>& data) const;
    bool updateVisualization(const std::string& visualizationName,
                             const VisualizationData& data);
    
    // Monitoring statistics
    std::map<std::string, double> getStatistics(const std::string& metricName) const;
    std::string generateMonitoringReport() const;
    std::string generatePerformanceReport() const;
    
private:
    std::map<std::string, std::unique_ptr<Monitoring>> monitors_;
    std::map<std::string, std::unique_ptr<Visualization>> visualizations_;
    std::chrono::milliseconds monitoringInterval_;
    bool monitoringEnabled_;
    std::thread monitoringThread_;
    mutable std::mutex dataMutex_;
    
    void monitoringWorker();
};

// Visualization manager
class VisualizationManager {
public:
    VisualizationManager();
    ~VisualizationManager() = default;
    
    // Visualization management
    bool addVisualization(std::unique_ptr<Visualization> visualization);
    bool removeVisualization(const std::string& name);
    bool hasVisualization(const std::string& name) const;
    
    // Visualization control
    bool initializeAll();
    void shutdownAll();
    
    // Visualization operations
    std::any renderVisualization(const std::string& name,
                                 const VisualizationData& data) const;
    bool updateVisualization(const std::string& name, const VisualizationData& data);
    bool saveVisualization(const std::string& name, const std::string& filepath) const;
    
    // Visualization configuration
    bool configureVisualization(const std::string& name,
                               const std::map<std::string, std::any>& config);
    std::map<std::string, std::any> getVisualizationConfig(const std::string& name) const;
    
    // Visualization listing
    std::vector<std::string> getVisualizationNames() const;
    std::vector<std::string> getVisualizationTypes(VisualizationType type) const;
    std::vector<std::string> getVisualizationNamesByType(VisualizationType type) const;
    
    // Visualization statistics
    std::string generateVisualizationReport() const;
    
private:
    std::map<std::string, std::unique_ptr<Visualization>> visualizations_;
    mutable std::mutex visualizationMutex_;
};

// Visualization exporter
class VisualizationExporter {
public:
    virtual ~VisualizationExporter() = default;
    
    virtual std::string getExportFormat() const = 0;
    virtual bool exportVisualization(const VisualizationData& data,
                                      const std::string& filepath) = 0;
    virtual bool exportVisualization(std::ostream& stream,
                                      const VisualizationData& data) = 0;
    virtual std::string exportVisualizationToString(const VisualizationData& data) = 0;
};

// Image visualization exporter
class ImageVisualizationExporter : public VisualizationExporter {
public:
    std::string getExportFormat() const override { return "IMAGE"; }
    bool exportVisualization(const VisualizationData& data,
                             const std::string& filepath) override;
    bool exportVisualization(std::ostream& stream,
                             const VisualizationData& data) override;
    std::string exportVisualizationToString(const VisualizationData& data) override;
};

// SVG visualization exporter
class SVGVisualizationExporter : public VisualizationExporter {
public:
    std::string getExportFormat() const override { return "SVG"; }
    bool exportVisualization(const VisualizationData& data,
                             const std::string& filepath) override;
    bool exportVisualization(std::ostream& stream,
                             const VisualizationData& data) override;
    std::string exportVisualizationToString(const VisualizationData& data) override;
};

// JSON visualization exporter
class JSONVisualizationExporter : public VisualizationExporter {
public:
    std::string getExportFormat() const override { return "JSON"; }
    bool exportVisualization(const VisualizationData& data,
                             const std::string& filepath) override;
    bool exportVisualization(std::ostream& stream,
                             const VisualizationData& data) override;
    std::string exportVisualizationToString(const VisualizationData& data) override;
};

// Monitoring utilities
namespace utils {
    // Data processing utilities
    std::vector<VisualizationData> processMonitoringData(const std::vector<MonitoringData>& rawData);
    std::vector<MonitoringData> filterMonitoringData(const std::vector<MonitoringData>& data,
                                                    const std::map<std::string, std::string>& filters);
    std::vector<MonitoringData> aggregateMonitoringData(const std::vector<MonitoringData>& data,
                                                       const std::string& aggregationMethod,
                                                       std::chrono::duration<double> interval);
    
    // Visualization utilities
    std::string generateVisualizationData(const VisualizationData& data);
    std::string formatMonitoringData(const MonitoringData& data);
    
    // Configuration utilities
    bool isValidVisualizationType(const std::string& type);
    bool isValidMonitoringType(const std::string& type);
    std::vector<std::string> getAvailableVisualizationTypes();
    std::vector<std::string> getAvailableMonitoringTypes();
    
    // Export utilities
    bool exportToFile(const std::string& filepath, const std::string& format);
    bool importFromFile(const std::string& filepath, std::string& format);
    
    // Utility functions
    std::string generateUUID();
    std::string generateTimestamp();
    std::string formatBytes(size_t bytes);
    std::string formatTime(std::chrono::duration<double> duration);
    
    bool isFileAccessible(const std::string& path, std::ios_base::openmode mode = std::ios_base::in);
    std::string getFileSize(const std::string& path);
    
    std::string escapeHTML(const std::string& input);
    std::string escapeJSON(const std::string& input);
}

} // namespace vizmon
} // namespace nlm

#endif // NLM_VISUALIZATION_MONITORING_HPP