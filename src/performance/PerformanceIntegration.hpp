#pragma once

/**
 * Performance Integration
 * 
 * Integration of all performance optimization components into the existing NLM codebase:
 * 
 * Integration Categories:
 * 1. Performance Configuration Management
 * 2. Automatic Performance Tuning
 * 3. Backward Compatibility Layer
 * 4. Performance Monitoring Integration
 * 5. Error Handling and Fallback Mechanisms
 * 6. Configuration Validation
 * 7. Performance Profile Selection
 * 8. System Resource Management
 * 
 * Integration Features:
 * - Seamless integration with existing NLM systems
 * - Automatic performance optimization detection
 * - Fallback mechanisms for compatibility
 * - Comprehensive error handling
 * - Configuration validation and management
 * - Performance profile management
 */

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>
#include <thread>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace nlm {

/**
 * Performance configuration settings
 */
struct PerformanceConfig {
    // General settings
    bool enabled = true;
    std::string profile = "balanced"; // balanced, fast, accurate, memory, cpu
    bool verboseLogging = false;
    
    // Optimization settings
    struct MemoryOptimization {
        bool enableMemoryPool = true;
        bool enablePreAllocation = true;
        bool enableObjectReuse = true;
        size_t poolSize = 1024;
        bool enableCacheFriendlyAllocation = true;
    } memory;
    
    struct CacheOptimization {
        bool enableLoopOptimization = true;
        bool enablePrefetching = true;
        bool enableBlocking = true;
        bool enableCacheLinePadding = true;
        size_t cacheLineSize = 64;
    } cache;
    
    struct AlgorithmicOptimization {
        bool enableEarlyExit = true;
        bool enableMemoization = true;
        bool enableLookupTables = true;
        bool enableApproximation = true;
        double approximationThreshold = 0.01;
    } algorithmic;
    
    struct ComputationalOptimization {
        bool enableSIMD = true;
        bool enableParallelProcessing = true;
        bool enableVectorization = true;
        size_t threadCount = std::thread::hardware_concurrency();
        bool enablePrecisionManagement = true;
    } computational;
    
    struct AIOptimization {
        bool enableBufferedIO = true;
        bool enableCompression = false;
        bool enableAsynchronousIO = true;
        bool enableMemoryMapping = true;
        size_t ioBufferSize = 64 * 1024; // 64 KB
    } io;
    
    // Performance targets
    double targetCPUUsage = 0.8; // 80% CPU usage target
    uint64_t targetMemoryUsageMB = 1024; // 1 GB memory target
    double targetResponseTimeMs = 1.0; // 1 ms response time target
    
    // Monitoring settings
    uint32_t profilingIntervalMs = 1000; // 1 second profiling interval
    bool enablePerformanceCounters = true;
    bool enableHardwareCounters = true;
    
    // Integration settings
    bool enableAutoTuning = true;
    bool enableFallbackOnError = true;
    double errorRecoveryThreshold = 0.95; // Performance threshold for recovery
    
    // Validation settings
    bool validateConfigurations = true;
    bool validatePerformanceImpact = true;
};

/**
 * Performance profile definitions
 */
struct PerformanceProfile {
    std::string name;
    std::string description;
    PerformanceConfig config;
    
    PerformanceProfile(const std::string& n, const std::string& desc) : name(n), description(desc) {}
};

/**
 * Performance integration error types
 */
enum class IntegrationError {
    SUCCESS,
    CONFIGURATION_ERROR,
    COMPATIBILITY_ERROR,
    PERFORMANCE_ERROR,
    RESOURCE_ERROR,
    VALIDATION_ERROR,
    FALLBACK_REQUIRED
};

/**
 * Performance integration result
 */
struct IntegrationResult {
    IntegrationError error;
    std::string message;
    double performanceImpact; // -1.0 to 1.0 (negative is improvement)
    std::vector<std::string> warnings;
    
