#pragma once

#include "Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <unordered_map>
#include <memory>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace nlm {

/**
 * AdvancedBrain - Enhanced brain interface with advanced features
 * 
 * This class provides an enhanced interface to the basic Brain class with
 * additional functionality for advanced users including:
 * - Performance monitoring and profiling
 * - Advanced configuration management
 * - Thread-safe operations
 * - Event-driven callbacks
 * - Memory management utilities
 * - Debug and profiling tools
 * 
 * This interface is designed for users who need fine-grained control
 * over the brain simulation and want to integrate it into larger systems.
 */

class AdvancedBrain {
public:
    /**
     * Constructor with enhanced configuration support
     * 
     * @param config Brain configuration
     * @param enableMonitoring Enable performance monitoring
     * @param enableThreading Enable thread-safe operations
     * @param enableProfiling Enable profiling tools
     */
    AdvancedBrain(std::shared_ptr<Config> config, 
                  bool enableMonitoring = true,
                  bool enableThreading = false,
                  bool enableProfiling = false);
    
    ~AdvancedBrain();
    
    // Disable copying, enable moving
    AdvancedBrain(const AdvancedBrain&) = delete;
    AdvancedBrain& operator=(const AdvancedBrain&) = delete;
    AdvancedBrain(AdvancedBrain&&) noexcept;
    AdvancedBrain& operator=(AdvancedBrain&&) noexcept;
    
    /**
     * Initialize the advanced brain with extended configuration
     * 
     * @param enableMemoryTracking Enable memory usage tracking
     * @param enableEventLogging Enable event logging
     * @param enablePerformanceCounters Enable performance counters
     * @return true if initialization succeeded
     */
    bool initialize(bool enableMemoryTracking = true,
                    bool enableEventLogging = false,
                    bool enablePerformanceCounters = true);
    
    /**
     * Run simulation with advanced monitoring and control
     * 
     * @param startStep Starting step number
     * @param numSteps Number of steps to run
     * @param callback Optional progress callback
     * @param monitorInterval Monitoring interval in steps
     * @return Simulation result summary
     */
    SimulationResult runWithMonitoring(SimulationStep startStep,
                                       SimulationStep numSteps,
                                       std::function<void(float)> callback = nullptr,
                                       SimulationStep monitorInterval = 1000);
    
    /**
     * Get comprehensive performance statistics
     * 
     * @return Performance statistics structure
     */
    PerformanceStats getPerformanceStats() const;
    
    /**
     * Get memory usage information
     * 
     * @return Memory usage in bytes
     */
    uint64_t getMemoryUsage() const;
    
    /**
     * Get current simulation statistics
     * 
     * @return Current simulation statistics
     */
    SimulationStats getCurrentStats() const;
    
    /**
     * Configure advanced features
     * 
     * @param feature Feature name
     * @param value Configuration value
     */
    void configureFeature(const std::string& feature, const ConfigValue& value);
    
    /**
     * Get current configuration value
     * 
     * @param feature Feature name
     * @return Current configuration value
     */
    std::optional<ConfigValue> getConfiguration(const std::string& feature) const;
    
    /**
     * Enable or disable specific advanced feature
     * 
     * @param feature Feature name
     * @param enabled Enable state
     */
    void enableFeature(const std::string& feature, bool enabled);
    
    /**
     * Check if a feature is enabled
     * 
     * @param feature Feature name
     * @return true if feature is enabled
     */
    bool isFeatureEnabled(const std::string& feature) const;
    
    /**
     * Export brain state with metadata
     * 
     * @param filepath Output file path
     * @param includeMetadata Include metadata in export
     * @return true if export succeeded
     */
    bool exportState(const std::string& filepath, bool includeMetadata = true);
    
    /**
     * Import brain state with metadata validation
     * 
     * @param filepath Input file path
     * @param validateMetadata Validate metadata consistency
     * @return true if import succeeded
     */
    bool importState(const std::string& filepath, bool validateMetadata = true);
    
    /**
     * Set up event callbacks for brain state changes
     * 
     * @param eventType Type of event to listen for
     * @param callback Callback function
     * @return Callback ID for later removal
     */
    uint32_t addEventCallback(BrainEvent eventType, 
                              std::function<void(const BrainEventData&)> callback);
    
