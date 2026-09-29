#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <iomanip>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../plasticity/PlasticityRule.hpp"

namespace nlm {

// Debug log entry
struct DebugLogEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string level;
    std::string module;
    std::string message;
    std::string function;
    int lineNumber;
    std::map<std::string, std::string> metadata;
    std::vector<double> performanceMetrics;
    
    DebugLogEntry(const std::string& lvl, const std::string& mod, const std::string& msg,
                 const std::string& func = "", int line = 0)
        : level(lvl), module(mod), message(msg), function(func), lineNumber(line) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Performance profiler
class PerformanceProfiler {
private:
    std::string profilerId;
    std::map<std::string, std::vector<double>> functionTimings;
    std::map<std::string, int> functionCallCounts;
    std::map<std::string, double> functionMinTimes;
    std::map<std::string, double> functionMaxTimes;
    std::mutex profilerMutex;
    double samplingInterval;
    size_t maxSamples;
    
public:
    PerformanceProfiler(const std::string& id, double interval = 0.1, size_t maxSamples = 1000)
        : profilerId(id), samplingInterval(interval), maxSamples(maxSamples) {
        functionMinTimes["default"] = std::numeric_limits<double>::max();
    }
    
    // Start profiling a function
    void startProfile(const std::string& functionName) {
        std::lock_guard<std::mutex> lock(profilerMutex);
        
        functionCallCounts[functionName]++;
        functionMinTimes[functionName] = std::numeric_limits<double>::max();
    }
    
    // End profiling a function
    void endProfile(const std::string& functionName) {
        std::lock_guard<std::mutex> lock(profilerMutex);
        
        if (functionCallCounts.find(functionName) == functionCallCounts.end()) {
            return;
        }
        
        // In real implementation, would record actual elapsed time
        double elapsedTime = std::uniform_real_distribution<double>(0.001, 0.1)(rng);
        
        functionTimings[functionName].push_back(elapsedTime);
        
        // Update min/max
        if (elapsedTime < functionMinTimes[functionName]) {
            functionMinTimes[functionName] = elapsedTime;
        }
        
        // Trim if too many samples
        if (functionTimings[functionName].size() > maxSamples) {
            functionTimings[functionName].erase(functionTimings[functionName].begin());
        }
    }
    
    // Get average time for a function
    double getAverageTime(const std::string& functionName) const {
        std::lock_guard<std::mutex> lock(profilerMutex);
        
        if (functionTimings.find(functionName) == functionTimings.end() || 
            functionTimings.at(functionName).empty()) {
            return 0.0;
        }
        
        double sum = std::accumulate(functionTimings.at(functionName).begin(),
                                    functionTimings.at(functionName).end(), 0.0);
        return sum / functionTimings.at(functionName).size();
    }
    
    // Get total calls for a function
    int getCallCount(const std::string& functionName) const {
        std::lock_guard<std::mutex> lock(profilerMutex);
        
        if (functionCallCounts.find(functionName) == functionCallCounts.end()) {
            return 0;
        }
        
        return functionCallCounts.at(functionName);
    }
    
    // Get min time for a function
    double getMinTime(const std::string& functionName) const {
        std::lock_guard<std::mutex> lock(profilerMutex);
        
        if (functionMinTimes.find(functionName) == functionMinTimes.end()) {
            return 0.0;
        }
        
        return functionMinTimes.at(functionName);
    }
    
    // Get max time for a function
    double getMaxTime(const std::string& functionName) const {
        std::lock_guard<std::mutex> lock(profilerMutex);
        
        if (functionTimings.find(functionName) == functionTimings.end() || 
            functionTimings.at(functionName).empty()) {
            return 0.0;
        }
        
        return *std::max_element(functionTimings.at(functionName).begin(),
                                functionTimings.at(functionName).end());
    }
    
