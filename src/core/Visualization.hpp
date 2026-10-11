#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include <functional>

namespace nlm {

// Visualization format types
enum class VisualizationFormat {
    ASCII,       // Simple ASCII art
    TEXT,        // Formatted text
    JSON,        // JSON data
    CSV,         // Comma-separated values
    HTML,        // Web HTML
    SVG,         // Scalable vector graphics
    WEBGL,       // 3D web graphics
    TERMINAL,    // Terminal-based visualization
    CONSOLE      // Console output
};

// Visualization data structures
struct NeuronVisualizationData {
    uint64_t id;
    float x;
    float y;
    float z;
    float radius;
    float potential;
    float threshold;
    bool isActive;
    bool isFiring;
    uint32_t color; // RGB hex value
    float activationLevel; // 0.0 to 1.0
};

struct SynapseVisualizationData {
    uint64_t sourceId;
    uint64_t targetId;
    float weight;
    float delay;
    bool isActive;
    bool isExcitatory;
    float strength;
    float color; // RGB hex value
};

struct RegionVisualizationData {
    uint64_t id;
    std::string name;
    float x;
    float y;
    float width;
    float height;
    std::vector<NeuronVisualizationData> neurons;
    std::vector<SynapseVisualizationData> synapses;
    uint32_t color;
};

struct BrainStateVisualizationData {
    uint64_t timestamp;
    float simulationTime;
    uint64_t totalNeurons;
    uint64_t totalSynapses;
    uint64_t activeNeurons;
    uint64_t firingNeurons;
    float averageFiringRate;
    float excitationInhibitionRatio;
    float averageMembranePotential;
    float maximumMembranePotential;
    std::vector<RegionVisualizationData> regions;
    std::vector<NeuronVisualizationData> highlightedNeurons;
    std::vector<SynapseVisualizationData> highlightedSynapses;
};

// Visualization interface
class IVisualizer {
public:
    virtual ~IVisualizer() = default;
    
    virtual void initialize(const BrainStateVisualizationData& initialState) = 0;
    virtual void update(const BrainStateVisualizationData& newState) = 0;
    virtual void render() = 0;
    virtual void close() = 0;
    
    virtual void setFormat(VisualizationFormat format) = 0;
    virtual VisualizationFormat getFormat() const = 0;
    
    virtual void setUpdateInterval(std::chrono::milliseconds interval) = 0;
    virtual std::chrono::milliseconds getUpdateInterval() const = 0;
    
    virtual void setDimensions(int width, int height) = 0;
    virtual void getDimensions(int& width, int& height) const = 0;
    
    virtual void setStyle(const std::string& style) = 0;
    virtual std::string getStyle() const = 0;
    
    // Interactive features
    virtual bool handleInput(const std::string& input) = 0;
    virtual void setCallback(std::function<void(const std::string&)> callback) = 0;
    
    // Export capabilities
    virtual bool exportState(const std::string& filepath) = 0;
    virtual bool exportFrame(const std::string& filepath) = 0;
    virtual bool exportAnimation(const std::string& directory, int fps = 30) = 0;
};

// Text-based visualizer (console output)
class TextVisualizer : public IVisualizer {
public:
    TextVisualizer();
    ~TextVisualizer() override;
    
    void initialize(const BrainStateVisualizationData& initialState) override;
    void update(const BrainStateVisualizationData& newState) override;
    void render() override;
    void close() override;
    
    void setFormat(VisualizationFormat format) override;
    VisualizationFormat getFormat() const override;
    
    void setUpdateInterval(std::chrono::milliseconds interval) override;
    std::chrono::milliseconds getUpdateInterval() const override;
    
    void setDimensions(int width, int height) override;
    void getDimensions(int& width, int& height) const override;
    
    void setStyle(const std::string& style) override;
    std::string getStyle() const override;
    
    bool handleInput(const std::string& input) override;
    void setCallback(std::function<void(const std::string&)> callback) override;
    
    bool exportState(const std::string& filepath) override;
    bool exportFrame(const std::string& filepath) override;
    bool exportAnimation(const std::string& directory, int fps = 30) override;
    
