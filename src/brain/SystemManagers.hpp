#pragma once

/**
 * SystemManagers.hpp - Optimized System Management Framework
 * 
 * Provides comprehensive system management with performance optimization
 * for NLM brain initialization and runtime operations.
 * 
 * Features:
 * 1. Efficient system access with hash-based lookups
 * 2. Optimized initialization management
 * 3. Performance monitoring integration
 * 4. Memory-efficient system state management
 * 5. Parallel system processing support
 */

#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <thread>
#include <atomic>
#include <algorithm>
#include <functional>

namespace nlm {

// Forward declarations
class Config;
class Brain;
class PerformanceMonitor;
class EventDrivenProcessor;
class ParallelNeuralProcessor;

/**
 * SystemState - Represents the current state of a system component
 * 
 * Efficiently tracks system component state with optimized memory usage
 * and fast access patterns.
 */
struct SystemState {
    // System identification
    uint64_t systemId;
    std::string name;
    std::string type;
    
    // Performance metrics
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    double diskIO = 0.0;
    
    // System health
    bool isHealthy = true;
    std::string healthStatus;
    std::chrono::system_clock::time_point lastCheck;
    
    // Configuration
    std::unordered_map<std::string, std::string> config;
    
    // Performance tracking
    struct PerformanceMetrics {
        double initializationTime = 0.0;
        double uptime = 0.0;
        double lastPerformanceCheck = 0.0;
        size_t operationCount = 0;
        double averageResponseTime = 0.0;
        double throughput = 0.0;
        
        PerformanceMetrics() : lastPerformanceCheck(0.0) {}
    } perfMetrics;
    
    SystemState() : systemId(0), isHealthy(true), lastPerformanceCheck(std::chrono::system_clock::now()) {}
};

/**
 * SystemManager - Manages system components with optimized access
 * 
 * Provides efficient management of system components with:
 * - Hash-based O(1) lookups
 * - Thread-safe operations
 * - Performance monitoring
 * - Automatic health checking
 */
class SystemManager {
public:
    /**
     * Create system manager with configuration
     */
    explicit SystemManager(std::shared_ptr<Config> config);
    
    ~SystemManager();
    
    // Disable copying
    SystemManager(const SystemManager&) = delete;
    SystemManager& operator=(const SystemManager&) = delete;
    
    /**
     * Register a system component
     * 
     * @param state System state to register
     * @return System ID assigned to component
     */
    uint64_t registerSystem(const SystemState& state);
    
    /**
     * Unregister a system component
     * 
     * @param systemId System ID to unregister
     * @return true if successfully unregistered
     */
    bool unregisterSystem(uint64_t systemId);
    
    /**
     * Get system state by ID
     * 
     * @param systemId System ID to look up
     * @return System state, or nullptr if not found
     */
    SystemState* getSystem(uint64_t systemId);
    
    /**
     * Get system state by ID (const version)
     */
    const SystemState* getSystem(uint64_t systemId) const;
    
    /**
     * Get system state by name
     */
    SystemState* getSystemByName(const std::string& name);
    
    /**
     * Get all systems of a specific type
     */
    std::vector<SystemState*> getSystemsByType(const std::string& type);
    
    /**
     * Update system state
     * 
     * @param systemId System ID to update
     * @param state New state
     * @return true if successfully updated
     */
    bool updateSystem(uint64_t systemId, const SystemState& state);
    
    /**
     * Remove unhealthy systems
     */
    void cleanupUnhealthySystems();
    
    /**
     * Get system count
     */
    size_t getSystemCount() const;
    
    /**
     * Get system count by type
     */
    size_t getSystemCountByType(const std::string& type) const;
    
    /**
     * Check if system exists
     */
    bool hasSystem(uint64_t systemId) const;
    
    /**
     * Check if system exists by name
     */
    bool hasSystemByName(const std::string& name) const;
    
    /**
     * Get all system IDs
     */
    std::vector<uint64_t> getAllSystemIds() const;
    
    /**
     * Initialize all systems
     */
    bool initializeAllSystems();
    
    /**
     * Shutdown all systems
     */
    void shutdownAllSystems();
    
    /**
     * Perform health check on all systems
     */
    void performHealthCheckAllSystems();
    
    /**
     * Get performance summary
     */
    std::string getPerformanceSummary() const;
    
    /**
     * Get memory usage statistics
     */
    size_t getTotalMemoryUsage() const;
    
    /**
     * Enable/disable performance monitoring
     */
    void enablePerformanceMonitoring(bool enable);
    
    /**
     * Set monitoring interval
     */
    void setMonitoringInterval(double interval);
    
    /**
     * Get system by ID with thread safety
     */
    SystemState* getSystemThreadSafe(uint64_t systemId) {
        std::shared_lock<std::shared_mutex> lock(systemMutex_);
        auto it = systemsById_.find(systemId);
        return it != systemsById_.end() ? it->second : nullptr;
    }
    
