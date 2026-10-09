#pragma once

/**
 * Enhanced Configuration System with Performance Optimization Support
 * 
 * Extended configuration system for neural network simulations with performance tuning options.
 * Supports memory pool configuration, performance profiling settings, and optimization parameters.
 * 
 * Features:
 * - Performance optimization configuration
 * - Memory pool settings
 * - Performance profiling options
 * - SIMD configuration
 * - Concurrent processing settings
 * - Event queue configuration
 * - Checkpoint system options
 * - Configuration validation for performance
 * 
 * Biological motivation:
 * - Adaptive resource allocation like neural homeostasis
 * - Performance tuning based on environmental demands
 * - Configurable plasticity mechanisms for learning and adaptation
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>

namespace nlm {

/**
 * Performance optimization configuration options
 */
enum class PerformanceOptimizationLevel {
    Minimal = 0,      // Basic functionality only
    Balanced = 1,     // Standard optimizations
    Optimized = 2,    // Aggressive optimizations
    Maximum = 3       // Maximum optimizations (maximum performance)
};

/**
 * SIMD instruction set options
 */
enum class SIMDInstructionSet {
    None = 0,         // No SIMD
    SSE = 1,          // SSE (4 floats)
    AVX2 = 2,         // AVX2 (8 floats)
    AVX512 = 3,       // AVX-512 (16 floats)
    Auto = 4          // Auto-detect best available
};

/**
 * Memory pool configuration options
 */
enum class MemoryPoolStrategy {
    Static = 0,       // Static allocation
    Dynamic = 1,      // Dynamic allocation with pressure monitoring
    Adaptive = 2      // Adaptive allocation based on usage patterns
};

/**
 * Thread pool configuration options
 */
enum class ThreadPoolStrategy {
    Fixed = 0,        // Fixed number of threads
    Dynamic = 1,      // Dynamic thread pool
    WorkStealing = 2  // Work-stealing thread pool
};

/**
 * Event queue configuration options
 */
enum class EventQueueStrategy {
    RingBuffer = 0,   // Ring buffer for spikes
    Priority = 1,     // Priority queue for events
    TimeBatched = 2   // Time-batched event processing
};

/**
 * Performance configuration options
 */
struct PerformanceConfig {
    // Optimization level
    PerformanceOptimizationLevel optimizationLevel;     // Overall optimization level
    SIMDInstructionSet simdInstructionSet;              // SIMD instruction set
    MemoryPoolStrategy memoryPoolStrategy;              // Memory pool allocation strategy
    ThreadPoolStrategy threadPoolStrategy;              // Thread pool strategy
    EventQueueStrategy eventQueueStrategy;              // Event queue strategy
    
    // SIMD configuration
    bool enableSIMD;                                    // Enable SIMD optimizations
    size_t simdAlignment;                               // SIMD alignment requirements
    
    // Memory pool configuration
    size_t memoryPoolBlockSize;                         // Memory pool block size
    size_t memoryPoolInitialBlocks;                     // Initial memory pool blocks
    size_t memoryPoolMaxTotalObjects;                   // Maximum total objects in pool
    float memoryPoolTargetUtilization;                  // Target memory utilization
    bool memoryPoolDynamicResizing;                     // Enable dynamic pool resizing
    uint64_t memoryPoolResizeCheckInterval;             // Resize check interval
    
    // Thread pool configuration
    size_t threadPoolNumThreads;                        // Number of threads (0 = auto)
    size_t threadPoolMinThreads;                        // Minimum threads
    size_t threadPoolMaxThreads;                        // Maximum threads
    size_t threadPoolTaskQueueSize;                     // Task queue size
    uint32_t threadPoolKeepAliveTime;                   // Thread keep-alive time (ms)
    
    // Event queue configuration
    size_t eventQueueRingBufferSize;                    // Ring buffer size
    size_t eventQueueDelayedSpikeBuckets;               // Number of delayed spike buckets
    size_t eventQueueMaxDelaySteps;                     // Maximum synaptic delay in steps
    size_t eventQueuePriorityQueueSize;                 // Priority queue size
    
    // Performance profiling configuration
    bool enablePerformanceProfiling;                   // Enable performance profiling
    uint32_t profilingSampleInterval;                   // Profiling sample interval (ms)
    bool profileMemoryUsage;                           // Profile memory usage
    bool profileExecutionTime;                         // Profile execution time
    bool profileCPUUsage;                              // Profile CPU usage
    float profilingThresholdSlow;                      // Threshold for slow operations (us)
    float profilingThresholdHighMemory;                // Threshold for high memory usage
    
    // Checkpoint configuration
    bool enableCheckpointing;                          // Enable checkpointing
    uint64_t checkpointSaveIntervalSteps;               // Checkpoint save interval
    size_t checkpointMaxCheckpoints;                    // Maximum number of checkpoints
    bool checkpointCompress;                           // Enable checkpoint compression
    std::string checkpointDirectory;                   // Checkpoint directory
    
    // Configuration validation
    bool validateConfig() const;
    std::string getValidationErrors() const;
    
