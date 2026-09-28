// Comprehensive integration example for NLM enhanced logging system
// This file demonstrates how all logging enhancements integrate with the existing NLM codebase

#pragma once

#include "core/Logger/Logger.hpp"
#include "core/Logging/LoggerManager.hpp"
#include "core/Debug/DebugConsole.hpp"
#include "core/Debug/SystemMonitor.hpp"
#include "core/Utilities/PerformanceProfiler.hpp"
#include "core/Utilities/MemoryTracker.hpp"

// Integration helper functions
namespace nlm::integration {

// Initialize the enhanced logging system
void initializeEnhancedLogging() {
    // Get logger manager instance
    auto& loggerManager = LoggerManager::getInstance();
    
    // Configure global logging settings
    loggerManager.setGlobalConfig("json", true, 10000);
    loggerManager.setMinimumLogLevel(LogLevel::Info);
    
    // Add performance logging thresholds
    loggerManager.setPerformanceThreshold("neuron_step", 100.0); // ms
    loggerManager.setMemoryThreshold(512 * 1024 * 1024); // 512MB
    loggerManager.setCpuThreshold(80.0); // 80%
    
    // Set up log file rotation
    loggerManager.setLogFileRotation("logs/nlm_debug", 10 * 1024 * 1024, 5); // 10MB, keep 5 files
    
    // Start system monitoring
    loggerManager.startSystemMonitoring();
    
    // Get debug console
    auto debugConsole = DebugConsole::getInstance();
    debugConsole->show();
    
    // Integrate all components
    loggerManager.setDebugConsole(debugConsole);
    
    // Get system monitor
    auto systemMonitor = SystemMonitor::getInstance();
    systemMonitor->integrateWithLogger(loggerManager);
    systemMonitor->integrateWithDebugConsole(debugConsole);
    
    // Get performance profiler
    auto performanceProfiler = PerformanceProfiler::getInstance();
    performanceProfiler->integrateWithLogger(loggerManager);
    performanceProfiler->integrateWithDebugConsole(debugConsole);
    performanceProfiler->integrateWithSystemMonitor(systemMonitor);
    
    // Get memory tracker
    auto memoryTracker = MemoryTracker::getInstance();
    memoryTracker->integrateWithLogger(loggerManager);
    memoryTracker->integrateWithDebugConsole(debugConsole);
    memoryTracker->integrateWithSystemMonitor(systemMonitor);
    
    // Log initialization
    Logger::getGlobal().info("Enhanced logging system initialized");
    debugConsole->logMessage("Debug console ready for NLM simulation monitoring", LogLevel::Info);
}

// Integration for brain simulation components
void integrateBrainComponents() {
    auto& loggerManager = LoggerManager::getInstance();
    
    // Get existing brain logger
    auto brainLogger = loggerManager.getLogger("brain");
    brainLogger->setLevel(LogLevel::Debug);
    brainLogger->setFormat("json");
    
    // Get performance profiler
    auto performanceProfiler = PerformanceProfiler::getInstance();
    
    // Set up performance tracking for key brain operations
    performanceProfiler->startProfile("neuron_activation");
    performanceProfiler->startProfile("synapse_formation");
    performanceProfiler->startProfile("plasticity_update");
    performanceProfiler->startProfile("network_stability");
    
    // Configure memory tracking
    auto memoryTracker = MemoryTracker::getInstance();
    memoryTracker->setAllocationTrackingEnabled(true);
    memoryTracker->setLeakDetectionEnabled(true);
    memoryTracker->addIgnorePattern("temp");
    memoryTracker->addIgnorePattern("buffer");
    
    // Log integration
    Logger::getGlobal().info("Brain components integrated with enhanced logging system");
}

// Integration for environment simulation components
void integrateEnvironmentComponents() {
    auto& loggerManager = LoggerManager::getInstance();
    
    // Get environment logger
    auto envLogger = loggerManager.getLogger("environment");
    envLogger->setLevel(LogLevel::Info);
    envLogger->setFormat("json");
    
    // Get system monitor
    auto systemMonitor = SystemMonitor::getInstance();
    
    // Set up environment-specific monitoring
    systemMonitor->updateComponentStatus("environment", "ok", "Environment simulation running");
    systemMonitor->addCustomMetric("environment_step_time", 50.0, "ms");
    systemMonitor->addCustomMetric("simulation_temperature", 37.0, "C");
    
    // Log integration
    Logger::getGlobal().info("Environment components integrated with enhanced logging system");
}

// Integration for experiment components
void integrateExperimentComponents() {
    auto& loggerManager = LoggerManager::getInstance();
    
    // Get experiment logger
    auto expLogger = loggerManager.getLogger("experiment");
    expLogger->setLevel(LogLevel::Warning); // Experiments log at warning level to reduce noise
    expLogger->setFormat("json");
    
    // Get performance profiler
    auto performanceProfiler = PerformanceProfiler::getInstance();
    
    // Set up experiment-specific performance tracking
    performanceProfiler->startProfile("experiment_run");
    performanceProfiler->startProfile("data_collection");
    performanceProfiler->startProfile("results_analysis");
    
    // Configure memory tracking for experiments
    auto memoryTracker = MemoryTracker::getInstance();
    memoryTracker->setThresholdForLeakDetection(10 * 1024 * 1024); // 10MB threshold for experiments
    
    // Log integration
    Logger::getGlobal().info("Experiment components integrated with enhanced logging system");
}

// Helper function to track simulation performance
void trackSimulationPerformance(const std::string& operation, double duration) {
    auto performanceProfiler = PerformanceProfiler::getInstance();
    performanceProfiler->startProfile(operation);
    std::this_thread::sleep_for(std::chrono::duration<double>(duration / 1000.0)); // Convert ms to seconds
    performanceProfiler->stopProfile(operation);
    
    // Log performance warning if threshold exceeded
    auto& loggerManager = LoggerManager::getInstance();
    double threshold = loggerManager.getPerformanceThreshold(operation);
    if (threshold > 0 && duration > threshold) {
        Logger::getGlobal().warning("Performance threshold exceeded for " + operation + ": " + std::to_string(duration) + "ms (threshold: " + std::to_string(threshold) + "ms)");
    }
}

// Helper function to track memory usage
void trackMemoryUsage(const std::string& context, size_t bytes) {
    auto memoryTracker = MemoryTracker::getInstance();
    memoryTracker->trackAllocation(nullptr, bytes, context); // Use nullptr for summary tracking
    memoryTracker->trackDeallocation(nullptr);
}

// Debug console helper functions
define DEBUG_CONSOLE_HELPERS struct DebugHelpers {
    static void updateAllComponents() {
        auto systemMonitor = SystemMonitor::getInstance();
        auto debugConsole = DebugConsole::getInstance();
        
        systemMonitor->update();
        debugConsole->update();
    }
    
    static void exportAllReports() {
        auto debugConsole = DebugConsole::getInstance();
        auto memoryTracker = MemoryTracker::getInstance();
        auto performanceProfiler = PerformanceProfiler::getInstance();
        
        std::time_t now = std::time(nullptr);
        char timestamp[100];
        std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&now));
        