    // Text-specific methods
    void setShowConnectionStrength(bool show);
    void setShowNeuronStates(bool show);
    void setShowRegionBoundaries(bool show);
    void setColorScheme(const std::string& scheme);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ASCII art visualizer
class ASCIIVisualizer : public IVisualizer {
public:
    ASCIIVisualizer();
    ~ASCIIVisualizer() override;
    
    void initialize(const BrainStateVisualizationData& initialState) override;
    void update(const BrainStateVisualizationData& newState) override;
    void render() override;
    void close() override;
    
    void setFormat(VisualizationFormat format) override;
    VisualizationFormat getFormat() const override;
    
    void setUpdateInterval(std::chrono::milliseconds interval) override;
    std::chrono::milliseconds getUpdateInterval() const override;
    
    void setDimensions(int width, int height) override;
    void getDimensions(int& width, int& height) const override;
    
    void setStyle(const std::string& style) override;
    std::string getStyle() const override;
    
    bool handleInput(const std::string& input) override;
    void setCallback(std::function<void(const std::string&)> callback) override;
    
    bool exportState(const std::string& filepath) override;
    bool exportFrame(const std::string& filepath) override;
    bool exportAnimation(const std::string& directory, int fps = 30) override;
    
    // ASCII-specific methods
    void setCellSize(int width, int height);
    void setShowConnections(bool show);
    void setSymbolMap(const std::string& symbols);
    void setGradientMapping(const std::string& mapping);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// HTML visualizer (for web-based visualization)
class HTMLVisualizer : public IVisualizer {
public:
    HTMLVisualizer();
    ~HTMLVisualizer() override;
    
    void initialize(const BrainStateVisualizationData& initialState) override;
    void update(const BrainStateVisualizationData& newState) override;
    void render() override;
    void close() override;
    
    void setFormat(VisualizationFormat format) override;
    VisualizationFormat getFormat() const override;
    
    void setUpdateInterval(std::chrono::milliseconds interval) override;
    std::chrono::milliseconds getUpdateInterval() const override;
    
    void setDimensions(int width, int height) override;
    void getDimensions(int& width, int& height) const override;
    
    void setStyle(const std::string& style) override;
    std::string getStyle() const override;
    
    bool handleInput(const std::string& input) override;
    void setCallback(std::function<void(const std::string&)> callback) override;
    
    bool exportState(const std::string& filepath) override;
    bool exportFrame(const std::string& filepath) override;
    bool exportAnimation(const std::string& directory, int fps = 30) override;
    
    // HTML-specific methods
    void generateHTML(const BrainStateVisualizationData& state);
    void generateJavaScript(const BrainStateVisualizationData& state);
    void generateCSS(const BrainStateVisualizationData& state);
    void generateDataJSON(const BrainStateVisualizationData& state);
    
