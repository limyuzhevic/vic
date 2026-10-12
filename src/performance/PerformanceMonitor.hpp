#pragma once

#include <chrono>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <atomic>

namespace nlm {

// Performance monitoring system
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        std::chrono::high_resolution_clock::time_point timestamp;
        
        // Core metrics
        uint64_t neuronCount;
        uint64_t synapseCount;
        uint64_t totalSpikes;
        uint64_t activeNeurons;
        uint64_t firingNeurons;
        double averageFiringRate;
        double excitationInhibitionRatio;
        
        // Performance metrics
        double simulationTimePerStep; // microseconds
        double stepsPerSecond;
        double memoryUsageMB;
        double cpuUsagePercent;
        
        // Resource usage
        uint64_t memoryAllocatedBytes;
        uint64_t memoryPoolUsedBytes;
        uint64_t eventQueueSize;
        
        // Activity metrics
        uint64_t sensoryInputsReceived;
        uint64_t motorCommandsProduced;
        uint64_t plasticityEvents;
        uint64_t developmentUpdates;
        
        // State information
        DevelopmentalStage developmentalStage;
        double neuromodulationLevel;
        double curiosityLevel;
        double noveltyLevel;
        
        PerformanceMetrics() : timestamp(std::chrono::high_resolution_clock::now()),
                              neuronCount(0), synapseCount(0), totalSpikes(0),
                              activeNeurons(0), firingNeurons(0), averageFiringRate(0.0),
                              excitationInhibitionRatio(1.0), simulationTimePerStep(0.0),
                              stepsPerSecond(0.0), memoryUsageMB(0.0), cpuUsagePercent(0.0),
                              memoryAllocatedBytes(0), memoryPoolUsedBytes(0), 
                              eventQueueSize(0), sensoryInputsReceived(0),
                              motorCommandsProduced(0), plasticityEvents(0), 
                              developmentUpdates(0), neuromodulationLevel(0.0),
                              curiosityLevel(0.0), noveltyLevel(0.0) {}
    };
    
    struct PerformanceAlert {
        enum class Level { INFO, WARNING, ERROR, CRITICAL };
        std::chrono::high_resolution_clock::time_point timestamp;
        Level level;
        std::string metric;
        std::string message;
        double currentValue;
        double threshold;
        std::string recommendation;
        
        PerformanceAlert(Level lvl, const std::string& metric, const std::string& msg, 
                        double current, double threshold, const std::string& rec)
            : timestamp(std::chrono::high_resolution_clock::now()), level(lvl), 
              metric(metric), message(msg), currentValue(current), threshold(threshold),
              recommendation(rec) {}
    };
    
    struct PerformanceProfile {
        std::string profileName;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::vector<PerformanceMetrics> metricsHistory;
        std::vector<PerformanceAlert> alerts;
        
        // Profile statistics
        size_t getDurationSteps() const {
            if (metricsHistory.empty()) return 0;
            return metricsHistory.back().totalSpikes - 
                   (metricsHistory.size() > 1 ? metricsHistory[0].totalSpikes : 0);
        }
        
        double getAverageStepsPerSecond() const {
            double totalTime = std::chrono::duration<double>(
                endTime - startTime).count();
            size_t steps = getDurationSteps();
            return steps / totalTime;
        }
        
        std::string generateReport() const;
    };
    
    struct ProfilingSession {
        std::string sessionName;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::vector<std::function<void(const PerformanceMetrics&)>> customMetrics;
        bool enabled;
        
        ProfilingSession(const std::string& name) 
            : sessionName(name), enabled(true) {}
        
        std::chrono::duration<double> getDuration() const {
            return std::chrono::high_resolution_clock::now() - startTime;
        }
    };

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Record performance metrics
    void recordMetrics(const PerformanceMetrics& metrics);
    
    // Get current metrics
    PerformanceMetrics getCurrentMetrics() const;
    
    // Get metrics history
    std::vector<PerformanceMetrics> getMetricsHistory(size_t maxPoints = 0) const;
    
    // Get profile data
    std::vector<PerformanceProfile> getProfiles() const;
    
    // Create profiling session
    std::string startProfiling(const std::string& profileName, 
                              const std::vector<std::function<void(const PerformanceMetrics&)>>& customMetrics = {});
    
    // End profiling session
    bool endProfiling(const std::string& profileName);
    
    // Get profiling session
    ProfilingSession* getProfilingSession(const std::string& profileName);
    
    // Performance alerting
    void addAlert(const PerformanceAlert& alert);
    std::vector<PerformanceAlert> getAlerts(PerformanceAlert::Level level = PerformanceAlert::Level::INFO) const;
    
    // Performance analysis
    std::string analyzePerformance(const PerformanceMetrics& current, const PerformanceMetrics& baseline) const;
    
    // Memory profiling
    void startMemoryProfiling();
    void stopMemoryProfiling();
    double getCurrentMemoryUsage() const;
    
    // Time profiling
    void startTimeProfiling(const std::string& operationName);
    void stopTimeProfiling(const std::string& operationName);
    std::chrono::duration<double> getLastTimeMeasurement(const std::string& operationName) const;
    std::map<std::string, std::chrono::duration<double>> getAllTimeMeasurements() const;
    
    // CPU profiling
    void startCPUMonitoring();
    void stopCPUMonitoring();
    double getCurrentCPUUsage() const;
    
    // Generate performance reports
    std::string generatePerformanceReport(const std::string& profileName = "") const;
    std::string generateRealTimeStatus() const;
    
    // Clear data
    void clear();
    void clearHistory(size_t maxPoints = 0);
    
    // Configuration
    void setMaxHistoryPoints(size_t maxPoints);
    void setAlertThreshold(const std::string& metric, double threshold, PerformanceAlert::Level level);
    void setPerformanceProfile(const PerformanceProfile& profile);
    
    // Export/Import
    bool exportMetrics(const std::string& filepath) const;
    bool importMetrics(const std::string& filepath);
    bool exportProfiles(const std::string& directory) const;
    bool importProfiles(const std::string& directory);
};