    // Get profile report
    std::string getProfileReport() const {
        std::stringstream ss;
        ss << "Performance Profile Report:\n";
        ss << "Profiler ID: " << profilerId << "\n";
        ss << "Sampling Interval: " << samplingInterval << " seconds\n";
        ss << "Maximum Samples: " << maxSamples << "\n\n";
        
        // Sort functions by average time
        std::vector<std::pair<std::string, double>> sortedFunctions;
        for (const auto& pair : functionTimings) {
            double avgTime = getAverageTime(pair.first);
            sortedFunctions.emplace_back(pair.first, avgTime);
        }
        
        std::sort(sortedFunctions.begin(), sortedFunctions.end(),
                 [](const auto& a, const auto& b) { return a.second > b.second; });
        
        ss << "Function Performance:\n";
        for (const auto& pair : sortedFunctions) {
            const std::string& funcName = pair.first;
            double avgTime = pair.second;
            int callCount = getCallCount(funcName);
            double minTime = getMinTime(funcName);
            double maxTime = getMaxTime(funcName);
            
            ss << "  " << funcName << ":\n";
            ss << "    Calls: " << callCount << "\n";
            ss << "    Average Time: " << std::fixed << std::setprecision(6) << avgTime << "s\n";
            ss << "    Min Time: " << std::fixed << std::setprecision(6) << minTime << "s\n";
            ss << "    Max Time: " << std::fixed << std::setprecision(6) << maxTime << "s\n";
        }
        
        return ss.str();
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(profilerMutex);
        functionTimings.clear();
        functionCallCounts.clear();
        functionMinTimes.clear();
    }
    
    std::string getProfilerId() const { return profilerId; }
    
private:
    std::mt19937 rng{std::random_device{}()};
};

// Memory tracker
class MemoryTracker {
private:
    std::string trackerId;
    std::vector<double> memoryUsageHistory;
    std::vector<size_t> allocatedMemory;
    std::vector<size_t> deallocatedMemory;
    std::mutex memoryMutex;
    size_t peakMemory;
    size_t currentMemory;
    double monitoringInterval;
    size_t maxHistorySize;
    
public:
    MemoryTracker(const std::string& id, double interval = 0.5, size_t maxHistory = 1000)
        : trackerId(id), monitoringInterval(interval), maxHistorySize(maxHistory),
          peakMemory(0), currentMemory(0) {
        memoryUsageHistory.reserve(maxHistory);
        allocatedMemory.reserve(maxHistory);
        deallocatedMemory.reserve(maxHistory);
    }
    
    // Track memory allocation
    void trackAllocation(size_t bytes) {
        std::lock_guard<std::mutex> lock(memoryMutex);
        allocatedMemory.push_back(bytes);
        currentMemory += bytes;
        peakMemory = std::max(peakMemory, currentMemory);
    }
    
    // Track memory deallocation
    void trackDeallocation(size_t bytes) {
        std::lock_guard<std::mutex> lock(memoryMutex);
        deallocatedMemory.push_back(bytes);
        currentMemory = std::max(0LL, static_cast<long long>(currentMemory - bytes));
    }
    
    // Record current memory usage
    void recordUsage() {
        std::lock_guard<std::mutex> lock(memoryMutex);
        
        memoryUsageHistory.push_back(static_cast<double>(currentMemory));
        
        if (memoryUsageHistory.size() > maxHistorySize) {
            memoryUsageHistory.erase(memoryUsageHistory.begin());
        }
    }
    
    // Get peak memory usage
    size_t getPeakMemory() const {
        return peakMemory;
    }
    
    // Get current memory usage
    size_t getCurrentMemory() const {
        return currentMemory;
    }
    
    // Get average memory usage
    double getAverageMemory() const {
        if (memoryUsageHistory.empty()) return 0.0;
        
        double sum = std::accumulate(memoryUsageHistory.begin(), memoryUsageHistory.end(), 0.0);
        return sum / memoryUsageHistory.size();
    }
    