        std::string baseName = "nlm_debug_" + std::string(timestamp);
        
        debugConsole->exportLogs(baseName + ".log");
        debugConsole->exportPerformanceData(baseName + "_performance.json");
        debugConsole->exportSystemInfo(baseName + "_system.json");
        memoryTracker->exportMemoryReport(baseName + "_memory.txt");
        performanceProfiler->exportProfiles(baseName + "_profiles.json");
        
        Logger::getGlobal().info("All debug reports exported with prefix: " + baseName);
    }
    
    static void showSystemStatus() {
        auto systemMonitor = SystemMonitor::getInstance();
        auto debugConsole = DebugConsole::getInstance();
        
        auto status = systemMonitor->getAllStatus();
        auto memoryInfo = systemMonitor->getMemoryInfo();
        auto cpuInfo = systemMonitor->getCpuInfo();
        
        debugConsole->clear();
        debugConsole->addSystemStatus("Memory", debugConsole->formatMemory(memoryInfo.used), "ok");
        debugConsole->addSystemStatus("CPU", debugConsole->formatPercentage(cpuInfo.usage), "ok");
        debugConsole->addSystemStatus("Active Threads", std::to_string(systemMonitor->getThreadInfo().size()), "ok");
        
        for (const auto& stat : status) {
            debugConsole->addSystemStatus(stat.component, stat.value, stat.status);
        }
    }
};

// Macro for easy integration with existing NLM logging macros
#define NLM_LOG_PERFORMANCE(operation, duration) \
    trackSimulationPerformance(operation, duration)

#define NLM_LOG_MEMORY(context, bytes) \
    trackMemoryUsage(context, bytes)

#define NLM_LOG_SIMULATION_STATUS(status, message) \
    systemMonitor->updateComponentStatus("simulation", status, message)

#define NLM_EXPORT_DEBUG_REPORTS() \
    DebugHelpers::exportAllReports()

#define NLM_SHOW_SYSTEM_STATUS() \
    DebugHelpers::showSystemStatus()

} // namespace nlm::integration

// Global integration instance
struct GlobalIntegration {
    GlobalIntegration() {
        nlm::integration::initializeEnhancedLogging();
    }
    
    ~GlobalIntegration() {
        nlm::integration::LoggerManager::getInstance().stopSystemMonitoring();
        auto debugConsole = nlm::DebugConsole::getInstance();
        debugConsole->hide();
    }
} globalIntegration;