// Performance analyzer for comparing different runs
class PerformanceAnalyzer {
public:
    struct ComparisonResult {
        std::string baselineName;
        std::string experimentalName;
        std::map<std::string, double> improvements; // Positive = improvement, negative = degradation
        std::map<std::string, std::string> recommendations;
        double overallScore; // Composite score
        
        std::string generateReport() const;
    };
    
    // Compare two performance profiles
    static ComparisonResult compareProfiles(const PerformanceMonitor::PerformanceProfile& baseline, 
                                           const PerformanceMonitor::PerformanceProfile& experimental);
    
    // Find performance bottlenecks
    static std::vector<std::string> findBottlenecks(const PerformanceMonitor::PerformanceProfile& profile);
    
    // Generate optimization recommendations
    static std::vector<std::string> generateRecommendations(const PerformanceMonitor::PerformanceProfile& profile);
    
    // Normalize metrics for comparison
    static std::map<std::string, double> normalizeMetrics(const PerformanceMonitor::PerformanceMetrics& metrics);
    
    // Calculate composite score
    static double calculatePerformanceScore(const PerformanceMonitor::PerformanceMetrics& metrics);
};

// Performance profiler for detailed operation timing
class PerformanceProfiler {
public:
    struct ProfileData {
        std::string operationName;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::chrono::duration<double> duration;
        uint64_t callCount;
        double totalTime;
        double averageTime;
        double minTime;
        double maxTime;
        
        ProfileData(const std::string& name) 
            : operationName(name), startTime(std::chrono::high_resolution_clock::now()),
              endTime(startTime), duration(0.0), callCount(0), totalTime(0.0),
              averageTime(0.0), minTime(std::numeric_limits<double>::max()),
              maxTime(0.0) {}
    };
    
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start profiling an operation
    void startProfiling(const std::string& operationName);
    