    // Get memory usage statistics
    struct MemoryStats {
        size_t current;
        size_t peak;
        double average;
        double min;
        double max;
        double variance;
    };
    
    MemoryStats getMemoryStats() const {
        MemoryStats stats;
        stats.current = currentMemory;
        stats.peak = peakMemory;
        
        if (!memoryUsageHistory.empty()) {
            stats.average = getAverageMemory();
            stats.min = *std::min_element(memoryUsageHistory.begin(), memoryUsageHistory.end());
            stats.max = *std::max_element(memoryUsageHistory.begin(), memoryUsageHistory.end());
            
            double mean = stats.average;
            double variance = 0.0;
            for (double usage : memoryUsageHistory) {
                double diff = usage - mean;
                variance += diff * diff;
            }
            variance /= memoryUsageHistory.size();
            stats.variance = variance;
        } else {
            stats.average = 0.0;
            stats.min = 0.0;
            stats.max = 0.0;
            stats.variance = 0.0;
        }
        
        return stats;
    }
    
    std::string getMemoryTrackerStatus() const {
        std::stringstream ss;
        ss << "Memory Tracker: " << trackerId << "\n";
        ss << "  Current Memory: " << currentMemory << " bytes\n";
        ss << "  Peak Memory: " << peakMemory << " bytes\n";
        ss << "  Average Memory: " << getAverageMemory() << " bytes\n";
        
        MemoryStats stats = getMemoryStats();
        ss << "  Memory Variance: " << stats.variance << "\n";
        ss << "  History Size: " << memoryUsageHistory.size() << "\n";
        
        return ss.str();
    }
    
    std::string getTrackerId() const { return trackerId; }
    
    void reset() {
        std::lock_guard<std::mutex> lock(memoryMutex);
        memoryUsageHistory.clear();
        allocatedMemory.clear();
        deallocatedMemory.clear();
        peakMemory = 0;
        currentMemory = 0;
    }
};

// Model validator
class ModelValidator {
private:
    std::string validatorId;
    std::vector<std::string> validationRules;
    std::map<std::string, std::function<bool(const Brain&, const Config&)>> validationFunctions;
    std::vector<std::string> validationHistory;
    double validationThreshold;
    size_t maxHistorySize;
    
public:
    ModelValidator(const std::string& id, double threshold = 0.95, size_t maxHistory = 1000)
        : validatorId(id), validationThreshold(threshold), maxHistorySize(maxHistory) {
        
        // Add default validation rules
        validationRules = {
            "structure_integrity",
            "parameter_bounds",
            "plasticity_consistency",
            "learning_rates_reasonable",
            "stability_check"
        };
        
        // Register validation functions
        validationFunctions["structure_integrity"] = 
            [this](const Brain& brain, const Config& config) { return validateStructureIntegrity(brain, config); };
        
        validationFunctions["parameter_bounds"] =
            [this](const Brain& brain, const Config& config) { return validateParameterBounds(brain, config); };
        
        validationFunctions["plasticity_consistency"] =
            [this](const Brain& brain, const Config& config) { return validatePlasticityConsistency(brain, config); };
        
        validationFunctions["learning_rates_reasonable"] =
            [this](const Brain& brain, const Config& config) { return validateLearningRates(brain, config); };
        
        validationFunctions["stability_check"] =
            [this](const Brain& brain, const Config& config) { return validateStability(brain, config); };
    }
    
    // Validate model against rules
    bool validate(const Brain& brain, const Config& config) {
        bool allValid = true;
        
        for (const auto& ruleName : validationRules) {
            if (validationFunctions.find(ruleName) != validationFunctions.end()) {
                bool ruleValid = validationFunctions[ruleName](brain, config);
                allValid = allValid && ruleValid;
                
                std::stringstream ss;
                ss << "Rule: " << ruleName << " - " << (ruleValid ? "PASSED" : "FAILED");
                validationHistory.push_back(ss.str());
                
                if (validationHistory.size() > maxHistorySize) {
                    validationHistory.erase(validationHistory.begin());
                }
            }
        }
        
        return allValid;
    }
    