    /**
     * Update system with thread safety
     */
    bool updateSystemThreadSafe(uint64_t systemId, const SystemState& state) {
        std::unique_lock<std::shared_mutex> lock(systemMutex_);
        auto it = systemsById_.find(systemId);
        if (it == systemsById_.end()) {
            return false;
        }
        *it->second = state;
        return true;
    }
    
    /**
     * Register system with thread safety
     */
    uint64_t registerSystemThreadSafe(const SystemState& state) {
        std::unique_lock<std::shared_mutex> lock(systemMutex_);
        uint64_t systemId = nextSystemId_++;
        auto systemPtr = std::make_unique<SystemState>(state);
        systemPtr->systemId = systemId;
        
        systemsById_[systemId] = systemPtr.get();
        systemsByName_[state.name] = systemPtr.get();
        systemsByType_[state.type].push_back(systemPtr.get());
        
        systemObjects_.push_back(std::move(systemPtr));
        
        return systemId;
    }

private:
    /**
     * Generate unique system ID
     */
    uint64_t generateSystemId() {
        return ++nextSystemId_;
    }
    
    /**
     * Remove system from all indexes
     */
    void removeSystemFromIndexes(uint64_t systemId, const std::string& name, const std::string& type);
    
    /**
     * Perform health check on a single system
     */
    void performHealthCheck(SystemState* system);
    
    /**
     * Update performance metrics for a system
     */
    void updatePerformanceMetrics(SystemState* system);
    
    /**
     * Calculate system memory usage
     */
    size_t calculateSystemMemoryUsage(const SystemState& system) const;

private:
    // System storage
    std::vector<std::unique_ptr<SystemState>> systemObjects_;
    std::unordered_map<uint64_t, SystemState*> systemsById_;
    std::unordered_map<std::string, SystemState*> systemsByName_;
    std::unordered_map<std::string, std::vector<SystemState*>> systemsByType_;
    
    // Thread safety
    mutable std::shared_mutex systemMutex_;
    
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Performance monitoring
    bool performanceMonitoringEnabled_ = true;
    double monitoringInterval_ = 1.0;  // seconds
    std::chrono::system_clock::time_point lastMonitoringUpdate_;
    
    // Statistics
    size_t totalOperations_ = 0;
    double totalResponseTime_ = 0.0;
    size_t unhealthySystems_ = 0;
    
    // System ID generation
    uint64_t nextSystemId_ = 1;
};

/**
 * SystemInitializationManager - Manages system initialization sequence
 * 
 * Coordinates the initialization of multiple system components
 * with dependency management and error handling.
 */
class SystemInitializationManager {
public:
    /**
     * Create initialization manager
     */
    SystemInitializationManager();
    
    ~SystemInitializationManager();
    
    // Disable copying
    SystemInitializationManager(const SystemInitializationManager&) = delete;
    SystemInitializationManager& operator=(const SystemInitializationManager&) = delete;
    
    /**
     * Add initialization task
     */
    void addInitializationTask(const std::string& name, 
                              std::function<bool()> task,
                              int priority = 0,
                              const std::vector<std::string>& dependencies = {});
    
    /**
     * Initialize all systems
     */
    bool initializeAllSystems();
    
    /**
     * Get initialization status
     */
    bool isInitialized(const std::string& name) const;
    
    /**
     * Get initialization error
     */
    std::string getInitializationError(const std::string& name) const;
    
    /**
     * Get initialization progress
     */
    double getInitializationProgress() const;
    
    /**
     * Cancel initialization
     */
    void cancelInitialization();
    
    /**
     * Check if initialization is in progress
     */
    bool isInitializing() const;
    
    /**
     * Get initialization statistics
     */
    std::string getInitializationStatistics() const;

private:
    /**
     * Execute all tasks in dependency order
     */
    bool executeInitializationTasks();
    
    /**
     * Check if all dependencies for a task are satisfied
     */
    bool checkDependencies(const std::string& name) const;
    
    /**
     * Get tasks that can be executed next
     */
    std::vector<std::string> getReadyTasks() const;
    
    /**
     * Execute a single task
     */
    bool executeTask(const std::string& name);
    
    /**
     * Get task by name
     */
    InitializationTask* getTask(const std::string& name);

private:
    // Initialization tasks
    std::vector<InitializationTask> tasks_;
    
    // Task execution state
    std::unordered_map<std::string, bool> taskInitialized_;
    std::unordered_map<std::string, std::string> taskErrors_;
    std::unordered_map<std::string, std::vector<std::string>> taskDependencies_;
    
    // Execution state
    bool initializing_ = false;
    bool cancelled_ = false;
    size_t completedTasks_ = 0;
    size_t totalTasks_ = 0;
    
    mutable std::mutex tasksMutex_;
};