    /**
     * Remove event callback
     * 
     * @param callbackId ID of callback to remove
     */
    void removeEventCallback(uint32_t callbackId);
    
    /**
     * Get brain uptime in seconds
     * 
     * @return Uptime in seconds
     */
    double getUptime() const;
    
    /**
     * Get current simulation speed (steps per second)
     * 
     * @return Simulation speed in steps per second
     */
    double getSimulationSpeed() const;
    
    /**
     * Pause simulation
     * 
     * @param reason Reason for pausing
     */
    void pause(const std::string& reason = "");
    
    /**
     * Resume simulation
     */
    void resume();
    
    /**
     * Check if simulation is paused
     * 
     * @return true if simulation is paused
     */
    bool isPaused() const;
    
    /**
     * Get current pause reason
     * 
     * @return Pause reason string
     */
    std::string getPauseReason() const;
    
    /**
     * Export performance data for analysis
     * 
     * @param filepath Output file path
     * @param format Export format (csv, json, etc.)
     * @return true if export succeeded
     */
    bool exportPerformanceData(const std::string& filepath, 
                               const std::string& format = "csv");
    
    /**
     * Load advanced configuration from file
     * 
     * @param filepath Configuration file path
     * @param source ConfigSource identifier
     * @return true if configuration loaded successfully
     */
    bool loadAdvancedConfig(const std::string& filepath, 
                           ConfigSource source = ConfigSource::File);
    
    /**
     * Get thread safety status
     * 
     * @return true if thread safety is enabled
     */
    bool isThreadSafe() const;
    
    /**
     * Get profiling information
     * 
     * @param maxSamples Maximum number of samples to collect
     * @return Profiling information
     */
    ProfilingInfo getProfilingInfo(size_t maxSamples = 100) const;
    
    /**
     * Reset performance counters
     */
    void resetPerformanceCounters();
    
    /**
     * Get the underlying brain instance
     * 
     * @return Pointer to underlying Brain instance
     */
    Brain* getBrain() { return pBrain.get(); }
    
    /**
     * Get the underlying brain instance (const)
     * 
     * @return Pointer to underlying Brain instance
     */
    const Brain* getBrain() const { return pBrain.get(); }
    
    /**
     * Get advanced configuration interface
     * 
     * @return Reference to advanced configuration
     */
    const AdvancedConfig& getAdvancedConfig() const { return advancedConfig; }
    
    /**
     * Check if monitoring is enabled
     * 
     * @return true if monitoring is enabled
     */
    bool isMonitoringEnabled() const { return monitoringEnabled; }
    
    /**
     * Check if threading is enabled
     * 
     * @return true if threading is enabled
     */
    bool isThreadingEnabled() const { return threadingEnabled; }
    
    /**
     * Check if profiling is enabled
     * 
     * @return true if profiling is enabled
     */
    bool isProfilingEnabled() const { return profilingEnabled; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<Brain> pBrain;
    AdvancedConfig advancedConfig;
    
    bool monitoringEnabled;
    bool threadingEnabled;
    bool profilingEnabled;
    
    mutable std::mutex statsMutex;
    mutable std::mutex callbackMutex;
    
    std::atomic<SimulationStep> currentStep;
    std::atomic<double> simulationSpeed;
    std::chrono::steady_clock::time_point startTime;
    
    uint64_t totalMemoryAllocations;
    uint64_t totalMemoryDeallocations;
    size_t maxMemoryUsage;
    
    std::vector<std::function<void(const BrainEventData&)>> eventCallbacks;
    uint32_t nextCallbackId;
    bool isPausedState;
    std::string pauseReason;
    
    /**
     * Update performance statistics
     */
    void updatePerformanceStats();
    
    /**
     * Collect memory usage information
     */
    void collectMemoryUsage();
    
    /**
     * Invoke event callbacks
     * 
     * @param event Event to propagate
     */
    void invokeEventCallbacks(const BrainEvent& event);
    
    /**
     * Setup default advanced configuration
     */
    void setupDefaultAdvancedConfig();
};

} // namespace nlm