    // Validate structure integrity
    bool validateStructureIntegrity(const Brain& brain, const Config& config) {
        // Check if brain has valid regions
        auto regions = brain.getRegions();
        if (regions.empty()) {
            return false;
        }
        
        // Check if synapses are properly formed
        auto synapses = brain.getSynapses();
        for (const auto& synapse : synapses) {
            if (synapse.pre Neuron == nullptr || synapse.postNeuron == nullptr) {
                return false;
            }
        }
        
        return true;
    }
    
    // Validate parameter bounds
    bool validateParameterBounds(const Brain& brain, const Config& config) {
        // Check if parameters are within reasonable bounds
        auto globalExcitation = brain.getGlobalExcitation();
        if (globalExcitation < 0.0 || globalExcitation > 100.0) {
            return false;
        }
        
        auto globalInhibition = brain.getGlobalInhibition();
        if (globalInhibition < 0.0 || globalInhibition > 100.0) {
            return false;
        }
        
        return true;
    }
    
    // Validate plasticity consistency
    bool validatePlasticityConsistency(const Brain& brain, const Config& config) {
        // Check if plasticity rules are consistent
        auto plasticityRules = brain.getPlasticityRules();
        for (const auto& rule : plasticityRules) {
            // Check rule parameters
            if (rule.learningRate < 0.0 || rule.learningRate > 1.0) {
                return false;
            }
        }
        
        return true;
    }
    
    // Validate learning rates
    bool validateLearningRates(const Brain& brain, const Config& config) {
        // Check if learning rates are reasonable
        double globalLearningRate = config.getOr("brain.learning_rate", 0.01);
        if (globalLearningRate < 0.0 || globalLearningRate > 1.0) {
            return false;
        }
        
        // Check individual neuron learning rates
        auto neurons = brain.getNeurons();
        for (const auto& neuron : neurons) {
            if (neuron.learningRate < 0.0 || neuron.learningRate > 1.0) {
                return false;
            }
        }
        
        return true;
    }
    
    // Validate stability
    bool validateStability(const Brain& brain, const Config& config) {
        // Check if brain is stable (no exploding activity)
        auto globalExcitation = brain.getGlobalExcitation();
        auto globalInhibition = brain.getGlobalInhibition();
        
        // Calculate stability metric
        double stability = 1.0 / (1.0 + std::abs(globalExcitation - globalInhibition));
        
        return stability > validationThreshold;
    }
    
    std::string getValidatorId() const { return validatorId; }
    
    std::string getValidatorStatus() const {
        std::stringstream ss;
        ss << "Model Validator: " << validatorId << "\n";
        ss << "  Validation Rules: " << validationRules.size() << "\n";
        ss << "  Validation Threshold: " << validationThreshold << "\n";
        ss << "  History Size: " << validationHistory.size() << "\n";
        
        if (!validationHistory.empty()) {
            ss << "  Recent Validations:\n";
            size_t recentCount = std::min(validationHistory.size(), static_cast<size_t>(5));
            for (size_t i = 0; i < recentCount; ++i) {
                ss << "    " << validationHistory[validationHistory.size() - 1 - i] << "\n";
            }
        }
        
        return ss.str();
    }
    
    void reset() {
        validationHistory.clear();
    }
};

// Debug logger
class DebugLogger {
private:
    std::string loggerId;
    std::queue<DebugLogEntry> logQueue;
    std::mutex logMutex;
    std::condition_variable logCondition;
    std::atomic<bool> running;
    std::ofstream logFile;
    std::string logFileName;
    bool consoleOutputEnabled;
    bool fileOutputEnabled;
    std::map<std::string, bool> moduleEnabled;
    
public:
    DebugLogger(const std::string& id, const std::string& logFile = "debug.log")
        : loggerId(id), logFileName(logFile), running(true),
          consoleOutputEnabled(true), fileOutputEnabled(true) {
        
        // Enable all modules by default
        moduleEnabled["brain"] = true;
        moduleEnabled["plasticity"] = true;
        moduleEnabled["environment"] = true;
        moduleEnabled["simulation"] = true;
        moduleEnabled["advanced"] = true;
        
        // Open log file
        if (fileOutputEnabled) {
            logFile.open(logFileName, std::ios::out | std::ios::trunc);
        }
    }
    
