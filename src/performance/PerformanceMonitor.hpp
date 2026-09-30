#pragma once

#include <string>
#include <chrono>
#include <vector>

namespace nlm {

// Performance metrics structure
struct PerformanceMetric {
    std::string name;
    double value;
    std::chrono::system_clock::time_point lastUpdate;
    std::vector<std::pair<double, std::chrono::system_clock::time_point>> history;
};

// Performance event structure
struct PerformanceEvent {
    std::string name;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
};

// Performance monitor class for tracking system performance
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Frame timing
    void startFrame();
    void endFrame();
    
    double getFPS() const;
    double getLastFrameTime() const;
    double getAverageFrameTime() const;
    
    // Metric tracking
    void recordMetric(const std::string& name, double value);
    const PerformanceMetric& getMetric(const std::string& name) const;
    std::vector<PerformanceMetric> getAllMetrics() const;
    
    // Event tracking
    void addEvent(const std::string& name, const std::string& description);
    std::vector<PerformanceEvent> getRecentEvents(size_t limit = 10) const;
    
    // Configuration
    void setHistorySize(size_t size);
    void enableHistory(bool enable);
    void setSlowFrameThreshold(double threshold);
    void setVerySlowFrameThreshold(double threshold);
    
    // Memory tracking
    void updateMemoryUsage(size_t memoryBytes);
    size_t getPeakMemoryUsage() const;
    size_t getCurrentMemoryUsage() const;
    
    // Reporting
    std::string getPerformanceReport() const;
    
    // Static methods for global access
    static PerformanceMonitor& getGlobal();
    static void setGlobal(std::shared_ptr<PerformanceMonitor> monitor);
    
    // Helper functions for timing common operations
    template<typename Func>
    double timeOperation(const std::string& name, Func&& func);
    
    template<typename Func>
    auto withPerformanceMonitoring(const std::string& operationName, Func&& func);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions for performance monitoring
namespace Performance {
    // Time a specific operation
    template<typename Func>
    double timeOperation(const std::string& name, Func&& func);
    
    // Execute with performance monitoring
    template<typename Func>
    auto withMonitoring(const std::string& operationName, Func&& func);
    
    // Record a simple metric
    void record(const std::string& name, double value);
    
    // Start timing a frame
    void startFrame();
    
    // End timing a frame
    void endFrame();
    
    // Get current FPS
    double getFPS();
    
    // Get performance report
    std::string getReport();
}

} // namespace nlm
