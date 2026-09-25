#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <thread>
#include <chrono>
#include <optional>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <set>
#include <deque>
#include <array>
#include <fstream>
#include <iostream>
#include "../visualization/VisualizationInterface.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/NeuralRegion.hpp"

namespace nlm {

// Visualization types
enum class VisualizationMode {
    Static,
    Dynamic,
    RealTime,
    EventDriven,
    PerformanceBased
};

enum class VisualizationComponent {
    NetworkStructure,
    NeuronActivity,
    SynapseWeights,
    SpikeRasters,
    PerformanceMetrics,
    MemoryUsage,
    Custom
};

enum class ExportFormat {
    JSON,
    CSV,
    PNG,
    SVG,
    HTML,
    VRML,
    OBJ,
    GLTF
};

// Custom visualization component
struct VisualizationComponent {
    std::string name;
    std::string type;
    std::string description;
    std::string implementation;
    std::map<std::string, std::string> parameters;
    std::vector<std::string> requiredData;
    bool isActive;
    std::function<void(const std::vector<float>& data, std::vector<float>& output)> processor;
    
    VisualizationComponent() : isActive(true) {}
    
    VisualizationComponent(const std::string& name, const std::string& type,
                          const std::string& desc, const std::string& impl)
        : name(name), type(type), description(desc), implementation(impl), isActive(true) {}
};

// Neural network visualization data
struct NeuralNetworkVisualization {
    std::vector<std::vector<float>> neuronPositions;
    std::vector<std::vector<float>> neuronSizes;
    std::vector<std::vector<float>> neuronActivations;
    std::vector<std::array<float, 3>> neuronColors;
    std::vector<std::pair<size_t, size_t>> synapses;
    std::vector<float> synapseWeights;
    std::vector<std::array<float, 3>> synapseColors;
    std::vector<std::vector<float>> populationCenters;
    std::vector<float> populationRadii;
    
    NeuralNetworkVisualization() = default;
};

// Performance dashboard data
struct PerformanceDashboard {
    std::vector<std::string> metricNames;
    std::vector<std::vector<double>> metricValues;
    std::vector<std::vector<double>> metricTimestamps;
    std::vector<std::array<float, 3>> metricColors;
    std::vector<std::string> metricUnits;
    std::vector<double> metricThresholds;
    std::map<std::string, std::string> metricDescriptions;
    
    PerformanceDashboard() = default;
};

// Debug visualization tools
struct DebugVisualization {
    std::string visualizationType;
    std::vector<std::vector<float>> debugData;
    std::string title;
    std::string description;
    std::string mode; // "heatmap", "scatter", "contour", "graph"
    std::map<std::string, std::string> options;
    
    DebugVisualization() = default;
};

// Export configuration
struct ExportConfig {
    ExportFormat format;
    std::string filepath;
    std::string filename;
    std::string title;
    std::string description;
    bool includeMetadata;
    bool compress;
    std::map<std::string, std::string> customOptions;
    
    ExportConfig() : format(ExportFormat::JSON), includeMetadata(true), compress(false) {}
    
    ExportConfig(ExportFormat fmt, const std::string& path, const std::string& name)
        : format(fmt), filepath(path), filename(name), includeMetadata(true), compress(false) {}
};

// Visualization update event
struct VisualizationUpdate {
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string eventType;
    std::string componentName;
    std::vector<float> data;
    std::map<std::string, std::string> metadata;
    
    VisualizationUpdate() = default;
    VisualizationUpdate(const std::string& type, const std::string& component,
                       const std::vector<float>& data)
        : eventType(type), componentName(component), data(data) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

// Advanced visualization API
class VisualizationAPI {
public:
    VisualizationAPI();
    ~VisualizationAPI();
    
    // Disable copying
    VisualizationAPI(const VisualizationAPI&) = delete;
    VisualizationAPI& operator=(const VisualizationAPI&) = delete;
    
