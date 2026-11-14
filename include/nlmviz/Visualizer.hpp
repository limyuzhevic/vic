#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <functional>

namespace nlmviz {

// Forward declarations
class Visualizer;

// Visualization types
enum class VisualizationType {
    NetworkGraph,
    BrainState,
    PerformanceMetrics,
    TimeSeries,
    Statistical,
    Custom
};

// Visualization data structure
struct VisualizationData {
    std::string name;
    std::string type;
    std::vector<double> data;
    std::map<std::string, std::string> metadata;
    std::string timestamp;
    double scaleMin;
    double scaleMax;
    std::string colorMap;
    
    VisualizationData() : scaleMin(0.0), scaleMax(1.0), colorMap("viridis") {}
};

// Visualization options
struct VisualizationOptions {
    int width;
    int height;
    std::string title;
    std::string outputFormat;
    std::string outputFile;
    bool interactive;
    bool exportData;
    double fps;
    int duration; // seconds
    std::map<std::string, std::string> customOptions;
    
    VisualizationOptions() : width(800), height(600), fps(30.0), duration(60), 
                           interactive(false), exportData(false) {}
};

// Base visualizer interface
class Visualizer {
public:
    virtual ~Visualizer() = default;
    
    // Get visualizer information
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual VisualizationType getType() const = 0;
    virtual std::string getVersion() const = 0;
    
    // Configuration
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual bool configure(const std::string& option, const std::string& value) = 0;
    virtual bool shutdown() = 0;
    
    // Data management
    virtual bool setData(const VisualizationData& data) = 0;
    virtual bool addData(const VisualizationData& data) = 0;
    virtual bool clearData() = 0;
    virtual bool mergeData(const std::vector<VisualizationData>& data) = 0;
    
    // Rendering
    virtual bool render() = 0;
    virtual bool renderToFile(const std::string& filepath) = 0;
    virtual bool renderToBuffer(std::string& buffer) = 0;
    
    // Interactive features
    virtual bool handleInput(const std::map<std::string, std::string>& input) = 0;
    virtual bool setInteractiveMode(bool enabled) = 0;
    virtual bool isInteractive() const = 0;
    
    // Export/import
    virtual bool exportData(const std::string& filepath) = 0;
    virtual bool importData(const std::string& filepath) = 0;
    virtual bool exportConfiguration(const std::string& filepath) = 0;
    virtual bool importConfiguration(const std::string& filepath) = 0;
    
    // Utilities
    virtual bool update() = 0;
    virtual bool reset() = 0;
    virtual bool validate() = 0;
    virtual std::string getStatus() const = 0;
    
    // Callbacks
    virtual void setDataCallback(std::function<void(const VisualizationData&)> callback) = 0;
    virtual void setRenderCallback(std::function<void(const std::string&)> callback) = 0;
    virtual void setErrorCallback(std::function<void(const std::string&)> callback) = 0;
    
    // Statistics
    virtual double getMemoryUsage() const = 0;
    virtual double getRenderTime() const = 0;
    virtual int getDataPointsCount() const = 0;
    virtual std::string getStatistics() const = 0;
};

// Visualization factory
class VisualizerFactory {
public:
    static VisualizerFactory& getInstance() {
        static VisualizerFactory instance;
        return instance;
    }
    
    // Register visualizer
    template<typename T>
    bool registerVisualizer(const std::string& name, const std::string& description) {
        std::string typeName = typeid(T).name();
        visualizers[name] = {
            name,
            description,
            std::make_unique<T>(),
            []() -> std::unique_ptr<Visualizer> {
                return std::make_unique<T>();
            }
        };
        return true;
    }
    
    // Create visualizer
    std::unique_ptr<Visualizer> createVisualizer(const std::string& name) {
        auto it = visualizers.find(name);
        if (it != visualizers.end()) {
            return it->second.factory();
        }
        return nullptr;
    }
    
    // Get all visualizer names
    std::vector<std::string> getVisualizerNames() const {
        std::vector<std::string> names;
        for (const auto& pair : visualizers) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get visualizer info
    struct VisualizerInfo {
        std::string name;
        std::string description;
        std::unique_ptr<Visualizer> instance;
        std::function<std::unique_ptr<Visualizer>()> factory;
    };
    
    std::vector<VisualizerInfo> getVisualizerInfos() const {
        std::vector<VisualizerInfo> infos;
        for (const auto& pair : visualizers) {
            infos.push_back(pair.second);
        }
        return infos;
    }
    
    // Check if visualizer exists
    bool hasVisualizer(const std::string& name) const {
        return visualizers.find(name) != visualizers.end();
    }
    
    // Remove visualizer
    bool removeVisualizer(const std::string& name) {
        auto it = visualizers.find(name);
        if (it != visualizers.end()) {
            visualizers.erase(it);
            return true;
        }
        return false;
    }

private:
    VisualizerFactory() = default;
    
    struct VisualizerEntry {
        std::string name;
        std::string description;
        std::unique_ptr<Visualizer> instance;
        std::function<std::unique_ptr<Visualizer>()> factory;
    };
    
    std::map<std::string, VisualizerEntry> visualizers;
};

// Visualization utilities
namespace VisualizationUtils {
    // Convert data to visualization format
    VisualizationData convertToVisualizationData(const std::vector<double>& data,
                                                  const std::string& name,
                                                  const std::map<std::string, std::string>& metadata = {});
    
    // Create visualization from multiple data sources
    std::vector<VisualizationData> combineData(const std::vector<VisualizationData>& datasets,
                                               const std::string& prefix = "");
    
    // Filter visualization data by time range
    std::vector<VisualizationData> filterByTime(const std::vector<VisualizationData>& data,
                                                double startTime, double endTime);
    