    PerformanceConfig()
        : optimizationLevel(PerformanceOptimizationLevel::Balanced),
          simdInstructionSet(SIMDInstructionSet::Auto),
          memoryPoolStrategy(MemoryPoolStrategy::Dynamic),
          threadPoolStrategy(ThreadPoolStrategy::Dynamic),
          eventQueueStrategy(EventQueueStrategy::RingBuffer),
          enableSIMD(true),
          simdAlignment(64),
          memoryPoolBlockSize(1024),
          memoryPoolInitialBlocks(4),
          memoryPoolMaxTotalObjects(1024 * 1024),
          memoryPoolTargetUtilization(0.75f),
          memoryPoolDynamicResizing(true),
          memoryPoolResizeCheckInterval(1000),
          threadPoolNumThreads(0),
          threadPoolMinThreads(1),
          threadPoolMaxThreads(32),
          threadPoolTaskQueueSize(10000),
          threadPoolKeepAliveTime(30000),
          eventQueueRingBufferSize(16384),
          eventQueueDelayedSpikeBuckets(1024),
          eventQueueMaxDelaySteps(100),
          eventQueuePriorityQueueSize(4096),
          enablePerformanceProfiling(false),
          profilingSampleInterval(100),
          profileMemoryUsage(true),
          profileExecutionTime(true),
          profileCPUUsage(false),
          profilingThresholdSlow(100.0f),
          profilingThresholdHighMemory(0.90f),
          enableCheckpointing(false),
          checkpointSaveIntervalSteps(10000),
          checkpointMaxCheckpoints(10),
          checkpointCompress(true),
          checkpointDirectory("./checkpoints") {}
};

/**
 * Enhanced configuration class with performance support
 */
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file with format detection
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from specific format files
    bool loadFromJSONFile(const std::string& filepath);
    bool loadFromYAMLFile(const std::string& filepath);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Performance configuration methods
    void setPerformanceConfig(const PerformanceConfig& config);
    PerformanceConfig getPerformanceConfig() const { return performanceConfig_; }
    
    // Performance-specific getter methods
    size_t getMemoryPoolBlockSize() const { return performanceConfig_.memoryPoolBlockSize; }
    size_t getThreadPoolNumThreads() const { return performanceConfig_.threadPoolNumThreads; }
    bool getEnableSIMD() const { return performanceConfig_.enableSIMD; }
    SIMDInstructionSet getSIMDInstructionSet() const { return performanceConfig_.simdInstructionSet; }
    bool getEnablePerformanceProfiling() const { return performanceConfig_.enablePerformanceProfiling; }
    bool getEnableCheckpointing() const { return performanceConfig_.enableCheckpointing; }
    
    // Performance configuration setter methods
    void setMemoryPoolBlockSize(size_t size) { performanceConfig_.memoryPoolBlockSize = size; }
    void setThreadPoolNumThreads(size_t threads) { performanceConfig_.threadPoolNumThreads = threads; }
    void setEnableSIMD(bool enable) { performanceConfig_.enableSIMD = enable; }
    void setSIMDInstructionSet(SIMDInstructionSet set) { performanceConfig_.simdInstructionSet = set; }
    void setEnablePerformanceProfiling(bool enable) { performanceConfig_.enablePerformanceProfiling = enable; }
    void setEnableCheckpointing(bool enable) { performanceConfig_.enableCheckpointing = enable; }
    
    // Check existence
    template<typename T>
    bool has(const std::string& key) const;
    bool has(const std::string& key) const;
    
    // Check for performance configuration keys
    bool hasPerformanceKey(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get performance configuration keys
    std::vector<std::string> getPerformanceKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get performance configuration summary
    std::string getPerformanceSummary() const;
    
    // Validate configuration
    bool validate() const;
    std::string getValidationErrors() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PerformanceConfig performanceConfig_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Performance configuration helpers
    void loadPerformanceFromJSON(const std::string& filepath);
    void loadPerformanceFromYAML(const std::string& filepath);
    bool savePerformanceToJSON(std::ostream& stream) const;
    
    // Configuration validation helpers
    std::vector<std::string> validatePerformanceConfig() const;
    
    // File format detection and parsing helpers
    static std::string detectFileFormat(const std::string& filepath);
    static void to_json(nlohmann::json& j, const ConfigValue& cv);
    static void from_json(const nlohmann::json& j, ConfigValue& cv);
    static void to_json(nlohmann::json& j, const PerformanceConfig& pc);
    static void from_json(const nlohmann::json& j, PerformanceConfig& pc);
    
    friend class ConfigLoader;
};

/**
 * Configuration loader utility class
 */
class ConfigLoader {
public:
    ConfigLoader();
    ~ConfigLoader();
    
    /**
     * Load performance configuration from file
     */
    static bool loadPerformanceConfig(Config& config, const std::string& filepath);
    
    /**
     * Save performance configuration to file
     */
    static bool savePerformanceConfig(const Config& config, const std::string& filepath);
    
    /**
     * Generate default performance configuration
     */
    static PerformanceConfig generateDefaultPerformanceConfig(
        PerformanceOptimizationLevel level = PerformanceOptimizationLevel::Balanced);
    
    /**
     * Apply performance configuration to brain initialization
     */
    static bool applyToBrain(Config& config, class Brain& brain);
    
    /**
     * Get performance optimization recommendations
     */
    static std::vector<std::string> getPerformanceOptimizationRecommendations(
        const PerformanceConfig& config);

private:
    static bool loadFromJSON(Config& config, const std::string& filepath);
    static bool loadFromYAML(Config& config, const std::string& filepath);
    static bool saveToJSON(const Config& config, const std::string& filepath);
    static bool saveToYAML(const Config& config, const std::string& filepath);
};