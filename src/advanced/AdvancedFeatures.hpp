#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <memory>
#include "../core/Config/Config.hpp"

namespace nlm {

// Configuration validation result
struct ConfigValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// Configuration change event
struct ConfigChangeEvent {
    std::string key;
    std::string oldValue;
    std::string newValue;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
};

// Configuration builder interface
class ConfigBuilder {
public:
    virtual ~ConfigBuilder() = default;
    virtual std::shared_ptr<Config> build() = 0;
    virtual ConfigBuilder& set(const std::string& key, const std::string& value) = 0;
    virtual ConfigBuilder& setInt(const std::string& key, int value) = 0;
    virtual ConfigBuilder& setDouble(const std::string& key, double value) = 0;
    virtual ConfigBuilder& setBool(const std::string& key, bool value) = 0;
};

// Simple config builder implementation
class SimpleConfigBuilder : public ConfigBuilder {
public:
    SimpleConfigBuilder() : config(std::make_shared<Config>()) {}
    
    std::shared_ptr<Config> build() override {
        auto result = config;
        config = std::make_shared<Config>();
        return result;
    }
    
    ConfigBuilder& set(const std::string& key, const std::string& value) override {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& setInt(const std::string& key, int value) override {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& setDouble(const std::string& key, double value) override {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& setBool(const std::string& key, bool value) override {
        config->set(key, value);
        return *this;
    }
    
private:
    std::shared_ptr<Config> config;
};

// Advanced configuration manager with history tracking
class AdvancedConfigManager {
public:
    AdvancedConfigManager();
    ~AdvancedConfigManager();
    
    // Apply preset configuration
    bool applyPreset(const std::string& presetName);
    
    // Validate configuration
    ConfigValidationResult validate(const Config& config) const;
    
    // Get configuration history
    std::vector<ConfigChangeEvent> getHistory() const;
    
    // Get configuration statistics
    std::string getStats() const;
    
    // Runtime configuration modifications
    bool modify(const std::string& key, const std::string& value);
    
    // Get configuration value with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Export configuration
    bool exportConfig(const std::string& filepath) const;
    
    // Import configuration with validation
    bool importConfig(const std::string& filepath);
    
    // Reset to previous state
    bool rollback(size_t steps = 1);
    
    // Clear history
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance metrics structure
struct PerformanceMetrics {
    double cpuUsage;              // CPU usage percentage
    double memoryUsage;           // Memory usage percentage
    size_t memoryAllocations;     // Number of memory allocations
    size_t memoryDeallocations;   // Number of memory deallocations
    double averageFiringRate;     // Average firing rate across brain
    size_t totalSpikes;           // Total spike count
    size_t activeNeurons;         // Number of active neurons
    double executionTime;          // Time for last step (ms)
    double wallClockTime;          // Wall clock time (ms)
    size_t timestepCount;          // Number of timesteps processed
    std::chrono::system_clock::time_point timestamp;
};

// Performance alert levels
enum class AlertLevel {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Performance alert
struct PerformanceAlert {
    AlertLevel level;
    std::string message;
    std::string metric;
    double value;
    double threshold;
    std::chrono::system_clock::time_point timestamp;
};

// Performance monitor for real-time tracking
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Record performance metrics
    void recordMetrics(const PerformanceMetrics& metrics);
    
    // Check for performance bottlenecks
    std::vector<std::string> detectBottlenecks() const;
    
    // Get performance alerts
    std::vector<PerformanceAlert> getAlerts() const;
    
    // Get current performance metrics
    PerformanceMetrics getCurrentMetrics() const;
    
    // Get performance statistics
    std::string getStats() const;
    
    // Reset monitor state
    void reset();
    
    // Set performance thresholds
    void setThreshold(const std::string& metric, double warning, double error, double critical);
    
    // Check resource usage
    bool checkResourceUsage() const;
    
    // Generate performance report
    std::string generateReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced experiment with optimization
class AdvancedExperiment {
public:
    AdvancedExperiment(const std::string& name);
    ~AdvancedExperiment();
    
    // Add parameter to optimize
    void addParameter(const std::string& name, double min, double max, double step = 0.1);
    
    // Add constraint
    void addConstraint(const std::string& condition, const std::string& description);
    
    // Run experiment with parameter sweep
    std::map<std::string, double> runParameterSweep(
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // Run experiment with adaptive optimization
    std::map<std::string, double> runAdaptiveOptimization(
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // Get experiment results
    std::string getResults() const;
    
    // Save experiment configuration
    bool save(const std::string& filepath) const;
    
    // Load experiment configuration
    bool load(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm