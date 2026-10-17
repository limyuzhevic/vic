#ifndef NLM_DEVTOOLS_HPP
#define NLM_DEVTOOLS_HPP

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>

namespace nlm {

class NLMDevTools {
public:
    struct MemoryInfo {
        size_t totalAllocated;
        size_t currentAllocated;
        size_t peakAllocated;
        std::map<std::string, size_t> allocationsByType;
    };

    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        size_t neuronsActive;
        size_t spikesPerSecond;
        double averageFiringRate;
        double eI_ratio;
        std::chrono::duration<double> lastUpdateTime;
    };

    struct DebugInfo {
        std::string timestamp;
        std::string eventType;
        std::string message;
        std::map<std::string, std::string> metadata;
        std::string stackTrace;
    };

    NLMDevTools();
    ~NLMDevTools() = default;

    // Configuration validation
    bool validateConfiguration(std::shared_ptr<Config> config) const;
    bool validateBrainConfiguration(std::shared_ptr<Brain> brain) const;
    bool checkConfigurationConsistency(std::shared_ptr<Config> config) const;

    // Debugging tools
    void enableDebugMode(bool enable);
    void logDebugInfo(const std::string& eventType, const std::string& message,
                     const std::map<std::string, std::string>& metadata = {});
    std::vector<DebugInfo> getDebugLog() const;
    void clearDebugLog();

    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    PerformanceMetrics getCurrentPerformanceMetrics() const;
    double getAverageCPUUsage(int samples = 10) const;
    double getAverageMemoryUsage(int samples = 10) const;
    void recordPerformanceSnapshot();
    std::vector<PerformanceMetrics> getPerformanceHistory() const;

    // Memory management
    MemoryInfo getMemoryInfo() const;
    void enableMemoryLeakDetection(bool enable);
    std::vector<std::string> getMemoryLeakReport() const;
    void forceGarbageCollection();

    // Brain state debugging
    bool compareBrainStates(std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2,
                           double tolerance = 0.01) const;
    std::string generateBrainStateDiff(std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) const;
    bool detectBrainAnomalies(std::shared_ptr<Brain> brain) const;
    std::vector<std::string> getBrainHealthReport(std::shared_ptr<Brain> brain) const;

    // Step-by-step execution
    void enableStepByStep(bool enable);
    void stepOne(bool pauseAfterStep = true);
    bool isPaused() const;
    void resumeExecution();
    void stopExecution();

    // Export/import tools
    bool exportBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) const;
    bool importBrainState(std::shared_ptr<Brain> brain, const std::string& filepath);
    bool exportConfiguration(std::shared_ptr<Config> config, const std::string& filepath) const;
    bool importConfiguration(std::shared_ptr<Config> config, const std::string& filepath);

    // Neural analysis tools
    double calculateConnectivityMetrics(std::shared_ptr<Brain> brain) const;
    double calculatePlasticityMetrics(std::shared_ptr<Brain> brain) const;
    double calculateMemoryMetrics(std::shared_ptr<Brain> brain) const;
    std::vector<std::pair<std::string, double>> getNeuralSystemStatus(std::shared_ptr<Brain> brain) const;

    // Event system
    using DebugCallback = std::function<void(const DebugInfo&)>;
    void addDebugCallback(const DebugCallback& callback);
    void removeDebugCallback(const DebugCallback& callback);

    // Configuration tools
    bool generateConfigurationTemplate(const std::string& filename) const;
    bool validateConfigurationSchema(const std::string& schemaFile) const;
    std::map<std::string, std::string> analyzeConfiguration(std::shared_ptr<Config> config) const;

protected:
    bool debugModeEnabled;
    bool performanceMonitoringEnabled;
    bool memoryLeakDetectionEnabled;
    bool stepByStepModeEnabled;
    bool executionPaused;
    bool executionStopped;

    mutable std::vector<DebugInfo> debugLog;
    mutable std::vector<PerformanceMetrics> performanceHistory;
    mutable std::vector<std::string> memoryLeaks;

    std::vector<DebugCallback> debugCallbacks;

    std::chrono::high_resolution_clock::time_point lastSnapshotTime;
    std::chrono::duration<double> performanceInterval;

    // Internal helpers
    void updatePerformanceMetrics(PerformanceMetrics& metrics) const;
    void checkMemoryLeaks() const;
    std::string getTimestamp() const;
    void notifyDebugCallbacks(const DebugInfo& info) const;
};

} // namespace nlm

#endif // NLM_DEVTOOLS_HPP