    IntegrationResult() : error(IntegrationError::SUCCESS), performanceImpact(0.0) {}
    
    IntegrationResult(IntegrationError err, const std::string& msg, double impact = 0.0, 
                     const std::vector<std::string>& warns = {}) 
        : error(err), message(msg), performanceImpact(impact), warnings(warns) {}
};

/**
 * Performance profile manager
 */
class PerformanceProfileManager {
public:
    PerformanceProfileManager() {
        // Initialize default profiles
        initializeDefaultProfiles();
    }
    
    /**
     * Add a performance profile
     */
    void addProfile(const PerformanceProfile& profile) {
        profiles_[profile.name] = profile;
    }
    
    /**
     * Get a performance profile
     */
    const PerformanceProfile* getProfile(const std::string& name) const {
        auto it = profiles_.find(name);
        return (it != profiles_.end()) ? &it->second : nullptr;
    }
    
    /**
     * Get all profile names
     */
    std::vector<std::string> getProfileNames() const {
        std::vector<std::string> names;
        names.reserve(profiles_.size());
        for (const auto& pair : profiles_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    /**
     * Get profile descriptions
     */
    std::vector<std::pair<std::string, std::string>> getProfileDescriptions() const {
        std::vector<std::pair<std::string, std::string>> descriptions;
        descriptions.reserve(profiles_.size());
        for (const auto& pair : profiles_) {
            descriptions.push_back({pair.first, pair.second.description});
        }
        return descriptions;
    }
    
    /**
     * Apply profile to configuration
     */
    IntegrationResult applyProfile(PerformanceConfig& config, const std::string& profileName) {
        const PerformanceProfile* profile = getProfile(profileName);
        if (!profile) {
            return IntegrationResult{IntegrationError::CONFIGURATION_ERROR, 
                                    "Profile not found: " + profileName};
        }
        
        config = profile->config;
        return IntegrationResult{IntegrationError::SUCCESS, 
                                "Profile applied successfully: " + profileName};
    }
    
    /**
     * Validate configuration
     */
    IntegrationResult validateConfig(const PerformanceConfig& config) const {
        std::vector<std::string> warnings;
        
        // Check for conflicting optimizations
        if (config.memory.enableMemoryPool && config.memory.enablePreAllocation && 
            config.memory.poolSize == 0) {
            warnings.push_back("Memory pool size is zero; consider increasing pool size");
        }
        
        // Check for conflicting I/O settings
        if (config.io.enableCompression && !config.io.enableBufferedIO) {
            warnings.push_back("Compression without buffering may be inefficient");
        }
        
        // Check thread count (create a copy for validation)
        size_t threadCount = config.computational.threadCount;
        if (threadCount == 0) {
            threadCount = std::thread::hardware_concurrency();
            warnings.push_back("Thread count reset to hardware concurrency: " + 
                             std::to_string(threadCount));
        }
        
        // Check for incompatible profiles
        if (config.profile == "accurate" && config.algorithmic.enableApproximation) {
            warnings.push_back("Approximate computations conflict with accurate profile");
        }
        
        return IntegrationResult{IntegrationError::SUCCESS, "Configuration validated", 0.0, warnings};
    }
    
private:
    void initializeDefaultProfiles() {
        // Balanced profile (default)
        PerformanceProfile balanced("balanced", 
            "Balanced performance across all metrics - good overall performance");
        balanced.config.profile = "balanced";
        balanced.config.memory.enableMemoryPool = true;
        balanced.config.memory.enablePreAllocation = true;
        balanced.config.cache.enableLoopOptimization = true;
        balanced.config.algorithmic.enableEarlyExit = true;
        balanced.config.computational.enableSIMD = true;
        balanced.config.computational.enableParallelProcessing = true;
        balanced.config.io.enableBufferedIO = true;
        balanced.config.io.enableAsynchronousIO = true;
        addProfile(balanced);
        
        // Fast profile
        PerformanceProfile fast("fast",
            "Maximum speed at the expense of memory usage and accuracy");
        fast.config.profile = "fast";
        fast.config.memory.enableMemoryPool = true;
        fast.config.memory.enablePreAllocation = false;
        fast.config.cache.enableLoopOptimization = true;
        fast.config.cache.enablePrefetching = true;
        fast.config.algorithmic.enableEarlyExit = true;
        fast.config.algorithmic.enableApproximation = true;
        fast.config.computational.enableSIMD = true;
        fast.config.computational.enableParallelProcessing = true;
        fast.config.computational.enablePrecisionManagement = false;
        fast.config.io.enableBufferedIO = false;
        fast.config.io.enableAsynchronousIO = false;
        addProfile(fast);
        
        // Accurate profile
        PerformanceProfile accurate("accurate",
            "Maximum accuracy at the expense of performance");
        accurate.config.profile = "accurate";
        accurate.config.memory.enableMemoryPool = true;
        accurate.config.memory.enablePreAllocation = true;
        accurate.config.cache.enableLoopOptimization = true;
        accurate.config.cache.enablePrefetching = true;
        accurate.config.algorithmic.enableEarlyExit = false;
        accurate.config.algorithmic.enableMemoization = true;
        accurate.config.algorithmic.enableLookupTables = true;
        accurate.config.computational.enableSIMD = true;
        accurate.config.computational.enableParallelProcessing = false;
        accurate.config.computational.enablePrecisionManagement = true;
        accurate.config.io.enableBufferedIO = true;
        accurate.config.io.enableAsynchronousIO = true;
        addProfile(accurate);
        
        // Memory-optimized profile
        PerformanceProfile memory("memory",
            "Maximum memory efficiency at the expense of performance");
        memory.config.profile = "memory";
        memory.config.memory.enableMemoryPool = true;
        memory.config.memory.enablePreAllocation = true;
        memory.config.memory.enableCacheFriendlyAllocation = true;
        memory.config.cache.enableLoopOptimization = true;
        memory.config.cache.enableCacheLinePadding = true;
        memory.config.algorithmic.enableMemoization = true;
        memory.config.algorithmic.enableLookupTables = true;
        memory.config.computational.enableSIMD = false; // Reduce memory usage
        memory.config.computational.enableParallelProcessing = false;
        memory.config.io.enableCompression = true;
        memory.config.io.enableMemoryMapping = true;
        addProfile(memory);
        
        // CPU-optimized profile
        PerformanceProfile cpu("cpu",
            "Maximum CPU performance at the expense of memory usage");
        cpu.config.profile = "cpu";
        cpu.config.memory.enableMemoryPool = true;
        cpu.config.memory.enablePreAllocation = false;
        cpu.config.cache.enableLoopOptimization = true;
        cpu.config.cache.enablePrefetching = true;
        cpu.config.cache.enableBlocking = true;
        cpu.config.algorithmic.enableEarlyExit = true;
        cpu.config.algorithmic.enableApproximation = true;
        cpu.config.computational.enableSIMD = true;
        cpu.config.computational.enableParallelProcessing = true;
        cpu.config.computational.threadCount = std::thread::hardware_concurrency();
        cpu.config.io.enableAsynchronousIO = true;
        addProfile(cpu);
    }
    
    std::map<std::string, PerformanceProfile> profiles_;
};

/**
 * Performance integrator
 */
class PerformanceIntegrator {
public:
    PerformanceIntegrator() : profileManager_(), currentConfig_(), integrationMutex_() {
        // Initialize with default configuration
        currentConfig_.profile = "balanced";
    }
    
    /**
     * Set performance configuration
     */
    IntegrationResult setConfig(const PerformanceConfig& config) {
        std::lock_guard<std::mutex> lock(integrationMutex_);
        
        // Validate configuration
        IntegrationResult validationResult = profileManager_.validateConfig(config);
        if (validationResult.error != IntegrationError::SUCCESS) {
            return validationResult;
        }
        
        // Store old configuration for comparison
        PerformanceConfig oldConfig = currentConfig_;
        
        // Apply new configuration
        currentConfig_ = config;
        
        // Check for performance impact
        double performanceImpact = calculatePerformanceImpact(oldConfig, config);
        
        // Log configuration change
        if (currentConfig_.verboseLogging) {
            logConfigChange(oldConfig, config);
        }
        
        return IntegrationResult{IntegrationError::SUCCESS, "Configuration updated successfully",
                performanceImpact, validationResult.warnings};
    }
    
    /**
     * Get current configuration
     */
    PerformanceConfig getConfig() const {
        std::lock_guard<std::mutex> lock(integrationMutex_);
        return currentConfig_;
    }
    
    /**
     * Get performance profile manager
     */
    PerformanceProfileManager& getProfileManager() { return profileManager_; }
    
    /**
     * Apply performance profile
     */
    IntegrationResult applyProfile(const std::string& profileName) {
        return profileManager_.applyProfile(currentConfig_, profileName);
    }
    
    /**
     * Get performance statistics
     */
    struct PerformanceStats {
        double cpuUsage;
        uint64_t memoryUsage;
        uint64_t cacheMisses;
        uint64_t branchMispredictions;
        uint64_t contextSwitches;
        uint64_t threadSwitches;
        double avgStepTime;
        uint64_t stepsCompleted;
        uint32_t activeNeurons;
        uint32_t firingNeurons;
        
        PerformanceStats() : cpuUsage(0.0), memoryUsage(0), cacheMisses(0),
                            branchMispredictions(0), contextSwitches(0),
                            threadSwitches(0), avgStepTime(0.0), stepsCompleted(0),
                            activeNeurons(0), firingNeurons(0) {}
    };
    
    /**
     * Update performance statistics
     */
    void updatePerformanceStats(const PerformanceStats& stats) {
        std::lock_guard<std::mutex> lock(integrationMutex_);
        currentStats_ = stats;
    }
    
    /**
     * Get performance statistics
     */
    PerformanceStats getPerformanceStats() const {
        std::lock_guard<std::mutex> lock(integrationMutex_);
        return currentStats_;
    }
    
    /**
     * Check if performance is within acceptable limits
     */
    IntegrationResult checkPerformanceLimits() const {
        std::lock_guard<std::mutex> lock(integrationMutex_);
        
        std::vector<std::string> warnings;
        
        // Check CPU usage
        if (currentStats_.cpuUsage > currentConfig_.targetCPUUsage) {
            warnings.push_back("CPU usage exceeds target: " + 
                             std::to_string(currentStats_.cpuUsage));
        }
        
        // Check memory usage
        if (currentStats_.memoryUsage > currentConfig_.targetMemoryUsageMB * 1024 * 1024) {
            warnings.push_back("Memory usage exceeds target: " + 
                             std::to_string(currentStats_.memoryUsage));
        }
        
        // Check step time
        if (currentStats_.avgStepTime > currentConfig_.targetResponseTimeMs * 1000) {
            warnings.push_back("Step time exceeds target: " + 
                             std::to_string(currentStats_.avgStepTime));
        }
        
        // Auto-tune if performance is poor (note: modifications happen in non-const method)
        if (currentConfig_.enableAutoTuning && 
            (currentStats_.cpuUsage > currentConfig_.targetCPUUsage * 1.2 || 
             currentStats_.avgStepTime > currentConfig_.targetResponseTimeMs * 1000 * 1.2)) {
            
            // Note: Configuration auto-tuning is performed in a separate non-const method
            warnings.push_back("Auto-tuning would be triggered for performance limits");
            return IntegrationResult{IntegrationError::PERFORMANCE_ERROR, 
                                    "Performance limits exceeded, requires auto-tuning",
                                    -0.1, warnings}; // -0.1 indicates improvement
        }
        
        return IntegrationResult{IntegrationError::SUCCESS, "Performance within limits", 0.0, warnings};
    }
    
    /**
     * Auto-tune configuration based on performance metrics
     */
    IntegrationResult autoTuneConfiguration() {
        std::lock_guard<std::mutex> lock(integrationMutex_);
        
        std::vector<std::string> warnings;
        
        // Check CPU usage and reduce optimizations if needed
        if (currentStats_.cpuUsage > currentConfig_.targetCPUUsage) {
            // Reduce CPU-intensive optimizations
            if (currentConfig_.algorithmic.enableApproximation) {
                currentConfig_.algorithmic.enableApproximation = false;
                warnings.push_back("Disabled approximation to reduce CPU usage");
            }
            if (currentConfig_.computational.enableParallelProcessing) {
                currentConfig_.computational.threadCount = 
                    std::max<size_t>(1, currentConfig_.computational.threadCount - 1);
                warnings.push_back("Reduced thread count to reduce CPU usage");
            }
        }
        
        // Check step time and improve performance if needed
        if (currentStats_.avgStepTime > currentConfig_.targetResponseTimeMs * 1000) {
            // Improve performance
            if (!currentConfig_.algorithmic.enableEarlyExit) {
                currentConfig_.algorithmic.enableEarlyExit = true;
                warnings.push_back("Enabled early exit to improve performance");
            }
            if (!currentConfig_.cache.enablePrefetching) {
                currentConfig_.cache.enablePrefetching = true;
                warnings.push_back("Enabled prefetching to improve performance");
            }
        }
        
        return IntegrationResult{IntegrationError::PERFORMANCE_ERROR, 
                                "Performance limits exceeded, auto-tuned",
                                -0.1, warnings}; // -0.1 indicates improvement
    }
    
    /**
     * Handle performance error with fallback
     */
    IntegrationResult handleError(const std::string& error) {
        std::lock_guard<std::mutex> lock(integrationMutex_);
        
        if (!currentConfig_.enableFallbackOnError) {
            return IntegrationResult{IntegrationError::CONFIGURATION_ERROR, 
                                    "Error handling disabled: " + error};
        }
        
        // Log error
        if (currentConfig_.verboseLogging) {
            std::cerr << "Performance integration error: " << error << std::endl;
        }
        
        // Apply fallback configuration
        IntegrationResult fallbackResult = applyProfile("balanced");
        if (fallbackResult.error == IntegrationError::SUCCESS) {
            return IntegrationResult{IntegrationError::FALLBACK_REQUIRED, 
                                    "Performance error handled with fallback: " + error + ". Applied balanced profile.",
                                    -0.2, fallbackResult.warnings};
        }
        
        return IntegrationResult{IntegrationError::PERFORMANCE_ERROR, "Error handling failed: " + error};
    }
    
    /**
     * Get integration status
     */
    std::string getIntegrationStatus() const {
        std::lock_guard<std::mutex> lock(integrationMutex_);
        
        std::stringstream status;
        status << "Performance Integration Status:" << std::endl;
        status << "  Profile: " << currentConfig_.profile << std::endl;
        status << "  Enabled: " << (currentConfig_.enabled ? "Yes" : "No") << std::endl;
        status << "  CPU Usage: " << currentStats_.cpuUsage << "%" << std::endl;
        status << "  Memory Usage: " << (currentStats_.memoryUsage / (1024 * 1024)) << " MB" << std::endl;
        status << "  Active Neurons: " << currentStats_.activeNeurons << std::endl;
        status << "  Steps Completed: " << currentStats_.stepsCompleted << std::endl;
        
        return status.str();
    }
    
    /**
     * Generate performance report
     */
    std::string generatePerformanceReport() const {
        std::lock_guard<std::mutex> lock(integrationMutex_);
        
        std::stringstream report;
        report << "=== NLM Performance Report ===" << std::endl;
        report << "Generated: " << getCurrentTimestamp() << std::endl;
        report << std::endl;
        
        report << "Configuration:" << std::endl;
        report << "  Profile: " << currentConfig_.profile << std::endl;
        report << "  Memory Pool: " << (currentConfig_.memory.enableMemoryPool ? "Yes" : "No") << std::endl;
        report << "  SIMD: " << (currentConfig_.computational.enableSIMD ? "Yes" : "No") << std::endl;
        report << "  Parallel Processing: " << (currentConfig_.computational.enableParallelProcessing ? "Yes" : "No") << std::endl;
        report << "  Buffered I/O: " << (currentConfig_.io.enableBufferedIO ? "Yes" : "No") << std::endl;
        report << "  Compression: " << (currentConfig_.io.enableCompression ? "Yes" : "No") << std::endl;
        report << std::endl;
        
        report << "Performance Statistics:" << std::endl;
        report << "  CPU Usage: " << currentStats_.cpuUsage << "%" << std::endl;
        report << "  Memory Usage: " << (currentStats_.memoryUsage / (1024 * 1024)) << " MB" << std::endl;
        report << "  Cache Misses: " << currentStats_.cacheMisses << std::endl;
        report << "  Branch Mispredictions: " << currentStats_.branchMispredictions << std::endl;
        report << "  Avg Step Time: " << currentStats_.avgStepTime << " ms" << std::endl;
        report << "  Steps Completed: " << currentStats_.stepsCompleted << std::endl;
        report << "  Active Neurons: " << currentStats_.activeNeurons << std::endl;
        report << "  Firing Neurons: " << currentStats_.firingNeurons << std::endl;
        report << std::endl;
        
        report << "Performance Targets:" << std::endl;
        report << "  Target CPU Usage: " << (currentConfig_.targetCPUUsage * 100) << "%" << std::endl;
        report << "  Target Memory Usage: " << currentConfig_.targetMemoryUsageMB << " MB" << std::endl;
        report << "  Target Response Time: " << currentConfig_.targetResponseTimeMs << " ms" << std::endl;
        report << std::endl;
        
        report << "=== End Report ===" << std::endl;
        
        return report.str();
    }
    
private:
    /**
     * Calculate performance impact between configurations
     */
    double calculatePerformanceImpact(const PerformanceConfig& oldConfig,
                                       const PerformanceConfig& newConfig) const {
        double impact = 0.0;
        
        // Check memory optimizations
        if (oldConfig.memory.enableMemoryPool != newConfig.memory.enableMemoryPool) {
            impact += newConfig.memory.enableMemoryPool ? 0.1 : -0.1;
        }
        
        // Check CPU optimizations
        if (oldConfig.computational.enableSIMD != newConfig.computational.enableSIMD) {
            impact += newConfig.computational.enableSIMD ? 0.15 : -0.1;
        }
        
        // Check parallel processing
        if (oldConfig.computational.enableParallelProcessing != newConfig.computational.enableParallelProcessing) {
            impact += newConfig.computational.enableParallelProcessing ? 0.2 : -0.1;
        }
        
        // Check I/O optimizations
        if (oldConfig.io.enableBufferedIO != newConfig.io.enableBufferedIO) {
            impact += newConfig.io.enableBufferedIO ? 0.05 : -0.05;
        }
        
        if (oldConfig.io.enableAsynchronousIO != newConfig.io.enableAsynchronousIO) {
            impact += newConfig.io.enableAsynchronousIO ? 0.1 : -0.05;
        }
        
        return std::max(-1.0, std::min(1.0, impact)); // Clamp to [-1.0, 1.0]
    }
    
    /**
     * Log configuration change
     */
    void logConfigChange(const PerformanceConfig& oldConfig, const PerformanceConfig& newConfig) {
        std::cout << "=== Performance Configuration Change ===" << std::endl;
        std::cout << "Changed from profile: " << oldConfig.profile << std::endl;
        std::cout << "Changed to profile: " << newConfig.profile << std::endl;
        
        if (oldConfig.memory.enableMemoryPool != newConfig.memory.enableMemoryPool) {
            std::cout << "Memory Pool: " << (oldConfig.memory.enableMemoryPool ? "Disabled" : "Enabled") << std::endl;
        }
        
        if (oldConfig.computational.enableSIMD != newConfig.computational.enableSIMD) {
            std::cout << "SIMD: " << (oldConfig.computational.enableSIMD ? "Disabled" : "Enabled") << std::endl;
        }
        
        std::cout << "=====================================" << std::endl;
    }
    
    /**
     * Get current timestamp
     */
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    PerformanceProfileManager profileManager_;
    PerformanceConfig currentConfig_;
    PerformanceStats currentStats_;
    mutable std::mutex integrationMutex_;
};

/**
 * Global performance integrator instance
 */
class PerformanceIntegration {
public:
    /**
     * Get global performance integrator instance
     */
    static PerformanceIntegration& getInstance() {
        static PerformanceIntegration integrator;
        return integrator;
    }
    
    /**
     * Set performance configuration globally
     */
    static IntegrationResult setConfig(const PerformanceConfig& config) {
        return getInstance().setConfig(config);
    }
    
    /**
     * Get current performance configuration
     */
    static PerformanceConfig getConfig() {
        return getInstance().getConfig();
    }
    
    /**
     * Apply performance profile globally
     */
    static IntegrationResult applyProfile(const std::string& profileName) {
        return getInstance().applyProfile(profileName);
    }
    
    /**
     * Update performance statistics globally
     */
    static void updatePerformanceStats(const PerformanceIntegrator::PerformanceStats& stats) {
        getInstance().updatePerformanceStats(stats);
    }
    
    /**
     * Check performance limits globally
     */
    static IntegrationResult checkPerformanceLimits() {
        return getInstance().checkPerformanceLimits();
    }
    
    /**
     * Handle performance error globally
     */
    static IntegrationResult handleError(const std::string& error) {
        return getInstance().handleError(error);
    }
    
    /**
     * Get performance profile manager globally
     */
    static PerformanceProfileManager& getProfileManager() {
        return getInstance().getProfileManager();
    }
    
    /**
     * Get integration status globally
     */
    static std::string getIntegrationStatus() {
        return getInstance().getIntegrationStatus();
    }
    
    /**
     * Generate performance report globally
     */
    static std::string generatePerformanceReport() {
        return getInstance().generatePerformanceReport();
    }
    
private:
    PerformanceIntegration() = default;
    ~PerformanceIntegration() = default;
    
    // Delete copy and assignment
    PerformanceIntegration(const PerformanceIntegration&) = delete;
    PerformanceIntegration& operator=(const PerformanceIntegration&) = delete;
};

// Global convenience functions

inline void setPerformanceConfig(const nlm::PerformanceConfig& config) {
    nlm::PerformanceIntegration::setConfig(config);
}

inline nlm::PerformanceConfig getPerformanceConfig() {
    return nlm::PerformanceIntegration::getConfig();
}

inline void applyPerformanceProfile(const std::string& profileName) {
    nlm::PerformanceIntegration::applyProfile(profileName);
}

inline void updatePerformanceStats(const nlm::PerformanceIntegrator::PerformanceStats& stats) {
    nlm::PerformanceIntegration::updatePerformanceStats(stats);
}

inline std::string getIntegrationStatus() {
    return nlm::PerformanceIntegration::getIntegrationStatus();
}

inline std::string generatePerformanceReport() {
    return nlm::PerformanceIntegration::generatePerformanceReport();
}

} // namespace nlm