    // Singleton access
    static VisualizationAPI& getInstance() {
        static VisualizationAPI instance;
        return instance;
    }
    
    // Visualization management
    bool initialize();
    void shutdown();
    bool isActive() const;
    
    // Visualization modes
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
    // Component management
    bool registerComponent(const VisualizationComponent& component);
    bool unregisterComponent(const std::string& componentName);
    VisualizationComponent* getComponent(const std::string& componentName);
    const VisualizationComponent* getComponent(const std::string& componentName) const;
    std::vector<VisualizationComponent> getAllComponents() const;
    
    // Neural network visualization
    bool visualizeNetwork(const Brain& brain, const NeuralNetworkVisualization& vizData);
    bool visualizeNetworkStructure(const Brain& brain);
    bool visualizeNeuronActivity(const Brain& brain, double threshold = 0.5);
    bool visualizeSynapseWeights(const Brain& brain, double minWeight = 0.0, double maxWeight = 1.0);
    bool visualizeSpikeRasters(const std::vector<std::vector<float>>& spikes, double timescale = 1.0);
    
    // Performance dashboard
    bool updatePerformanceDashboard(const PerformanceDashboard& dashboard);
    bool createPerformanceDashboard(const std::vector<std::string>& metricNames,
                                   const std::vector<std::vector<double>>& values);
    bool visualizePerformanceMetrics(const std::string& sessionId = "");
    
    // Debug visualization
    bool createDebugVisualization(const DebugVisualization& debugViz);
    bool updateDebugVisualization(const std::string& visualizationId,
                                 const std::vector<std::vector<float>>& data);
    bool showDebugVisualization(const std::string& visualizationId);
    
    // Export functionality
    bool exportVisualization(const ExportConfig& config,
                            const NeuralNetworkVisualization& networkData,
                            const PerformanceDashboard& perfData,
                            const std::vector<DebugVisualization>& debugVizs);
    bool exportToJSON(const std::string& filepath,
                    const NeuralNetworkVisualization& networkData,
                    const PerformanceDashboard& perfData) const;
    bool exportToCSV(const std::string& filepath,
                    const PerformanceDashboard& perfData) const;
    bool exportToImage(const std::string& filepath,
                     const ExportConfig& config) const;
    
    // Real-time visualization updates
    void addUpdateListener(std::function<void(const VisualizationUpdate&)> listener);
    void removeUpdateListener(const std::string& listenerId);
    void broadcastUpdate(const VisualizationUpdate& update);
    
    // Configuration integration
    void setVisualizationConfig(const Config& config);
    Config getVisualizationConfig() const;
    
    // Interactive features
    bool enableInteractiveMode();
    bool disableInteractiveMode();
    bool isInteractiveModeEnabled() const;
    
    // Component processing
    bool processComponent(const std::string& componentName,
                         const std::vector<float>& inputData,
                         std::vector<float>& outputData);
    std::vector<float> processAllComponents(const std::vector<float>& inputData);
    
    // Visualization statistics
    struct VisualizationStats {
        size_t totalComponents{0};
        size_t activeComponents{0};
        size_t totalUpdates{0};
        uint64_t totalProcessingTime{0};
        double averageFrameTime{0.0};
        size_t memoryUsage{0};
        double cpuUsage{0.0};
        std::map<std::string, double> componentPerformance;
        
        void reset() {
            totalComponents = activeComponents = totalUpdates = 0;
            totalProcessingTime = 0;
            averageFrameTime = 0.0;
            memoryUsage = 0;
            cpuUsage = 0.0;
            componentPerformance.clear();
        }
    };
    
    VisualizationStats getStats() const;
    
    // Error handling
    std::string getLastError() const { return lastError_; }
    void setLastError(const std::string& error) { lastError_ = error; }
    