    // Web-specific features
    void setWebSocketPort(int port);
    void setServerMode(bool enable);
    void setAutoRefresh(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Monitoring system interface
class IMonitor {
public:
    virtual ~IMonitor() = default;
    
    virtual void initialize() = 0;
    virtual void update(const BrainStateVisualizationData& state) = 0;
    virtual void shutdown() = 0;
    
    virtual void startMonitoring() = 0;
    virtual void stopMonitoring() = 0;
    virtual bool isMonitoring() const = 0;
    
    virtual void addMetric(const std::string& name, double value) = 0;
    virtual double getMetric(const std::string& name) const = 0;
    virtual std::vector<std::string> getAllMetrics() const = 0;
    
    virtual void addAlert(const std::string& condition, const std::string& message) = 0;
    virtual std::vector<std::string> getAlerts() const = 0;
    
    virtual void setThreshold(const std::string& metric, double threshold, bool above = true) = 0;
    virtual void clearThresholds() = 0;
    
    virtual std::string getReport() const = 0;
    virtual std::string getJSONReport() const = 0;
};

// Performance monitor
class PerformanceMonitor : public IMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor() override;
    
    void initialize() override;
    void update(const BrainStateVisualizationData& state) override;
    void shutdown() override;
    
    void startMonitoring() override;
    void stopMonitoring() override;
    bool isMonitoring() const override;
    
    void addMetric(const std::string& name, double value) override;
    double getMetric(const std::string& name) const override;
    std::vector<std::string> getAllMetrics() const override;
    
    void addAlert(const std::string& condition, const std::string& message) override;
    std::vector<std::string> getAlerts() const override;
    
    void setThreshold(const std::string& metric, double threshold, bool above = true) override;
    void clearThresholds() override;
    
    std::string getReport() const override;
    std::string getJSONReport() const override;
    
    // Performance-specific methods
    void recordExecutionTime(const std::string& operation, double time);
    void recordMemoryUsage(size_t memory);
    void recordCPUUsage(double usage);
    void recordNetworkUsage(size_t bytes, double duration);
    void recordDiskUsage(size_t bytes);
    
    double getAverageExecutionTime(const std::string& operation) const;
    double getMaxExecutionTime(const std::string& operation) const;
    double getMinExecutionTime(const std::string& operation) const;
    size_t getMemoryUsage() const;
    double getCPUUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory monitor
class MemoryMonitor : public IMonitor {
public:
    MemoryMonitor();
    ~MemoryMonitor() override;
    
    void initialize() override;
    void update(const BrainStateVisualizationData& state) override;
    void shutdown() override;
    
    void startMonitoring() override;
    void stopMonitoring() override;
    bool isMonitoring() const override;
    
    void addMetric(const std::string& name, double value) override;
    double getMetric(const std::string& name) const override;
    std::vector<std::string> getAllMetrics() const override;
    
    void addAlert(const std::string& condition, const std::string& message) override;
    std::vector<std::string> getAlerts() const override;
    
    void setThreshold(const std::string& metric, double threshold, bool above = true) override;
    void clearThresholds() override;
    
    std::string getReport() const override;
    std::string getJSONReport() const override;
    
    // Memory-specific methods
    void recordMemoryAllocation(size_t size, const std::string& type = "unknown");
    void recordMemoryDeallocation(size_t size, const std::string& type = "unknown");
    void recordMemoryFragmentation(float fragmentation);
    void recordSwapUsage(size_t used, size_t total);
    void recordPageFaults(uint64_t faults);
    
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    float getMemoryFragmentation() const;
    size_t getSwapUsage() const;
    uint64_t getPageFaults() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// System status tracker
class SystemStatusTracker {
public:
    SystemStatusTracker();
    ~SystemStatusTracker();
    
    void initialize();
    void update(const BrainStateVisualizationData& state);
    void shutdown();
    
    // Status tracking
    void setStatus(const std::string& component, const std::string& status);
    std::string getStatus(const std::string& component) const;
    
    // Health monitoring
    void setHealth(const std::string& component, double healthScore);
    double getHealth(const std::string& component) const;
    std::vector<std::string> getUnhealthyComponents() const;
    
    // Performance tracking
    void recordPerformance(const std::string& component, double performanceMetric);
    double getPerformance(const std::string& component) const;
    
    // Status report
    std::string getSystemStatus() const;
    std::string getHealthReport() const;
    std::string getPerformanceReport() const;
    
    // Export
    bool exportStatus(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global visualizer and monitor instances
IVisualizer& getVisualizer();
IMonitor& getMonitor();
SystemStatusTracker& getSystemStatus();

// Convenience macros for monitoring
#define NLM_MONITOR_START(component) nlm::getMonitor().startMonitoring(); nlm::getSystemStatus().setStatus(component, "ACTIVE")
#define NLM_MONITOR_STOP(component) nlm::getMonitor().stopMonitoring(); nlm::getSystemStatus().setStatus(component, "INACTIVE")
#define NLM_RECORD_METRIC(name, value) nlm::getMonitor().addMetric(name, value)
#define NLM_SET_HEALTH(component, score) nlm::getSystemStatus().setHealth(component, score)

} // namespace nlm