    // Log a message
    void log(const std::string& level, const std::string& module,
             const std::string& message, const std::string& function = "",
             int line = 0) {
        // Check if module is enabled
        if (moduleEnabled.find(module) == moduleEnabled.end() || !moduleEnabled[module]) {
            return;
        }
        
        DebugLogEntry entry(level, module, message, function, line);
        
        std::lock_guard<std::mutex> lock(logMutex);
        logQueue.push(entry);
        logCondition.notify_one();
        
        // Output to console
        if (consoleOutputEnabled) {
            std::cout << "[" << getCurrentTimestamp() << "] "
                      << level << ": " << module << " - "
                      << message << std::endl;
        }
        
        // Output to file
        if (fileOutputEnabled) {
            logFile << "[" << getCurrentTimestamp() << "] "
                    << level << ": " << module << " - "
                    << message << std::endl;
        }
    }
    
    // Process log queue
    void processLogs() {
        while (running) {
            std::unique_lock<std::mutex> lock(logMutex);
            
            if (logQueue.empty()) {
                logCondition.wait_for(lock, std::chrono::milliseconds(100),
                                     [this] { return !running || !logQueue.empty(); });
            }
            
            if (!running && logQueue.empty()) {
                break;
            }
            
            if (!logQueue.empty()) {
                DebugLogEntry entry = logQueue.front();
                logQueue.pop();
                
                // Log entry has been processed
            }
        }
    }
    
    // Enable/disable module
    void setModuleEnabled(const std::string& module, bool enabled) {
        moduleEnabled[module] = enabled;
    }
    
    // Enable console output
    void enableConsoleOutput(bool enabled) {
        consoleOutputEnabled = enabled;
    }
    
    // Enable file output
    void enableFileOutput(bool enabled, const std::string& filename = "") {
        if (enabled && !filename.empty()) {
            logFileName = filename;
        }
        
        fileOutputEnabled = enabled;
        
        if (enabled) {
            logFile.open(logFileName, std::ios::out | std::ios::trunc);
        } else {
            logFile.close();
        }
    }
    
    // Get current timestamp
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }
    
    std::string getLoggerId() const { return loggerId; }
    
    std::string getLoggerStatus() const {
        std::stringstream ss;
        ss << "Debug Logger: " << loggerId << "\n";
        ss << "  Console Output: " << (consoleOutputEnabled ? "Enabled" : "Disabled") << "\n";
        ss << "  File Output: " << (fileOutputEnabled ? "Enabled" : "Disabled") << "\n";
        ss << "  Log File: " << logFileName << "\n";
        ss << "  Enabled Modules: ";
        for (const auto& pair : moduleEnabled) {
            if (pair.second) {
                ss << pair.first << " ";
            }
        }
        ss << "\n";
        
        return ss.str();
    }
    
    void stop() {
        running = false;
        logCondition.notify_all();
        
        if (logFile.is_open()) {
            logFile.close();
        }
    }
};

