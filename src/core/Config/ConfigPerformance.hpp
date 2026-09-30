// Performance-related configuration parameters

#pragma once

#include <string>
#include "core/Config/Config.hpp"

namespace nlm {

// Performance configuration structure
struct PerformanceConfig {
    // Threading and parallelism
    size_t num_threads;
    bool enable_simd;
    std::string performance_mode;
    
    // Optimization settings
    size_t memory_pool_size;
    bool enable_profiling;
    std::string benchmark_mode;
    double target_fps;
    size_t max_memory_usage_mb;
    
    // Checkpointing
    size_t checkpoint_interval;
    
    PerformanceConfig() : num_threads(std::max(1u, std::thread::hardware_concurrency())),
                         enable_simd(true), performance_mode("balanced"),
                         memory_pool_size(1024 * 1024),  // 1MB
                         enable_profiling(false), benchmark_mode("off"),
                         target_fps(30.0), max_memory_usage_mb(1024),  // 1GB
                         checkpoint_interval(100) {}
};

// Performance configuration manager
class ConfigPerformance {
public:
    ConfigPerformance() = default;
    ~ConfigPerformance() = default;
    
    // Load from Config object
    void loadFromConfig(const Config& config);
    
    // Save to Config object
    void saveToConfig(Config& config) const;
    
    // Validate configuration
    bool validate() const;
    
    // Get parameters
    const PerformanceConfig& get() const { return params; }
    PerformanceConfig& get() { return params; }
    
    // Set parameters
    void set(const PerformanceConfig& p) { params = p; }
    
    // Update individual parameter
    template<typename T>
    void setParameter(const std::string& key, const T& value);
    
    // Get parameter
    template<typename T>
    T getParameter(const std::string& key, const T& defaultValue = T()) const;
    
private:
    PerformanceConfig params;
};

} // namespace nlm