    // Resample visualization data
    std::vector<VisualizationData> resampleData(const VisualizationData& data,
                                                size_t newSize);
    
    // Calculate statistical information for visualization data
    struct StatisticalInfo {
        double mean;
        double median;
        double stddev;
        double min;
        double max;
        double range;
        double skewness;
        double kurtosis;
    };
    
    StatisticalInfo calculateStatistics(const std::vector<double>& data);
    
    // Create color map for visualization
    std::vector<std::string> createColorMap(const std::string& colormapName, int colorCount);
    
    // Get supported color map names
    std::vector<std::string> getColorMapNames();
    
    // Validate visualization data
    bool isValid(const VisualizationData& data);
    std::string validateData(const VisualizationData& data);
    
    // File format utilities
    std::string getFileExtension(const std::string& filepath);
    bool isImageFile(const std::string& filepath);
    bool isTextFile(const std::string& filepath);
    bool isDataFile(const std::string& filepath);
    
    // Generate unique filename
    std::string generateUniqueFilename(const std::string& basePath,
                                       const std::string& prefix = "",
                                       const std::string& extension = "");
    
    // Convert bytes to human-readable format
    std::string bytesToHumanReadable(size_t bytes);
    
    // Convert time to human-readable format
    std::string timeToHumanReadable(double seconds);
}

// Visualization exception classes
class VisualizationException : public std::runtime_error {
public:
    VisualizationException(const std::string& message) : std::runtime_error(message) {}
};

class VisualizationInitializationException : public VisualizationException {
public:
    VisualizationInitializationException(const std::string& message) 
        : VisualizationException("Visualization initialization failed: " + message) {}
};

class VisualizationRenderException : public VisualizationException {
public:
    VisualizationRenderException(const std::string& message) 
        : VisualizationException("Visualization rendering failed: " + message) {}
};

class VisualizationDataException : public VisualizationException {
public:
    VisualizationDataException(const std::string& message) 
        : VisualizationException("Visualization data error: " + message) {}
};

// Visualization error codes
enum class VisualizationErrorCode {
    Success = 0,
    InitializationFailed = 1,
    RenderingFailed = 2,
    DataError = 3,
    ConfigurationError = 4,
    FileError = 5,
    MemoryError = 6,
    UnsupportedFormat = 7,
    InvalidData = 8,
    UnknownError = 9
};

// Visualization progress tracking
class VisualizationProgress {
public:
    VisualizationProgress() : currentStep(0), totalSteps(0), progressPercent(0.0), status("Idle") {}
    
    void startProgress(int totalSteps) {
        currentStep = 0;
        totalSteps = totalSteps;
        progressPercent = 0.0;
        status = "Running";
    }
    
    void updateProgress(int step) {
        currentStep = step;
        if (totalSteps > 0) {
            progressPercent = static_cast<double>(currentStep) / totalSteps * 100.0;
        }
    }
    
    void completeProgress() {
        currentStep = totalSteps;
        progressPercent = 100.0;
        status = "Completed";
    }
    
    bool isCompleted() const { return status == "Completed"; }
    bool isRunning() const { return status == "Running"; }
    
    std::string getStatus() const { return status; }
    double getProgressPercent() const { return progressPercent; }
    int getCurrentStep() const { return currentStep; }
    int getTotalSteps() const { return totalSteps; }
    
private:
    int currentStep;
    int totalSteps;
    double progressPercent;
    std::string status;
};

// Visualization callback types
using DataCallback = std::function<void(const VisualizationData&)>;
using RenderCallback = std::function<void(const std::string&)>;
using ErrorCallback = std::function<void(const std::string&)>;
using ProgressCallback = std::function<void(const VisualizationProgress&)>;

// Visualization event types
enum class VisualizationEvent {
    DataLoaded,
    RenderingStarted,
    RenderingCompleted,
    RenderingFailed,
    ConfigurationChanged,
    ErrorOccurred,
    ProgressUpdated,
    ExportCompleted,
    ImportCompleted
};

// Visualization event handler
class VisualizationEventHandler {
public:
    virtual ~VisualizationEventHandler() = default;
    
    virtual void onEvent(VisualizationEvent event, const std::map<std::string, std::string>& data) = 0;
    virtual bool shouldHandleEvent(VisualizationEvent event) const = 0;
};

} // namespace nlmviz

// Global visualization instance management
namespace Visualization {
    // Initialize global visualization system
    bool initialize();
    
    // Shutdown global visualization system
    bool shutdown();
    
    // Get global visualizer factory
    nlmviz::VisualizerFactory& getFactory();
    
    // Register built-in visualizers
    void registerBuiltInVisualizers();
    
    // Create visualizer
    std::unique_ptr<nlmviz::Visualizer> createVisualizer(const std::string& name);
    
    // Get list of available visualizers
    std::vector<std::string> getAvailableVisualizers();
    
    // Set global configuration
    bool setGlobalConfig(const std::map<std::string, std::string>& config);
    
    // Get global configuration
    std::map<std::string, std::string> getGlobalConfig();
    
    // Get visualization system status
    std::string getSystemStatus();
    
    // Get system version
    std::string getSystemVersion();
}

// Convenience functions
inline std::unique_ptr<nlmviz::Visualizer> createVisualizer(const std::string& name) {
    return Visualization::createVisualizer(name);
}

inline std::vector<std::string> getAvailableVisualizers() {
    return Visualization::getAvailableVisualizers();
}

inline bool initializeVisualization() {
    return Visualization::initialize();
}

inline bool shutdownVisualization() {
    return Visualization::shutdown();
}