    // Stop profiling an operation
    void stopProfiling(const std::string& operationName);
    
    // Get profile data for operation
    std::optional<ProfileData> getProfileData(const std::string& operationName) const;
    
    // Get all profile data
    std::vector<ProfileData> getAllProfileData() const;
    
    // Generate profile report
    std::string generateProfileReport() const;
    
    // Clear profile data
    void clear();
    void clearOperation(const std::string& operationName);
    
    // Export/Import profile data
    bool exportProfile(const std::string& filepath) const;
    bool importProfile(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance metrics exporter for various formats
class PerformanceExporter {
public:
    enum class ExportFormat {
        CSV,
        JSON,
        YAML,
        XML,
        SQL,
        Markdown,
        HTML
    };
    
    // Export metrics to file
    static bool exportMetrics(const PerformanceMonitor::PerformanceProfile& profile,
                             const std::string& filepath,
                             ExportFormat format = ExportFormat::CSV,
                             const std::map<std::string, std::string>& options = {});
    
    // Export multiple profiles to file
    static bool exportMultipleProfiles(const std::vector<PerformanceMonitor::PerformanceProfile>& profiles,
                                       const std::string& directory,
                                       ExportFormat format = ExportFormat::CSV,
                                       const std::map<std::string, std::string>& options = {});
    
    // Import metrics from file
    static std::optional<PerformanceMonitor::PerformanceProfile> importMetrics(
        const std::string& filepath, ExportFormat format = ExportFormat::CSV);
    
    // Convert metrics to string (in-memory)
    static std::string metricsToString(const PerformanceMonitor::PerformanceProfile& profile,
                                      ExportFormat format = ExportFormat::CSV,
                                      const std::map<std::string, std::string>& options = {});
    
    // Convert metrics from string
    static std::optional<PerformanceMonitor::PerformanceProfile> stringToMetrics(
        const std::string& data, ExportFormat format = ExportFormat::CSV);
};

// Real-time performance monitor for web dashboards
class RealTimePerformanceMonitor {
public:
    struct DashboardData {
        std::chrono::high_resolution_clock::time_point timestamp;
        PerformanceMonitor::PerformanceMetrics metrics;
        std::vector<PerformanceMonitor::PerformanceAlert> alerts;
        std::vector<std::pair<std::string, double>> customMetrics;
        
        std::string toJSON() const;
        std::string toHTML() const;
    };
    
    RealTimePerformanceMonitor();
    ~RealTimePerformanceMonitor();
    
    // Start dashboard
    void startDashboard(size_t port = 8080, const std::string& host = "localhost");
    
    // Stop dashboard
    void stopDashboard();
    
    // Check if dashboard is running
    bool isDashboardRunning() const;
    
    // Get dashboard data
    DashboardData getDashboardData() const;
    
    // Add custom metric
    void addCustomMetric(const std::string& name, double value);
    
    // Remove custom metric
    void removeCustomMetric(const std::string& name);
    
    // Get custom metrics
    std::vector<std::pair<std::string, double>> getCustomMetrics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global performance monitor instance
extern PerformanceMonitor& getPerformanceMonitor();

// Convenience functions
inline void recordPerformanceMetrics(const PerformanceMonitor::PerformanceMetrics& metrics) {
    getPerformanceMonitor().recordMetrics(metrics);
}

inline PerformanceMonitor::PerformanceMetrics getCurrentPerformanceMetrics() {
    return getPerformanceMonitor().getCurrentMetrics();
}

inline void startPerformanceMonitoring() {
    getPerformanceMonitor().startMonitoring();
}

inline void stopPerformanceMonitoring() {
    getPerformanceMonitor().stopMonitoring();
}

inline std::string generatePerformanceReport(const std::string& profileName = "") {
    return getPerformanceMonitor().generatePerformanceReport(profileName);
}

} // namespace nlm