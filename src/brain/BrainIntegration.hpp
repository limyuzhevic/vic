#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <functional>

namespace nlm {

// Forward declarations
class Brain;
class VisualizationInterface;
class DebugSystem;
class EnhancedLogger;

// Visualization output formats
enum class OutputFormat {
    PNG,
    SVG,
    PDF,
    Video,
    WebGL,
    JSON
};

// Integration configuration for visualization and debugging
struct IntegrationConfig {
    bool enableVisualization{true};
    bool enableRealTimeDebugging{true};
    bool enablePerformanceProfiling{true};
    bool enableMemoryTracking{true};
    bool enableNeuralActivityTracking{true};
    bool enableSimulationStateTracking{true};
    
    // Visualization settings
    double visualizationUpdateRate{30.0};
    int visualizationWidth{1920};
    int visualizationHeight{1080};
    bool visualizationFullscreen{false};
    
    // Debug settings
    int debugLevel{2};
    size_t maxDebugEvents{1000};
    double debugUpdateInterval{0.1}; // seconds
    
    // Performance settings
    bool enableCPUProfiling{true};
    bool enableMemoryProfiling{true};
    bool enableGPUMonitoring{false}; // Would require GPU API
    
    // Neural activity tracking settings
    bool trackNeuronFiring{true};
    bool trackMembranePotentials{true};
    bool trackSynapticWeights{true};
    
    // Simulation tracking settings
    bool trackStepTransitions{true};
    bool trackMemoryUsage{true};
    bool trackErrorHistory{true};
};

// Brain integration class for visualization and debugging
class BrainIntegration {
public:
    BrainIntegration();
    ~BrainIntegration();
    
    // Initialize integration with configuration
    bool initialize(std::shared_ptr<Brain> brain, const IntegrationConfig& config = IntegrationConfig());
    
    // Update integration (called each simulation step)
    void update();
    
    // Get visualization interface
    VisualizationInterface& getVisualization() const;
    
    // Get debug system
    DebugSystem& getDebugSystem() const;
    
    // Get enhanced logger
    EnhancedLogger& getLogger() const;
    
    // Check if integration is active
    bool isActive() const;
    
    // Configuration methods
    void setVisualizationEnabled(bool enabled);
    void setRealTimeDebuggingEnabled(bool enabled);
    void setPerformanceProfilingEnabled(bool enabled);
    void setMemoryTrackingEnabled(bool enabled);
    void setNeuralActivityTrackingEnabled(bool enabled);
    void setSimulationStateTrackingEnabled(bool enabled);
    
    // Control methods
    void startMonitoring();
    void stopMonitoring();
    void pauseMonitoring();
    void resumeMonitoring();
    
    // Debugging methods
    void startProfiling();
    void stopProfiling();
    void startMemoryTracking();
    void stopMemoryTracking();
    void startNeuralActivityTracking();
    void stopNeuralActivityTracking();
    void startSimulationStateTracking();
    void stopSimulationStateTracking();
    
    // Export and reporting
    std::string generateDebugReport() const;
    bool exportVisualization(const std::string& filename, OutputFormat format = OutputFormat::PNG, 
                            int quality = 90);
    bool exportDebugData(const std::string& filename);
    
    // Statistics and metrics
    struct IntegrationStats {
        double simulationTime;
        double visualizationFPS;
        size_t memoryUsage;
        size_t peakMemoryUsage;
        double cpuLoad;
        double gpuLoad;
        uint64_t totalNeurons;
        uint64_t activeNeurons;
        uint64_t totalSpikes;
        double currentFiringRate;
        uint64_t currentStep;
        double currentTime;
        size_t debugEventsCount;
        bool isMonitoring;
        bool isProfiling;
        bool isVisualizationActive;
    };
    
    IntegrationStats getStats() const;
    
    // Event callbacks
    using VisualizationCallback = std::function<void(const VisualizationInterface&)>;
    using DebugCallback = std::function<void(const DebugSystem&)>;
    using PerformanceCallback = std::function<void(double fps, double frameTime)>;
    
    void registerVisualizationCallback(const VisualizationCallback& callback);
    void unregisterVisualizationCallback();
    void registerDebugCallback(const DebugCallback& callback);
    void unregisterDebugCallback();
    void registerPerformanceCallback(const PerformanceCallback& callback);
    void unregisterPerformanceCallback();
    
    // Performance measurement
    struct PerformanceMeasurement {
        std::string operationName;
        double duration; // seconds
        std::chrono::system_clock::time_point timestamp;
        std::string context;
    };
    
    void startPerformanceMeasurement(const std::string& operationName,
                                    const std::string& context = "");
    void stopPerformanceMeasurement();
    double getCurrentPerformanceMeasurement() const;
    void recordCustomPerformance(const std::string& name, double value,
                                const std::string& unit = "ms",
                                const std::string& context = "");
    
    // Neural activity analysis
    struct NeuralActivityAnalysis {
        uint64_t neuronId;
        double averageFiringRate;
        double standardDeviation;
        double maxFiringRate;
        double minFiringRate;
        size_t spikeCount;
        double averageMembranePotential;
        double membranePotentialStdDev;
        size_t timeStepsAnalyzed;
    };
    
    std::vector<NeuralActivityAnalysis> analyzeNeuralActivity(uint64_t startStep = 0,
                                                              uint64_t endStep = 0) const;
    std::string getNeuralActivityReport() const;
    
    // Memory analysis
    struct MemoryAnalysis {
        size_t totalAllocated;
        size_t currentUsage;
        size_t peakUsage;
        std::vector<size_t> allocationHistory;
        double averageAllocation;
        double allocationStdDev;
    };
    
    MemoryAnalysis analyzeMemory() const;
    std::string getMemoryReport() const;
    
    // Simulation analysis
    struct SimulationAnalysis {
        uint64_t totalSteps;
        double totalTime;
        double averageStepTime;
        double simulationSpeed;
        size_t errorCount;
        std::vector<std::string> errors;
        std::vector<std::pair<uint64_t, double>> stepTimes;
    };
    
    SimulationAnalysis analyzeSimulation() const;
    std::string getSimulationReport() const;
    
    // Event system
    struct DebugEvent {
        std::chrono::system_clock::time_point timestamp;
        std::string eventType;
        std::string message;
        std::unordered_map<std::string, std::string> metadata;
    };
    
    void recordEvent(const std::string& eventType, const std::string& message,
                    const std::unordered_map<std::string, std::string>& metadata = {});
    std::vector<DebugEvent> getRecentEvents(size_t count = 100) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
