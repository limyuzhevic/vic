#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <chrono>

namespace nlm {

// Metric data structure
struct MetricData {
    std::string name;
    std::string category;
    std::string description;
    double value;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> tags;
    
    MetricData(const std::string& name, const std::string& category, 
               const std::string& description, double value)
        : name(name), category(category), description(description), 
          value(value), timestamp(std::chrono::system_clock::now()) {}
};

// Metric collector function signature
using MetricCollector = std::function<double()>;

// Built-in metric categories
enum class MetricCategory {
    NEURAL_PERFORMANCE,
    MEMORY_PERFORMANCE,
    COGNITIVE_PERFORMANCE,
    SYSTEM_PERFORMANCE,
    BEHAVIOR_PERFORMANCE,
    CUSTOM
};

// Metric registry implementation
class MetricRegistry {
public:
    MetricRegistry();
    ~MetricRegistry() = default;
    
    // Register a built-in metric
    bool registerBuiltInMetric(const std::string& name, 
                              MetricCategory category,
                              const std::string& description,
                              MetricCollector collector);
    
    // Register a custom metric
    bool registerCustomMetric(const std::string& name,
                             const std::string& category,
                             const std::string& description,
                             MetricCollector collector,
                             const std::unordered_map<std::string, std::string>& tags = {});
    
    // Get metric collector
    MetricCollector* getCollector(const std::string& name);
    
    // Get metric metadata
    const MetricData* getMetricInfo(const std::string& name) const;
    
    // Collect all registered metrics
    std::vector<MetricData> collectAllMetrics() const;
    
    // Get metrics by category
    std::vector<const MetricData*> getMetricsByCategory(MetricCategory category) const;
    
    // List all registered metric names
    std::vector<std::string> getMetricNames() const;
    
    // Remove metric
    bool unregisterMetric(const std::string& name);
    
    // Check if metric exists
    bool hasMetric(const std::string& name) const;
    
    // Get registry statistics
    size_t getMetricCount() const { return metrics_.size(); }
    size_t getBuiltInMetricCount() const { return builtInMetrics_.size(); }
    size_t getCustomMetricCount() const { return customMetrics_.size(); }
    
    // Clear all metrics
    void clear();
    
    // Export metrics to JSON
    std::string exportToJSON() const;
    
    // Import metrics from JSON
    bool importFromJSON(const std::string& jsonData);
    
    // Thread safety
    void lock() { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() { std::lock_guard<std::mutex> lock(mutex_); }
    
private:
    struct MetricEntry {
        std::string name;
        std::string categoryStr;
        std::string description;
        MetricCollector collector;
        std::unordered_map<std::string, std::string> tags;
        MetricCategory category;
        bool isBuiltIn;
        
        MetricEntry(const std::string& name, MetricCategory category,
                   const std::string& description, MetricCollector collector,
                   bool isBuiltIn = false)
            : name(name), category(category), description(description),
              collector(std::move(collector)), isBuiltIn(isBuiltIn) {}
    };
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, MetricEntry> metrics_;
    std::unordered_map<std::string, MetricEntry> builtInMetrics_;
    std::unordered_map<std::string, MetricEntry> customMetrics_;
    
    // Generate unique IDs for custom metrics
    size_t customMetricCounter_;
};

} // namespace nlm