    // Configuration integration helpers
    bool isComponentEnabled(const std::string& componentName) const;
    double getComponentUpdateRate(const std::string& componentName) const;
    
private:
    // Internal visualization processing
    bool initializeComponents();
    void cleanupComponents();
    bool updateComponent(const VisualizationComponent& component,
                        const std::vector<float>& inputData,
                        std::vector<float>& outputData);
    
    // Export helpers
    bool writeJSONExport(const std::string& filepath,
                       const NeuralNetworkVisualization& networkData,
                       const PerformanceDashboard& perfData,
                       const std::vector<DebugVisualization>& debugVizs) const;
    bool writeCSVExport(const std::string& filepath,
                      const PerformanceDashboard& perfData) const;
    bool writeImageExport(const std::string& filepath,
                        const ExportConfig& config) const;
    
    // Thread synchronization
    mutable std::mutex systemMutex_;
    std::atomic<bool> isLocked_;
    
    // Visualization state
    bool isActive_;
    VisualizationMode currentMode_;
    bool interactiveModeEnabled_;
    
    // Components
    std::unordered_map<std::string, VisualizationComponent> components_;
    std::vector<std::function<void(const VisualizationUpdate&)>> updateListeners_;
    mutable std::mutex componentsMutex_;
    mutable std::mutex listenersMutex_;
    
    // Visualization data
    NeuralNetworkVisualization currentNetworkData_;
    PerformanceDashboard currentPerfData_;
    std::unordered_map<std::string, DebugVisualization> debugVisualizations_;
    mutable std::mutex dataMutex_;
    
    // Configuration
    Config visualizationConfig_;
    mutable std::mutex configMutex_;
    
    // Statistics
    VisualizationStats stats_;
    mutable std::mutex statsMutex_;
    
    // Error handling
    std::string lastError_;
    mutable std::mutex errorMutex_;
    
    // Performance monitoring
    std::chrono::high_resolution_clock::time_point lastUpdateTime_;
    double frameTimeAccumulator_;
    size_t frameCount_;
    mutable std::mutex performanceMutex_;
};

// Visualization helper functions
inline std::string exportFormatToString(ExportFormat format) {
    switch (format) {
        case ExportFormat::JSON: return "json";
        case ExportFormat::CSV: return "csv";
        case ExportFormat::PNG: return "png";
        case ExportFormat::SVG: return "svg";
        case ExportFormat::HTML: return "html";
        case ExportFormat::VRML: return "vrml";
        case ExportFormat::OBJ: return "obj";
        case ExportFormat::GLTF: return "gltf";
        default: return "unknown";
    }
}

inline ExportFormat stringToExportFormat(const std::string& str) {
    if (str == "json") return ExportFormat::JSON;
    if (str == "csv") return ExportFormat::CSV;
    if (str == "png") return ExportFormat::PNG;
    if (str == "svg") return ExportFormat::SVG;
    if (str == "html") return ExportFormat::HTML;
    if (str == "vrml") return ExportFormat::VRML;
    if (str == "obj") return ExportFormat::OBJ;
    if (str == "gltf") return ExportFormat::GLTF;
    return ExportFormat::JSON;
}

inline std::string visualizationModeToString(VisualizationMode mode) {
    switch (mode) {
        case VisualizationMode::Static: return "static";
        case VisualizationMode::Dynamic: return "dynamic";
        case VisualizationMode::RealTime: return "realtime";
        case VisualizationMode::EventDriven: return "eventdriven";
        case VisualizationMode::PerformanceBased: return "performancebased";
        default: return "unknown";
    }
}

inline VisualizationMode stringToVisualizationMode(const std::string& str) {
    if (str == "static") return VisualizationMode::Static;
    if (str == "dynamic") return VisualizationMode::Dynamic;
    if (str == "realtime") return VisualizationMode::RealTime;
    if (str == "eventdriven") return VisualizationMode::EventDriven;
    if (str == "performancebased") return VisualizationMode::PerformanceBased;
    return VisualizationMode::Static;
}

} // namespace nlm