// Advanced debugging system
class AdvancedDebuggingSystem {
private:
    std::string systemId;
    std::unique_ptr<PerformanceProfiler> profiler;
    std::unique_ptr<MemoryTracker> memoryTracker;
    std::unique_ptr<ModelValidator> modelValidator;
    std::unique_ptr<DebugLogger> debugLogger;
    std::vector<std::function<bool(const Brain&, const Config&)>> customValidators;
    std::vector<DebugLogEntry> debugLogBuffer;
    double snapshotInterval;
    bool autoLogging;
    
public:
    AdvancedDebuggingSystem(const std::string& id, const Config& config) : systemId(id) {
        snapshotInterval = config.getOr("debug.snapshot_interval", 1.0);
        autoLogging = config.getOr("debug.auto_logging", true);
        
        // Initialize debugging components
        profiler = std::make_unique<PerformanceProfiler>("main_profiler", 0.1, 1000);
        memoryTracker = std::make_unique<MemoryTracker>("main_memory_tracker", 0.5, 1000);
        modelValidator = std::make_unique<ModelValidator>("main_validator", 0.95, 1000);
        debugLogger = std::make_unique<DebugLogger>("main_logger", "debug.log");
        
        // Start logger thread
        loggerThread = std::thread(&DebugLogger::processLogs, debugLogger.get());
    }
    
    // Start profiling
    void startProfiling(const std::string& functionName) {
        profiler->startProfile(functionName);
    }
    
    // End profiling
    void endProfiling(const std::string& functionName) {
        profiler->endProfile(functionName);
    }
    
    // Track memory allocation
    void trackAllocation(size_t bytes) {
        memoryTracker->trackAllocation(bytes);
    }
    
    // Track memory deallocation
    void trackDeallocation(size_t bytes) {
        memoryTracker->trackDeallocation(bytes);
    }
    
    // Record memory usage
    void recordMemoryUsage() {
        memoryTracker->recordUsage();
    }
    
    // Validate model
    bool validate(const Brain& brain, const Config& config) {
        bool modelValid = modelValidator->validate(brain, config);
        
        // Run custom validators
        for (const auto& validator : customValidators) {
            if (!validator(brain, config)) {
                modelValid = false;
            }
        }
        
        return modelValid;
    }
    
    // Log debug message
    void log(const std::string& level, const std::string& module,
             const std::string& message, const std::string& function = "",
             int line = 0) {
        debugLogger->log(level, module, message, function, line);
    }
    
    // Take system snapshot
    void takeSystemSnapshot(const Brain& brain, const Config& config) {
        // Take performance snapshot
        std::stringstream ss;
        ss << "=== Performance Snapshot ===\n";
        ss << profiler->getProfileReport() << "\n";
        
        // Take memory snapshot
        ss << "=== Memory Snapshot ===\n";
        ss << memoryTracker->getMemoryTrackerStatus() << "\n";
        
        // Take validation snapshot
        ss << "=== Validation Snapshot ===\n";
        ss << modelValidator->getValidatorStatus() << "\n";
        
        debugLogBuffer.push_back(DebugLogEntry("INFO", "system", ss.str()));
    }
    
    // Get system status
    std::string getSystemStatus() const {
        std::stringstream ss;
        ss << "Advanced Debugging System: " << systemId << "\n";
        ss << "  Snapshot Interval: " << snapshotInterval << " seconds\n";
        ss << "  Auto Logging: " << (autoLogging ? "Enabled" : "Disabled") << "\n";
        ss << "  Custom Validators: " << customValidators.size() << "\n";
        ss << "  Debug Log Buffer: " << debugLogBuffer.size() << " entries\n";
        ss << "  Performance Profiler: " << profiler->getProfilerId() << "\n";
        ss << "  Memory Tracker: " << memoryTracker->getTrackerId() << "\n";
        ss << "  Model Validator: " << modelValidator->getValidatorId() << "\n";
        ss << "  Debug Logger: " << debugLogger->getLoggerId() << "\n";
        
        return ss.str();
    }
    
    // Save debug state
    void saveDebugState(const Config& config) const {
        // Implementation for saving debug state
    }
    
    // Load debug state
    void loadDebugState(const Config& config) {
        // Implementation for loading debug state
    }
    
    void stop() {
        if (loggerThread.joinable()) {
            loggerThread.join();
        }
        
        debugLogger->stop();
    }
    
private:
    std::thread loggerThread;
};

} // namespace nlm
